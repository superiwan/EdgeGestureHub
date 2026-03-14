#ifndef SSD1306_H
#define SSD1306_H

int SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_DrawString(int x, int y, const char *str);
void SSD1306_Update(void);

#endif
