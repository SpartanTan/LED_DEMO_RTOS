#include "lcd.h"

#include <stdio.h>

#include "delay.h"
#include "lcdfont.h"

lcd_dev_t lcddev = {0};
uint32_t g_point_color = RED;
uint32_t g_back_color = WHITE;

static void delay_us(uint32_t us)
{
  while (us-- != 0U)
  {
    delay_busy_cycles(SystemCoreClock / 3000000U);
  }
}

#define delay_ms delay_busy_ms

/* Pull in vendor IC register init sequences used by lcd_init(). */
#include "lcd_ex.c"

void lcd_wr_data(volatile uint16_t data)
{
  data = data;
  LCD->LCD_RAM = data;
}

void lcd_wr_regno(volatile uint16_t regno)
{
  regno = regno;
  LCD->LCD_REG = regno;
}

void lcd_write_reg(uint16_t regno, uint16_t data)
{
  LCD->LCD_REG = regno;
  LCD->LCD_RAM = data;
}

static uint16_t lcd_rd_data(void)
{
  volatile uint16_t ram;
  ram = LCD->LCD_RAM;
  return ram;
}

void lcd_write_ram_prepare(void)
{
  LCD->LCD_REG = lcddev.wramcmd;
}

void lcd_display_on(void)
{
  lcd_wr_regno((lcddev.id == 0x5510U) ? 0x2900U : 0x29U);
}

void lcd_display_off(void)
{
  lcd_wr_regno((lcddev.id == 0x5510U) ? 0x2800U : 0x28U);
}

void lcd_set_cursor(uint16_t x, uint16_t y)
{
  if (lcddev.id == 0x5510U)
  {
    lcd_wr_regno(lcddev.setxcmd);
    lcd_wr_data((uint16_t)(x >> 8));
    lcd_wr_regno((uint16_t)(lcddev.setxcmd + 1U));
    lcd_wr_data((uint16_t)(x & 0xFF));

    lcd_wr_regno(lcddev.setycmd);
    lcd_wr_data((uint16_t)(y >> 8));
    lcd_wr_regno((uint16_t)(lcddev.setycmd + 1U));
    lcd_wr_data((uint16_t)(y & 0xFF));
  }
  else
  {
    lcd_wr_regno(lcddev.setxcmd);
    lcd_wr_data((uint16_t)(x >> 8));
    lcd_wr_data((uint16_t)(x & 0xFF));

    lcd_wr_regno(lcddev.setycmd);
    lcd_wr_data((uint16_t)(y >> 8));
    lcd_wr_data((uint16_t)(y & 0xFF));
  }
}

void lcd_scan_dir(uint8_t dir)
{
  uint16_t regval = 0U;
  uint16_t temp;

  if ((lcddev.dir == 1U && lcddev.id != 0x1963U) || (lcddev.dir == 0U && lcddev.id == 0x1963U))
  {
    switch (dir)
    {
      case L2R_U2D:
        dir = D2U_L2R;
        break;

      case L2R_D2U:
        dir = D2U_R2L;
        break;

      case R2L_U2D:
        dir = U2D_L2R;
        break;

      case R2L_D2U:
        dir = U2D_R2L;
        break;

      case U2D_L2R:
        dir = L2R_D2U;
        break;

      case U2D_R2L:
        dir = L2R_U2D;
        break;

      case D2U_L2R:
        dir = R2L_D2U;
        break;

      case D2U_R2L:
        dir = R2L_U2D;
        break;

      default:
        dir = DFT_SCAN_DIR;
        break;
    }
  }

  switch (dir)
  {
    case L2R_U2D:
      regval = 0U;
      break;

    case L2R_D2U:
      regval = (1U << 7);
      break;

    case R2L_U2D:
      regval = (1U << 6);
      break;

    case R2L_D2U:
      regval = (1U << 7) | (1U << 6);
      break;

    case U2D_L2R:
      regval = (1U << 5);
      break;

    case U2D_R2L:
      regval = (1U << 6) | (1U << 5);
      break;

    case D2U_L2R:
      regval = (1U << 7) | (1U << 5);
      break;

    case D2U_R2L:
      regval = (1U << 7) | (1U << 6) | (1U << 5);
      break;

    default:
      regval = 0U;
      break;
  }

  if (lcddev.id == 0x9341U || lcddev.id == 0x7789U || lcddev.id == 0x7796U)
  {
    regval |= 0x08U;
  }

  lcd_write_reg((lcddev.id == 0x5510U) ? 0x3600U : 0x36U, regval);

  if (lcddev.id != 0x1963U)
  {
    if ((regval & 0x20U) != 0U)
    {
      if (lcddev.width < lcddev.height)
      {
        temp = lcddev.width;
        lcddev.width = lcddev.height;
        lcddev.height = temp;
      }
    }
    else
    {
      if (lcddev.width > lcddev.height)
      {
        temp = lcddev.width;
        lcddev.width = lcddev.height;
        lcddev.height = temp;
      }
    }
  }

  lcd_set_window(0U, 0U, lcddev.width, lcddev.height);
}

