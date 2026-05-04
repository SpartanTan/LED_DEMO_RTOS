#include <stdio.h>

#include "cmsis_os.h"
#include "lcd.h"
#include "interrupt_demo.h"

void lcd_demo_task(void *argument)
{
  (void)argument;

  interrupt_demo_stats_t stats;
  char line[32];

  uint32_t tick = osKernelGetTickCount();

  for (;;)
  {
    interrupt_demo_get_stats(&stats);

    snprintf(line, sizeof(line), "TIM3: %lu", stats.tim3);
    lcd_show_string(40, 100, 300, 24, 24, "                    ", WHITE);
    lcd_show_string(40, 100, 300, 24, 24, line, BLUE);

    snprintf(line, sizeof(line), "TIM5: %lu", stats.tim5);
    lcd_show_string(40, 140, 300, 24, 24, "                    ", WHITE);
    lcd_show_string(40, 140, 300, 24, 24, line, BLUE);

    snprintf(line, sizeof(line), "TOTAL: %lu", stats.total);
    lcd_show_string(40, 180, 300, 24, 24, "                    ", WHITE);
    lcd_show_string(40, 180, 300, 24, 24, line, RED);

    tick += 500;
    osDelayUntil(tick);
  }
}