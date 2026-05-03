/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    delay.h
  * @brief   Blocking delay helpers that do not depend on HAL or RTOS tick.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void delay_busy_cycles(uint32_t cycles);
void delay_busy_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __DELAY_H */
