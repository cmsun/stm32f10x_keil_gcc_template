#ifndef SM_GPIOE_H
#define SM_GPIOE_H

#include "Sm_GPIO.h"

typedef Sm_GPIO<GPIOE_BASE, RCC_APB2Periph_GPIOE, GPIO_PortSourceGPIOE> Sm_GPIOE;

#endif
