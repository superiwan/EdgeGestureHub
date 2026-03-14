# Project Handoff - EdgeGestureHub

## 1) Project Goal
Build an STM32F4-based EdgeGestureHub demo that reads MPU6050 motion data, shows status on SSD1306 OLED, streams telemetry over UART (FireWater format), and then evolves from threshold-based gesture recognition to optional AI inference.

## 2) Current Status
- Workspace has CubeMX project at `cubemx/EdgeGestureHub` and Keil project at `cubemx/EdgeGestureHub/MDK-ARM/EdgeGestureHub.uvprojx`.
- `main.c` is integrated with app loop (`App_Init()` and `App_Loop()` are called).
- Keil/ST-Link pipeline has been exercised before; documented build/flash success exists.
- Driver implementation is still stubbed:
  - `Drivers/BSP/Src/mpu6050.c` currently returns mock values.
  - `Drivers/BSP/Src/ssd1306.c` currently contains no real display transfer logic.
- No unified handoff file existed before this thread; this file is now the canonical handoff entry.

## 3) Important Paths
- Workspace root: `C:/Users/prohibit/Desktop/stm32f4_project`
- CubeMX project: `cubemx/EdgeGestureHub/EdgeGestureHub.ioc`
- Firmware entry: `cubemx/EdgeGestureHub/Core/Src/main.c`
- MPU6050 driver: `cubemx/EdgeGestureHub/Drivers/BSP/Src/mpu6050.c`
- SSD1306 driver: `cubemx/EdgeGestureHub/Drivers/BSP/Src/ssd1306.c`
- Keil project: `cubemx/EdgeGestureHub/MDK-ARM/EdgeGestureHub.uvprojx`
- Pipeline scripts: `scripts/build-keil.ps1`, `scripts/flash-stlink.ps1`, `scripts/dev-all.ps1`
- Prior troubleshooting notes:
  - `EdgeGestureHub_问题排查记录.md`
  - `stm32-keil-stlink-pipeline_问题与修改记录.md`

## 4) Environment and Tooling Notes
- Windows + PowerShell workflow.
- This directory is **not** a Git repo currently.
- `rg` may be unavailable in this shell; use PowerShell file enumeration when needed.
- Keil stability depends on opening workflow and Debug/Utilities settings (see troubleshooting note).
- A path referenced by `AGENTS.md` (`project-skills/edgegesture-handoff/...`) is currently missing in workspace.

## 5) Known Pitfalls
- Keil command return code can be non-zero even when build log shows `0 Error(s)`.
- Debug/flash failures can happen if Keil project carries wrong DLL/debugger state across sessions.
- Pipeline templates may assume `Objects/Project.*` outputs while this project uses `EdgeGestureHub/EdgeGestureHub.*` outputs.

## 6) Exact Next Step
Implement real `MPU6050_Init()` and `MPU6050_ReadFrame()` in `cubemx/EdgeGestureHub/Drivers/BSP/Src/mpu6050.c`, then validate live `ax,ay,az,gx,gy,gz` UART output (115200, FireWater) before touching OLED/gesture thresholds.

## 7) Recent Thread Note
User invoked `$project-thread-handoff`; created this first canonical handoff file after checking existing docs and confirming that handoff references in `AGENTS.md` are missing from workspace.
