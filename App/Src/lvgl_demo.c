#include "lvgl_demo.h"

#include <stdint.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "interrupt_demo.h"
#include "lcd.h"
#include "lvgl.h"

#define LVGL_DEMO_BUF_LINES 4U
#define LVGL_DEMO_MAX_HOR_RES 800U
#define LVGL_DEMO_TICK_PERIOD_MS 5U
#define LVGL_DEMO_STATS_PERIOD_MS 200U

static uint8_t lvgl_draw_buf[LVGL_DEMO_MAX_HOR_RES * LVGL_DEMO_BUF_LINES * 2U];

static lv_obj_t *tim3_label;
static lv_obj_t *tim5_label;
static lv_obj_t *total_label;

static void lvgl_lcd_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  const int32_t width = area->x2 - area->x1 + 1;
  const int32_t height = area->y2 - area->y1 + 1;
  const uint16_t *color = (const uint16_t *)px_map;

  if ((width <= 0) || (height <= 0))
  {
    lv_display_flush_ready(disp);
    return;
  }

  lcd_set_window((uint16_t)area->x1, (uint16_t)area->y1, (uint16_t)width, (uint16_t)height);
  lcd_write_ram_prepare();

  for (int32_t y = 0; y < height; y++)
  {
    for (int32_t x = 0; x < width; x++)
    {
      LCD->LCD_RAM = *color++;
    }
  }

  lv_display_flush_ready(disp);
}

static void lvgl_demo_create_ui(void)
{
  lv_obj_t *screen = lv_screen_active();

  lv_obj_set_style_bg_color(screen, lv_color_hex(0x101820), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

  lv_obj_t *title = lv_label_create(screen);
  lv_label_set_text(title, "Hello LVGL");
  lv_obj_set_style_text_color(title, lv_color_hex(0x00D1B2), LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 32, 32);

  lv_obj_t *lcd_id_label = lv_label_create(screen);
  lv_label_set_text_fmt(lcd_id_label, "LCD ID: %04X", lcddev.id);
  lv_obj_set_style_text_color(lcd_id_label, lv_color_hex(0xF9D923), LV_PART_MAIN);
  lv_obj_align(lcd_id_label, LV_ALIGN_TOP_LEFT, 32, 72);

  tim3_label = lv_label_create(screen);
  lv_obj_set_style_text_color(tim3_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_align(tim3_label, LV_ALIGN_TOP_LEFT, 32, 124);

  tim5_label = lv_label_create(screen);
  lv_obj_set_style_text_color(tim5_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_align(tim5_label, LV_ALIGN_TOP_LEFT, 32, 156);

  total_label = lv_label_create(screen);
  lv_obj_set_style_text_color(total_label, lv_color_hex(0xFF6B6B), LV_PART_MAIN);
  lv_obj_align(total_label, LV_ALIGN_TOP_LEFT, 32, 188);
}

static void lvgl_demo_update_stats(void)
{
  interrupt_demo_stats_t stats;

  interrupt_demo_get_stats(&stats);

  lv_label_set_text_fmt(tim3_label, "TIM3:  %lu", stats.tim3);
  lv_label_set_text_fmt(tim5_label, "TIM5:  %lu", stats.tim5);
  lv_label_set_text_fmt(total_label, "TOTAL: %lu", stats.total);
}

void lvgl_demo_task(void *argument)
{
  (void)argument;

  lv_init();

  lv_display_t *display = lv_display_create(lcddev.width, lcddev.height);
  lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
  lv_display_set_flush_cb(display, lvgl_lcd_flush_cb);
  lv_display_set_buffers(display, lvgl_draw_buf, NULL, sizeof(lvgl_draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

  lvgl_demo_create_ui();
  lvgl_demo_update_stats();

  uint32_t last_tick = osKernelGetTickCount();
  uint32_t last_stats_tick = last_tick;

  for (;;)
  {
    const uint32_t now = osKernelGetTickCount();
    const uint32_t elapsed = now - last_tick;

    if (elapsed > 0U)
    {
      lv_tick_inc(elapsed);
      last_tick = now;
    }

    if ((now - last_stats_tick) >= LVGL_DEMO_STATS_PERIOD_MS)
    {
      lvgl_demo_update_stats();
      last_stats_tick = now;
    }

    lv_timer_handler();
    osDelay(LVGL_DEMO_TICK_PERIOD_MS);
  }
}
