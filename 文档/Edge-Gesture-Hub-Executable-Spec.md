# Edge-Gesture Hub 可执行开发规格书（I2C + FireWater）

## 1. 项目定义

### 1.1 项目名称
基于边缘AI的嵌入式手持动作识别与智能交互控制器（Edge-Gesture Hub）

### 1.2 一句话描述
基于 STM32F4 的手持动作识别系统，通过 I2C 采集 MPU6050 六轴数据、I2C 驱动 OLED 实时显示、UART 以 FireWater 文本协议输出 VOFA+ 调试数据，并预留 NanoEdge AI Studio 与 STM32Cube.AI 的模型部署接口，实现本地动作识别与闭环控制。

### 1.3 本版约束（已确认）
1. 传感器通信使用 `I2C`（非 SPI）。
2. VOFA+ 输出协议使用 `FireWater` 文本格式。
3. 提供 `可编译工程骨架`（STM32CubeMX + HAL + Keil/STM32CubeIDE 兼容）。

---

## 2. 目标与验收

### 2.1 功能目标
1. 采集 MPU6050 原始数据：`ax/ay/az/gx/gy/gz/temp`。
2. 实现 3 类基础动作识别：`Swing`、`Rotate`、`Static`（可扩展）。
3. OLED 显示当前动作与系统状态。
4. UART 输出 FireWater 文本至 VOFA+ 实时可视化。
5. 提供 AI 扩展接口：
   1. NanoEdge AI 异常检测接口。
   2. STM32Cube.AI 分类模型接口。

### 2.2 性能目标
1. 采样频率：`100 Hz`（10 ms 周期，I2C Fast Mode 400 kHz）。
2. 识别延迟：`< 150 ms`。
3. 串口输出频率：`50 Hz`（降采样输出，减轻串口占用）。
4. 稳定运行：连续运行 `2 小时`无死机。

### 2.3 验收标准
1. 上电后 3 秒内完成初始化并显示 `READY`。
2. VOFA+ 可稳定接收 `ax,ay,az,gx,gy,gz,cls` 七通道数据。
3. 动作识别在定义场景下准确率达到：
   1. 阈值算法基线：`>= 85%`。
   2. AI 模型（后续阶段）：`>= 92%`（以测试集为准）。

---

## 3. 硬件规格

### 3.1 BOM
1. STM32F407 开发板（主控）。
2. MPU6050 模块（六轴，I2C 地址 0x68）。
3. 0.96 寸 SSD1306 OLED（I2C 地址 0x3C）。
4. USB-TTL 模块（CH340/CP2102）。
5. 4.7k 上拉电阻 2 个（SDA/SCL）。

### 3.2 接线定义
1. `PB6 -> I2C1_SCL`（MPU6050 + OLED 共线）。
2. `PB7 -> I2C1_SDA`（MPU6050 + OLED 共线）。
3. `PA9 -> USART1_TX`，`PA10 -> USART1_RX`。
4. 全系统共地，MPU6050/OLED 使用 3.3V。

### 3.3 电气约束
1. I2C 上拉到 3.3V，推荐 4.7k。
2. I2C 线长建议 < 20 cm。
3. 串口默认 115200 8N1。

---

## 4. 软件与工具链

### 4.1 开发工具
1. STM32CubeMX（生成初始化代码）。
2. Keil MDK 或 STM32CubeIDE（编译与烧录）。
3. VOFA+（FireWater 波形调试）。
4. NanoEdge AI Studio（异常检测库生成，后续）。
5. STM32Cube.AI（神经网络模型转换，后续）。

### 4.2 固件框架
1. HAL 驱动层：I2C/UART/GPIO/TIM。
2. BSP 层：MPU6050、SSD1306、串口协议。
3. APP 层：采样、滤波、特征、识别、显示、通信、控制。

---

## 5. 可编译工程骨架（建议目录）

```text
EdgeGestureHub/
  Core/
    Inc/
      app_config.h
      app_task.h
      gesture_engine.h
      firewater.h
      control_logic.h
    Src/
      main.c
      app_task.c
      gesture_engine.c
      firewater.c
      control_logic.c
  Drivers/
    BSP/
      Inc/
        mpu6050.h
        ssd1306.h
      Src/
        mpu6050.c
        ssd1306.c
  Middlewares/
    AI/
      nanoedge_port.h
      cubeai_port.h
  Application/
    Inc/
      ai_service.h
    Src/
      ai_service.c
  Docs/
    Edge-Gesture-Hub-Executable-Spec.md
  Project.uvprojx
  EdgeGestureHub.ioc
```

说明：
1. `Core` 放业务逻辑与系统入口。
2. `Drivers/BSP` 放具体器件驱动。
3. `Middlewares/AI` 放 AI 适配层，避免与业务耦合。
4. `Application` 放策略组合逻辑。

---

## 6. CubeMX 关键配置（STM32F407）

### 6.1 时钟
1. SYSCLK: 168 MHz（或 84/168 MHz，按板卡资源）。
2. APB1/2 分频满足 I2C 与 USART 时序。

### 6.2 外设
1. I2C1: PB6/PB7，400 kHz。
2. USART1: PA9/PA10，115200。
3. TIM6: 1 kHz 基准节拍（软件分频到 100 Hz 采样）。

### 6.3 中断与 DMA
1. 阶段1可先用阻塞式 HAL。
2. 阶段2建议 I2C/UART 增加 DMA，提高吞吐稳定性。

---

## 7. 模块接口规格

### 7.1 传感器模块（mpu6050）
```c
int MPU6050_Init(I2C_HandleTypeDef *hi2c);
int MPU6050_ReadRaw(int16_t acc[3], int16_t gyro[3], int16_t *temp);
int MPU6050_ReadSI(float acc_ms2[3], float gyro_dps[3], float *temp_c);
```

