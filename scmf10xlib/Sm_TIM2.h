#ifndef SM_TIM2_H
#define SM_TIM2_H

#include "Sm_TIM.h"

//定时器2有外部时钟源功能，可以对外部脉冲进行计数
#define EXTRA_CLOCK_MODE2

extern Sm::SmCallback TIM2_Callback;
extern void *TIM2_CbArg;
extern void TIM2_PWM_DefPin_Init(Sm::PWM_Chl);
extern void TIM2_PWM_PartialRemap1_Init(Sm::PWM_Chl);
extern void TIM2_PWM_PartialRemap2_Init(Sm::PWM_Chl);
extern void TIM2_PWM_FullRemap_Init(Sm::PWM_Chl);
extern void TIM2_ETR_DefPin_Init(void);
extern void TIM2_ETR_PartialRemap1_Init(void);
extern void TIM2_ETR_PartialRemap2_Init(void);
extern void TIM2_ETR_FullRemap_Init(void);


typedef Sm_TIM<TIM2_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM2,
        TIM2_PWM_DefPin_Init,
        TIM2_ETR_DefPin_Init,
        TIM2_IRQn,
        &TIM2_Callback,
        &TIM2_CbArg> Sm_TIM2;

typedef Sm_TIM<TIM2_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM2,
        TIM2_PWM_PartialRemap1_Init,
        TIM2_ETR_PartialRemap1_Init,
        TIM2_IRQn,
        &TIM2_Callback,
        &TIM2_CbArg> Sm_TIM2_PartialRemap1;

typedef Sm_TIM<TIM2_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM2,
        TIM2_PWM_PartialRemap2_Init,
        TIM2_ETR_PartialRemap2_Init,
        TIM2_IRQn,
        &TIM2_Callback,
        &TIM2_CbArg> Sm_TIM2_PartialRemap2;

typedef Sm_TIM<TIM2_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM2,
        TIM2_PWM_FullRemap_Init,
        TIM2_ETR_FullRemap_Init,
        TIM2_IRQn,
        &TIM2_Callback,
        &TIM2_CbArg> Sm_TIM2_FullRemap;

#undef EXTRA_CLOCK_MODE2

#endif
