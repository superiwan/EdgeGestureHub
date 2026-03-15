#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

/* 初始化 OLED，自动探测地址并下发配置序列。 */
int SSD1306_Init(void);

/* 清空显存缓冲区（尚未发送到屏幕）。 */
void SSD1306_Clear(void);

/* 在显存中绘制字符串。 */
void SSD1306_DrawString(int x, int y, const char *str);

/* 将显存缓冲区刷新到 OLED 屏幕。 */
void SSD1306_Update(void);

/* 调试信息：用于串口打印当前 OLED 绑定状态。 */
int SSD1306_GetDebugInfo(int *bus_index, uint8_t *addr7, int *driver_type);

#endif
