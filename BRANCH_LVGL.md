# Branch: exp/lvgl

This branch extends the FreeRTOS interrupt demo with a FSMC 8080 LCD, LVGL, and an EEZ Studio generated UI.

The LCD is initialized in `main()` and switched to landscape mode with `lcd_display_dir(1U)` so it matches the EEZ project size of `800x480`.

`lvglTask` owns all LVGL work. It initializes LVGL, creates the display from `lcddev.width` and `lcddev.height`, registers the FSMC LCD flush callback, calls the EEZ generated `ui_init()`, and then repeatedly calls `ui_tick()` and `lv_timer_handler()`.

EEZ Studio output lives under `App/UI`:

- `App/UI/UI.eez-project`: EEZ Studio project file
- `App/UI/src/ui/ui.c` / `ui.h`: generated UI entry points
- `App/UI/src/ui/screens.c` / `screens.h`: generated screens and widgets
- `App/UI/src/ui/images.c`, `styles.c`, and headers: generated UI assets and style code

LVGL is included as a trimmed git submodule under `Middlewares/Third_Party/LVGL/lvgl`, with project-specific configuration in `Middlewares/Third_Party/LVGL/lv_conf.h`. The trimmed LVGL submodule keeps only the code needed by this project; `lvgl.mk` is also trimmed to avoid scanning removed `demos` and `examples` paths.

Helper scripts:

- `scripts/patch_makefile.sh`: re-adds App, BSP, and EEZ generated sources/includes after CubeMX regenerates the Makefile
- `scripts/update_vscode_config.sh`: regenerates `.vscode/c_cpp_properties.json`
- `Middlewares/Third_Party/LVGL/lvgl/trim_lvgl_mk.sh`: removes unused `demos` and `examples` scan lines from the LVGL submodule Makefile

Build:

```sh
make
```

Flash:

```sh
STM32_Programmer_CLI -c port=SWD -w build/LED_DEMO_RTOS.elf -v -rst
```
