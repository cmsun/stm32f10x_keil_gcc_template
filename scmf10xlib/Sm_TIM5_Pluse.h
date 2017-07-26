#ifndef SM_TIM5_PLUSE_H
#define SM_TIM5_PLUSE_H

#include "Sm_TIM5.h"
#include "Sm_Pluse.h"
#include "Sm_GPIOA.h"

//方向信号
#define TIM5_PLUSE_DIRECTION_CLASS      Sm_GPIOA
#define TIM5_PLUSE_DIRECTION_PIN        GPIO_Pin_1

//原点信号
#define TIM5_PLUSE_HOME_CLASS           Sm_GPIOA
#define TIM5_PLUSE_HOME_PIN             GPIO_Pin_3
#define TIM5_PLUSE_HOME_PINSOURCE       GPIO_PinSource3

//正限位信号
#define TIM5_PLUSE_FORWARDLIMIT_CLASS   Sm_GPIOA
#define TIM5_PLUSE_FORWARDLIMIT_PIN     GPIO_Pin_2

//负限位信号
#define TIM5_PLUSE_REVERSELIMIT_CLASS   Sm_GPIOA
#define TIM5_PLUSE_REVERSELIMIT_PIN     GPIO_Pin_4

//TIM5 CR1寄存器
#define TIM5_CR1_ADDR 0x40000c00

extern Sm::Pluse_t                      TIM5_Pluse;

//脉冲输出：GPIO PA0
typedef Sm_Pluse<Sm_TIM5, Sm::PWM_Channel1, TIM5_CR1_ADDR,
        RCC_AHBPeriph_DMA2, DMA2_Channel2_BASE, DMA2_Channel2_IRQn,
        TIM5_PLUSE_DIRECTION_CLASS, TIM5_PLUSE_DIRECTION_PIN,
        TIM5_PLUSE_HOME_CLASS, TIM5_PLUSE_HOME_PINSOURCE,
        TIM5_PLUSE_FORWARDLIMIT_CLASS, TIM5_PLUSE_FORWARDLIMIT_PIN,
        TIM5_PLUSE_REVERSELIMIT_CLASS, TIM5_PLUSE_REVERSELIMIT_PIN,
        &TIM5_Pluse> Sm_TIM5_Pluse;

#endif