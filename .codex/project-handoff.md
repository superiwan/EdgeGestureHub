# Project Handoff - EdgeGestureHub

## 1) Project Goal
Build an STM32F4-based EdgeGestureHub demo that reads MPU6050 data, shows gesture text on OLED, and streams telemetry to VOFA+ for threshold/AI workflow.

## 2) Current Status
- Hardware wiring is now back to formal topology:
  - MPU6050 -> I2C1 (PB6/PB7)
  - OLED -> I2C2 (PB10/PB11)
- `mpu6050.c` remains real implementation (init/read/convert).
- `ssd1306.c` has been simplified to hardware I2C2 only (removed temporary probe/debug path).
- `app_task.c` removed temporary bus-scan diag output and restored business loop only.
- `main.c` removed temporary tick/heartbeat test output and keeps clean `App_Init()/App_Loop()` flow.
- Build + flash completed successfully (`0 errors, 0 warnings`).

## 3) Important Paths
- Workspace root: `C:/Users/prohibit/Desktop/stm32f4_project`
- Firmware entry: `cubemx/EdgeGestureHub/Core/Src/main.c`
- App task: `cubemx/EdgeGestureHub/Core/Src/app_task.c`
- I2C init: `cubemx/EdgeGestureHub/Core/Src/i2c.c`
- MPU6050 driver: `cubemx/EdgeGestureHub/Drivers/BSP/Src/mpu6050.c`
- SSD1306 driver: `cubemx/EdgeGestureHub/Drivers/BSP/Src/ssd1306.c`
- Keil project: `cubemx/EdgeGestureHub/MDK-ARM/EdgeGestureHub.uvprojx`
- Pipeline scripts: `scripts/build-keil.ps1`, `scripts/flash-stlink.ps1`, `scripts/dev-all.ps1`

## 4) Environment and Tooling Notes
- Windows + PowerShell workflow.
- This directory is not a Git repo.
- `AGENTS.md` references `project-skills/edgegesture-handoff/...` paths that are missing in this workspace.

## 5) Known Pitfalls
- Keil occasionally returns non-zero even when no compile error; verify by log content.
- If OLED no display again, first verify module mode and pin order before changing firmware.

## 6) Exact Next Step
Reconnect full device stack and run serial monitor/VOFA+ to tune gesture thresholds using live MPU data (OLED should already follow gesture label updates).

## 7) Recent Thread Note
Switched firmware from temporary debug branch back to production wiring (MPU on I2C1, OLED on I2C2), removed test telemetry, and reflashed successfully.
