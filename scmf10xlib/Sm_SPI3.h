#ifndef SM_SPI3_H
#define SM_SPI3_H

#include "Sm_SPI.h"
#include "Sm_GPIOA.h"
#include "Sm_GPIOB.h"

typedef Sm_SPI<SPI3_BASE, RCC_APB1Periph_SPI3,
        Sm_GPIOA, GPIO_Pin_15,
        Sm_GPIOB, GPIO_Pin_3,
        Sm_GPIOB, GPIO_Pin_4,
        Sm_GPIOB, GPIO_Pin_5> Sm_SPI3;

#endif /* end of include guard: SM_SPI3_H */
