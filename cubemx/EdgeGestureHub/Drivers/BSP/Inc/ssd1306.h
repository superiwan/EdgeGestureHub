#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

int SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_DrawString(int x, int y, const char *str);
void SSD1306_Update(void);

/* 调试信息：用于串口打印当前 OLED 绑定状态。 */
int SSD1306_GetDebugInfo(int *bus_index, uint8_t *addr7, int *driver_type);

#endif
