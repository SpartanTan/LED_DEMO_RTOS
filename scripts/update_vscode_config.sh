#!/usr/bin/env bash
set -euo pipefail

out=".vscode/c_cpp_properties.json"

mkdir -p .vscode

include_paths=(
  'Core/Inc'
  'Drivers/STM32F1xx_HAL_Driver/Inc'
  'Drivers/STM32F1xx_HAL_Driver/Inc/Legacy'
  'Drivers/CMSIS/Device/ST/STM32F1xx/Include'
  'Drivers/CMSIS/Include'
  'Middlewares/Third_Party/FreeRTOS/Source/include'
  'Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2'
  'Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3'
  'Middlewares/Third_Party/LVGL'
  'Middlewares/Third_Party/LVGL/lvgl'
  'Middlewares/Third_Party/LVGL/lvgl/src'
  'App/Inc'
  'App/UI/src/ui'
  'Drivers/BSP/Inc'
)

browse_paths=(
  'Core'
  'Drivers/STM32F1xx_HAL_Driver'
  'Drivers/CMSIS/Device/ST/STM32F1xx/Include'
  'Drivers/CMSIS/Include'
  'Middlewares/Third_Party/FreeRTOS/Source/include'
  'Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2'
  'Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3'
  'Middlewares/Third_Party/LVGL'
  'Middlewares/Third_Party/LVGL/lvgl'
  'Middlewares/Third_Party/LVGL/lvgl/src'
  'App'
  'App/UI'
  'Drivers/BSP'
)

filter_existing_paths() {
  local path
  for path in "$@"; do
    [[ -d "$path" ]] && printf '%s\n' "$path"
  done
}

emit_json_array() {
  local indent="$1"
  shift

  local items=("$@")
  local i

  for ((i = 0; i < ${#items[@]}; i++)); do
    printf '%s"${workspaceFolder}/%s"' "$indent" "${items[$i]}"
    if ((i + 1 < ${#items[@]})); then
      printf ','
    fi
    printf '\n'
  done
}

mapfile -t include_paths < <(filter_existing_paths "${include_paths[@]}")
mapfile -t browse_paths < <(filter_existing_paths "${browse_paths[@]}")

{
  cat <<'JSON_HEAD'
{
  "configurations": [
    {
      "name": "STM32F103xE",
      "compilerPath": "/usr/bin/arm-none-eabi-gcc",
      "compilerArgs": [
        "-mcpu=cortex-m3",
        "-mthumb"
      ],
      "intelliSenseMode": "gcc-arm",
      "cStandard": "c11",
      "cppStandard": "c++17",
      "defines": [
        "USE_HAL_DRIVER",
        "STM32F103xE"
      ],
      "includePath": [
JSON_HEAD

  emit_json_array "        " "${include_paths[@]}"

  cat <<'JSON_MID'
      ],
      "browse": {
        "path": [
JSON_MID

  emit_json_array "          " "${browse_paths[@]}"

  cat <<'JSON_TAIL'
        ],
        "limitSymbolsToIncludedHeaders": true
      }
    }
  ],
  "version": 4
}
JSON_TAIL
} > "$out"

echo "updated $out"
