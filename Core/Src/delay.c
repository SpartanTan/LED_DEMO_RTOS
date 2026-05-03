#include "delay.h"

#include "stm32f1xx_hal.h"

/*
 * These delays intentionally do not use HAL_Delay(), osDelay(), or vTaskDelay().
 * They are useful while FreeRTOS-managed interrupts are masked by BASEPRI.
 */

void delay_busy_cycles(uint32_t cycles)
{
  while (cycles-- != 0U)
  {
    __NOP();
  }
}

void delay_busy_ms(uint32_t ms)
{
  /*
   * This is approximate. The loop body takes multiple CPU cycles, so divide
   * SystemCoreClock by a small empirical factor to get a millisecond-scale wait.
   */
  const uint32_t cycles_per_ms = SystemCoreClock / 6000U;

  while (ms-- != 0U)
  {
    delay_busy_cycles(cycles_per_ms);
  }
}