### 7.2 手势引擎（gesture_engine）
```c
typedef enum {
  GESTURE_UNKNOWN = 0,
  GESTURE_STATIC,
  GESTURE_SWING,
  GESTURE_ROTATE_CW,
  GESTURE_ROTATE_CCW
} gesture_t;

typedef struct {
  float ax, ay, az;
  float gx, gy, gz;
  float temp;
} imu_frame_t;

gesture_t Gesture_Run(const imu_frame_t *frame);
const char* Gesture_ToString(gesture_t g);
```

### 7.3 FireWater 协议模块（firewater）
```c
void FireWater_SendFrame(UART_HandleTypeDef *huart,
                         float ax, float ay, float az,
                         float gx, float gy, float gz,
                         int cls);
```

FireWater 输出行格式（每帧一行）：
```text
ax:0.123,ay:-0.456,az:9.801,gx:1.20,gy:-0.80,gz:0.33,cls:2\n
```

### 7.4 控制逻辑（control_logic）
```c
void ControlLogic_OnGesture(gesture_t g);
```

规则示例：
1. `ROTATE_CW` -> 输出 `CTRL:VOL_UP`。
2. `ROTATE_CCW` -> 输出 `CTRL:VOL_DOWN`。
3. `SWING` -> 输出 `CTRL:NEXT_PAGE`。
4. `STATIC` -> 输出 `CTRL:IDLE`。

---

## 8. 主循环时序

1. 1 ms 系统节拍计数。
2. 每 10 ms 读取一次 MPU6050（100 Hz）。
3. 每帧执行：滤波 -> 特征 -> 手势识别。
4. 每 20 ms 输出一次 VOFA+ FireWater（50 Hz）。
5. 每 100 ms 更新一次 OLED。

---

## 9. 算法分阶段

### 9.1 阶段A：阈值基线（当前可落地）
1. 特征：`delta_a`、`|gz|`、窗口均值。
2. 分类：Static / Swing / Rotate CW / Rotate CCW。
3. 参数可在线串口调参。

### 9.2 阶段B：NanoEdge 异常检测
1. 正常动作数据采集 5~10 分钟。
2. 训练 NanoEdge 模型并导出 C 库。
3. 接口：输入固定窗口特征，输出 anomaly score。

### 9.3 阶段C：STM32Cube.AI 分类模型
1. 数据集标注（挥动/旋转/静止）。
2. Python 训练轻量 1D-CNN/MLP。
3. 量化并转换为 Cube.AI 代码。
4. 接口替换 `Gesture_Run` 内核。

---

## 10. 可编译最小代码框架（关键片段）

### 10.1 main.c 主流程
```c
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();

  App_Init();

  while (1) {
    App_Loop();
  }
}
```

### 10.2 app_task.c 调度
```c
void App_Loop(void)
{
  static uint32_t t_sample = 0, t_uart = 0, t_oled = 0;
  uint32_t now = HAL_GetTick();

  if (now - t_sample >= 10) {
    t_sample = now;
    imu_frame_t f;
    if (IMU_ReadFrame(&f) == 0) {
      g_last = Gesture_Run(&f);
      ControlLogic_OnGesture(g_last);
      g_cache = f;
    }
  }

  if (now - t_uart >= 20) {
    t_uart = now;
    FireWater_SendFrame(&huart1, g_cache.ax, g_cache.ay, g_cache.az,
                        g_cache.gx, g_cache.gy, g_cache.gz, (int)g_last);
  }

  if (now - t_oled >= 100) {
    t_oled = now;
    OLED_ShowGesture(Gesture_ToString(g_last));
  }
}
```

---

## 11. 联调规范

### 11.1 VOFA+ 配置
1. 串口：115200 8N1。
2. 协议：FireWater。
3. 通道键名：`ax ay az gx gy gz cls`。

### 11.2 校准流程
1. 静止 3 秒采集零偏。
2. 写入 RAM 偏置并在运行时扣除。
3. 观测 `ax/ay/az` 静止状态是否接近 `(0, 0, 9.8)`。

### 11.3 调试输出级别
1. `INFO`: 系统状态。
2. `DATA`: FireWater 数据帧。
3. `ERR`: I2C/UART/OLED 错误。

---

## 12. 风险与对策

1. I2C 带宽瓶颈：
   1. 对策：采样与显示分频，必要时上 DMA。
2. MPU6050 漂移与噪声：
   1. 对策：静态校准 + 一阶低通滤波。
3. 阈值模型泛化差：
   1. 对策：尽快进入 NanoEdge/Cube.AI 阶段。
4. 串口拥塞丢帧：
   1. 对策：输出限频到 50 Hz，日志分级可关。

---

## 13. 里程碑计划

1. M1（1-2天）：
   1. 工程骨架可编译。
   2. MPU6050 读取 + OLED 显示 + UART FireWater。
2. M2（2-3天）：
   1. 阈值识别 + 参数调优 + 用例联调。
3. M3（3-5天）：
   1. NanoEdge 或 Cube.AI 接入与对比测试。
4. M4（1-2天）：
   1. 闭环控制策略完善，验收报告输出。

---

## 14. 交付件清单

1. `EdgeGestureHub.ioc`。
2. `Keil/CubeIDE 工程`（可编译）。
3. `BSP 驱动源码`（MPU6050/SSD1306）。
4. `手势算法模块`（阈值版 + AI接口）。
5. `FireWater 串口协议模块`。
6. `本规格书`。

---

## 15. 版本记录

1. `v1.0`（2026-03-10）：合并原三份文档，统一为 I2C + FireWater + 可编译工程骨架执行规格。
