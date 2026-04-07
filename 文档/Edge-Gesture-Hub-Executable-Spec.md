# Edge-Gesture Hub 可执行规格（代码对齐版）

## 1. 范围
本规格仅描述当前仓库已实现并可复现的行为，不包含未落地方案。

## 2. 已实现目标
- 读取 MPU6050 六轴数据并换算物理量。
- NanoEdge AI 三态输出：`LEARN / NORM / ANOM`。
- OLED 显示当前三态。
- 双串口输出 FireWater CSV。
- `ANOM` 边沿触发 `PD12` 业务动作。

## 3. 真实硬件映射
- `I2C1(PB6/PB7)` -> MPU6050
- `I2C2(PB10/PB11)` -> OLED
- `USART1(PA9/PA10, 115200)` -> USB 调试串口
- `USART2(PA2/PA3, 9600)` -> 无线串口
- `PD12` -> 业务 LED

## 4. 调度与时序
- 采样周期：`10ms`（100Hz）
- 串口上报周期：`20ms`（50Hz）
- OLED 刷新周期：`100ms`

## 5. 输出协议
函数：`FireWater_SendFrame(...)`
格式：`ax,ay,az,gx,gy,gz,cls\n`
说明：
- `cls=0` 学习中
- `cls=1` 正常
- `cls=2` 异常

## 6. 识别规则（当前代码）
- 上电后先进行固定学习锁（默认 `3000ms`）。
- 学习完成后进入检测；相似度与阈值比较决定 `NORM/ANOM`。
- NanoEdge 异常时自动回退到阈值兜底分类，避免系统无输出。

## 7. 控制规则（当前代码）
- 触发条件：`ANOM` 边沿。
- 冷却：`500ms`。
- 门控：必须先回到 `STATIC` 才允许下一次异常触发。

## 8. 参数文件
`Core/Inc/app_config.h`
- `APP_SAMPLE_PERIOD_MS 10`
- `APP_UART_PERIOD_MS 20`
- `APP_OLED_PERIOD_MS 100`
- `GESTURE_NEAI_STARTUP_LEARN_LOCK_MS 3000`
- `GESTURE_NEAI_NOMINAL_SIMILARITY_TH 80`

## 9. 构建与烧录命令
- `scripts/build-keil.ps1`
- `scripts/flash-stlink.ps1`
- `scripts/dev-all.ps1`

## 10. 判定为“与代码不一致”的旧口径（已废弃）
- OLED 与 MPU6050 共用 I2C1。
- 系统仅单串口输出。
- VOFA+ 使用 JustFloat 文本键值格式。
以上内容不再作为当前工程依据。
