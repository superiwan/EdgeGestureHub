#include "app_task.h"
#include "app_config.h"
#include "gesture_engine.h"
#include "firewater.h"
#include "control_logic.h"
#include "imu_service.h"
#include "oled_service.h"
#include "usart.h"
#include "stm32f4xx_hal.h"

static imu_frame_t s_cache;
static gesture_t s_last = GESTURE_UNKNOWN;

void App_Init(void)
{
  Gesture_Init();
  (void)IMU_Service_Init();
  (void)OLED_Service_Init();
  ControlLogic_Init();
}

void App_Loop(void)
{
  static uint32_t t_sample = 0U;
  static uint32_t t_uart = 0U;
  static uint32_t t_oled = 0U;
  uint32_t now = HAL_GetTick();

  if ((now - t_sample) >= APP_SAMPLE_PERIOD_MS) {
    t_sample = now;
    if (IMU_ReadFrame(&s_cache) == 0) {
      s_last = Gesture_Run(&s_cache);
      ControlLogic_OnGesture(s_last);
    }
  }

  if ((now - t_uart) >= APP_UART_PERIOD_MS) {
    t_uart = now;
    FireWater_SendFrame(&huart1,
                        s_cache.ax, s_cache.ay, s_cache.az,
                        s_cache.gx, s_cache.gy, s_cache.gz,
                        (int)s_last);
  }

  if ((now - t_oled) >= APP_OLED_PERIOD_MS) {
    t_oled = now;
    OLED_ShowGesture(Gesture_ToString(s_last));
  }
}
