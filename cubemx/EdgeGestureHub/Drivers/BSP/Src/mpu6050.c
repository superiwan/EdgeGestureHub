#include "mpu6050.h"
#include "i2c.h"

#include <stddef.h>
#include <stdint.h>

/* MPU6050 7-bit 地址 0x68，对应 HAL 8-bit 传输地址 0xD0。 */
#define MPU6050_I2C_ADDR      (0x68U << 1)

/* 关键寄存器地址。 */
#define MPU6050_REG_PWR_MGMT1 0x6B
#define MPU6050_REG_SMPLRT    0x19
#define MPU6050_REG_CONFIG    0x1A
#define MPU6050_REG_GYRO_CFG  0x1B
#define MPU6050_REG_ACCEL_CFG 0x1C
#define MPU6050_REG_DATA      0x3B
#define MPU6050_REG_WHO_AM_I  0x75

/* 量程换算系数：当前配置为 Accel ±2g，Gyro ±250 dps。 */
#define MPU6050_ACCEL_LSB_PER_G   16384.0f
#define MPU6050_GYRO_LSB_PER_DPS    131.0f
#define MPU6050_GRAVITY_MS2          9.80665f

static int mpu6050_write_reg(uint8_t reg, uint8_t val)
{
  if (HAL_I2C_Mem_Write(&hi2c1,
                        MPU6050_I2C_ADDR,
                        reg,
                        I2C_MEMADD_SIZE_8BIT,
                        &val,
                        1U,
                        100U) != HAL_OK) {
    return -1;
  }
  return 0;
}

static int mpu6050_read_regs(uint8_t reg, uint8_t *buf, uint16_t len)
{
  if (HAL_I2C_Mem_Read(&hi2c1,
                       MPU6050_I2C_ADDR,
                       reg,
                       I2C_MEMADD_SIZE_8BIT,
                       buf,
                       len,
                       100U) != HAL_OK) {
    return -1;
  }
  return 0;
}

static int16_t be16_to_i16(const uint8_t *p)
{
  return (int16_t)((int16_t)(p[0] << 8) | p[1]);
}

int MPU6050_Init(void)
{
  uint8_t whoami = 0U;

  /* 先读 WHO_AM_I 做在线检查（0x68 为常见值）。 */
  if (mpu6050_read_regs(MPU6050_REG_WHO_AM_I, &whoami, 1U) != 0) {
    return -1;
  }

  /* 退出睡眠模式，使用内部时钟。 */
  if (mpu6050_write_reg(MPU6050_REG_PWR_MGMT1, 0x00U) != 0) {
    return -1;
  }
  HAL_Delay(10U);

  /* 采样率 = GyroOutputRate/(1+SMPLRT_DIV)；这里约 1kHz。 */
  if (mpu6050_write_reg(MPU6050_REG_SMPLRT, 0x07U) != 0) {
    return -1;
  }

  /* DLPF 配置为 0x03，兼顾噪声与响应。 */
  if (mpu6050_write_reg(MPU6050_REG_CONFIG, 0x03U) != 0) {
    return -1;
  }

  /* Gyro 量程设为 ±250 dps。 */
  if (mpu6050_write_reg(MPU6050_REG_GYRO_CFG, 0x00U) != 0) {
    return -1;
  }

  /* Accel 量程设为 ±2g。 */
  if (mpu6050_write_reg(MPU6050_REG_ACCEL_CFG, 0x00U) != 0) {
    return -1;
  }

  return 0;
}

int MPU6050_ReadFrame(imu_frame_t *out)
{
  uint8_t raw[14];
  int16_t ax_raw;
  int16_t ay_raw;
  int16_t az_raw;
  int16_t temp_raw;
  int16_t gx_raw;
  int16_t gy_raw;
  int16_t gz_raw;

  if (out == NULL) {
    return -1;
  }

  /* 从 ACCEL_XOUT_H 开始连续读取 14 字节：Accel(6)+Temp(2)+Gyro(6)。 */
  if (mpu6050_read_regs(MPU6050_REG_DATA, raw, sizeof(raw)) != 0) {
    return -1;
  }

  ax_raw = be16_to_i16(&raw[0]);
  ay_raw = be16_to_i16(&raw[2]);
  az_raw = be16_to_i16(&raw[4]);
  temp_raw = be16_to_i16(&raw[6]);
  gx_raw = be16_to_i16(&raw[8]);
  gy_raw = be16_to_i16(&raw[10]);
  gz_raw = be16_to_i16(&raw[12]);

  /* 输出为工程中更直观的单位：m/s^2、deg/s、摄氏度。 */
  out->ax = ((float)ax_raw / MPU6050_ACCEL_LSB_PER_G) * MPU6050_GRAVITY_MS2;
  out->ay = ((float)ay_raw / MPU6050_ACCEL_LSB_PER_G) * MPU6050_GRAVITY_MS2;
  out->az = ((float)az_raw / MPU6050_ACCEL_LSB_PER_G) * MPU6050_GRAVITY_MS2;

  out->gx = (float)gx_raw / MPU6050_GYRO_LSB_PER_DPS;
  out->gy = (float)gy_raw / MPU6050_GYRO_LSB_PER_DPS;
  out->gz = (float)gz_raw / MPU6050_GYRO_LSB_PER_DPS;

  out->temp = ((float)temp_raw / 340.0f) + 36.53f;

  return 0;
}
