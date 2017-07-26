#ifndef SM_UART5_H
#define SM_UART5_H

#include "Sm_USART.h"

extern Sm::USART_IRQ uart5_irq;
extern void UART5_Init_RCC(void);

typedef Sm_USART<UART5_BASE, UART5_IRQn,
                GPIOC_BASE, GPIO_Pin_12,
                GPIOD_BASE, GPIO_Pin_2,
                UART5_Init_RCC,
                &uart5_irq> Sm_UART5;

#endif
