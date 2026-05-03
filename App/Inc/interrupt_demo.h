#ifndef INTERRUPT_DEMO_H
#define INTERRUPT_DEMO_H

#include "tim.h"
void interrupt_demo_task(void *argument);
void interrupt_demo_tim_period_elapsed_callback(TIM_HandleTypeDef *htim);

#endif
