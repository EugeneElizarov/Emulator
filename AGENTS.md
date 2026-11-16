# Repository guidance for Codex

## Project identity
- This repository contains firmware for a physiological-parameter emulator (pulse and galvanic skin response / GSR).
- MCU/project configuration currently targets **STM32F303CCT6** (STM32F303CCTx, LQFP48), Cortex-M4.
- The IDE project is **Keil MDK/uVision**: `MDK-ARM/HR_Emulator.uvprojx`.
- STM32CubeMX configuration is `HR_Emulator.ioc`; the project was generated with STM32CubeMX 6.14.0 and STM32Cube FW_F3 V1.11.6.
- The checked-in clock configuration currently describes an 8 MHz HSE and 72 MHz SYSCLK. Treat the checked-in project files as the source of truth; do not silently change clock assumptions.

## Before making changes
1. Inspect the relevant source, headers, CubeMX configuration, and Keil project settings before editing.
2. Distinguish confirmed hardware facts from assumptions. If a pin, external circuit, signal polarity, timing requirement, or electrical range is not documented, ask rather than inventing it.
3. Keep changes focused. Do not perform broad refactoring or rename public symbols without a concrete reason.
4. Do not claim that firmware builds or works on hardware unless the relevant build/test was actually run.

## STM32CubeMX-generated code
- Preserve all `/* USER CODE BEGIN ... */` and `/* USER CODE END ... */` sections and their contents.
- If a peripheral, pin, clock, or interrupt configuration must change, keep `HR_Emulator.ioc` and the generated C source/header files consistent.
- Avoid hand-editing generated sections when the same change should be made in CubeMX. Explain any deliberate manual exception.
- Do not regenerate the project wholesale or overwrite user code.

## Current implementation snapshot
- `main.c` initializes HAL, system clock, GPIO, ADC1, DAC, and USB PCD; the main loop is currently empty.
- ADC1 is configured for one 12-bit software-triggered regular conversion on ADC1_IN1 / PA0.
- DAC channel 1 is configured on PA4 (DAC_OUT1), with output buffer enabled and no trigger.
- USB is configured as USB Full Speed device using the embedded PHY, PA11 (DM) and PA12 (DP), with 8 device endpoints configured in the PCD handle.
- GPIO labels include KEY, status LEDs, HV_EN, HEAT_ON, and GSRb0..GSRb11. Consult `Core/Inc/main.h`, `Core/Src/gpio.c`, and `HR_Emulator.ioc` for exact assignments and modes before touching them.
- Do not infer complete pulse-generation, GSR-emulation, USB protocol, or safety behavior from the project name: these functions are not yet implemented in the current `main.c` snapshot.

## Build and project files
- Keep `MDK-ARM/HR_Emulator.uvprojx`, `MDK-ARM/startup_stm32f303xc.s`, `HR_Emulator.ioc`, and `.mxproject` under version control.
- Treat `*.uvoptx` / `*.uvopt` as per-user uVision settings, not shared project configuration.
- Do not add Keil build products (Objects, Listings, AXF/ELF/HEX/BIN/MAP, etc.) to commits.
- The Keil project currently names the target `HR_Emulator` and selects STM32F303CCTx. Preserve these unless the user explicitly requests a target migration.
- Check the configured compiler/toolchain and project options before changing build settings.

## Code quality
- Match the existing C style and STM32 HAL usage.
- Use fixed-width integer types where appropriate and make units/ranges explicit in names and comments.
- For interrupt/callback code, keep work bounded and avoid blocking operations unless explicitly justified.
- Consider electrical safety whenever changing HV_EN, HEAT_ON, DAC, or GSR switching outputs. Never enable a high-voltage or heating output by default without explicit requirements and a safe-state analysis.
- Do not introduce dynamic allocation, an RTOS, or new third-party dependencies unless the task calls for them.

## Reporting
- Summarize files changed and the reason for each.
- State what was and was not built/tested.
- Call out unresolved hardware assumptions and any required CubeMX/uVision follow-up.
