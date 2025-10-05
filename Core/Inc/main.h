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

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define INT1_Pin GPIO_PIN_2
#define INT1_GPIO_Port GPIOE
#define INT2_Pin GPIO_PIN_3
#define INT2_GPIO_Port GPIOE
#define ADC12_INP10_Pin GPIO_PIN_0
#define ADC12_INP10_GPIO_Port GPIOC
#define ADC12_INP12_Pin GPIO_PIN_2
#define ADC12_INP12_GPIO_Port GPIOC
#define ADC12_INP13_Pin GPIO_PIN_3
#define ADC12_INP13_GPIO_Port GPIOC
#define ETH_RMII_REF_CLK_Pin GPIO_PIN_1
#define ETH_RMII_REF_CLK_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define SPI1_CLK_Pin GPIO_PIN_5
#define SPI1_CLK_GPIO_Port GPIOA
#define ETH_RMII_CRS_DV_Pin GPIO_PIN_7
#define ETH_RMII_CRS_DV_GPIO_Port GPIOA
#define ETH_RMII_RXD0_Pin GPIO_PIN_4
#define ETH_RMII_RXD0_GPIO_Port GPIOC
#define ETH_RMII_RXD1_Pin GPIO_PIN_5
#define ETH_RMII_RXD1_GPIO_Port GPIOC
#define PHY_IRQ_Pin GPIO_PIN_7
#define PHY_IRQ_GPIO_Port GPIOE
#define PHY_RX_ER_Pin GPIO_PIN_8
#define PHY_RX_ER_GPIO_Port GPIOE
#define PHY_RESET_Pin GPIO_PIN_10
#define PHY_RESET_GPIO_Port GPIOE
#define PHY_GPIO_Pin GPIO_PIN_11
#define PHY_GPIO_GPIO_Port GPIOE
#define ETH_RMII_TXD0_Pin GPIO_PIN_12
#define ETH_RMII_TXD0_GPIO_Port GPIOB
#define ETH_RMII_TXD1_Pin GPIO_PIN_15
#define ETH_RMII_TXD1_GPIO_Port GPIOB
#define SPI2_CS_Pin GPIO_PIN_11
#define SPI2_CS_GPIO_Port GPIOA
#define SPI2_CLK_Pin GPIO_PIN_12
#define SPI2_CLK_GPIO_Port GPIOA
#define ETH_RMII_TX_EN_Pin GPIO_PIN_11
#define ETH_RMII_TX_EN_GPIO_Port GPIOG
#define WATCHDOG_Pin GPIO_PIN_14
#define WATCHDOG_GPIO_Port GPIOG
#define FAULT_MCU_Pin GPIO_PIN_15
#define FAULT_MCU_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
