#include "gesture_engine.h"
#include <math.h>
#include <stdint.h>

/* 保存上一帧，用于计算当前帧变化量。 */
static imu_frame_t s_prev;

/* 去抖状态：先做原始判定，再经过连续帧确认后输出。 */
static gesture_t s_raw_prev = GESTURE_UNKNOWN;
static gesture_t s_stable = GESTURE_UNKNOWN;
static uint8_t s_same_count = 0U;

void Gesture_Init(void)
{
  s_prev.ax = 0.0f;
  s_prev.ay = 0.0f;
  s_prev.az = 0.0f;

  s_raw_prev = GESTURE_UNKNOWN;
  s_stable = GESTURE_UNKNOWN;
  s_same_count = 0U;
}

static gesture_t gesture_classify_raw(const imu_frame_t *frame)
{
  /*
   * 首版阈值（按当前日志做了放宽）：
   * - 静止阈值适当放宽，减少 STATIC/UNKNOWN 来回跳
   * - 旋转与挥动阈值稍提高，减少误触发
   */
  const float static_da_th = 0.45f;
  const float static_g_th = 12.0f;
  const float rot_gz_th = 80.0f;
  const float swing_da_th = 1.8f;

  float da = fabsf(frame->ax - s_prev.ax) + fabsf(frame->ay - s_prev.ay) + fabsf(frame->az - s_prev.az);
  float gz = frame->gz;

  /* 更新上一帧缓存供下一次计算。 */
  s_prev = *frame;

  if (da < static_da_th && fabsf(frame->gx) < static_g_th && fabsf(frame->gy) < static_g_th && fabsf(frame->gz) < static_g_th) {
    return GESTURE_STATIC;
  }
  if (gz > rot_gz_th) {
    return GESTURE_ROTATE_CW;
  }
  if (gz < -rot_gz_th) {
    return GESTURE_ROTATE_CCW;
  }
  if (da > swing_da_th) {
    return GESTURE_SWING;
  }
  return GESTURE_UNKNOWN;
}

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
