/**
 * configuration file for BLDC
 *
 */
#ifndef __BLDC_CONF_H__
#define __BLDC_CONF_H__

#include "main.h"

/* PWM timer ports */
#define BLDC_PWM_TIM_PORT TIM1
#define BLDC_PWM1H_GPIO_Port PWM1H_GPIO_Port
#define BLDC_PWM1H_Pin PWM1H_Pin
#define BLDC_PWM2H_GPIO_Port PWM2H_GPIO_Port
#define BLDC_PWM2H_Pin PWM2H_Pin
#define BLDC_PWM3H_GPIO_Port PWM3H_GPIO_Port
#define BLDC_PWM3H_Pin PWM3H_Pin
#define BLDC_PWM1L_GPIO_Port PWM1L_GPIO_Port
#define BLDC_PWM1L_Pin PWM1L_Pin
#define BLDC_PWM2L_GPIO_Port PWM2L_GPIO_Port
#define BLDC_PWM2L_Pin PWM2L_Pin
#define BLDC_PWM3L_GPIO_Port PWM3L_GPIO_Port
#define BLDC_PWM3L_Pin PWM3L_Pin

/* PWM configuration */
// PWM_frequency = PWM_TIM_clock_frequency / (BLDC_PWM_TIM_Prescaler + 1) /
// (BLDC_PWM_TIM_Autoreload + 1)
#define BLDC_PWM_TIM_Prescaler 0
#define BLDC_PWM_TIM_Autoreload 4095

/* PWM mode */
// #define BLDC_PWM_MODE_H_PWM  // high side PWM, low side ON
// #define BLDC_PWM_MODE_L_PWM  // high side ON, low side PWM
#define BLDC_PWM_MODE_FRONT_PWM  // first 60deg PWM, second 60deg ON
// #define BLDC_PWM_MODE_BACK_PWM  // first 60deg ON, second 60deg PWM

#endif  // __BLDC_CONF_H__