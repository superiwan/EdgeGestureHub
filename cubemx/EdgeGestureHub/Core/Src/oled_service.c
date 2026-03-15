#include "oled_service.h"
#include "ssd1306.h"

/* OLED 服务初始化：当前绑定 SSD1306/SH1106 兼容驱动。 */
int OLED_Service_Init(void)
{
  /* OLED 底层初始化由 ssd1306.c 提供。 */
  return SSD1306_Init();
}

/* OLED 业务展示：固定显示状态标题 + 当前三态标签。 */
void OLED_ShowGesture(const char *label)
{
  SSD1306_Clear();
  SSD1306_DrawString(0, 0, "STATUS:");
  SSD1306_DrawString(0, 16, label);
  SSD1306_Update();
}
