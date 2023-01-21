#include "bldc_hall.h"

#include <math.h>

/* private defines */
#define HALL_STATE_A ((BLDC_HALLA_Pin >> GPIO_PIN_MASK_POS) & 0xFFFFU)
#define HALL_STATE_B ((BLDC_HALLB_Pin >> GPIO_PIN_MASK_POS) & 0xFFFFU)
#define HALL_STATE_C ((BLDC_HALLC_Pin >> GPIO_PIN_MASK_POS) & 0xFFFFU)

/* static variables */
static BLDC_Direction_t _direction = DIRECTION_POSITIVE;
static const uint32_t _com_table_positive[6] = {
    BLDC_COM_ABC_001, BLDC_COM_ABC_101, BLDC_COM_ABC_100,
    BLDC_COM_ABC_110, BLDC_COM_ABC_010, BLDC_COM_ABC_011};
static uint32_t _com_table_negative[6];
static uint32_t _com_table_positiveH_negativeL[6];
static uint32_t _com_table_negativeH_positiveL[6];
static uint32_t _com_table_positiveFront_negativeBack[6];
static uint32_t _com_table_negativeFront_positiveBack[6];

/* private functions declration */
static void BLDC_InitHallPheripherals(void);
static void BLDC_InitPWMPheripherals(void);
static void BLDC_InitComTable();

static uint8_t BLDC_ReadPosition(void);
static inline uint8_t next_pos(uint8_t pos, BLDC_Direction_t dir);
static void BLDC_PreloadPWMOutput(uint8_t pos, BLDC_Direction_t dir);

/* function definitions */
static void BLDC_InitHallPheripherals(void) {
  // init GPIO for Hall sensors
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = BLDC_HALLA_Pin | BLDC_HALLB_Pin | BLDC_HALLC_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(BLDC_HALL_GPIO_Port, &GPIO_InitStruct);

  // set TIM counting method
  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  TIM_InitStruct.Prescaler =
      0xFFFF;  // TODO: for slower motor speed, this value must be larger to
               // avoid overflowing the counter pass autoreload value
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 0xFFFF;  // fixed to max value
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(BLDC_HALL_TIM_PORT, &TIM_InitStruct);
  LL_TIM_SetClockSource(BLDC_HALL_TIM_PORT, LL_TIM_CLOCKSOURCE_INTERNAL);

  // set TIM to reset on any change of the Hall sensors
  LL_TIM_IC_EnableXORCombination(BLDC_HALL_TIM_PORT);
  LL_TIM_SetTriggerInput(BLDC_HALL_TIM_PORT, LL_TIM_TS_TI1F_ED);
  LL_TIM_SetSlaveMode(BLDC_HALL_TIM_PORT, LL_TIM_SLAVEMODE_RESET);

  // set TIM channel 1 to capture on any change of the Hall sensors
  LL_TIM_IC_InitTypeDef TIM_IC_InitStruct = {0};
  TIM_IC_InitStruct.ICPolarity = LL_TIM_IC_POLARITY_RISING;
  TIM_IC_InitStruct.ICActiveInput = LL_TIM_ACTIVEINPUT_TRC;
  TIM_IC_InitStruct.ICPrescaler = LL_TIM_ICPSC_DIV1;
  TIM_IC_InitStruct.ICFilter = LL_TIM_IC_FILTER_FDIV1;
  LL_TIM_IC_Init(BLDC_HALL_TIM_PORT, LL_TIM_CHANNEL_CH1, &TIM_IC_InitStruct);

  // set TIM trigger output to OC2REF
  LL_TIM_SetTriggerOutput(BLDC_HALL_TIM_PORT, LL_TIM_TRGO_CC1IF);

  // enable TIM
  LL_TIM_EnableCounter(BLDC_HALL_TIM_PORT);
  LL_TIM_CC_EnableChannel(BLDC_HALL_TIM_PORT, LL_TIM_CHANNEL_CH1);
}

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

  // set TIM trigger input to Hall timer trigger output
  LL_TIM_SetTriggerInput(BLDC_PWM_TIM_PORT, BLDC_PWM_TIM_TS);

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

  // set TIM output mode preloaded and updated on trigger input
  LL_TIM_CC_EnablePreload(BLDC_PWM_TIM_PORT);
  LL_TIM_CC_SetUpdate(BLDC_PWM_TIM_PORT, LL_TIM_CCUPDATESOURCE_COMG_AND_TRGI);

  // enable TIM
  LL_TIM_EnableCounter(BLDC_PWM_TIM_PORT);
}

