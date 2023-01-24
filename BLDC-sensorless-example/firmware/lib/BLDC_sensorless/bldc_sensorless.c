#include "bldc_sensorless.h"

#include <math.h>

/* static variables */
static BLDC_Direction_t _direction = DIRECTION_POSITIVE;

/* private functions declration */
static void BLDC_InitPWMPheripherals(void);

/* function definitions */
static void BLDC_InitPWMPheripherals(void) {
  // init GPIO for PWM outputs
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

  GPIO_InitStruct.Pin = BLDC_PWM1H_Pin;
  LL_GPIO_Init(BLDC_PWM1H_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = BLDC_PWM2H_Pin;
  LL_GPIO_Init(BLDC_PWM2H_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = BLDC_PWM3H_Pin;
  LL_GPIO_Init(BLDC_PWM3H_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = BLDC_PWM1L_Pin;
  LL_GPIO_Init(BLDC_PWM1L_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = BLDC_PWM2L_Pin;
  LL_GPIO_Init(BLDC_PWM2L_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = BLDC_PWM3L_Pin;
  LL_GPIO_Init(BLDC_PWM3L_GPIO_Port, &GPIO_InitStruct);

  // set TIM counting method
  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  TIM_InitStruct.Prescaler = BLDC_PWM_TIM_Prescaler;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = BLDC_PWM_TIM_Autoreload;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(BLDC_PWM_TIM_PORT, &TIM_InitStruct);
  LL_TIM_SetClockSource(BLDC_PWM_TIM_PORT, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_EnableARRPreload(BLDC_PWM_TIM_PORT);

  // set TIM channel 1,2,3 output
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 0;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
  TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
  LL_TIM_OC_Init(BLDC_PWM_TIM_PORT,
                 LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH3,
                 &TIM_OC_InitStruct);
  LL_TIM_OC_EnablePreload(BLDC_PWM_TIM_PORT, LL_TIM_CHANNEL_CH1 |
                                                 LL_TIM_CHANNEL_CH2 |
                                                 LL_TIM_CHANNEL_CH3);

  // set TIM output mode preloaded and updated when generate COM
  LL_TIM_CC_EnablePreload(BLDC_PWM_TIM_PORT);
  LL_TIM_CC_SetUpdate(BLDC_PWM_TIM_PORT, LL_TIM_CCUPDATESOURCE_COMG_ONLY);

  // enable TIM
  LL_TIM_EnableCounter(BLDC_PWM_TIM_PORT);
}

void BLDC_Init(void) { BLDC_InitPWMPheripherals(); }

void BLDC_Start(void) {
  BLDC_SetPWM(300);
  LL_TIM_EnableAllOutputs(BLDC_PWM_TIM_PORT);

  LL_TIM_CC_EnableChannel(BLDC_PWM_TIM_PORT, LL_TIM_CHANNEL_CH1);
  LL_TIM_OC_SetMode(BLDC_PWM_TIM_PORT, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);

  LL_TIM_CC_EnableChannel(BLDC_PWM_TIM_PORT, LL_TIM_CHANNEL_CH2N);
  LL_TIM_OC_SetMode(BLDC_PWM_TIM_PORT, LL_TIM_CHANNEL_CH2,
                    LL_TIM_OCMODE_FORCED_ACTIVE);

  LL_TIM_GenerateEvent_COM(BLDC_PWM_TIM_PORT);
}

void BLDC_Stop(void) { LL_TIM_DisableAllOutputs(BLDC_PWM_TIM_PORT); }

void BLDC_SetPWM(uint16_t pwm) {
  // avoid 0 as compare value, since OCREF will not properly switch to low when
  // entering PWM1 mode from FORCED_ACTIVE mode. See TIM_CCMR1 register OC1M
  // notes.
  if (pwm == 0) pwm++;

  LL_TIM_OC_SetCompareCH1(BLDC_PWM_TIM_PORT, pwm);
  LL_TIM_OC_SetCompareCH2(BLDC_PWM_TIM_PORT, pwm);
  LL_TIM_OC_SetCompareCH3(BLDC_PWM_TIM_PORT, pwm);
}

void BLDC_SetDirection(BLDC_Direction_t dir) { _direction = dir; }

BLDC_Direction_t BLDC_GetDirection(void) { return _direction; }