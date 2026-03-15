#ifndef MPU6050_H
#define MPU6050_H

#include "gesture_engine.h"

/* 初始化 MPU6050 量程、滤波和采样配置。 */
int MPU6050_Init(void);

/* 读取一帧传感器数据并换算成工程统一单位。 */
int MPU6050_ReadFrame(imu_frame_t *out);

#endif
