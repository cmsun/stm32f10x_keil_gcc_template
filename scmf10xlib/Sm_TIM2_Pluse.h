#ifndef SM_TIM2_PLUSE_H
#define SM_TIM2_PLUSE_H

#include "Sm_TIM2.h"
#include "Sm_Pluse.h"
#include "Sm_GPIOA.h"
#include "Sm_GPIOB.h"
#include "Sm_GPIOC.h"

//方向信号
#define TIM2_PLUSE_DIRECTION_CLASS      Sm_GPIOB
#define TIM2_PLUSE_DIRECTION_PIN        GPIO_Pin_3

//原点信号
#define TIM2_PLUSE_HOME_CLASS           Sm_GPIOB
#define TIM2_PLUSE_HOME_PIN             GPIO_Pin_0
#define TIM2_PLUSE_HOME_PINSOURCE       GPIO_PinSource0

//正限位信号
#define TIM2_PLUSE_FORWARDLIMIT_CLASS   Sm_GPIOC
#define TIM2_PLUSE_FORWARDLIMIT_PIN     GPIO_Pin_0

//负限位信号
#define TIM2_PLUSE_REVERSELIMIT_CLASS   Sm_GPIOC
#define TIM2_PLUSE_REVERSELIMIT_PIN     GPIO_Pin_1

//TIM2 CR1寄存器
#define TIM2_CR1_ADDR 0x40000000

extern Sm::Pluse_t                      TIM2_Pluse;

//脉冲输出：GPIO PA15
typedef Sm_Pluse<Sm_TIM2_PartialRemap1, Sm::PWM_Channel1, TIM2_CR1_ADDR,
        RCC_AHBPeriph_DMA1, DMA1_Channel2_BASE, DMA1_Channel2_IRQn,
        TIM2_PLUSE_DIRECTION_CLASS, TIM2_PLUSE_DIRECTION_PIN,
        TIM2_PLUSE_HOME_CLASS, TIM2_PLUSE_HOME_PINSOURCE,
        TIM2_PLUSE_FORWARDLIMIT_CLASS, TIM2_PLUSE_FORWARDLIMIT_PIN,
        TIM2_PLUSE_REVERSELIMIT_CLASS, TIM2_PLUSE_REVERSELIMIT_PIN,
        &TIM2_Pluse> Sm_TIM2_Pluse;

#endif
