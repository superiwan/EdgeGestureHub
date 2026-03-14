#include "ssd1306.h"
#include "i2c.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define SSD1306_WIDTH          128U
#define SSD1306_HEIGHT          64U
#define SSD1306_PAGE_COUNT      (SSD1306_HEIGHT / 8U)
#define OLED_COL_OFFSET_SH1106   2U

#define OLED_DRIVER_UNIVERSAL    3

static uint16_t s_oled_addr = 0U;
static int s_oled_bus_index = 0;
static int s_oled_driver_type = 0;

static uint8_t s_oled_buf[SSD1306_WIDTH * SSD1306_PAGE_COUNT];

static int ssd1306_write_cmd(uint8_t cmd)
{
  uint8_t packet[2] = {0x00U, cmd};

  if (s_oled_addr == 0U) {
    return -1;
  }

  if (HAL_I2C_Master_Transmit(&hi2c2, s_oled_addr, packet, sizeof(packet), 100U) != HAL_OK) {
    return -1;
  }
  return 0;
}

static int ssd1306_write_data(const uint8_t *data, uint16_t len)
{
  uint8_t packet[1 + SSD1306_WIDTH];

  if (s_oled_addr == 0U || len > SSD1306_WIDTH) {
    return -1;
  }

  packet[0] = 0x40U;
  memcpy(&packet[1], data, len);

  if (HAL_I2C_Master_Transmit(&hi2c2, s_oled_addr, packet, (uint16_t)(len + 1U), 100U) != HAL_OK) {
    return -1;
  }
  return 0;
}

static void ssd1306_draw_pixel(int x, int y, int on)
{
  uint16_t index;
  uint8_t bit;

  if (x < 0 || y < 0 || x >= (int)SSD1306_WIDTH || y >= (int)SSD1306_HEIGHT) {
    return;
  }

  index = (uint16_t)x + ((uint16_t)y / 8U) * SSD1306_WIDTH;
  bit = (uint8_t)(1U << ((uint8_t)y & 0x07U));

  if (on) {
    s_oled_buf[index] |= bit;
  } else {
    s_oled_buf[index] &= (uint8_t)~bit;
  }
}

