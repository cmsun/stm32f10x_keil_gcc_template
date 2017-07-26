#ifndef SM_GPIOF_H
#define SM_GPIOF_H

#include "Sm_GPIO.h"

typedef Sm_GPIO<GPIOF_BASE, RCC_APB2Periph_GPIOF, GPIO_PortSourceGPIOF> Sm_GPIOF;

#endif