void lcd_display_dir(uint8_t dir)
{
  lcddev.dir = dir ? 1U : 0U;

  if (lcddev.id == 0x1963U)
  {
    lcddev.width = (lcddev.dir == 0U) ? 480U : 800U;
    lcddev.height = (lcddev.dir == 0U) ? 800U : 480U;
  }
  else if (lcddev.id == 0x5510U)
  {
    lcddev.wramcmd = 0x2C00U;
    lcddev.setxcmd = 0x2A00U;
    lcddev.setycmd = 0x2B00U;
    lcddev.width = (lcddev.dir == 0U) ? 480U : 800U;
    lcddev.height = (lcddev.dir == 0U) ? 800U : 480U;
  }
  else if (lcddev.id == 0x9806U)
  {
    lcddev.width = (lcddev.dir == 0U) ? 480U : 800U;
    lcddev.height = (lcddev.dir == 0U) ? 800U : 480U;
  }
  else if (lcddev.id == 0x7796U || lcddev.id == 0x5310U)
  {
    lcddev.width = (lcddev.dir == 0U) ? 320U : 480U;
    lcddev.height = (lcddev.dir == 0U) ? 480U : 320U;
  }
  else
  {
    lcddev.width = (lcddev.dir == 0U) ? 240U : 320U;
    lcddev.height = (lcddev.dir == 0U) ? 320U : 240U;
  }

  lcd_scan_dir(DFT_SCAN_DIR);
}

void lcd_ssd_backlight_set(uint8_t pwm)
{
  (void)pwm;
  LCD_BL(1);
}

void lcd_clear(uint16_t color)
{
  uint32_t index;
  const uint32_t total = (uint32_t)lcddev.width * (uint32_t)lcddev.height;

  lcd_set_window(0U, 0U, lcddev.width, lcddev.height);
  lcd_write_ram_prepare();

  for (index = 0U; index < total; index++)
  {
    LCD->LCD_RAM = color;
  }
}

void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
  const uint16_t ex = (uint16_t)(sx + width - 1U);
  const uint16_t ey = (uint16_t)(sy + height - 1U);

  if (lcddev.id == 0x5510U)
  {
    lcd_wr_regno(lcddev.setxcmd);
    lcd_wr_data((uint16_t)(sx >> 8));
    lcd_wr_regno((uint16_t)(lcddev.setxcmd + 1U));
    lcd_wr_data((uint16_t)(sx & 0xFFU));
    lcd_wr_regno((uint16_t)(lcddev.setxcmd + 2U));
    lcd_wr_data((uint16_t)(ex >> 8));
    lcd_wr_regno((uint16_t)(lcddev.setxcmd + 3U));
    lcd_wr_data((uint16_t)(ex & 0xFFU));

    lcd_wr_regno(lcddev.setycmd);
    lcd_wr_data((uint16_t)(sy >> 8));
    lcd_wr_regno((uint16_t)(lcddev.setycmd + 1U));
    lcd_wr_data((uint16_t)(sy & 0xFFU));
    lcd_wr_regno((uint16_t)(lcddev.setycmd + 2U));
    lcd_wr_data((uint16_t)(ey >> 8));
    lcd_wr_regno((uint16_t)(lcddev.setycmd + 3U));
    lcd_wr_data((uint16_t)(ey & 0xFFU));
  }
  else
  {
    lcd_wr_regno(lcddev.setxcmd);
    lcd_wr_data((uint16_t)(sx >> 8));
    lcd_wr_data((uint16_t)(sx & 0xFFU));
    lcd_wr_data((uint16_t)(ex >> 8));
    lcd_wr_data((uint16_t)(ex & 0xFFU));

    lcd_wr_regno(lcddev.setycmd);
    lcd_wr_data((uint16_t)(sy >> 8));
    lcd_wr_data((uint16_t)(sy & 0xFFU));
    lcd_wr_data((uint16_t)(ey >> 8));
    lcd_wr_data((uint16_t)(ey & 0xFFU));
  }
}

