#ifndef SM_DAC_H
#define SM_DAC_H

#include "Sm_Config.h"
#include "Sm_Debug.h"

#define DHR12R1_OFFSET ((uint32_t)0x00000008)
#define DHR12R2_OFFSET ((uint32_t)0x00000014) /* Only applicable for STM32F072 devices */
#define DHR12RD_OFFSET ((uint32_t)0x00000020) /* Only applicable for STM32F072 devices */

#define PI 3.1415926f

template<uint32_t DAC_Channel_x,
         uint32_t DHR12Rx_OFFSET,
         void (*initOutputPin)(void),
         uint32_t DMA2_Channelx>
class Sm_DAC
{
protected:
    static uint32_t mDataAlign;  //数据对齐方式   
    static uint8_t mBitShift;    //如果是12位左对齐，则给DHR12Rx寄存器赋值是在左移4个位，其他对齐方式不左移。
    static float mRefVoltage;    //参考电压
    static float mVoltageRate;   //8位数据时mVoltageRate=0xff/mRefVoltage，12位数据时mVoltageRate=0x0fff/mRefVoltage。

public:
    static void initialize(void)
    {
        Sm_assert(IS_DAC_CHANNEL(DAC_Channel_x));
        Sm_assert(DHR12Rx_OFFSET == DHR12R1_OFFSET || DHR12Rx_OFFSET == DHR12R2_OFFSET);

        initOutputPin();    //初始化对应GPIO

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
        DAC->CR |= (DAC_CR_EN1 << DAC_Channel_x);       //使能DAC通道
        DAC->CR |= (DAC_CR_BOFF1 << DAC_Channel_x);     //禁止输出缓存
    }

    /*
    ********************************************************************************
    *                               setRefVoltage 
    *
    * Description : 如果VSSA和VDDA连接了参考电压，则要用此函数设置DAC参考电压。
    *
    * Arguments   : refvol 参考电压的值。
    *
    * Returns     : none
    *
    * Note(s)     : 如果没有连接VSSA和VDDA，但是PA4或者PA5输出的最大电压不等于3.3V，
    *               则可以使用这个函数校准偏差。校准方法是:
    *               先用setAlign(DAC_Channel_1, DAC_Align_12b_R)设置成12位右对齐，
    *               然后用setChannel1Data(0x0fff)设置最大值0x0fff，并测量输出的实际电压，
    *               最后调用setRefVoltage()，参数为测量到的电压。校准后使用setVoltage()
    *               输出电压，而不要使用setData()。
    ********************************************************************************
    */
    static void setRefVoltage(float refvol)
    {
        mRefVoltage = refvol;
        mVoltageRate = (mDataAlign == DAC_Align_8b_R ? 0xff : 0x0fff) / mRefVoltage;
    }

    static void setAlign(uint32_t DAC_Align_x)
    {
        Sm_assert(IS_DAC_ALIGN(DAC_Align_x));

        mDataAlign = DAC_Align_x; 
        mBitShift = mDataAlign == DAC_Align_12b_L ? 4 : 0;
        mVoltageRate = (mDataAlign == DAC_Align_8b_R ? 0xff : 0x0fff) / mRefVoltage;
    }

    static void setData(uint16_t data)
    {
        Sm_assert(data <= (uint16_t)(mVoltageRate*mRefVoltage + 0.5f));

        uint32_t tmp = (uint32_t)DAC_BASE; 
        tmp += DHR12Rx_OFFSET + mDataAlign;
        *(volatile uint32_t *)tmp = data << mBitShift;
    }

    /*
    ********************************************************************************
    *                               setVoltage 
    *
    * Description : 设置DAC通道1的输出电压。
    *
    * Arguments   : voltage 通道1的输出电压。输出电压小于等于参考电压，且不能为负数。
    *
    * Returns     : none
    *
    * Note(s)     : none
    ********************************************************************************
    */
    static void setVoltage(float voltage)
    {
        Sm_assert(Sm_Float_LessEqual(voltage, mRefVoltage));

        uint16_t data =  (uint16_t)(voltage*mVoltageRate + 0.5f); 
        uint32_t tmp = (uint32_t)DAC_BASE; 
        tmp += DHR12Rx_OFFSET + mDataAlign;
        *(volatile uint32_t *)tmp = data << mBitShift;
    }
    
    static void triggerConfig(uint32_t DAC_Trigger_x)
    {
        Sm_assert(IS_DAC_TRIGGER(DAC_Trigger_x));

        uint32_t tmpCR = DAC->CR;
        tmpCR &= ~(0x38 << DAC_Channel_x);          //清除TSEL
        tmpCR |= (DAC_Trigger_x << DAC_Channel_x);  //设置TSEL，并使能Trigger
        DAC->CR = tmpCR; 
    }

