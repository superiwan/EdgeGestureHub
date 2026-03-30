# NanoEdge AI 全程操作文档（EdgeGestureHub 项目版）

本文档用于说明本项目从 NanoEdge AI Studio 到 STM32 工程落地的完整流程，重点解释：

- 检测训练中（Learning In Progress）代码如何写
- 检测训练完成后（Learning Done）代码如何写
- 两者核心区别是什么

---

## 1. NanoEdge AI Studio 客户端操作步骤

### 1.1 新建工程（Project Settings）

1. 打开 NanoEdge AI Studio，新建项目。
2. Target 选择 `STM32F407G-DISC1`（与你工程一致）。
3. Memory footprint 设为你的目标约束（本项目为 32KB RAM / 512KB Flash）。
4. Sensor 选 `Generic`，轴数为 `6 axes`（ax, ay, az, gx, gy, gz）。

### 1.2 导入信号（Signals）

1. 准备 CSV：每行 6 列（不要带 cls 列）。
2. `Regular signals` 导入静止数据（nominal）。
3. `Abnormal signals` 导入动作数据（anomaly）。
4. 注意：若文件无表头，`Ignore first header line` 关闭。

### 1.3 Benchmark

1. 进入 Benchmark 页面，先用默认搜索空间跑第一轮。
2. 选分数高且 RAM/Flash 满足约束的库。

### 1.4 Validation

1. 勾选候选库，创建新实验。
2. 使用“未参与训练”的新数据验证（Learn/Regular/Abnormal）。
3. 指标通过后进入 Deployment。

### 1.5 Deployment

1. Toolchain 选择与你固件工程一致（本项目最终用 ARM/Keil 版本库）。
2. 导出压缩包（包含 `libneai.a`、`NanoEdgeAI.h`、`metadata.json`）。

---

## 2. 如何合并 NanoEdge 导出的压缩包到工程

以当前工程为例：

1. 将导出的库目录放在工程根目录，例如：
   - `libneai_project-2026-03-15-16-00_3/`
2. 复制头文件到工程包含目录：
   - `libneai.../NanoEdgeAI.h` -> `cubemx/EdgeGestureHub/Core/Inc/NanoEdgeAI.h`
3. 在 Keil 工程链接参数中加入静态库路径：
   - `..\..\..\libneai_project-2026-03-15-16-00_3\libneai.a`
4. 工程 ABI 要与库匹配（非常关键）：
   - 若链接报 `wchar/enum` 冲突，按库属性对齐。
   - 本项目稳定配置：
     - `EnumInt=1`
     - `vShortWch=0`
     - C 编译 `MiscControls=--wchar32`

说明：ABI 不一致时，常见报错是 `L6242E ... attributes are incompatible`。

---

## 3. 模型部署后，“检测训练中”代码片段（Learning 未完成）

当前工程核心逻辑在：

- `cubemx/EdgeGestureHub/Core/Src/gesture_engine.c`

### 3.1 初始化

```c
st = neai_anomalydetection_init(false);
if (st == NEAI_OK) {
  s_neai_ready = true;
}
```

`false` 表示 on-board training：上电后在设备端学习正常模式。

### 3.2 训练中分支（学习阶段）

```c
if (!s_neai_learning_done) {
  st = neai_anomalydetection_learn(input_signal);
  if (st == NEAI_LEARNING_DONE) {
    s_neai_learning_done = true;
  } else if (st != NEAI_LEARNING_IN_PROGRESS) {
    s_neai_ready = false;
    return gesture_classify_fallback(frame);
  }
  s_class_code = 0;         // LEARN
  return GESTURE_UNKNOWN;
}
```

这段代码的行为：

1. 调用 `neai_anomalydetection_learn()` 持续学习正常分布。
2. 训练未完成时输出学习态：`cls=0` / OLED 显示 `LEARN`。
3. 不做异常检测判定，不输出 `NORM/ANOM`。

---

## 4. 训练完成后，“真实检测”代码片段（Learning 完成）

### 4.1 检测分支（推理阶段）

```c
st = neai_anomalydetection_detect(input_signal, &s_neai_similarity);
if (st != NEAI_OK) {
  if (st == NEAI_LEARNING_IN_PROGRESS) {
    s_neai_learning_done = false;
    s_class_code = 0;
    return GESTURE_UNKNOWN;
  }
  s_neai_ready = false;
  return gesture_classify_fallback(frame);
}

if (s_neai_similarity < GESTURE_NEAI_NOMINAL_SIMILARITY_TH) {
  s_class_code = 2;         // ANOM
  return GESTURE_SWING;
}
s_class_code = 1;           // NORM
return GESTURE_STATIC;
```

### 4.2 映射到对外状态

```c
int Gesture_GetClassCode(void)
{
  return s_class_code;      // 0=LEARN, 1=NORM, 2=ANOM
}
```

---

## 5. 第3点和第4点代码的核心区别（重点）

### 5.1 调用 API 不同

1. 训练中：`neai_anomalydetection_learn()`
2. 训练后：`neai_anomalydetection_detect()`

### 5.2 输出语义不同

1. 训练中：固定输出学习态（`cls=0`）
2. 训练后：根据 `similarity` 与阈值比较输出 `NORM/ANOM`（`cls=1/2`）

### 5.3 业务作用不同

1. 训练中：更新模型知识，不做最终业务决策
2. 训练后：执行实时判定，驱动 OLED 和 LED 等业务动作

### 5.4 状态转移不同

1. 训练中 -> 训练后：收到 `NEAI_LEARNING_DONE`
2. 训练后 -> 训练中：若检测返回 `NEAI_LEARNING_IN_PROGRESS`，回退继续学习

---

## 6. 本项目当前状态机（简化）

1. 上电：`init(false)`
2. 学习阶段：`learn()`，输出 `LEARN`
3. 完成后：`detect()`，输出 `NORM/ANOM`
4. 异常返回学习请求时：回到学习阶段

---

## 7. 建议的工程实践

1. 把阈值、学习锁时间放在 `app_config.h` 作为宏，便于现场调参。
2. 每次换库（新 zip）先确认 ABI，再编译链接。
3. 先用串口验证 `cls` 稳定，再绑业务动作（LED/继电器等）。
