#include "interrupt_demo.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "delay.h"
#include "main.h"
#include "task.h"
#include "tim.h"

static volatile uint32_t tim3_count = 0;
static volatile uint32_t tim5_count = 0;
static uint32_t total_count = 0;

void interrupt_demo_task(void *argument)
{
  (void)argument;

  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim5);

  while (1)
  {
    total_count = tim3_count + tim5_count;
    osDelay(200);
  }
}

void interrupt_demo_tim_period_elapsed_callback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
    tim3_count++;
    // printf("TIM3 Interrupt triggered!\r\n");
    HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
  }
  else if (htim->Instance == TIM5)
  {
    tim5_count++;
    // printf("TIM5 Interrupt triggered!\r\n");
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
  }
}

void interrupt_demo_get_stats(interrupt_demo_stats_t *stats)
{
  if (stats == NULL)
  {
    return;
  }

  taskENTER_CRITICAL();
  stats->tim3 = tim3_count;
  stats->tim5 = tim5_count;
  stats->total = total_count;
  taskEXIT_CRITICAL();
}
