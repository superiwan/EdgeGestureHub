# NanoEdge AI 操作文档（代码对齐版）

## 1. 当前工程怎么用 NanoEdge
文件：`Core/Src/gesture_engine.c`

- 初始化：`neai_anomalydetection_init(false)`
- 学习阶段：`neai_anomalydetection_learn(...)`
- 检测阶段：`neai_anomalydetection_detect(..., &similarity)`

## 2. 状态语义
- `cls=0` -> `LEARN`
- `cls=1` -> `NORM`
- `cls=2` -> `ANOM`

## 3. 状态切换规则
- 上电后先进入学习锁窗口（默认 3000ms），只学习不报异常。
- 学习完成后开始检测，按相似度阈值判定 `NORM/ANOM`。
- 若检测阶段返回需要继续学习，则回到学习态。

## 4. 业务关联
- 控制层将异常映射为 `GESTURE_SWING` 语义用于触发逻辑。
- 最终业务动作由 `control_logic.c` 执行，不在 NanoEdge 层直接操作 GPIO。

## 5. 兜底机制
- NanoEdge 初始化或调用异常时，会回退到阈值分类，保证系统不断流。

## 6. 调参入口
文件：`Core/Inc/app_config.h`
- `GESTURE_NEAI_STARTUP_LEARN_LOCK_MS`
- `GESTURE_NEAI_NOMINAL_SIMILARITY_TH`

## 7. 现行库路径
- 链接库：`libneai_project-2026-03-15-16-00_3/libneai.a`
- 头文件：`Core/Inc/NanoEdgeAI.h`

## 8. 校验点
- OLED 必须能看到 `LEARN/NORM/ANOM`。
- 串口 `cls` 变化要与实际动作一致。
- `ANOM` 触发后 LED 行为符合冷却和门控规则。