    #define DAC_DMA_Init(PeripheralDataSize, MemoryDataSize) \
    { \
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); \
        DMA_DeInit((DMA_Channel_TypeDef *)DMA2_Channelx); \
 \
        uint32_t addr = (uint32_t)DAC_BASE + DHR12Rx_OFFSET + mDataAlign; \
 \
        DMA_InitTypeDef dma_init; \
        dma_init.DMA_PeripheralBaseAddr = addr; \
        dma_init.DMA_MemoryBaseAddr = (uint32_t)buff; \
        dma_init.DMA_DIR = DMA_DIR_PeripheralDST; \
        dma_init.DMA_BufferSize = size; \
        dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable; \
        dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable; \
        dma_init.DMA_PeripheralDataSize = PeripheralDataSize; \
        dma_init.DMA_MemoryDataSize = MemoryDataSize; \
        dma_init.DMA_Mode = DMA_Mode_Circular; \
        dma_init.DMA_Priority = DMA_Priority_High; \
        dma_init.DMA_M2M = DMA_M2M_Disable; \
 \
        DMA_Init((DMA_Channel_TypeDef *)DMA2_Channelx, &dma_init); \
        DMA_Cmd((DMA_Channel_TypeDef *)DMA2_Channelx, ENABLE); \
 \
        DAC_DMACmd(DAC_Channel_x, ENABLE); \
    }

    static void DMAConfig(const uint8_t *buff, uint16_t size)
    {
        Sm_assert(DAC_Align_8b_R == mDataAlign);

        DAC_DMA_Init(DMA_PeripheralDataSize_Byte, DMA_MemoryDataSize_Byte);
    }

    static void DMAConfig(const uint16_t *buff, uint16_t size)
    {
        Sm_assert(DAC_Align_12b_R == mDataAlign || DAC_Align_12b_L == mDataAlign);

        DAC_DMA_Init(DMA_PeripheralDataSize_HalfWord, DMA_MemoryDataSize_HalfWord);
    }

    /*
     ********************************************************************************
     *                               computeSine 
     *
     * Description : 生成正弦波数据。
     *
     * Arguments   : buff_: 数据缓存地址
     *               size_: 缓存区大小
     *               amplitude_: 正弦波的振幅，单位是电压伏特
     *
     * Returns     : none
     *
     * Note(s)     : 振幅不能大于参考电压。
     ********************************************************************************
     */
    #define computeSine(buff_, size_, amplitude_) \
    { \
        /* 一个周期(360度)对应的弧度 */ \
        float radian = 360.0f * PI/180.0f; \
        /* 一个细分对应的弧度 */ \
        float stepRadian = radian / size_; \
        float rate = 1.0f/2 * amplitude_ * mVoltageRate; \
        for(int i = 0; i < size_; ++i) \
            buff_[i] = (uint16_t)((sin((float)i*stepRadian)+1.0f) * rate + 0.5f) << mBitShift; \
    }

    static void createSine(uint8_t *buff, uint16_t size, float amplitude)
    {
        Sm_assert(DAC_Align_8b_R == mDataAlign);
        Sm_assert(Sm_Float_LessEqual(amplitude, mRefVoltage));

        computeSine(buff, size, amplitude);
    }

    static void createSine(uint16_t *buff, uint16_t size, float amplitude)
    {
        Sm_assert(DAC_Align_12b_R == mDataAlign || DAC_Align_12b_L == mDataAlign);
        Sm_assert(Sm_Float_LessEqual(amplitude, mRefVoltage));

        computeSine(buff, size, amplitude);
    }

    /*
    ********************************************************************************
    *                               computeEscalator 
    *
    * Description : 创建梯形波数据。
    *
    * Arguments   : buff_: 数据缓存地址
    *               size_: 缓存区大小
    *               amplitude_: 正弦波的振幅，单位是电压伏特
    *
    * Returns     : none
    *
     * Note(s)     : 振幅不能大于参考电压。
    ********************************************************************************
    */
    #define computeEscalator(buff_, size_, amplitude_) \
    { \
        float rate = amplitude / size * mVoltageRate; \
        for(int i = 0; i < size; ++i) \
            buff[i] = (uint16_t)(i*rate + 0.5f) << mBitShift; \
    }

    static void createEscalator(uint8_t *buff, uint16_t size, float amplitude)
    {
        Sm_assert(DAC_Align_8b_R == mDataAlign);
        Sm_assert(Sm_Float_LessEqual(amplitude, mRefVoltage));

        computeEscalator(buff, size, amplitude);
    }

    static void createEscalator(uint16_t *buff, uint16_t size, float amplitude)
    {
        Sm_assert(DAC_Align_12b_R == mDataAlign || DAC_Align_12b_L == mDataAlign);
        Sm_assert(Sm_Float_LessEqual(amplitude, mRefVoltage));

        computeEscalator(buff, size, amplitude);
   }
};

#endif /* end of include guard: SM_DAC_H */
