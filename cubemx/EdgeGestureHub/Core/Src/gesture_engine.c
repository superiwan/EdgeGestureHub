#include "gesture_engine.h"
#include "NanoEdgeAI.h"
#include "app_config.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

/*
 * 手势引擎说明：
 * 1) 主路径使用 NanoEdge AI（在线学习 + 异常检测）；
 * 2) 异常检测结果映射到现有业务三态编码（0/1/2）；
 * 3) 当 AI 库异常时，自动回退到阈值规则分类器，保证系统不断流。
 */

/* 保存上一帧，用于兜底规则分类时计算当前帧变化量。 */
static imu_frame_t s_prev;

/* 去抖状态：先做原始判定，再经过连续帧确认后输出。 */
static gesture_t s_raw_prev = GESTURE_UNKNOWN;
static gesture_t s_stable = GESTURE_UNKNOWN;
static uint8_t s_same_count = 0U;

/* NanoEdge AI 运行状态。 */
static bool s_neai_ready = false;
static bool s_neai_learning_done = false;
static uint8_t s_neai_similarity = 0U;

/* 三态分类编码：0=学习中，1=正常，2=异常。 */
static int s_class_code = 0;

/* 上电后固定学习窗口：前 N 毫秒只学习，不输出异常。 */
static uint32_t s_startup_tick = 0U;

/* 兜底规则分类器：当 NanoEdge 初始化/调用失败时仍可输出基础结果。 */
static gesture_t gesture_classify_fallback(const imu_frame_t *frame)
{
  const float static_da_th = 0.45f;
  const float static_g_th = 12.0f;
  const float rot_gz_th = 80.0f;
  const float swing_da_th = 1.8f;

  float da = fabsf(frame->ax - s_prev.ax) + fabsf(frame->ay - s_prev.ay) + fabsf(frame->az - s_prev.az);
  float gz = frame->gz;

  s_prev = *frame;

  if (da < static_da_th && fabsf(frame->gx) < static_g_th && fabsf(frame->gy) < static_g_th && fabsf(frame->gz) < static_g_th) {
    s_class_code = 1;
    return GESTURE_STATIC;
  }
  if (gz > rot_gz_th) {
    s_class_code = 2;
    return GESTURE_ROTATE_CW;
  }
  if (gz < -rot_gz_th) {
    s_class_code = 2;
    return GESTURE_ROTATE_CCW;
  }
  if (da > swing_da_th) {
    s_class_code = 2;
    return GESTURE_SWING;
  }
  s_class_code = 0;
  return GESTURE_UNKNOWN;
}

/* 初始化算法内部状态，通常在上电后调用一次。 */
void Gesture_Init(void)
{
  enum neai_state st;

  s_prev.ax = 0.0f;
  s_prev.ay = 0.0f;
  s_prev.az = 0.0f;

  s_raw_prev = GESTURE_UNKNOWN;
  s_stable = GESTURE_UNKNOWN;
  s_same_count = 0U;

  s_neai_ready = false;
  s_neai_learning_done = false;
  s_neai_similarity = 0U;
  s_class_code = 0;
  s_startup_tick = HAL_GetTick();

  /* 初始化 NanoEdge：
   * use_pretrained=false 表示按当前设备实时学习（与你当前实验流程一致）。
   */
  st = neai_anomalydetection_init(false);
  if (st == NEAI_OK) {
    s_neai_ready = true;
  }
}

/* 原始分类器：
 * 输入当前帧，输出未经去抖的瞬时分类结果。 */
