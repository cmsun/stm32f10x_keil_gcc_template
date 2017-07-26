#ifndef SM_GPIOD_H
#define SM_GPIOD_H

#include "Sm_GPIO.h"

typedef Sm_GPIO<GPIOD_BASE, RCC_APB2Periph_GPIOD, GPIO_PortSourceGPIOD> Sm_GPIOD;

#endif
