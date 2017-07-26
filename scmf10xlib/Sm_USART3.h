#ifndef SM_USART3_H
#define SM_USART3_H

#include "Sm_USART.h"

extern Sm::USART_IRQ usart3_irq;
extern void USART3_Init_DefPinRCC(void);
extern void USART3_Init_FullRemapPinRCC(void);
extern void USART3_Init_PartialRemapPinRCC(void);

typedef Sm_USART<USART3_BASE, USART3_IRQn,
                GPIOB_BASE, GPIO_Pin_10,
                GPIOB_BASE, GPIO_Pin_11,
                USART3_Init_DefPinRCC,
                &usart3_irq> Sm_USART3;

typedef Sm_USART<USART3_BASE, USART3_IRQn,
                GPIOC_BASE, GPIO_Pin_10,
                GPIOC_BASE, GPIO_Pin_11,
                USART3_Init_PartialRemapPinRCC,
                &usart3_irq> Sm_USART3_PartialRemap;

typedef Sm_USART<USART3_BASE, USART3_IRQn,
                GPIOD_BASE, GPIO_Pin_8,
                GPIOD_BASE, GPIO_Pin_9,
                USART3_Init_FullRemapPinRCC,
                &usart3_irq> Sm_USART3_FullRemap;

#endif
