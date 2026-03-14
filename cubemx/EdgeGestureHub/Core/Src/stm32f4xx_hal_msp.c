/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f4xx_hal_msp.c
  * @brief        本文件提供 MSP 初始化
  *               及反初始化代码。
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
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* 私有类型定义 -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* 私有宏定义 ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* 私有宏 -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* 私有变量 ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* 私有函数声明 -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* 外部函数 --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * 初始化全局 MSP。
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* 系统中断初始化*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

