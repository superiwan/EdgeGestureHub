#include "oled_service.h"
#include "ssd1306.h"

int OLED_Service_Init(void)
{
  return SSD1306_Init();
}

void OLED_ShowGesture(const char *label)
{
  SSD1306_Clear();
  SSD1306_DrawString(0, 0, "Gesture:");
  SSD1306_DrawString(0, 16, label);
  SSD1306_Update();
}
