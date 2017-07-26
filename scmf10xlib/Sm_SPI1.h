#ifndef SM_SPI1_H
#define SM_SPI1_H

#include "Sm_SPI.h"
#include "Sm_GPIOA.h"

typedef Sm_SPI<SPI1_BASE, RCC_APB2Periph_SPI1,
        Sm_GPIOA, GPIO_Pin_4,
        Sm_GPIOA, GPIO_Pin_5,
        Sm_GPIOA, GPIO_Pin_6,
        Sm_GPIOA, GPIO_Pin_7> Sm_SPI1;

#endif /* end of include guard: SM_SPI1_H */
