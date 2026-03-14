#ifndef GESTURE_ENGINE_H
#define GESTURE_ENGINE_H

/* 手势分类结果。 */
typedef enum {
  GESTURE_UNKNOWN = 0,
  GESTURE_STATIC,
  GESTURE_SWING,
  GESTURE_ROTATE_CW,
  GESTURE_ROTATE_CCW
} gesture_t;

/* 一帧 IMU 数据：加速度(m/s^2) + 角速度(deg/s) + 温度。 */
typedef struct {
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
  float temp;
} imu_frame_t;

/* 初始化手势引擎内部状态。 */
void Gesture_Init(void);
/* 输入一帧 IMU 数据，输出当前手势类别。 */
gesture_t Gesture_Run(const imu_frame_t *frame);
/* 将手势枚举转成可显示字符串。 */
const char *Gesture_ToString(gesture_t g);

#endif
