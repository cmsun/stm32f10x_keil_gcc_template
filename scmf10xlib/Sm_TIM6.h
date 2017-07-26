#ifndef SM_TIM6_H
#define SM_TIM6_H

#include "Sm_TIM.h"

extern Sm::SmCallback TIM6_Callback;   
extern void *TIM6_CbArg;
extern void TIM6_PWM_DefPin_Init(Sm::PWM_Chl);
extern void TIM6_ETR_DefPin_Init(void);

typedef Sm_TIM<TIM6_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM6,
        TIM6_PWM_DefPin_Init,
        TIM6_ETR_DefPin_Init,
        TIM6_IRQn,
        &TIM6_Callback,
        &TIM6_CbArg> Sm_TIM6;

#endif
