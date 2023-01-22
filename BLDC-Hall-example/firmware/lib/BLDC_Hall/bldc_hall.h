#ifndef __BLDC_HALL_H__
#define __BLDC_HALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bldc_hall_conf.h"

typedef enum {
  DIRECTION_POSITIVE = 0,
  DIRECTION_NEGATIVE = 1
} BLDC_Direction_t;

void BLDC_Init(void);

void BLDC_Start(void);

void BLDC_Stop(void);

// set the PWM duty cycle as pwm/BLDC_PWM_AUTORELOAD
void BLDC_SetPWM(uint16_t pwm);

void BLDC_SetDirection(BLDC_Direction_t direction);

BLDC_Direction_t BLDC_GetDirection(void);

// speed in rev/s
float BLDC_GetSpeed(void);

void BLDC_HALL_TIM_IRQHandler();

#ifdef __cplusplus
}
#endif

#endif  // __BLDC_HALL_H__