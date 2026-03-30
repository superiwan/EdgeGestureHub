# EdgeGestureHub（稳定版本说明）

本项目为 STM32F407 + MPU6050 + OLED + 无线串口 的手势边缘识别演示工程。
当前版本已完成从硬件到业务动作的闭环：

- NanoEdge AI 状态识别：`LEARN / NORM / ANOM`
- OLED 状态显示：`STATUS:` + 三态标签
- 业务动作：`ANOM` 触发切换 LED（PD12）

---

## 1. 当前稳定行为

### 1.1 状态定义

- `LEARN`：学习中（上电初期/模型要求继续学习）
- `NORM`：正常（静止/正常模式）
- `ANOM`：异常（动作触发）

### 1.2 LED 业务逻辑（已稳定）

- 触发条件：`ANOM` 边沿（从非异常进入异常）
- 防抖机制：500ms 冷却
- 触发门控：必须先回到 `STATIC/NORM` 才允许下一次触发

效果：

- 静止时 LED 不会自动闪
- 做一次有效动作会切换一次亮灭
- 连续抖动不会无限连跳

---

## 2. 关键硬件连接（当前版本）

- MPU6050：I2C1（PB6/PB7）
- OLED：I2C2（PB10/PB11）
- 无线串口数据：USART2（PA2/PA3）
- 业务 LED：PD12（建议串联 220R~1k 电阻）

外接 LED 推荐接法：

- `PD12 -> 电阻 -> LED正极`
- `LED负极 -> GND`

---

## 3. 关键参数

文件：`cubemx/EdgeGestureHub/Core/Inc/app_config.h`

- `GESTURE_NEAI_STARTUP_LEARN_LOCK_MS`：启动学习锁时间（默认 3000ms）
- `GESTURE_NEAI_NOMINAL_SIMILARITY_TH`：正常相似度阈值（默认 80）

建议：先保持当前参数不变，在此稳定版本上迭代。

---

## 4. 核心代码位置

- 主循环调度：`cubemx/EdgeGestureHub/Core/Src/app_task.c`
- 手势引擎（NanoEdge + 三态）：`cubemx/EdgeGestureHub/Core/Src/gesture_engine.c`
- OLED 业务显示：`cubemx/EdgeGestureHub/Core/Src/oled_service.c`
- 业务动作映射（LED 切换）：`cubemx/EdgeGestureHub/Core/Src/control_logic.c`
- OLED 驱动：`cubemx/EdgeGestureHub/Drivers/BSP/Src/ssd1306.c`
- IMU 驱动：`cubemx/EdgeGestureHub/Drivers/BSP/Src/mpu6050.c`

---

## 5. 构建与烧录（脚本）

工程根目录下执行：

- 构建：`powershell -ExecutionPolicy Bypass -File scripts/build-keil.ps1`
- 烧录：`powershell -ExecutionPolicy Bypass -File scripts/flash-stlink.ps1`
- 构建+烧录：`powershell -ExecutionPolicy Bypass -File scripts/dev-all.ps1 -NoMonitor`

串口监控（9600）：

- `powershell -ExecutionPolicy Bypass -File scripts/monitor-serial.ps1 -NoTimestamp`

---

## 6. 本版本 NanoEdge 库说明

当前工程链接库路径：

- `..\..\..\libneai_project-2026-03-15-16-00_3\libneai.a`

对应头文件：

- `cubemx/EdgeGestureHub/Core/Inc/NanoEdgeAI.h`

注意：如重新导出 NanoEdge 库，请保持工具链与 Keil 工程 ABI 一致。

---

## 7. 快速验收流程（推荐）

按 3 段各 10 秒：

1. 静止（应主要为 `NORM`）
2. 连续动作（应出现 `ANOM`）
3. 回静止（应恢复 `NORM`）

LED 联动验收：

- 第2段动作触发时应切换一次亮灭
- 回静止后再动作可再次触发

---

## 8. 常见问题

### 8.1 OLED 仍显示旧文案（Gesture/STATIC/SWING）

通常是烧录了旧固件。请重新 Build + Download，并确认是最新 `hex`。

### 8.2 LED 不亮但接 3.3V 会亮

优先检查：

- 是否串了限流电阻
- 极性是否正确
- 是否接在 `PD12` 上

### 8.3 串口无数据

优先检查：

- COM 口是否被其他软件占用
- 波特率是否 `9600`

---

## 9. 下一步建议

- 在 `control_logic.c` 中把 LED 业务替换为你的目标业务（继电器/命令发送/页面切换等）
- 保留本版本作为稳定基线（建议打包备份）
