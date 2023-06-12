/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define LCD_CS_Pin GPIO_PIN_1
#define LCD_CS_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_2
#define LCD_DC_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_3
#define LCD_RST_GPIO_Port GPIOC
#define CAMERA_RESET_Pin GPIO_PIN_5
#define CAMERA_RESET_GPIO_Port GPIOC
#define SCL_Pin   GPIO_PIN_10
#define SCL_GPIO_Port GPIOB
#define SDA_Pin  GPIO_PIN_11
#define SDA_GPIO_Port GPIOB
#define VS_Pin    GPIO_PIN_7
#define VS_GPIO_Port GPIOB
#define HS_Pin   GPIO_PIN_4
#define HS_GPIO_PORT GPIOA
#define PCLK_Pin  GPIO_PIN_6
#define PCLK_GPIO_Port GPIOA
#define XCLK_Pin  GPIO_PIN_8
#define XCLK_GPIO_Port GPIOA
#define D0_Pin    GPIO_PIN_6
#define D0_GPIO_Port GPIOC
#define D1_Pin    GPIO_PIN_7
#define D1_GPIO_Port GPIOC
#define D2_Pin    GPIO_PIN_8
#define D2_GPIO_Port GPIOC
#define D3_Pin    GPIO_PIN_9
#define D3_GPIO_Port GPIOC
#define D4_Pin    GPIO_PIN_11
#define D4_GPIO_Port GPIOC
#define D5_Pin    GPIO_PIN_6
#define D5_GPIO_Port GPIOB
#define D6_Pin    GPIO_PIN_8
#define D6_GPIO_Port GPIOB
#define D7_Pin    GPIO_PIN_9
#define D7_GPIO_Port GPIOB
#define ILI9341_SPI SPI1
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
