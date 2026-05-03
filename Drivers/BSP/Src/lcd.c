#include "lcd.h"

#include <stdio.h>

#include "delay.h"

lcd_dev_t lcddev = {0};

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
    lcd_write_reg(0x3600U, 0x0000U);
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

  lcd_set_window(0U, 0U, lcddev.width, lcddev.height);
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
