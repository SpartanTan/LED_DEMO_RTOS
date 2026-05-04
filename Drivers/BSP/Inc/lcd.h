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

#define L2R_U2D 0U
#define L2R_D2U 1U
#define R2L_U2D 2U
#define R2L_D2U 3U
#define U2D_L2R 4U
#define U2D_R2L 5U
#define D2U_L2R 6U
#define D2U_R2L 7U
#define DFT_SCAN_DIR L2R_U2D

/* 常用画笔颜色 */
#define WHITE           0xFFFF      /* 白色 */
#define BLACK           0x0000      /* 黑色 */
#define RED             0xF800      /* 红色 */
#define GREEN           0x07E0      /* 绿色 */
#define BLUE            0x001F      /* 蓝色 */ 
#define MAGENTA         0XF81F      /* 品红色/紫红色 = BLUE + RED */
#define YELLOW          0XFFE0      /* 黄色 = GREEN + RED */
#define CYAN            0X07FF      /* 青色 = GREEN + BLUE */  

/* 非常用颜色 */
#define BROWN           0XBC40      /* 棕色 */
#define BRRED           0XFC07      /* 棕红色 */
#define GRAY            0X8430      /* 灰色 */ 
#define DARKBLUE        0X01CF      /* 深蓝色 */
#define LIGHTBLUE       0X7D7C      /* 浅蓝色 */ 
#define GRAYBLUE        0X5458      /* 灰蓝色 */ 
#define LIGHTGREEN      0X841F      /* 浅绿色 */  
#define LGRAY           0XC618      /* 浅灰色(PANNEL),窗体背景色 */ 
#define LGRAYBLUE       0XA651      /* 浅灰蓝色(中间层颜色) */ 
#define LBBLUE          0X2B12      /* 浅棕蓝色(选择条目的反色) */ 

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
extern uint32_t g_point_color;
extern uint32_t g_back_color;

void lcd_wr_data(volatile uint16_t data);
void lcd_wr_regno(volatile uint16_t regno);
void lcd_write_reg(uint16_t regno, uint16_t data);
void lcd_write_ram_prepare(void);
void lcd_display_on(void);
void lcd_display_off(void);
void lcd_set_cursor(uint16_t x, uint16_t y);
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);
void lcd_scan_dir(uint8_t dir);
void lcd_display_dir(uint8_t dir);
void lcd_ssd_backlight_set(uint8_t pwm);
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);
void lcd_clear(uint16_t color);
void lcd_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */
