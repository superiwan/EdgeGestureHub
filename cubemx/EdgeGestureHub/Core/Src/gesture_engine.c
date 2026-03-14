#include "gesture_engine.h"
#include <math.h>

static imu_frame_t s_prev;

void Gesture_Init(void)
{
  s_prev.ax = 0.0f;
  s_prev.ay = 0.0f;
  s_prev.az = 0.0f;
}

gesture_t Gesture_Run(const imu_frame_t *frame)
{
  float da = fabsf(frame->ax - s_prev.ax) + fabsf(frame->ay - s_prev.ay) + fabsf(frame->az - s_prev.az);
  float gz = frame->gz;

  s_prev = *frame;

  if (da < 0.25f && fabsf(frame->gx) < 8.0f && fabsf(frame->gy) < 8.0f && fabsf(frame->gz) < 8.0f) {
    return GESTURE_STATIC;
  }
  if (gz > 60.0f) {
    return GESTURE_ROTATE_CW;
  }
  if (gz < -60.0f) {
    return GESTURE_ROTATE_CCW;
  }
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
