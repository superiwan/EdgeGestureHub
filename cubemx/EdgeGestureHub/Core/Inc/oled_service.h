#ifndef OLED_SERVICE_H
#define OLED_SERVICE_H

/* OLED 服务初始化。 */
int OLED_Service_Init(void);

/* 在 OLED 上显示当前手势标签文本。 */
void OLED_ShowGesture(const char *label);

#endif
