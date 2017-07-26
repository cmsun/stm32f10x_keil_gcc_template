#ifndef SM_SPI2_H
#define SM_SPI2_H

#include "Sm_SPI.h"
#include "Sm_GPIOB.h"

typedef Sm_SPI<SPI2_BASE, RCC_APB1Periph_SPI2,
        Sm_GPIOB, GPIO_Pin_12,
        Sm_GPIOB, GPIO_Pin_13,
        Sm_GPIOB, GPIO_Pin_14,
        Sm_GPIOB, GPIO_Pin_15> Sm_SPI2;

#endif /* end of include guard: SM_SPI2_H */
