#include "firewater.h"
#include <stdio.h>
#include <string.h>

void FireWater_SendFrame(UART_HandleTypeDef *huart,
                         float ax, float ay, float az,
                         float gx, float gy, float gz,
                         int cls)
{
  char line[160];

  /*
   * FireWater 文本协议（一行一帧）：
   *  ax:...,ay:...,az:...,gx:...,gy:...,gz:...,cls:...\n
   * VOFA+ 中可按字段名直接建通道观察。
   */
  int n = snprintf(line, sizeof(line),
                   "ax:%.3f,ay:%.3f,az:%.3f,gx:%.3f,gy:%.3f,gz:%.3f,cls:%d\n",
                   ax, ay, az, gx, gy, gz, cls);

  if (n > 0) {
    HAL_UART_Transmit(huart, (uint8_t *)line, (uint16_t)strlen(line), 50);
  }
}
