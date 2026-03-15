#include "app_task.h"
#include "app_config.h"
#include "gesture_engine.h"
#include "firewater.h"
#include "control_logic.h"
#include "imu_service.h"
#include "oled_service.h"
#include "usart.h"
#include "stm32f4xx_hal.h"

/* 最近一帧 IMU 数据缓存：供串口和 OLED 在不同周期复用。 */
static imu_frame_t s_cache;

/* 最近一次稳定手势结果缓存。 */
static gesture_t s_last = GESTURE_UNKNOWN;

/* 应用初始化顺序：算法 -> 传感器 -> 显示 -> 控制执行层。 */
void App_Init(void)
{
  Gesture_Init();
  (void)IMU_Service_Init();
  (void)OLED_Service_Init();
  ControlLogic_Init();
}

/* 应用主循环（时间片调度）：
 * 1) 按采样周期读取 IMU 并分类；
 * 2) 按串口周期输出遥测；
 * 3) 按 OLED 周期更新显示。 */
void App_Loop(void)
{
  static uint32_t t_sample = 0U;
  static uint32_t t_uart = 0U;
  static uint32_t t_oled = 0U;
  uint32_t now = HAL_GetTick();

  /* 采样任务：读取 IMU 并更新手势状态。 */
  if ((now - t_sample) >= APP_SAMPLE_PERIOD_MS) {
    t_sample = now;
    if (IMU_ReadFrame(&s_cache) == 0) {
      s_last = Gesture_Run(&s_cache);
      ControlLogic_OnGesture(s_last);
    }
  }

  /* 遥测任务：给上位机/无线链路发送 CSV 数据。 */
  if ((now - t_uart) >= APP_UART_PERIOD_MS) {
    int cls = Gesture_GetClassCode();

    t_uart = now;
    /* 同时输出到 USB 串口(USART1) 与无线串口(USART2)。
     * cls 三态编码：0=学习中，1=正常，2=异常。
     */
    FireWater_SendFrame(&huart1,
                        s_cache.ax, s_cache.ay, s_cache.az,
                        s_cache.gx, s_cache.gy, s_cache.gz,
                        cls);
    FireWater_SendFrame(&huart2,
                        s_cache.ax, s_cache.ay, s_cache.az,
                        s_cache.gx, s_cache.gy, s_cache.gz,
                        cls);
  }

  /* 显示任务：刷新当前三态标签。 */
  if ((now - t_oled) >= APP_OLED_PERIOD_MS) {
    t_oled = now;
    OLED_ShowGesture(Gesture_GetClassLabel());
  }
}
