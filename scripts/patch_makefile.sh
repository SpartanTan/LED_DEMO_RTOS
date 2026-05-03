#!/usr/bin/env bash
set -euo pipefail

makefile="${1:-Makefile}"

if [[ ! -f "$makefile" ]]; then
  echo "error: $makefile not found" >&2
  exit 1
fi

shopt -s nullglob
sources=(App/Src/*.c BSP/Src/*.c)
includes=()
[[ -d App/Inc ]] && includes+=("-IApp/Inc")
[[ -d BSP/Inc ]] && includes+=("-IBSP/Inc")

source_anchor="Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c"
include_anchor="-IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3"

tmp="${makefile}.tmp"
inserted_sources=0
inserted_includes=0

emit_block() {
  local -n items_ref="$1"
  local i

  for ((i = 0; i < ${#items_ref[@]}; i++)); do
    if ((i + 1 < ${#items_ref[@]})); then
      printf '%s \\\n' "${items_ref[$i]}"
    else
      printf '%s\n' "${items_ref[$i]}"
    fi
  done
}

while IFS= read -r line || [[ -n "$line" ]]; do
  case "$line" in
    App/Src/*.c*|BSP/Src/*.c*|-IApp/Inc*|-IBSP/Inc*)
      continue
      ;;
  esac

  if [[ "$line" == "Core/Src/delay.c"* && ! -f Core/Src/delay.c ]]; then
    continue
  fi

  if [[ "$line" == "$source_anchor"* && ${#sources[@]} -gt 0 ]]; then
    printf '%s \\\n' "$source_anchor"
    emit_block sources
    inserted_sources=1
    continue
  fi

  if [[ "$line" == "$include_anchor"* && ${#includes[@]} -gt 0 ]]; then
    printf '%s \\\n' "$include_anchor"
    emit_block includes
    inserted_includes=1
    continue
  fi

  printf '%s\n' "$line"
done < "$makefile" > "$tmp"

if [[ ${#sources[@]} -gt 0 && "$inserted_sources" -ne 1 ]]; then
  rm -f "$tmp"
  echo "error: could not find C_SOURCES insertion point in $makefile" >&2
  exit 1
fi

if [[ ${#includes[@]} -gt 0 && "$inserted_includes" -ne 1 ]]; then
  rm -f "$tmp"
  echo "error: could not find C_INCLUDES insertion point in $makefile" >&2
  exit 1
fi

mv "$tmp" "$makefile"
echo "patched $makefile"
