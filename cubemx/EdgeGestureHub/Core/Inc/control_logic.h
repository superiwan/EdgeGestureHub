#ifndef CONTROL_LOGIC_H
#define CONTROL_LOGIC_H

#include "gesture_engine.h"

/* 控制逻辑初始化：
 * 用于初始化蜂鸣器、继电器、PWM 等本地执行器。 */
void ControlLogic_Init(void);

/* 手势事件入口：
 * 输入识别结果，在本地执行对应控制动作。 */
void ControlLogic_OnGesture(gesture_t g);

#endif
