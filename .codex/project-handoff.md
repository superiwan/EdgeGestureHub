# Project Handoff - EdgeGestureHub

## 1) Project Goal
Build an STM32F4-based EdgeGestureHub demo with stable power, MPU6050 + OLED bring-up, and wireless UART telemetry for VOFA+/debug.

## 2) Current Status
- Firmware baseline is stable and builds cleanly.
- Active hardware mapping:
  - MPU6050 -> I2C1 (PB6/PB7)
  - OLED -> I2C2 (PB10/PB11)
  - Wireless module data UART -> USART2 (PA2/PA3)
- Wireless module control strategy is now simplified per user decision:
  - MD0/AUX intentionally not used in final wiring for now.
- USART2 baudrate is currently set to **9600** (changed from 115200 by latest request).
- Temporary `RF_TEST_123` probe output was added for diagnosis and then removed; current output is pure business telemetry again.
- Runtime serial test at 9600 previously showed valid CSV telemetry when COM18 was free.

## 3) Important Paths
- Workspace root: `C:/Users/prohibit/Desktop/stm32f4_project`
- Firmware entry: `cubemx/EdgeGestureHub/Core/Src/main.c`
- App task: `cubemx/EdgeGestureHub/Core/Src/app_task.c`
- UART init: `cubemx/EdgeGestureHub/Core/Src/usart.c`
- GPIO init: `cubemx/EdgeGestureHub/Core/Src/gpio.c`
- MCU pins define: `cubemx/EdgeGestureHub/Core/Inc/main.h`
- I2C init: `cubemx/EdgeGestureHub/Core/Src/i2c.c`
- Keil project: `cubemx/EdgeGestureHub/MDK-ARM/EdgeGestureHub.uvprojx`
- Pipeline scripts: `scripts/build-keil.ps1`, `scripts/flash-stlink.ps1`, `scripts/dev-all.ps1`, `scripts/monitor-serial.ps1`
- Latest schematic netlist reviewed: `C:/Users/prohibit/Desktop/Netlist_Schematic1_2026-03-14.tel`

## 4) Environment and Tooling Notes
- Windows + PowerShell workflow.
- Workspace is not a Git repo.
- COM port contention happens frequently (VOFA+/serial tools may lock COM18).
- `AGENTS.md` references `project-skills/edgegesture-handoff/...` but that path is missing in this workspace.

## 5) Known Pitfalls
- Keil can occasionally return non-zero despite successful compile; verify log details.
- When COM18 is occupied, monitor script reports `PermissionError(13)`.
- For ATK-MW1278D, mismatched module UART parameters can present as garbage or zero-like data.
- In PCB stage, avoid P1/P2 physical overlap; it can create accidental shorts.

## 6) Exact Next Step
Continue PCB implementation in JLCEDA with defined rules (mm), finish routing + DRC, then re-verify wireless telemetry at USART2=9600 with COM18 exclusively opened by one tool.

## 7) Recent Thread Note
Finalized wireless strategy for now as PA2/PA3 data-only (MD0/AUX not connected), toggled USART2 baudrate per requests, restored pure business output, and completed one more netlist sanity review.

- 2026-03-15: Resumed via $project-thread-handoff; validated AGENTS/handoff paths and confirmed .codex/project-handoff.md remains canonical with no firmware changes in this thread.

- 2026-03-15: Added Chinese module/function comments across core and BSP custom files; documented that current gesture engine is rule-based and NanoEdge AI should be integrated by replacing gesture_classify_raw() while keeping public interfaces stable.

- 2026-03-15: Added a new reading-order guide at 文档/代码阅读顺序指南.md to help navigate modules and NanoEdge AI integration entry point.

- 2026-03-15: Integrated NanoEdge AI deployment library into firmware. Gesture engine now uses neai learn/detect path with Chinese comments and fallback rule-based classifier. Copied NanoEdgeAI.h to Core/Inc and linked libneai.a in Keil project. Updated Keil short_wchar to 0 to match NanoEdge metadata. Build passed (0 errors, 0 warnings).


- 2026-03-15: Externalized NanoEdge runtime params to app_config.h (startup learn lock + nominal similarity threshold). Added class status API in gesture_engine (0/1/2 and learning-done getter). Updated OLED display to show STATUS + LEARN/NORM/ANOM and extended SSD1306 glyph table with L/M for new labels. Rebuilt and flashed successfully.


- 2026-03-15: Fixed Keil+NanoEdge integration with new ARM toolchain library (libneai_project-2026-03-15-16-00_3). Resolved build/link issues by setting project C compiler attributes to match library ABI (EnumInt=1, vShortWch=0, C MiscControls=--wchar32) and library path to ..\\..\\..\\libneai_project-2026-03-15-16-00_3\\libneai.a. Build and flash now succeed; generated HEX contains STATUS/LEARN/NORM/ANOM strings (no Gesture string).

