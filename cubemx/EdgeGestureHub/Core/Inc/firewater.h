#ifndef FIREWATER_H
#define FIREWATER_H

#include "stm32f4xx_hal.h"

/*
 * 发送一帧 FireWater 文本数据（用于 VOFA+ 实时波形显示）。
 * cls 为手势类别编号（对应 gesture_t 枚举值）。
 */
void FireWater_SendFrame(UART_HandleTypeDef *huart,
                         float ax, float ay, float az,
                         float gx, float gy, float gz,
                         int cls);

#endif
