/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 主程序主体
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* 头文件 ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* 私有头文件 ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_task.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* 私有类型定义 -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* 私有宏定义 ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USER_LED_GPIO_PORT GPIOD
#define USER_LED_GPIO_PIN  GPIO_PIN_12

/* USER CODE END PD */

/* 私有宏 -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* 私有变量 ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* 私有函数声明 -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* 私有用户代码 ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  应用程序入口点。
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU 配置--------------------------------------------------------*/

  /* 复位所有外设，初始化 Flash 接口与 SysTick。 */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* 配置系统时钟 */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* 初始化所有已配置外设 */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  App_Init();
  {
    const char *boot_msg = "EdgeGestureHub boot ok\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)boot_msg, (uint16_t)strlen(boot_msg), 100);
    HAL_GPIO_WritePin(USER_LED_GPIO_PORT, USER_LED_GPIO_PIN, GPIO_PIN_RESET);
  }

  /* USER CODE END 2 */

  /* 无限循环 */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    App_Loop();

    static uint32_t last_tick = 0;
    static uint32_t beat = 0;
    static uint32_t last_pwm_ms = 0;
    static uint32_t last_breath_ms = 0;
    static uint8_t pwm_counter = 0;
    static uint8_t duty = 0;
    static int8_t duty_step = 1;
    uint32_t now = HAL_GetTick();

    if (now - last_pwm_ms >= 1U)
    {
      last_pwm_ms = now;
      pwm_counter = (uint8_t)((pwm_counter + 1U) % 100U);
      if (pwm_counter < duty)
      {
        HAL_GPIO_WritePin(USER_LED_GPIO_PORT, USER_LED_GPIO_PIN, GPIO_PIN_SET);
      }
      else
      {
        HAL_GPIO_WritePin(USER_LED_GPIO_PORT, USER_LED_GPIO_PIN, GPIO_PIN_RESET);
      }
    }

    if (now - last_breath_ms >= 15U)
    {
      last_breath_ms = now;
      if (duty >= 99U)
      {
        duty = 99U;
        duty_step = -1;
      }
      else if (duty == 0U)
      {
        duty_step = 1;
      }
      duty = (uint8_t)(duty + duty_step);
    }

    if (now - last_tick >= 1000U)
    {
      char msg[64];
      int len = snprintf(msg, sizeof(msg), "tick=%lu beat=%lu duty=%u\r\n",
                         (unsigned long)now, (unsigned long)beat++, duty);
      if (len > 0)
      {
        HAL_UART_Transmit(&huart1, (uint8_t *)msg, (uint16_t)len, 100);
      }
      last_tick = now;
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief 系统时钟配置
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 配置主内部稳压器输出电压
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** 根据指定参数初始化 RCC 振荡器
  * （参数位于 RCC_OscInitTypeDef 结构体中）。
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** 初始化 CPU、AHB 与 APB 总线时钟
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  当发生错误时执行此函数。
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* 用户可在此添加自定义实现，用于上报 HAL 错误返回状态 */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  报告源文件名与源码行号
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */











