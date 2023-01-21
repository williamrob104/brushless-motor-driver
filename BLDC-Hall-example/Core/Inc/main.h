/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_utils.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

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
#define BUTTON_RUN_Pin LL_GPIO_PIN_5
#define BUTTON_RUN_GPIO_Port GPIOC
#define BUTTON_RUN_EXTI_IRQn EXTI9_5_IRQn
#define POT_Pin LL_GPIO_PIN_0
#define POT_GPIO_Port GPIOB
#define BUTTON_STOP_Pin LL_GPIO_PIN_1
#define BUTTON_STOP_GPIO_Port GPIOB
#define BUTTON_STOP_EXTI_IRQn EXTI1_IRQn
#define BUTTON_UP_Pin LL_GPIO_PIN_10
#define BUTTON_UP_GPIO_Port GPIOB
#define BUTTON_UP_EXTI_IRQn EXTI15_10_IRQn
#define BUTTON_DOWN_Pin LL_GPIO_PIN_11
#define BUTTON_DOWN_GPIO_Port GPIOB
#define BUTTON_DOWN_EXTI_IRQn EXTI15_10_IRQn
#define BUTTON_DIR_Pin LL_GPIO_PIN_12
#define BUTTON_DIR_GPIO_Port GPIOB
#define BUTTON_DIR_EXTI_IRQn EXTI15_10_IRQn
#define PWM1L_Pin LL_GPIO_PIN_13
#define PWM1L_GPIO_Port GPIOB
#define PWM2L_Pin LL_GPIO_PIN_14
#define PWM2L_GPIO_Port GPIOB
#define PWM3L_Pin LL_GPIO_PIN_15
#define PWM3L_GPIO_Port GPIOB
#define HALLA_Pin LL_GPIO_PIN_6
#define HALLA_GPIO_Port GPIOC
#define HALLB_Pin LL_GPIO_PIN_7
#define HALLB_GPIO_Port GPIOC
#define HALLC_Pin LL_GPIO_PIN_8
#define HALLC_GPIO_Port GPIOC
#define PWM1H_Pin LL_GPIO_PIN_8
#define PWM1H_GPIO_Port GPIOA
#define PWM2H_Pin LL_GPIO_PIN_9
#define PWM2H_GPIO_Port GPIOA
#define PWM3H_Pin LL_GPIO_PIN_10
#define PWM3H_GPIO_Port GPIOA
#define OLED_CS_Pin LL_GPIO_PIN_3
#define OLED_CS_GPIO_Port GPIOB
#define OLED_DC_Pin LL_GPIO_PIN_4
#define OLED_DC_GPIO_Port GPIOB
#define OLED_RES_Pin LL_GPIO_PIN_5
#define OLED_RES_GPIO_Port GPIOB
#define OLED_MOSI_Pin LL_GPIO_PIN_8
#define OLED_MOSI_GPIO_Port GPIOB
#define OLED_CLK_Pin LL_GPIO_PIN_9
#define OLED_CLK_GPIO_Port GPIOB
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0                                   \
  ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority, \
                              4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1                                   \
  ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority, \
                              3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2                                   \
  ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority, \
                              2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3                                   \
  ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority, \
                              1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4                                   \
  ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority, \
                              0 bit  for subpriority */
#endif

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
