#include "imu_service.h"
#include "mpu6050.h"

int IMU_Service_Init(void)
{
  return MPU6050_Init();
}

int IMU_ReadFrame(imu_frame_t *out)
{
  return MPU6050_ReadFrame(out);
}
