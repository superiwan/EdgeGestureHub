#include "control_logic.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"

/* 触发冷却（毫秒）：防止重复触发。 */
#define CTRL_TOGGLE_COOLDOWN_MS  500U

/* 冷却时间戳。 */
static uint32_t s_cooldown_until = 0U;

/* 上一拍稳定手势：用于做“异常边沿”检测。 */
static gesture_t s_prev_gesture = GESTURE_UNKNOWN;

/* 触发使能：只有先回到 STATIC（正常）后，才允许下一次异常触发。 */
static uint8_t s_ready_for_anom = 0U;

void ControlLogic_Init(void)
{
  s_cooldown_until = 0U;
  s_prev_gesture = GESTURE_UNKNOWN;
  s_ready_for_anom = 0U;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
}

void ControlLogic_OnGesture(gesture_t g)
{
  uint32_t now = HAL_GetTick();
  uint8_t anomaly_edge = 0U;

  /* 一旦回到 STATIC，重新允许下一次异常触发。 */
  if (g == GESTURE_STATIC) {
    s_ready_for_anom = 1U;
  }

  /* 异常边沿：从非异常进入异常时触发一次。 */
  if (g == GESTURE_SWING && s_prev_gesture != GESTURE_SWING) {
    anomaly_edge = 1U;
  }
  s_prev_gesture = g;

  if (!anomaly_edge) {
    return;
  }

  /* 没有先回到正常状态，不允许再次触发。 */
  if (!s_ready_for_anom) {
    return;
  }

  if (now < s_cooldown_until) {
    return;
  }

  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
  s_cooldown_until = now + CTRL_TOGGLE_COOLDOWN_MS;

  /* 触发后先锁住，必须等回到 STATIC 才能再触发。 */
  s_ready_for_anom = 0U;
}
