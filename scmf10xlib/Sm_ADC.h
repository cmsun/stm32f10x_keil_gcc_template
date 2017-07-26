#ifndef SM_ADC_H
#define SM_ADC_H

#include "Sm_Config.h"
#include "Sm_GPIOA.h"
#include "Sm_GPIOB.h"
#include "Sm_GPIOC.h"
#include "Sm_GPIOF.h"

template<uint32_t ADCx, uint32_t RCC_APB2Periph_ADCx>
class Sm_ADC
{
public:
    Sm_ADC(uint8_t nbrOfChannel,
            FunctionalState scanConvMode = DISABLE,
            FunctionalState continuousConvMode = ENABLE,
            uint32_t externalTrigConv = ADC_ExternalTrigConv_None,
            uint32_t dataAlign = ADC_DataAlign_Right,
            uint32_t mode = ADC_Mode_Independent)
    {
        initialize(nbrOfChannel, scanConvMode, continuousConvMode,
                   externalTrigConv, dataAlign, mode);
    }

    static void initialize(uint8_t nbrOfChannel = 1,
            FunctionalState scanConvMode = DISABLE,
            FunctionalState continuousConvMode = ENABLE,
            uint32_t externalTrigConv = ADC_ExternalTrigConv_None,
            uint32_t dataAlign = ADC_DataAlign_Right,
            uint32_t mode = ADC_Mode_Independent)
    {
        //使能ADC时钟。
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADCx, ENABLE);
        //设置ADC时钟分频因子。ADC时钟ADCCLK大于14M会导致转换不准，
        //所以只能选择6分频和8分频：72M/6=12M, 72/8=9M
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);

        ADC_DeInit((ADC_TypeDef *)ADCx);

        ADC_InitTypeDef adc_init;
        adc_init.ADC_Mode = mode;                               //ADC模式。
        adc_init.ADC_ScanConvMode = scanConvMode;               //是否开启扫描模式(扫描模式用于多通道采集)
        adc_init.ADC_ContinuousConvMode = continuousConvMode;   //是否连续不断转换。
        adc_init.ADC_ExternalTrigConv = externalTrigConv;       //是否使用外部触发转换。
        adc_init.ADC_DataAlign = dataAlign;                     //数据对齐。
        adc_init.ADC_NbrOfChannel = nbrOfChannel;               //要转换的通道数目。
        ADC_Init((ADC_TypeDef *)ADCx, &adc_init);

        ADC_Cmd((ADC_TypeDef *)ADCx, ENABLE);

        ADC_ResetCalibration((ADC_TypeDef *)ADCx);
        while(ADC_GetResetCalibrationStatus((ADC_TypeDef *)ADCx))
            ;
        ADC_StartCalibration((ADC_TypeDef *)ADCx);
        while(ADC_GetCalibrationStatus((ADC_TypeDef *)ADCx))
            ;
    }

    static void regularChannelConfig(uint8_t ADC_Channel_x, uint8_t Rank,
            uint8_t ADC_SampleTime = ADC_SampleTime_239Cycles5)
    {
        if(ADCx == ADC1_BASE || ADCx == ADC2_BASE)
        {
            if(ADC_Channel_x <= ADC_Channel_7)
            {
                Sm_GPIOA::init_IO( 1 << (ADC_Channel_x+1), GPIO_Mode_AIN);
            }
            else if(ADC_Channel_x <= ADC_Channel_9)
            {
                Sm_GPIOB::init_IO( 1 << (ADC_Channel_x-7), GPIO_Mode_AIN);
            }
            else if(ADC_Channel_x <= ADC_Channel_15)
            {
                Sm_GPIOC::init_IO( 1 << (ADC_Channel_x-9), GPIO_Mode_AIN);
            }
            else
            {
                //通道16：芯片温度传感器
                //通道17：内部参照电压
                Sm_assert(ADCx == ADC1_BASE);
            }
        }
        else //ADC3
        {
            Sm_assert(ADC_Channel_x < ADC_Channel_13 && ADC_Channel_x != ADC_Channel_9);

            if(ADC_Channel_x <= ADC_Channel_3)
            {
                Sm_GPIOA::init_IO( 1 << (ADC_Channel_x+1), GPIO_Mode_AIN);
            }
            else if(ADC_Channel_x <= ADC_Channel_8)
            {
                Sm_GPIOF::init_IO( 1 << (ADC_Channel_x+3), GPIO_Mode_AIN);
            }
            else
            {
                Sm_GPIOC::init_IO( 1 << (ADC_Channel_x-9), GPIO_Mode_AIN);
            }
        }

        ADC_RegularChannelConfig((ADC_TypeDef *)ADCx, ADC_Channel_x, Rank, ADC_SampleTime);
    }

    static void softwareStartConvCmd(FunctionalState NewState)
    {
        ADC_SoftwareStartConvCmd((ADC_TypeDef *)ADCx, NewState);
    }

    //当只用一个ADC通道时，使用这个函数进行软件触发ADC转换，并读取转换值。
    static uint16_t softwareGetValue(void)
    {
        ADC_SoftwareStartConvCmd((ADC_TypeDef *)ADCx, ENABLE);
        while(!ADC_GetFlagStatus((ADC_TypeDef *)ADCx, ADC_FLAG_EOC));
        return ADC_GetConversionValue((ADC_TypeDef *)ADCx);
    }

/*
 *     void externalTrigInjectedConvConfig(uint32_t ADC_ExternalTrigInjecConv_x)
 *     {
 *         ADC_ExternalTrigConvCmd(ADCx, ENABLE);
 *         ADC_ExternalTrigInjectedConvConfig(ADC_ExternalTrigInjecConv_x);
 *     }
 */

/*
 *     void softwareGetInjectedValue(FunctionalState NewState)
 *     {
 *         ADC_SoftwareStartInjectedConvCmd(ADCx, NewState);
 *         while(!ADC_GetIn(ADCx, ADC_FLAG_EOC ));
 *     }
 */

    static void regularDMAConfig(uint16_t *buff, uint16_t size)
    {
        Sm_assert(size >= 1 && size <= 16);
        Sm_assert(ADCx == ADC1_BASE || ADCx == ADC3_BASE);

        DMA_InitTypeDef dma_init;
        dma_init.DMA_PeripheralBaseAddr = (uint32_t)&((ADC_TypeDef *)ADCx)->DR;
        dma_init.DMA_MemoryBaseAddr = (uint32_t)buff;
        dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;
        dma_init.DMA_BufferSize = size;
        dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
        dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        dma_init.DMA_Mode = DMA_Mode_Circular;
        dma_init.DMA_Priority = DMA_Priority_High;
        dma_init.DMA_M2M = DMA_M2M_Disable;

        if(ADCx == ADC1_BASE)
        {
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
            DMA_DeInit(DMA1_Channel1);
            DMA_Init(DMA1_Channel1, &dma_init);
            DMA_Cmd(DMA1_Channel1, ENABLE);
        }
        else
        {
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
            DMA_DeInit(DMA2_Channel5);
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
            DMA_DeInit(DMA2_Channel5);
        }

        ADC_DMACmd((ADC_TypeDef *)ADCx, ENABLE); //只有ADC1和ADC3有DMA功能
    }
};

#endif /* end of include guard: SM_ADC_H */
