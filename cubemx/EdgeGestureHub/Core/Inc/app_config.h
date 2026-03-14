#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * 任务调度周期（毫秒）
 * 采样周期越小，响应越快，但 CPU/串口压力越大。
 */
#define APP_SAMPLE_PERIOD_MS   10U
#define APP_UART_PERIOD_MS     20U
#define APP_OLED_PERIOD_MS     100U

/* 预留滤波系数：后续可在 IMU 处理链路中使用。 */
#define FILTER_ALPHA           0.30f

#endif
