#ifndef GESTURE_ENGINE_H
#define GESTURE_ENGINE_H

typedef enum {
  GESTURE_UNKNOWN = 0,
  GESTURE_STATIC,
  GESTURE_SWING,
  GESTURE_ROTATE_CW,
  GESTURE_ROTATE_CCW
} gesture_t;

typedef struct {
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
  float temp;
} imu_frame_t;

void Gesture_Init(void);
gesture_t Gesture_Run(const imu_frame_t *frame);
const char *Gesture_ToString(gesture_t g);

#endif
