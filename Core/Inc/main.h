/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_Pin GPIO_PIN_13
#define KEY_GPIO_Port GPIOC
#define LED3G_Pin GPIO_PIN_14
#define LED3G_GPIO_Port GPIOC
#define LED3R_Pin GPIO_PIN_15
#define LED3R_GPIO_Port GPIOC
#define LED1G_Pin GPIO_PIN_5
#define LED1G_GPIO_Port GPIOA
#define LED1R_Pin GPIO_PIN_6
#define LED1R_GPIO_Port GPIOA
#define LED2G_Pin GPIO_PIN_7
#define LED2G_GPIO_Port GPIOA
#define GSRb0_Pin GPIO_PIN_0
#define GSRb0_GPIO_Port GPIOB
#define GSRb1_Pin GPIO_PIN_1
#define GSRb1_GPIO_Port GPIOB
#define GSRb2_Pin GPIO_PIN_2
#define GSRb2_GPIO_Port GPIOB
#define GSRb10_Pin GPIO_PIN_10
#define GSRb10_GPIO_Port GPIOB
#define GSRb11_Pin GPIO_PIN_12
#define GSRb11_GPIO_Port GPIOB
#define HV_EN_Pin GPIO_PIN_8
#define HV_EN_GPIO_Port GPIOA
#define HEAT_ON_Pin GPIO_PIN_9
#define HEAT_ON_GPIO_Port GPIOA
#define LED2R_Pin GPIO_PIN_10
#define LED2R_GPIO_Port GPIOA
#define GSRb3_Pin GPIO_PIN_3
#define GSRb3_GPIO_Port GPIOB
#define GSRb4_Pin GPIO_PIN_4
#define GSRb4_GPIO_Port GPIOB
#define GSRb5_Pin GPIO_PIN_5
#define GSRb5_GPIO_Port GPIOB
#define GSRb6_Pin GPIO_PIN_6
#define GSRb6_GPIO_Port GPIOB
#define GSRb7_Pin GPIO_PIN_7
#define GSRb7_GPIO_Port GPIOB
#define GSRb8_Pin GPIO_PIN_8
#define GSRb8_GPIO_Port GPIOB
#define GSRb9_Pin GPIO_PIN_9
#define GSRb9_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
