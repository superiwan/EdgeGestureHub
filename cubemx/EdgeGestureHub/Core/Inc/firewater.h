#ifndef FIREWATER_H
#define FIREWATER_H

#include "stm32f4xx_hal.h"

void FireWater_SendFrame(UART_HandleTypeDef *huart,
                         float ax, float ay, float az,
                         float gx, float gy, float gz,
                         int cls);

#endif
