# EdgeGestureHub 项目实施步骤（从 0 到可演示）

## 0. 先定阶段目标

1. 第一阶段：跑通 `MPU6050 + OLED + UART(FireWater)`，不做 AI。
2. 第二阶段：阈值手势识别稳定（`Swing/Rotate/Static`）。
3. 第三阶段：接入 NanoEdge 或 STM32Cube.AI。

---

## 1. 硬件与原理图阶段

1. 建立原理图工程（KiCad/AD/EasyEDA 均可）。
2. 先画最小系统框图：
   - STM32F407
   - MPU6050（I2C）
   - SSD1306 OLED（I2C）
   - USB-TTL（UART）
3. 关键连线：
   - `PB6 -> I2C1_SCL`
   - `PB7 -> I2C1_SDA`
   - `PA9 -> USART1_TX`
   - `PA10 -> USART1_RX`
4. I2C 上拉：
   - SDA、SCL 各 `4.7k` 上拉到 `3.3V`
5. 电源与去耦：
   - 所有模块共地
   - 每个芯片电源脚附近放 `100nF`
6. 原理图检查重点：
   - 3.3V/5V 不接反
   - MPU6050 的 AD0 固定（建议接 GND，地址 0x68）
   - UART 的 TX/RX 交叉连接

---

## 2. 面包板联调（建议先于 PCB）

1. 按原理图先用杜邦线搭建原型。
2. 先只测 I2C 扫描：
   - 检测到 `0x68`（MPU6050）
   - 检测到 `0x3C`（OLED）
3. 再测 UART：
   - 串口可看到固定调试输出。

---

## 3. CubeMX 工程配置

1. 芯片选择 `STM32F407VETx`（按你的板卡型号）。
2. 开启外设：
   - `I2C1`（PB6/PB7，400kHz）
   - `USART1`（PA9/PA10，115200）
3. 时钟配置：
   - HSE 8MHz
   - `SYSCLK=168MHz`
   - `APB1=/4`
   - `APB2=/2`
4. 生成 Keil MDK 工程。

---

## 4. 并入当前代码骨架

1. 确认 `main.c` 中已接入：
   - `App_Init();`
   - `App_Loop();`
2. 确认 Keil 工程中已包含：
   - `app_task.c`
   - `firewater.c`
   - `gesture_engine.c`
   - `mpu6050.c`
   - `ssd1306.c`
3. 先编译一次，确保 0 error。

---

## 5. 驱动落地（关键步骤）

1. 完成 `mpu6050.c`：
   - 寄存器初始化
   - 连续读取 14 字节
   - 换算 `ax/ay/az/gx/gy/gz/temp`
2. 完成 `ssd1306.c`：
   - 初始化
   - 清屏
   - 显示基础字符串
3. 验证：
   - OLED 显示手势状态
   - 串口数据随动作变化

---

## 6. VOFA+ FireWater 调试

1. 串口参数：`115200 8N1`
2. FireWater 每行输出字段：
   - `ax,ay,az,gx,gy,gz,cls`
3. 在 VOFA+ 建立同名通道，观察波形连续性与稳定性。

---

## 7. 手势算法调参（阈值版）

1. 静止采样，做零偏校准。
2. 调整 `gesture_engine.c` 阈值：
   - Static 判定
   - Rotate CW/CCW 判定
   - Swing 判定
3. 目标：达到“可重复稳定触发”。

---

## 8. 闭环控制（最小演示）

1. 定义动作映射：
   - `ROTATE_CW -> VOL_UP`
   - `ROTATE_CCW -> VOL_DOWN`
   - `SWING -> NEXT_PAGE`
2. 先用 UART 文本模拟控制输出，后续再接实际执行器。

---

## 9. PCB 阶段（原理图确认后）

1. 布局原则：
   - I2C 走线短
   - 电源和地优先
   - 传感器远离干扰源
2. 先做一版小板，或继续用面包板完成软件联调。

---

## 10. AI 阶段（最后接入）

1. 先录制并标注动作数据集。
2. 二选一接入：
   - NanoEdge（异常检测）
   - STM32Cube.AI（动作分类）
3. 对比阈值法与 AI 法：准确率、延迟、资源占用。

---

## 当前最优先的 3 件事

1. 先完成最小原理图（STM32 + MPU6050 + OLED + UART）。
2. 完成 `mpu6050.c` 与 `ssd1306.c` 的可运行驱动。
3. 跑 VOFA+ 看波形并完成阈值调参。

---

## 里程碑建议

1. M1（1-2 天）：
   - 工程可编译
   - MPU6050 采样 + OLED + FireWater 输出
2. M2（2-3 天）：
   - 阈值手势识别稳定
   - 三个动作可重复触发
3. M3（3-5 天）：
   - AI 接入（NanoEdge 或 Cube.AI）
   - 完成效果对比
4. M4（1-2 天）：
   - 闭环控制完善
   - 整体演示验收
