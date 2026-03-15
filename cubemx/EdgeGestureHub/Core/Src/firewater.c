#include "firewater.h"
#include <stdio.h>
#include <string.h>

/* FireWater 输出函数：
 * 将 IMU 六轴 + 分类编号按 CSV 一行发送。 */
void FireWater_SendFrame(UART_HandleTypeDef *huart,
                         float ax, float ay, float az,
                         float gx, float gy, float gz,
                         int cls)
{
  char line[160];

  /*
   * VOFA+ FireWater 推荐格式：纯 CSV（不带 key）。
   * 通道映射：
   *   I0=ax, I1=ay, I2=az, I3=gx, I4=gy, I5=gz, I6=cls
   */
  int n = snprintf(line, sizeof(line),
                   "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%d\n",
                   ax, ay, az, gx, gy, gz, cls);

  if (n > 0) {
    /* 阻塞发送：50ms 超时。若后续吞吐变大可改 DMA/中断发送。 */
    HAL_UART_Transmit(huart, (uint8_t *)line, (uint16_t)strlen(line), 50);
  }
}
