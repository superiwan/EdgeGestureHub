#include "oled_service.h"
#include "ssd1306.h"

int OLED_Service_Init(void)
{
  /* OLED 底层初始化由 ssd1306.c 提供。 */
  return SSD1306_Init();
}

void OLED_ShowGesture(const char *label)
{
  /* 简单两行显示：标题 + 当前手势文本。 */
  SSD1306_Clear();
  SSD1306_DrawString(0, 0, "Gesture:");
  SSD1306_DrawString(0, 16, label);
  SSD1306_Update();
}
