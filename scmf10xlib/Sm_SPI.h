#ifndef SM_SPI_H
#define SM_SPI_H

#include "Sm_Config.h"
#include "Sm_Debug.h"

template<uint32_t SPIx, uint32_t RCC_APBxPeriph_SPIx,
    typename Sm_GPIO_NSS, uint16_t GPIO_Pin_NSS,
    typename Sm_GPIO_SCK, uint16_t GPIO_Pin_SCK,
    typename Sm_GPIO_MISO, uint16_t GPIO_Pin_MISO,
    typename Sm_GPIO_MOSI, uint16_t GPIO_Pin_MOSI>
class Sm_SPI
{
public:
    static void SPI_Init(uint16_t SPI_Direction_x = SPI_Direction_2Lines_FullDuplex,
            uint16_t SPI_Mode_x = SPI_Mode_Master,
            uint16_t SPI_FirstBit_x = SPI_FirstBit_MSB,
            uint16_t SPI_CPOL_x = SPI_CPOL_High,
            uint16_t SPI_CPHA_x = SPI_CPHA_2Edge)
    {
        Sm_assert(IS_SPI_DIRECTION_MODE(SPI_Direction_x));
        Sm_assert(IS_SPI_MODE(SPI_Mode_x));
        Sm_assert(IS_SPI_FIRST_BIT(SPI_FirstBit_x));
        Sm_assert(IS_SPI_CPOL(SPI_CPOL_x));
        Sm_assert(IS_SPI_CPHA(SPI_CPHA_x));
        
        //使能 SPI 的 SCK 信号管脚
        Sm_GPIO_SCK::init_IO(GPIO_Pin_SCK, GPIO_Mode_AF_PP);
        //SPI3的 SCK 信号管脚 PB3 与 SWJ_JTAG 有冲突
        //要先打开PB3的时钟复用，然后关闭SWJ_JTAG，才能正常使用SPI3
        if(RCC_APBxPeriph_SPIx == RCC_APB1Periph_SPI3)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
        }

        //使能 SPI 的 MOSI 和 MISO 管脚
        switch(SPI_Direction_x)
        {
            case SPI_Direction_1Line_Tx:
                Sm_GPIO_MOSI::init_IO(GPIO_Pin_MOSI, GPIO_Mode_AF_PP); break;
            case SPI_Direction_1Line_Rx:
                Sm_GPIO_MISO::init_IO(GPIO_Pin_MISO, GPIO_Mode_AF_PP); break;
            default:
                Sm_GPIO_MOSI::init_IO(GPIO_Pin_MOSI, GPIO_Mode_AF_PP);
                Sm_GPIO_MISO::init_IO(GPIO_Pin_MISO, GPIO_Mode_AF_PP);
        }

        if(RCC_APBxPeriph_SPIx == RCC_APB2Periph_SPI1)
            RCC_APB2PeriphClockCmd(RCC_APBxPeriph_SPIx, ENABLE);
        else
            RCC_APB1PeriphClockCmd(RCC_APBxPeriph_SPIx, ENABLE);

        SPI_InitTypeDef spi_init;
        spi_init.SPI_Direction = SPI_Direction_x;                   //默认设置SPI全双工
        spi_init.SPI_Mode = SPI_Mode_x;                             //默认设置SPI工作模式为主SPI
        spi_init.SPI_DataSize = SPI_DataSize_8b;                    //默认8位帧结构
        spi_init.SPI_CPOL = SPI_CPOL_x;                             //选择了串行时钟的稳态:时钟悬空高
        spi_init.SPI_CPHA = SPI_CPHA_x;                             //数据捕获于第二个时钟沿
        spi_init.SPI_NSS = SPI_NSS_Soft;                            //默认NSS信号由软件管理
        spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;   //默认预分频4
        spi_init.SPI_FirstBit = SPI_FirstBit_x;                     //数据传输从MSB位开始
        spi_init.SPI_CRCPolynomial = 7;                             //CRC值计算的多项式
        ::SPI_Init((SPI_TypeDef *)SPIx, &spi_init);                 //根据指定的参数初始化外设 SPIx 寄存器

        SPI_Cmd((SPI_TypeDef *)SPIx, ENABLE);                       //使能SPI外设
    }

    void I2S_Init()
    {

    }

    static void speedConfig(u8 SPI_BaudRatePrescaler_x)
    {
        Sm_assert(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler_x));
        //stm32的SPI时钟不能超过18M，SPI1是接在APB2上的(默认72M)，所以分频要大于等于4 
        Sm_assert(RCC_APBxPeriph_SPIx != RCC_APB2Periph_SPI1 ||
                    SPI_BaudRatePrescaler_x != SPI_BaudRatePrescaler_2);

        ((SPI_TypeDef *)SPIx)->CR1 &= 0XFFC7;
        ((SPI_TypeDef *)SPIx)->CR1 |= SPI_BaudRatePrescaler_x; //设置 SPIx 速度
        SPI_Cmd((SPI_TypeDef *)SPIx, ENABLE);
    }

    static void dataSizeConfig(uint16_t SPI_DataSize_x)
    {
        Sm_assert(IS_SPI_DATASIZE(SPI_DataSize_x));

        SPI_DataSizeConfig((SPI_TypeDef *)SPIx, SPI_DataSize_x);
    }

    static void SSOutputCmd(FunctionalState NewState) 
    {
        Sm_assert(IS_FUNCTIONAL_STATE(NewState));

        if(NewState == ENABLE)
        {
            Sm_GPIO_NSS::init_IO(GPIO_Pin_NSS, GPIO_Pin_NSS);
        }
        SPI_SSOutputCmd((SPI_TypeDef *)SPIx, NewState);
    }

    static void ITConfig(uint8_t SPI_I2S_IT_x, FunctionalState NewState)
    {
        SPI_I2S_ITConfig((SPI_TypeDef *)SPIx, SPI_I2S_IT_x, NewState);
    }

    static inline void DMAConfig()
    {

    }

    static inline FlagStatus getFlagStatus(uint16_t SPI_I2S_FLAG_x)
    {
        Sm_assert(IS_SPI_I2S_GET_FLAG(SPI_I2S_FLAG_x));
        // return SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);

        return ((SPI_TypeDef *)SPIx)->SR & SPI_I2S_FLAG_x ? SET : RESET;
    }

    static inline void sendData(uint16_t data)
    { 
        // SPI_I2S_SendData((SPI_TypeDef *)SPIx, data);
        ((SPI_TypeDef *)SPIx)->DR = data;
    }
    
    static inline uint16_t ReceiveData(void)
    {
        // return SPI_I2S_ReceiveData((SPI_TypeDef *)SPIx);
        return ((SPI_TypeDef *)SPIx)->DR;
    }
};

#endif /* end of include guard: SM_SPI_H */
