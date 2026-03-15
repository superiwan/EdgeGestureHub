#ifndef IMU_SERVICE_H
#define IMU_SERVICE_H

#include "gesture_engine.h"

/* IMU 服务初始化：
 * 对外隐藏底层具体传感器初始化细节。 */
int IMU_Service_Init(void);

/* 读取一帧标准化 IMU 数据。 */
int IMU_ReadFrame(imu_frame_t *out);

#endif
