#include "interrupt_demo.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "delay.h"
#include "main.h"
#include "task.h"
#include "tim.h"

void interrupt_demo_task(void *argument)
{
  HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim5);

  uint32_t num = 0;

  while (1)
  {
    if (++num == 5)
    {
      printf("FreeRTOS Disable Interrupt!\r\n");
      portDISABLE_INTERRUPTS();

      delay_busy_ms(5000);

      portENABLE_INTERRUPTS();
      printf("FreeRTOS Enable Interrupt!\r\n");

      num = 0;
    }

    osDelay(1000);
  }
}

void interrupt_demo_tim_period_elapsed_callback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
    printf("TIM3 Interrupt triggered!\r\n");

    HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
  }
  else if (htim->Instance == TIM5)
  {
    printf("TIM5 Interrupt triggered!\r\n");
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
  }
}
