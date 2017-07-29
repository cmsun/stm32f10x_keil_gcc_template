#ifndef SM_TIM5_H
#define SM_TIM5_H

#include "Sm_TIM.h"

extern Sm::CALLBACK TIM5_Callback;   
extern void TIM5_PWM_DefPin_Init(Sm::PWM_Channel);
extern void TIM5_Encoder_DefPin_Init(uint16_t);
extern void TIM5_ETR_DefPin_Init(void);

typedef Sm_TIM<TIM5_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM5,
        TIM5_PWM_DefPin_Init,
        TIM5_Encoder_DefPin_Init,
        TIM5_ETR_DefPin_Init,
        TIM5_IRQn,
        &TIM5_Callback> Sm_TIM5;

#endif
