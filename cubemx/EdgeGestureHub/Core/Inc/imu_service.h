#ifndef IMU_SERVICE_H
#define IMU_SERVICE_H

#include "gesture_engine.h"

int IMU_Service_Init(void);
int IMU_ReadFrame(imu_frame_t *out);

#endif
