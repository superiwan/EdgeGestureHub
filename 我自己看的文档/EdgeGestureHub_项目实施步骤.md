# EdgeGestureHub 项目实施步骤（代码对齐版）

## 目标
在不改架构的前提下，保持当前稳定基线并迭代业务功能。

## 基线确认
- 主工程：`cubemx/EdgeGestureHub`
- 调度：`10ms/20ms/100ms`
- 三态：`LEARN/NORM/ANOM`
- 业务动作：`ANOM` 边沿触发 `PD12`

## 硬件接线
- MPU6050 -> `I2C1 PB6/PB7`
- OLED -> `I2C2 PB10/PB11`
- USB 调试 -> `USART1 PA9/PA10 115200`
- 无线串口 -> `USART2 PA2/PA3 9600`

## 必做联调
1. 上电后先看到 `LEARN`。
2. 学习后静止为 `NORM`。
3. 动作时出现 `ANOM`，LED 只在异常边沿切换一次。
4. 串口持续输出 7 列 CSV。

## 构建与烧录
- 构建：`scripts/build-keil.ps1`
- 烧录：`scripts/flash-stlink.ps1`
- 一键：`scripts/dev-all.ps1 -NoMonitor`

## 迭代建议
- 新业务优先改 `control_logic.c`，不直接改识别核心。
- 灵敏度优先调 `app_config.h` 阈值与学习锁时间。
- 先看 `cls` 稳定性，再接外部执行器。
