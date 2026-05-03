#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "main.h"

typedef struct
{
  volatile uint16_t LCD_REG;
  volatile uint16_t LCD_RAM;
} LCD_TypeDef;

typedef struct
{
  uint16_t width;
  uint16_t height;
  uint16_t id;
  uint8_t dir;
  uint16_t wramcmd;
  uint16_t setxcmd;
  uint16_t setycmd;
} lcd_dev_t;

#define LCD_FSMC_NEX 4U
#define LCD_FSMC_AX 10U

#define LCD_BASE                                                                                   \
  ((uint32_t)((0x60000000U + (0x04000000U * (LCD_FSMC_NEX - 1U))) | (((1UL << LCD_FSMC_AX) * 2UL) - 2UL)))
#define LCD ((LCD_TypeDef *)LCD_BASE)

#define WHITE 0xFFFFU
#define BLACK 0x0000U

/* SSD1963 parameters kept for compatibility with vendor lcd_ex.c */
#define SSD_HOR_RESOLUTION 800U
#define SSD_VER_RESOLUTION 480U
#define SSD_HOR_PULSE_WIDTH 1U
#define SSD_HOR_BACK_PORCH 46U
#define SSD_HOR_FRONT_PORCH 210U
#define SSD_VER_PULSE_WIDTH 1U
#define SSD_VER_BACK_PORCH 23U
#define SSD_VER_FRONT_PORCH 22U
#define SSD_HT (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_BACK_PORCH)
#define SSD_VT (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

#define LCD_BL(x)                                                                                  \
  do                                                                                                \
  {                                                                                                 \
    HAL_GPIO_WritePin(LCD_BL_GPIO_PIN_GPIO_Port, LCD_BL_GPIO_PIN_Pin,                              \
                      ((x) ? GPIO_PIN_SET : GPIO_PIN_RESET));                                       \
  } while (0)

extern lcd_dev_t lcddev;

void lcd_wr_data(volatile uint16_t data);
void lcd_wr_regno(volatile uint16_t regno);
void lcd_write_reg(uint16_t regno, uint16_t data);
void lcd_write_ram_prepare(void);
void lcd_set_cursor(uint16_t x, uint16_t y);
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);
void lcd_display_dir(uint8_t dir);
void lcd_ssd_backlight_set(uint8_t pwm);
void lcd_clear(uint16_t color);
void lcd_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */
