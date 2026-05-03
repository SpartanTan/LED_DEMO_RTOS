# Branch Features

LED + NVIC + USART1

This branch contains a STM32F103 + FreeRTOS interrupt masking experiment.
It demonstrates task scheduling, timer interrupts with different NVIC priorities, and `portDISABLE_INTERRUPTS()`/`portENABLE_INTERRUPTS()` behavior.
USART `printf` output and a simple App/BSP code layout are also included for easier experiment extension.

Goal: keep one experiment per branch in this repository, so each branch remains focused and easy to compare.
