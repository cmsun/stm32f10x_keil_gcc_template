#ifndef SM_TIM8_H
#define SM_TIM8_H

//声明TIM8是高级定时器,对模板类Sm_TIM进行条件编译
#define ADVANCED_TIMER 1

//定时器8有外部时钟源功能，可以对外部脉冲进行计数
#define EXTRA_CLOCK_MODE2

#include "Sm_TIM.h"

extern Sm::CALLBACK TIM8_Callback;   
extern void TIM8_PWM_DefPin_Init(Sm::PWM_Channel);
extern void TIM8_Encoder_DefPin_Init(uint16_t);
extern void TIM8_ETR_DefPin_Init(void);

typedef Sm_TIM<TIM8_BASE,
        RCC_APB2PeriphClockCmd,
        RCC_APB2Periph_TIM8,
        TIM8_PWM_DefPin_Init,
        TIM8_Encoder_DefPin_Init,
        TIM8_ETR_DefPin_Init,
        TIM8_UP_IRQn,
        &TIM8_Callback> Sm_TIM8;

#undef EXTRA_CLOCK_MODE2

#undef ADVANCED_TIMER

#endif
