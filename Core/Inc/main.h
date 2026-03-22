/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern IWDG_HandleTypeDef hiwdg;
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IMU_INT1_Pin GPIO_PIN_2
#define IMU_INT1_GPIO_Port GPIOE
#define IMU_INT2_Pin GPIO_PIN_3
#define IMU_INT2_GPIO_Port GPIOE
#define TOF_RST_Pin GPIO_PIN_4
#define TOF_RST_GPIO_Port GPIOE
#define TOF_LPN_Pin GPIO_PIN_5
#define TOF_LPN_GPIO_Port GPIOE
#define MUX1_SEL1_Pin GPIO_PIN_6
#define MUX1_SEL1_GPIO_Port GPIOF
#define MUX1_SEL2_Pin GPIO_PIN_7
#define MUX1_SEL2_GPIO_Port GPIOF
#define MUX1_SEL3_Pin GPIO_PIN_8
#define MUX1_SEL3_GPIO_Port GPIOF
#define MUX1_SEL4_Pin GPIO_PIN_9
#define MUX1_SEL4_GPIO_Port GPIOF
#define PHY_IRQ_Pin GPIO_PIN_7
#define PHY_IRQ_GPIO_Port GPIOE
#define PHY_IRQ_EXTI_IRQn EXTI7_IRQn
#define PHY_RX_ER_Pin GPIO_PIN_8
#define PHY_RX_ER_GPIO_Port GPIOE
#define PHY_RESET_Pin GPIO_PIN_10
#define PHY_RESET_GPIO_Port GPIOE
#define PHY_GPIO_Pin GPIO_PIN_11
#define PHY_GPIO_GPIO_Port GPIOE
#define DEBUG_LED1_Pin GPIO_PIN_3
#define DEBUG_LED1_GPIO_Port GPIOG
#define DEBUG_LED2_Pin GPIO_PIN_4
#define DEBUG_LED2_GPIO_Port GPIOG
#define MUX2_SEL1_Pin GPIO_PIN_6
#define MUX2_SEL1_GPIO_Port GPIOC
#define MUX2_SEL2_Pin GPIO_PIN_7
#define MUX2_SEL2_GPIO_Port GPIOC
#define MUX2_SEL3_Pin GPIO_PIN_8
#define MUX2_SEL3_GPIO_Port GPIOC
#define MUX2_SEL4_Pin GPIO_PIN_9
#define MUX2_SEL4_GPIO_Port GPIOC
#define MSB_ADDR_Pin GPIO_PIN_10
#define MSB_ADDR_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
