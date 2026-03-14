#include "mpu6050.h"
#include "i2c.h"

int MPU6050_Init(void)
{
  /* TODO: Add real MPU6050 register init here. */
  return 0;
}

int MPU6050_ReadFrame(imu_frame_t *out)
{
  /* TODO: Replace with real I2C read + scale conversion. */
  (void)hi2c1;
  out->ax = 0.0f;
  out->ay = 0.0f;
  out->az = 9.8f;
  out->gx = 0.0f;
  out->gy = 0.0f;
  out->gz = 0.0f;
  out->temp = 25.0f;
  return 0;
}
