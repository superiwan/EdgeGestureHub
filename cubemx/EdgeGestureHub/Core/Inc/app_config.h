#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * 任务调度周期（毫秒）
 * 采样周期越小，响应越快，但 CPU/串口压力越大。
 */
#define APP_SAMPLE_PERIOD_MS   10U
#define APP_UART_PERIOD_MS     20U
#define APP_OLED_PERIOD_MS     100U

/* NanoEdge 参数（可调）：
 * 1) 启动学习锁：上电后固定学习窗口，避免刚启动就误报。
 * 2) 相似度阈值：越低越敏感（更容易判异常），越高越保守。 */
#define GESTURE_NEAI_STARTUP_LEARN_LOCK_MS   3000U
#define GESTURE_NEAI_NOMINAL_SIMILARITY_TH     80U

/* 预留滤波系数：后续可在 IMU 处理链路中使用。 */
#define FILTER_ALPHA           0.30f

#endif