void BLDC_Init(void) {
  BLDC_InitHallPheripherals();
  BLDC_InitPWMPheripherals();
  BLDC_InitComTable();

  LL_TIM_EnableIT_CC1(BLDC_HALL_TIM_PORT);
}

void BLDC_Start(void) {
  LL_TIM_EnableAllOutputs(BLDC_PWM_TIM_PORT);

  BLDC_Direction_t dir = _direction;
  uint8_t pos = BLDC_ReadPosition();
  BLDC_PreloadPWMOutput(pos, dir);
  LL_TIM_GenerateEvent_COM(BLDC_PWM_TIM_PORT);
  BLDC_PreloadPWMOutput(next_pos(pos, dir), dir);
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

static uint32_t _TIM_CHANNEL_Base(uint32_t channel) {
  if (READ_BIT(channel, LL_TIM_CHANNEL_CH1N)) {
    CLEAR_BIT(channel, LL_TIM_CHANNEL_CH1N);
    SET_BIT(channel, LL_TIM_CHANNEL_CH1);
  }
  if (READ_BIT(channel, LL_TIM_CHANNEL_CH2N)) {
    CLEAR_BIT(channel, LL_TIM_CHANNEL_CH2N);
    SET_BIT(channel, LL_TIM_CHANNEL_CH2);
  }
  if (READ_BIT(channel, LL_TIM_CHANNEL_CH3N)) {
    CLEAR_BIT(channel, LL_TIM_CHANNEL_CH3N);
    SET_BIT(channel, LL_TIM_CHANNEL_CH3);
  }
  return channel;
}

static void BLDC_InitComTable(void) {
  uint8_t i;

  // _com_table_negative is derived from _com_table_positive, e.g.
  // (LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2N)   becomes
  // (LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH1N)
  for (i = 0; i < 6; i++) {
    _com_table_negative[i] = _com_table_positive[i];
    if (READ_BIT(_com_table_negative[i],
                 LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N))
      _com_table_negative[i] ^= (LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N);
    if (READ_BIT(_com_table_negative[i],
                 LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N))
      _com_table_negative[i] ^= (LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N);
    if (READ_BIT(_com_table_negative[i],
                 LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH3N))
      _com_table_negative[i] ^= (LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH3N);
  }

  // find high side and low side channels, e.g.
  // (LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2N)   has
  // high side LL_TIM_CHANNEL_CH1, low side LL_TIM_CHANNEL_CH2
  for (i = 0; i < 6; i++) {
    _com_table_positiveH_negativeL[i] =
        READ_BIT(_com_table_positive[i],
                 LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH3);
    _com_table_negativeH_positiveL[i] = _TIM_CHANNEL_Base(READ_BIT(
        _com_table_positive[i],
        LL_TIM_CHANNEL_CH1N | LL_TIM_CHANNEL_CH2N | LL_TIM_CHANNEL_CH3N));
  }

  // find front 60 degree channel and back 60 degree channel, e.g.
  // (LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2N)  with next sequence
  // (LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH3N)  has
  // front 60 degree LL_TIM_CHANNEL_CH1, back 60 degree LL_TIM_CHANNEL_CH2
  for (i = 0; i < 6; i++) {
    uint32_t temp = _com_table_positive[i] &
                    _com_table_positive[next_pos(i, DIRECTION_POSITIVE)];
    _com_table_positiveFront_negativeBack[i] = _TIM_CHANNEL_Base(temp);
    _com_table_negativeFront_positiveBack[i] =
        _TIM_CHANNEL_Base(_com_table_positive[i] ^ temp);
  }
}

// position to be used in _com_table_* index
static uint8_t BLDC_ReadPosition(void) {
  uint32_t hall_state = READ_BIT(LL_GPIO_ReadInputPort(BLDC_HALL_GPIO_Port),
                                 HALL_STATE_A | HALL_STATE_B | HALL_STATE_C);

  uint8_t pos = 0;
  switch (hall_state) {
    case HALL_STATE_C:
      pos = 0;
      break;
    case HALL_STATE_A | HALL_STATE_C:
      pos = 1;
      break;
    case HALL_STATE_A:
      pos = 2;
      break;
    case HALL_STATE_A | HALL_STATE_B:
      pos = 3;
      break;
    case HALL_STATE_B:
      pos = 4;
      break;
    case HALL_STATE_B | HALL_STATE_C:
      pos = 5;
      break;
    default:
      // Hall sensors all on or all off not possible
      break;
  }
  return pos;
}

static inline uint8_t next_pos(uint8_t pos, BLDC_Direction_t dir) {
  return dir == DIRECTION_POSITIVE ? (pos + 1) % 6 : (pos + 5) % 6;
}

static void BLDC_PreloadPWMOutput(uint8_t pos, BLDC_Direction_t dir) {
  LL_TIM_CC_DisableChannel(BLDC_PWM_TIM_PORT,
                           LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N |
                               LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N |
                               LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH3N);
  LL_TIM_CC_EnableChannel(BLDC_PWM_TIM_PORT, dir == DIRECTION_POSITIVE
                                                 ? _com_table_positive[pos]
                                                 : _com_table_negative[pos]);

#if defined(BLDC_PWM_MODE_H_PWM)
  // high side PWM
  LL_TIM_OC_SetMode(BLDC_PWM_TIM_PORT,
                    dir == DIRECTION_POSITIVE
                        ? _com_table_positiveH_negativeL[pos]
                        : _com_table_negativeH_positiveL[pos],
                    LL_TIM_OCMODE_PWM1);
  // low side ON
  LL_TIM_OC_SetMode(BLDC_PWM_TIM_PORT,
                    dir == DIRECTION_POSITIVE
                        ? _com_table_negativeH_positiveL[pos]
                        : _com_table_positiveH_negativeL[pos],
                    LL_TIM_OCMODE_FORCED_ACTIVE);
#elif defined(BLDC_PWM_MODE_L_PWM)
  // high side ON
  LL_TIM_OC_SetMode(BLDC_PWM_TIM_PORT,
                    dir == DIRECTION_POSITIVE
                        ? _com_table_positiveH_negativeL[pos]
                        : _com_table_negativeH_positiveL[pos],
                    LL_TIM_OCMODE_FORCED_ACTIVE);
  // low side PWM
  LL_TIM_OC_SetMode(BLDC_PWM_TIM_PORT,
                    dir == DIRECTION_POSITIVE
                        ? _com_table_negativeH_positiveL[pos]
                        : _com_table_positiveH_negativeL[pos],
                    LL_TIM_OCMODE_PWM1);
#elif defined(BLDC_PWM_MODE_FRONT_PWM)
  // front 60 degrees PWM
  LL_TIM_OC_SetMode(BLDC_PWM_TIM_PORT,
                    dir == DIRECTION_POSITIVE
                        ? _com_table_positiveFront_negativeBack[pos]
                        : _com_table_negativeFront_positiveBack[pos],
                    LL_TIM_OCMODE_PWM1);
  // back 60 degrees ON
  LL_TIM_OC_SetMode(BLDC_PWM_TIM_PORT,
                    dir == DIRECTION_POSITIVE
                        ? _com_table_negativeFront_positiveBack[pos]
                        : _com_table_positiveFront_negativeBack[pos],
                    LL_TIM_OCMODE_FORCED_ACTIVE);
#elif defined(BLDC_PWM_MODE_BACK_PWM)
  // front 60 degrees ON
  LL_TIM_OC_SetMode(BLDC_PWM_TIM_PORT,
                    dir == DIRECTION_POSITIVE
                        ? _com_table_positiveFront_negativeBack[pos]
                        : _com_table_negativeFront_positiveBack[pos],
                    LL_TIM_OCMODE_FORCED_ACTIVE);
  // back 60 degrees PWM
  LL_TIM_OC_SetMode(BLDC_PWM_TIM_PORT,
                    dir == DIRECTION_POSITIVE
                        ? _com_table_negativeFront_positiveBack[pos]
                        : _com_table_positiveFront_negativeBack[pos],
                    LL_TIM_OCMODE_PWM1);
#else
#error "BLDC_PWM_MODE_* not selected"
#endif
}

void BLDC_HALL_TIM_IRQHandler() {
  LL_TIM_ClearFlag_CC1(BLDC_HALL_TIM_PORT);

  BLDC_Direction_t dir = _direction;
  uint8_t pos = BLDC_ReadPosition();
  BLDC_PreloadPWMOutput(pos, dir);
  LL_TIM_GenerateEvent_COM(BLDC_PWM_TIM_PORT);
  BLDC_PreloadPWMOutput(next_pos(pos, dir), dir);
}