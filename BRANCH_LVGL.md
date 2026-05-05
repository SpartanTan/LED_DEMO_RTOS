# Branch: exp/lvgl

This branch extends the FreeRTOS interrupt demo with a FSMC 8080 LCD and a minimal LVGL port.

The LCD is initialized in `main()`, while `lvglTask` runs the LVGL tick, timer handler, display flush callback, and a small test UI. The screen shows `Hello LVGL`, the LCD ID, and live `TIM3`, `TIM5`, and total interrupt statistics collected by the existing interrupt demo.

LVGL is included as a trimmed git submodule under `Middlewares/Third_Party/LVGL/lvgl`, with project-specific configuration in `Middlewares/Third_Party/LVGL/lv_conf.h`.

Build:

```sh
make
```

Flash:

```sh
STM32_Programmer_CLI -c port=SWD -w build/LED_DEMO_RTOS.elf -v -rst
```
