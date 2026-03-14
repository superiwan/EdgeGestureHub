#include "app_task.h"
#include "app_config.h"
#include "gesture_engine.h"
#include "firewater.h"
#include "control_logic.h"
#include "imu_service.h"
#include "oled_service.h"
#include "usart.h"
#include "stm32f4xx_hal.h"

/* 最近一次采样缓存：UART 和 OLED 都从这里取数据。 */
static imu_frame_t s_cache;
/* 最近一次手势分类结果。 */
static gesture_t s_last = GESTURE_UNKNOWN;

void App_Init(void)
{
  /* 1) 初始化阈值手势引擎 */
  Gesture_Init();
  /* 2) 初始化 IMU 设备（MPU6050） */
  (void)IMU_Service_Init();
  /* 3) 初始化 OLED 设备 */
  (void)OLED_Service_Init();
  /* 4) 初始化本地控制逻辑（继电器/PWM 等可放这里） */
  ControlLogic_Init();
}

void App_Loop(void)
{
  /* 三个软定时器：采样、串口上报、OLED 刷新。 */
  static uint32_t t_sample = 0U;
  static uint32_t t_uart = 0U;
  static uint32_t t_oled = 0U;
  uint32_t now = HAL_GetTick();

  /* A. 固定周期采样 IMU，并做手势分类。 */
  if ((now - t_sample) >= APP_SAMPLE_PERIOD_MS) {
    t_sample = now;
    if (IMU_ReadFrame(&s_cache) == 0) {
      /*
       * 当前是阈值法：
       *   这里调用 Gesture_Run() 返回手势类别。
       * 未来接入 NanoEdge AI 时：
       *   可在此处改成 AI 推理接口，输出同样的手势枚举。
       */
      s_last = Gesture_Run(&s_cache);
      ControlLogic_OnGesture(s_last);
    }
  }

  /*
   * B. 固定周期通过 UART 发一帧 FireWater 文本。
   * VOFA+ 可直接按字段名 ax/ay/az/gx/gy/gz/cls 建通道看波形。
   */
  if ((now - t_uart) >= APP_UART_PERIOD_MS) {
    t_uart = now;
    FireWater_SendFrame(&huart1,
                        s_cache.ax, s_cache.ay, s_cache.az,
                        s_cache.gx, s_cache.gy, s_cache.gz,
                        (int)s_last);
  }

  /* C. 固定周期刷新 OLED，显示当前手势文本。 */
  if ((now - t_oled) >= APP_OLED_PERIOD_MS) {
    t_oled = now;
    OLED_ShowGesture(Gesture_ToString(s_last));
  }
}
