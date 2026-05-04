#ifndef INTERRUPT_DEMO_H
#define INTERRUPT_DEMO_H

#include "tim.h"

typedef struct
{
  uint32_t tim3;
  uint32_t tim5;
  uint32_t total;
} interrupt_demo_stats_t;

void interrupt_demo_get_stats(interrupt_demo_stats_t *stats);

void interrupt_demo_task(void *argument);
void interrupt_demo_tim_period_elapsed_callback(TIM_HandleTypeDef *htim);

#endif