static const uint8_t *glyph5x7(char c)
{
  static const uint8_t g_colon[5] = {0x00, 0x36, 0x36, 0x00, 0x00};
  static const uint8_t g_under[5] = {0x40, 0x40, 0x40, 0x40, 0x40};
  static const uint8_t g_space[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

  static const uint8_t A[5] = {0x7E, 0x11, 0x11, 0x11, 0x7E};
  static const uint8_t C[5] = {0x3E, 0x41, 0x41, 0x41, 0x22};
  static const uint8_t E[5] = {0x7F, 0x49, 0x49, 0x49, 0x41};
  static const uint8_t G[5] = {0x3E, 0x41, 0x49, 0x49, 0x7A};
  static const uint8_t I[5] = {0x00, 0x41, 0x7F, 0x41, 0x00};
  static const uint8_t K[5] = {0x7F, 0x08, 0x14, 0x22, 0x41};
  static const uint8_t N[5] = {0x7F, 0x02, 0x04, 0x08, 0x7F};
  static const uint8_t O[5] = {0x3E, 0x41, 0x41, 0x41, 0x3E};
  static const uint8_t R[5] = {0x7F, 0x09, 0x19, 0x29, 0x46};
  static const uint8_t S[5] = {0x46, 0x49, 0x49, 0x49, 0x31};
  static const uint8_t T[5] = {0x01, 0x01, 0x7F, 0x01, 0x01};
  static const uint8_t U[5] = {0x3F, 0x40, 0x40, 0x40, 0x3F};
  static const uint8_t W[5] = {0x7F, 0x20, 0x18, 0x20, 0x7F};

  static const uint8_t e[5] = {0x38, 0x54, 0x54, 0x54, 0x18};
  static const uint8_t g[5] = {0x18, 0xA4, 0xA4, 0xA4, 0x7C};
  static const uint8_t r[5] = {0x7C, 0x08, 0x04, 0x04, 0x08};
  static const uint8_t s[5] = {0x48, 0x54, 0x54, 0x54, 0x24};
  static const uint8_t t[5] = {0x04, 0x3F, 0x44, 0x40, 0x20};
  static const uint8_t u[5] = {0x3C, 0x40, 0x40, 0x20, 0x7C};

  switch (c) {
    case 'A': return A;
    case 'C': return C;
    case 'E': return E;
    case 'G': return G;
    case 'I': return I;
    case 'K': return K;
    case 'N': return N;
    case 'O': return O;
    case 'R': return R;
    case 'S': return S;
    case 'T': return T;
    case 'U': return U;
    case 'W': return W;
    case 'e': return e;
    case 'g': return g;
    case 'r': return r;
    case 's': return s;
    case 't': return t;
    case 'u': return u;
    case ':': return g_colon;
    case '_': return g_under;
    case ' ': return g_space;
    default:  return g_space;
  }
}

static void ssd1306_draw_char(int x, int y, char c)
{
  const uint8_t *glyph = glyph5x7(c);
  int col;
  int row;

  for (col = 0; col < 5; ++col) {
    uint8_t bits = glyph[col];
    for (row = 0; row < 8; ++row) {
      ssd1306_draw_pixel(x + col, y + row, (bits >> row) & 0x01U);
    }
  }

  for (row = 0; row < 8; ++row) {
    ssd1306_draw_pixel(x + 5, y + row, 0);
  }
}

static int ssd1306_pick_bus_and_addr(void)
{
  uint16_t cand[] = { (0x3CU << 1), (0x3DU << 1) };
  uint32_t i;

  for (i = 0; i < (sizeof(cand) / sizeof(cand[0])); ++i) {
    if (HAL_I2C_IsDeviceReady(&hi2c2, cand[i], 2U, 50U) == HAL_OK) {
      s_oled_addr = cand[i];
      s_oled_bus_index = 2;
      return 0;
    }
  }

  s_oled_addr = 0U;
  s_oled_bus_index = 0;
  return -1;
}

int SSD1306_Init(void)
{
  const uint8_t init_seq[] = {
    0xAE,
    0xD5, 0x80,
    0xA8, 0x3F,
    0xD3, 0x00,
    0x40,
    0xA1,
    0xC8,
    0xDA, 0x12,
    0x81, 0x7F,
    0xA4,
    0xA6,
    0xD9, 0xF1,
    0xDB, 0x40,
    0x8D, 0x14,
    0xAD, 0x8B,
    0xAF
  };
  uint32_t i;

  HAL_Delay(100U);

  if (ssd1306_pick_bus_and_addr() != 0) {
    s_oled_driver_type = 0;
    return -1;
  }

  for (i = 0; i < (uint32_t)sizeof(init_seq); ++i) {
    if (ssd1306_write_cmd(init_seq[i]) != 0) {
      s_oled_driver_type = 0;
      return -1;
    }
  }

  s_oled_driver_type = OLED_DRIVER_UNIVERSAL;
  SSD1306_Clear();
  SSD1306_Update();
  return 0;
}

void SSD1306_Clear(void)
{
  memset(s_oled_buf, 0, sizeof(s_oled_buf));
}

void SSD1306_DrawString(int x, int y, const char *str)
{
  int cursor = x;

  if (str == NULL) {
    return;
  }

  while (*str != '\0') {
    if (cursor > (int)SSD1306_WIDTH - 6) {
      break;
    }
    ssd1306_draw_char(cursor, y, *str);
    cursor += 6;
    ++str;
  }
}

void SSD1306_Update(void)
{
  uint8_t page;

  for (page = 0U; page < SSD1306_PAGE_COUNT; ++page) {
    uint16_t offset = (uint16_t)page * SSD1306_WIDTH;
    uint8_t col = OLED_COL_OFFSET_SH1106;

    if (ssd1306_write_cmd((uint8_t)(0xB0U + page)) != 0) {
      return;
    }
    if (ssd1306_write_cmd((uint8_t)(0x00U + (col & 0x0FU))) != 0) {
      return;
    }
    if (ssd1306_write_cmd((uint8_t)(0x10U + ((col >> 4) & 0x0FU))) != 0) {
      return;
    }
    if (ssd1306_write_data(&s_oled_buf[offset], SSD1306_WIDTH) != 0) {
      return;
    }
  }
}

int SSD1306_GetDebugInfo(int *bus_index, uint8_t *addr7, int *driver_type)
{
  if (bus_index != NULL) {
    *bus_index = s_oled_bus_index;
  }
  if (addr7 != NULL) {
    *addr7 = (uint8_t)(s_oled_addr >> 1);
  }
  if (driver_type != NULL) {
    *driver_type = s_oled_driver_type;
  }
  return (s_oled_addr != 0U) ? 0 : -1;
}
