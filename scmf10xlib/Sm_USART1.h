#ifndef SM_USART1_H
#define SM_USART1_H

#include "Sm_USART.h"

extern Sm::USART_IRQ usart1_irq;
extern void USART1_Init_DefPinRCC(void);
extern void USART1_Init_RemapPinRCC(void);

typedef Sm_USART<USART1_BASE, USART1_IRQn,
                GPIOA_BASE, GPIO_Pin_9,
                GPIOA_BASE, GPIO_Pin_10,
                USART1_Init_DefPinRCC,
                &usart1_irq> Sm_USART1;

typedef Sm_USART<USART1_BASE, USART1_IRQn,
                GPIOB_BASE, GPIO_Pin_6,
                GPIOB_BASE, GPIO_Pin_7,
                USART1_Init_RemapPinRCC,
                &usart1_irq> Sm_USART1_Remap;

#endif