void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
  if ((x >= lcddev.width) || (y >= lcddev.height))
  {
    return;
  }

  lcd_set_cursor(x, y);
  lcd_write_ram_prepare();
  LCD->LCD_RAM = (uint16_t)color;
}

void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
  uint8_t temp;
  uint8_t t1;
  uint8_t t;
  uint16_t y0 = y;
  uint8_t csize;
  const uint8_t *pfont = NULL;

  if ((chr < ' ') || (chr > '~'))
  {
    return;
  }

  csize = (uint8_t)((size / 8U + ((size % 8U) ? 1U : 0U)) * (size / 2U));
  chr = (char)(chr - ' ');

  switch (size)
  {
    case 12:
      pfont = asc2_1206[(uint8_t)chr];
      break;

    case 16:
      pfont = asc2_1608[(uint8_t)chr];
      break;

    case 24:
      pfont = asc2_2412[(uint8_t)chr];
      break;

    case 32:
      pfont = asc2_3216[(uint8_t)chr];
      break;

    default:
      return;
  }

  for (t = 0; t < csize; t++)
  {
    temp = pfont[t];

    for (t1 = 0; t1 < 8U; t1++)
    {
      if ((temp & 0x80U) != 0U)
      {
        lcd_draw_point(x, y, color);
      }
      else if (mode == 0U)
      {
        lcd_draw_point(x, y, g_back_color);
      }

      temp <<= 1;
      y++;

      if (y >= lcddev.height)
      {
        return;
      }

      if ((y - y0) == size)
      {
        y = y0;
        x++;

        if (x >= lcddev.width)
        {
          return;
        }

        break;
      }
    }
  }
}

static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
  uint32_t result = 1U;

  while (n-- != 0U)
  {
    result *= m;
  }

  return result;
}

void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
  uint8_t t;
  uint8_t temp;
  uint8_t enshow = 0U;

  for (t = 0; t < len; t++)
  {
    temp = (uint8_t)((num / lcd_pow(10U, (uint8_t)(len - t - 1U))) % 10U);

    if ((enshow == 0U) && (t < (len - 1U)))
    {
      if (temp == 0U)
      {
        lcd_show_char((uint16_t)(x + (size / 2U) * t), y, ' ', size, 0U, color);
        continue;
      }

      enshow = 1U;
    }

    lcd_show_char((uint16_t)(x + (size / 2U) * t), y, (char)(temp + '0'), size, 0U, color);
  }
}

void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
  uint8_t t;
  uint8_t temp;
  uint8_t enshow = 0U;

  for (t = 0; t < len; t++)
  {
    temp = (uint8_t)((num / lcd_pow(10U, (uint8_t)(len - t - 1U))) % 10U);

    if ((enshow == 0U) && (t < (len - 1U)) && (temp == 0U))
    {
      lcd_show_char((uint16_t)(x + (size / 2U) * t), y, ((mode & 0x80U) ? '0' : ' '), size,
                    (uint8_t)(mode & 0x01U), color);
      continue;
    }

    enshow = 1U;
    lcd_show_char((uint16_t)(x + (size / 2U) * t), y, (char)(temp + '0'), size,
                  (uint8_t)(mode & 0x01U), color);
  }
}

void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
  uint16_t x0 = x;
  width = (uint16_t)(width + x);
  height = (uint16_t)(height + y);

  while ((*p <= '~') && (*p >= ' '))
  {
    if (x >= width)
    {
      x = x0;
      y = (uint16_t)(y + size);
    }

    if (y >= height)
    {
      break;
    }

    lcd_show_char(x, y, *p, size, 0U, color);
    x = (uint16_t)(x + size / 2U);
    p++;
  }
}

