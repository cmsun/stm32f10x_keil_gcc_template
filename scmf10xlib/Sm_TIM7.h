#ifndef SM_TIM7_H
#define SM_TIM7_H

#include "Sm_TIM.h"

extern Sm::SmCallback TIM7_Callback;   
extern void *TIM7_CbArg;
extern void TIM7_PWM_DefPin_Init(Sm::PWM_Chl);
extern void TIM7_ETR_DefPin_Init(void);

typedef Sm_TIM<TIM7_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM7,
        TIM7_PWM_DefPin_Init,
        TIM7_ETR_DefPin_Init,
        TIM7_IRQn,
        &TIM7_Callback,
        &TIM7_CbArg> Sm_TIM7;

#endif
