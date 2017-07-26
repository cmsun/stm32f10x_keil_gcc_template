#ifndef SM_USART2_H
#define SM_USART2_H

#include "Sm_USART.h"

extern Sm::USART_IRQ usart2_irq;
extern void USART2_Init_DefPinRCC(void);
extern void USART2_Init_RemapPinRCC(void);

typedef Sm_USART<USART2_BASE, USART2_IRQn,
                GPIOA_BASE, GPIO_Pin_2,
                GPIOA_BASE, GPIO_Pin_3,
                USART2_Init_DefPinRCC,
                &usart2_irq> Sm_USART2;

typedef Sm_USART<USART2_BASE, USART2_IRQn,
                GPIOD_BASE, GPIO_Pin_5,
                GPIOD_BASE, GPIO_Pin_6,
                USART2_Init_RemapPinRCC,
                &usart2_irq> Sm_USART2_Remap;

#endif
