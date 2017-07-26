#ifndef SM_TIM1_H
#define SM_TIM1_H

//声明TIM1是高级定时器,对模板类Sm_TIM进行条件编译
#define ADVANCED_TIMER 1

//定时器1有外部时钟源功能，可以对外部脉冲进行计数
#define EXTRA_CLOCK_MODE2

#include "Sm_TIM.h"

extern Sm::SmCallback TIM1_Callback;   
extern void *TIM1_CbArg;
extern void TIM1_PWM_DefPin_Init(Sm::PWM_Chl);
extern void TIM1_PWM_FullRemap_Init(Sm::PWM_Chl);
extern void TIM1_ETR_DefPin_Init(void);
extern void TIM1_ETR_FullRemap_Init(void);

typedef Sm_TIM<TIM1_BASE,
        RCC_APB2PeriphClockCmd,
        RCC_APB2Periph_TIM1,
        TIM1_PWM_DefPin_Init,
        TIM1_ETR_DefPin_Init,
        TIM1_UP_IRQn,
        &TIM1_Callback,
        &TIM1_CbArg> Sm_TIM1;

typedef Sm_TIM<TIM1_BASE,
        RCC_APB2PeriphClockCmd,
        RCC_APB2Periph_TIM1,
        TIM1_PWM_FullRemap_Init,
        TIM1_ETR_FullRemap_Init,
        TIM1_UP_IRQn,
        &TIM1_Callback,
        &TIM1_CbArg> Sm_TIM1_FullRemap;

#undef EXTRA_CLOCK_MODE2

#undef ADVANCED_TIMER

#endif
