#ifndef SM_UART4_H
#define SM_UART4_H

#include "Sm_USART.h"

extern Sm::USART_IRQ uart4_irq;
extern void UART4_Init_RCC(void);

typedef Sm_USART<UART4_BASE, UART4_IRQn,
                GPIOC_BASE, GPIO_Pin_10,
                GPIOC_BASE, GPIO_Pin_11,
                UART4_Init_RCC,
                &uart4_irq> Sm_UART4;

#endif
