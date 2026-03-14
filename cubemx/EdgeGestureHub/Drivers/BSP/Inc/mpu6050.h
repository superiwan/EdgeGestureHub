#ifndef MPU6050_H
#define MPU6050_H

#include "gesture_engine.h"

int MPU6050_Init(void);
int MPU6050_ReadFrame(imu_frame_t *out);

#endif
