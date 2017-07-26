#ifndef SM_DAC2_H
#define SM_DAC2_H

#include "Sm_DAC.h"

extern void initDAC2GPIOA5(void);

typedef Sm_DAC<DAC_Channel_2,
               DHR12R2_OFFSET,
               initDAC2GPIOA5,
               DMA2_Channel4_BASE> Sm_DAC2_Base;

class Sm_DAC2 : public Sm_DAC2_Base
{
public:
    static void setDualChannelData(uint32_t DAC_Align_x, uint16_t data2, uint16_t data1)
    {
        Sm_assert(IS_DAC_ALIGN(DAC_Align_x));
        Sm_assert(IS_DAC_DATA(data2));
        Sm_assert(IS_DAC_DATA(data1));

        DAC_SetDualChannelData(DAC_Align_x, data2, data1);
    }

    static void setDualChannelVoltage(uint32_t DAC_Align_x, float voltage2, float voltage1)
    {
        Sm_assert(IS_DAC_ALIGN(DAC_Align_x));

        float rate = (DAC_Align_x == DAC_Align_8b_R ? 0xff : 0x0fff) / mRefVoltage;
        uint16_t data2 = (uint16_t)(voltage2 * rate + 0.5f);
        uint16_t data1 = (uint16_t)(voltage1 * rate + 0.5f);

        DAC_SetDualChannelData(DAC_Align_x, data2, data1);
    }

    static void DualDMAConfig(uint32_t DAC_Align_x, const uint32_t *buff, uint16_t size)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
        DMA_DeInit(DMA2_Channel4); 

        DMA_InitTypeDef dma_init;
        dma_init.DMA_PeripheralBaseAddr = (uint32_t)DAC_BASE + DHR12RD_OFFSET + DAC_Align_x;
        dma_init.DMA_MemoryBaseAddr = (uint32_t)buff;
        dma_init.DMA_DIR = DMA_DIR_PeripheralDST;
        dma_init.DMA_BufferSize = size;
        dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
        dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
        dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
        dma_init.DMA_Mode = DMA_Mode_Circular;
        dma_init.DMA_Priority = DMA_Priority_High;
        dma_init.DMA_M2M = DMA_M2M_Disable;

        DMA_Init(DMA2_Channel4, &dma_init);
        DMA_Cmd(DMA2_Channel4, ENABLE);

        DAC_DMACmd(DAC_Channel_1, ENABLE);
        DAC_DMACmd(DAC_Channel_2, ENABLE);
    }
};

#endif /* end of include guard: SM_DAC2_H */
