#ifndef SM_TIM3_PLUSE_H
#define SM_TIM3_PLUSE_H

#include "Sm_TIM3.h"
#include "Sm_Pluse.h"
#include "Sm_GPIOA.h"
#include "Sm_GPIOB.h"

//方向信号
#define TIM3_PLUSE_DIRECTION_CLASS      Sm_GPIOA
#define TIM3_PLUSE_DIRECTION_PIN        GPIO_Pin_7

//原点信号
#define TIM3_PLUSE_HOME_CLASS           Sm_GPIOB
#define TIM3_PLUSE_HOME_PIN             GPIO_Pin_1
#define TIM3_PLUSE_HOME_PINSOURCE       GPIO_PinSource1

//正限位信号
#define TIM3_PLUSE_FORWARDLIMIT_CLASS   Sm_GPIOB
#define TIM3_PLUSE_FORWARDLIMIT_PIN     GPIO_Pin_4

//负限位信号
#define TIM3_PLUSE_REVERSELIMIT_CLASS   Sm_GPIOB
#define TIM3_PLUSE_REVERSELIMIT_PIN     GPIO_Pin_5

//TIM3 CR1寄存器
#define TIM3_CR1_ADDR 0x40000400

extern Sm::Pluse_t                      TIM3_Pluse;

//脉冲输出：GPIO PA6
typedef Sm_Pluse<Sm_TIM3, Sm::PWM_Channel_1, TIM3_CR1_ADDR,
                 RCC_AHBPeriph_DMA1, DMA1_Channel3_BASE, DMA1_Channel3_IRQn,
                 TIM3_PLUSE_DIRECTION_CLASS, TIM3_PLUSE_DIRECTION_PIN,
                 TIM3_PLUSE_HOME_CLASS, TIM3_PLUSE_HOME_PINSOURCE,
                 TIM3_PLUSE_FORWARDLIMIT_CLASS, TIM3_PLUSE_FORWARDLIMIT_PIN,
                 TIM3_PLUSE_REVERSELIMIT_CLASS, TIM3_PLUSE_REVERSELIMIT_PIN,
                 &TIM3_Pluse> Sm_TIM3_Pluse;

#endif
