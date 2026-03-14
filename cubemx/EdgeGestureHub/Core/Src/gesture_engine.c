#include "gesture_engine.h"
#include <math.h>

/* 保存上一帧，用于计算当前帧变化量。 */
static imu_frame_t s_prev;

void Gesture_Init(void)
{
  s_prev.ax = 0.0f;
  s_prev.ay = 0.0f;
  s_prev.az = 0.0f;
}

gesture_t Gesture_Run(const imu_frame_t *frame)
{
  /*
   * da: 当前帧与上一帧加速度变化总量（简化特征）
   * gz: Z 轴角速度，主要用于区分顺/逆时针旋转
   */
  float da = fabsf(frame->ax - s_prev.ax) + fabsf(frame->ay - s_prev.ay) + fabsf(frame->az - s_prev.az);
  float gz = frame->gz;

  s_prev = *frame;

  /* 静止：加速度变化小，且三轴角速度都很小。 */
  if (da < 0.25f && fabsf(frame->gx) < 8.0f && fabsf(frame->gy) < 8.0f && fabsf(frame->gz) < 8.0f) {
    return GESTURE_STATIC;
  }
  /* 顺时针旋转（阈值可按实测数据调）。 */
  if (gz > 60.0f) {
    return GESTURE_ROTATE_CW;
  }
  /* 逆时针旋转。 */
  if (gz < -60.0f) {
    return GESTURE_ROTATE_CCW;
  }
  /* 挥动：加速度突变较大。 */
  if (da > 1.2f) {
    return GESTURE_SWING;
  }
  return GESTURE_UNKNOWN;
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