void lcd_init(void)
{
  /* MX_FSMC_Init() / MX_GPIO_Init() are already responsible for low-level bus/pin setup. */
  delay_busy_ms(50U);

  lcddev.wramcmd = 0x2CU;
  lcddev.setxcmd = 0x2AU;
  lcddev.setycmd = 0x2BU;

  /* Try ILI9341 first. */
  lcd_wr_regno(0xD3U);
  lcddev.id = lcd_rd_data();
  lcddev.id = lcd_rd_data();
  lcddev.id = lcd_rd_data();
  lcddev.id <<= 8;
  lcddev.id |= lcd_rd_data();

  if (lcddev.id != 0x9341U)
  {
    /* Try ST7789 */
    lcd_wr_regno(0x04U);
    lcddev.id = lcd_rd_data();
    lcddev.id = lcd_rd_data();
    lcddev.id = lcd_rd_data();
    lcddev.id <<= 8;
    lcddev.id |= lcd_rd_data();
    if (lcddev.id == 0x8552U)
    {
      lcddev.id = 0x7789U;
    }

    if (lcddev.id != 0x7789U)
    {
      /* Try NT35310 */
      lcd_wr_regno(0xD4U);
      lcddev.id = lcd_rd_data();
      lcddev.id = lcd_rd_data();
      lcddev.id = lcd_rd_data();
      lcddev.id <<= 8;
      lcddev.id |= lcd_rd_data();

      if (lcddev.id != 0x5310U)
      {
        /* Try ST7796 */
        lcd_wr_regno(0xD3U);
        lcddev.id = lcd_rd_data();
        lcddev.id = lcd_rd_data();
        lcddev.id = lcd_rd_data();
        lcddev.id <<= 8;
        lcddev.id |= lcd_rd_data();

        if (lcddev.id != 0x7796U)
        {
          /* Try NT35510 */
          lcd_write_reg(0xF000U, 0x0055U);
          lcd_write_reg(0xF001U, 0x00AAU);
          lcd_write_reg(0xF002U, 0x0052U);
          lcd_write_reg(0xF003U, 0x0008U);
          lcd_write_reg(0xF004U, 0x0001U);

          lcd_wr_regno(0xC500U);
          lcddev.id = lcd_rd_data();
          lcddev.id <<= 8;
          lcd_wr_regno(0xC501U);
          lcddev.id |= lcd_rd_data();
          delay_busy_ms(5U);

          if (lcddev.id != 0x5510U)
          {
            /* Try ILI9806 */
            lcd_wr_regno(0xD3U);
            lcddev.id = lcd_rd_data();
            lcddev.id = lcd_rd_data();
            lcddev.id = lcd_rd_data();
            lcddev.id <<= 8;
            lcddev.id |= lcd_rd_data();

            if (lcddev.id != 0x9806U)
            {
              /* Try SSD1963 */
              lcd_wr_regno(0xA1U);
              lcddev.id = lcd_rd_data();
              lcddev.id = lcd_rd_data();
              lcddev.id <<= 8;
              lcddev.id |= lcd_rd_data();
              if (lcddev.id == 0x5761U)
              {
                lcddev.id = 0x1963U;
              }
            }
          }
        }
      }
    }
  }

  printf("LCD ID:%x\r\n", lcddev.id);

  if (lcddev.id == 0x7789U)
  {
    lcd_ex_st7789_reginit();
  }
  else if (lcddev.id == 0x9341U)
  {
    lcd_ex_ili9341_reginit();
  }
  else if (lcddev.id == 0x5310U)
  {
    lcd_ex_nt35310_reginit();
  }
  else if (lcddev.id == 0x7796U)
  {
    lcd_ex_st7796_reginit();
  }
  else if (lcddev.id == 0x5510U)
  {
    lcddev.wramcmd = 0x2C00U;
    lcddev.setxcmd = 0x2A00U;
    lcddev.setycmd = 0x2B00U;
    lcd_ex_nt35510_reginit();
  }
  else if (lcddev.id == 0x9806U)
  {
    lcd_ex_ili9806_reginit();
  }
  else if (lcddev.id == 0x1963U)
  {
    lcd_ex_ssd1963_reginit();
    lcd_ssd_backlight_set(100U);
  }
  else
  {
    /* Fallback to common init if ID read fails on some boards. */
    lcd_ex_ili9341_reginit();
    lcddev.id = 0x9341U;
  }

  lcd_display_dir(0U);
  LCD_BL(1);
  lcd_clear(WHITE);
}
