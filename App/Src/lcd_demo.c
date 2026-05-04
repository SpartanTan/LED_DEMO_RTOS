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

  lcd_show_string(40, 140, 80, 24, 24, "TIM3:", BLUE);
  lcd_show_string(40, 180, 80, 24, 24, "TIM5:", BLUE);
  lcd_show_string(40, 220, 100, 24, 24, "TOTAL:", RED);

  for (;;)
  {
    interrupt_demo_get_stats(&stats);

    snprintf(line, sizeof(line), "%lu", stats.tim3);
    lcd_show_string(130, 140, 160, 24, 24, line, BLUE);

    snprintf(line, sizeof(line), "%lu", stats.tim5);
    lcd_show_string(130, 180, 160, 24, 24, line, BLUE);

    snprintf(line, sizeof(line), "%lu", stats.total);
    lcd_show_string(130, 220, 160, 24, 24, line, RED);

    tick += 100;
    osDelayUntil(tick);
  }
}
