#ifndef APP_TASK_H
#define APP_TASK_H

/* 应用层初始化：
 * 负责初始化手势引擎、IMU 服务、OLED 服务与控制逻辑。 */
void App_Init(void);

/* 应用层主循环：
 * 按周期执行采样、分类、上报和显示。 */
void App_Loop(void);

#endif
