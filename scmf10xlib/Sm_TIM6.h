#ifndef SM_TIM6_H
#define SM_TIM6_H

#include "Sm_TIM.h"

extern Sm::CALLBACK TIM6_Callback;   
extern void TIM6_PWM_DefPin_Init(Sm::PWM_Channel);
extern void TIM6_Encoder_DefPin_Init(uint16_t);
extern void TIM6_ETR_DefPin_Init(void);

typedef Sm_TIM<TIM6_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM6,
        TIM6_PWM_DefPin_Init,
        TIM6_Encoder_DefPin_Init,
        TIM6_ETR_DefPin_Init,
        TIM6_IRQn,
        &TIM6_Callback> Sm_TIM6;

#endif
