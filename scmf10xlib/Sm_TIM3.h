#ifndef SM_TIM3_H
#define SM_TIM3_H

#include "Sm_TIM.h"

//定时器3有外部时钟源功能，可以对外部脉冲进行计数
#define EXTRA_CLOCK_MODE2

extern Sm::CALLBACK TIM3_Callback;   
extern void TIM3_PWM_DefPin_Init(Sm::PWM_Channel);
extern void TIM3_PWM_PartialRemap_Init(Sm::PWM_Channel);
extern void TIM3_PWM_FullRemap_Init(Sm::PWM_Channel);
extern void TIM3_Encoder_DefPin_Init(uint16_t);
extern void TIM3_Encoder_PartialRemap_Init(uint16_t);
extern void TIM3_Encoder_FullRemap_Init(uint16_t);
extern void TIM3_ETR_DefPin_Init(void);

typedef Sm_TIM<TIM3_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM3,
        TIM3_PWM_DefPin_Init,
        TIM3_Encoder_DefPin_Init,
        TIM3_ETR_DefPin_Init,
        TIM3_IRQn,
        &TIM3_Callback> Sm_TIM3;

typedef Sm_TIM<TIM3_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM3,
        TIM3_PWM_PartialRemap_Init,
        TIM3_Encoder_PartialRemap_Init,
        TIM3_ETR_DefPin_Init,
        TIM3_IRQn,
        &TIM3_Callback> Sm_TIM3_PartialRemap;

typedef Sm_TIM<TIM3_BASE,
        RCC_APB1PeriphClockCmd,
        RCC_APB1Periph_TIM3,
        TIM3_PWM_FullRemap_Init,
        TIM3_Encoder_FullRemap_Init,
        TIM3_ETR_DefPin_Init,
        TIM3_IRQn,
        &TIM3_Callback> Sm_TIM3_FullRemap;

#undef EXTRA_CLOCK_MODE2

#endif