static gesture_t gesture_classify_raw(const imu_frame_t *frame)
{
  enum neai_state st;
  float input_signal[NEAI_INPUT_SIGNAL_LENGTH * NEAI_INPUT_AXIS_NUMBER];

  /* 若 NanoEdge 不可用，自动退回阈值分类器。 */
  if (!s_neai_ready) {
    return gesture_classify_fallback(frame);
  }

  /* 你的库配置为 1 个样本 x 6 轴：ax, ay, az, gx, gy, gz。 */
  input_signal[0] = frame->ax;
  input_signal[1] = frame->ay;
  input_signal[2] = frame->az;
  input_signal[3] = frame->gx;
  input_signal[4] = frame->gy;
  input_signal[5] = frame->gz;

  /* 启动学习锁：上电后前 N 毫秒无条件学习，不输出异常。 */
  if ((HAL_GetTick() - s_startup_tick) < GESTURE_NEAI_STARTUP_LEARN_LOCK_MS) {
    st = neai_anomalydetection_learn(input_signal);
    if (st == NEAI_LEARNING_DONE) {
      s_neai_learning_done = true;
    } else if (st != NEAI_LEARNING_IN_PROGRESS) {
      s_neai_ready = false;
      return gesture_classify_fallback(frame);
    }
    s_class_code = 0;
    return GESTURE_UNKNOWN;
  }

  /* 阶段 1：在线学习“正常模式”（静止）。 */
  if (!s_neai_learning_done) {
    st = neai_anomalydetection_learn(input_signal);
    if (st == NEAI_LEARNING_DONE) {
      s_neai_learning_done = true;
    } else if (st != NEAI_LEARNING_IN_PROGRESS) {
      /* 学习失败时退回兜底分类，避免系统无输出。 */
      s_neai_ready = false;
      return gesture_classify_fallback(frame);
    }
    s_class_code = 0;
    return GESTURE_UNKNOWN;
  }

  /* 阶段 2：检测阶段，输出相似度百分比。 */
  st = neai_anomalydetection_detect(input_signal, &s_neai_similarity);
  if (st != NEAI_OK) {
    if (st == NEAI_LEARNING_IN_PROGRESS) {
      /* 若库要求继续学习，回到学习阶段。 */
      s_neai_learning_done = false;
      s_class_code = 0;
      return GESTURE_UNKNOWN;
    }
    s_neai_ready = false;
    return gesture_classify_fallback(frame);
  }

  /* 将二分类结果映射到现有业务枚举：
   * - 相似度高：判定为正常
   * - 相似度低：判定为异常
   */
  if (s_neai_similarity < GESTURE_NEAI_NOMINAL_SIMILARITY_TH) {
    s_class_code = 2;
    return GESTURE_SWING;
  }
  s_class_code = 1;
  return GESTURE_STATIC;
}

/* 手势引擎主入口：
 * 将原始分类结果做连续帧确认，输出稳定结果。 */
gesture_t Gesture_Run(const imu_frame_t *frame)
{
  gesture_t raw;

  /* 连续 3 帧相同才切换输出，抑制抖动。 */
  const uint8_t confirm_frames = 3U;

  raw = gesture_classify_raw(frame);

  if (raw == s_raw_prev) {
    if (s_same_count < 255U) {
      ++s_same_count;
    }
  } else {
    s_raw_prev = raw;
    s_same_count = 1U;
  }

  if (s_same_count >= confirm_frames) {
    s_stable = raw;
  }

  return s_stable;
}

/* 枚举到字符串映射：用于串口与 OLED 显示。 */
const char *Gesture_ToString(gesture_t g)
{
  switch (g) {
    case GESTURE_STATIC: return "STATIC";
    case GESTURE_SWING: return "SWING";
    case GESTURE_ROTATE_CW: return "ROT_CW";
    case GESTURE_ROTATE_CCW: return "ROT_CCW";
    default: return "UNKNOWN";
  }
}

/* 获取当前三态分类编码：0=学习中，1=正常，2=异常。 */
int Gesture_GetClassCode(void)
{
  return s_class_code;
}

/* 获取当前三态标签：用于 OLED 显示。 */
const char *Gesture_GetClassLabel(void)
{
  if (s_class_code == 0) {
    return "LEARN";
  }
  if (s_class_code == 1) {
    return "NORM";
  }
  return "ANOM";
}

/* 获取学习完成状态：0=未完成，1=已完成。 */
int Gesture_IsLearningDone(void)
{
  return s_neai_learning_done ? 1 : 0;
}
