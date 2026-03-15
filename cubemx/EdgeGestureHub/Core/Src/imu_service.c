#include "imu_service.h"
#include "mpu6050.h"

/* IMU 服务初始化：统一入口，当前绑定 MPU6050。 */
int IMU_Service_Init(void)
{
  /* 服务层仅做转发：底层初始化由 mpu6050.c 完成。 */
  return MPU6050_Init();
}

/* IMU 读取：输出标准化 imu_frame_t，供算法层直接使用。 */
int IMU_ReadFrame(imu_frame_t *out)
{
  /* 对外统一 IMU 读取接口，便于后续替换传感器。 */
  return MPU6050_ReadFrame(out);
}
