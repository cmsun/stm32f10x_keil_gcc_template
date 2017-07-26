#ifndef SM_DAC1_H
#define SM_DAC1_H

#include "Sm_DAC.h"

extern void initDAC1GPIOA4(void);

typedef Sm_DAC<DAC_Channel_1,
               DHR12R1_OFFSET,
               initDAC1GPIOA4,
               DMA2_Channel3_BASE> Sm_DAC1;


#endif /* end of include guard: SM_DAC1_H */
