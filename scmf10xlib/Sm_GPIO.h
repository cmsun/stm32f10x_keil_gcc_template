/*
 * ODR，输出数据寄存器(可读可写)：既能控制管脚为高电平，也能控制管脚为低电平。
 * 写1则GPIO管脚对应位输出高电平，写0输出低电平。低16位使用，高16位保留。
 * 
 * IDR，输入数据寄存器(只读)：每个APB2时钟周期捕捉I/O管脚上的数据。IDR反映IO管脚
 * 的真实状态：比如GPIOA的ODR设置Bit0为1使GPIOA的Pin0输出高电平，但对应管脚被接地。
 * 此时读取ODR的Bit0得到的值为1说明GPIOA的Pin0输出了高电平，读取IDR得到的值为0说明
 * 当前管脚的真实电平为低电平。低16位使用，高16位保留。
 *
 * BSRR, 32位置位/复位寄存器(只写)：既能控制管脚为高电平，也能控制管脚为低电平。
 * 对寄存器低16bit写1对应管脚为高电平，对寄存器高 16bit 写1对应管脚为低电平。写0无动作。
 * 
 * BRR，16位复位寄存器(只写)：只能改变管脚状态为低电平，对寄存器管脚对应位写 1 相
 * 应管脚会为低电平。写 0 无动作。低16位使用，高16位保留。
 */

#ifndef SM_GPIO_H
#define SM_GPIO_H

#include "stm32f10x.h"
#include "Sm_Config.h"
#include "Sm_Debug.h"

extern Sm::CALLBACK EXTI_IRQ_Table[];

#define GPIO_TEMPLATE_PARAMETER uint32_t GPIOx_BASE, uint32_t RCC_APB2Periph_GPIOx, uint8_t GPIO_PortSourceGPIOx
#define GPIO_CLASS Sm_GPIO<GPIOx_BASE, RCC_APB2Periph_GPIOx, GPIO_PortSourceGPIOx>

template<GPIO_TEMPLATE_PARAMETER>
class Sm_GPIO
{
public:
    Sm_GPIO(uint16_t GPIO_Pin_x,
            GPIOMode_TypeDef GPIO_Mode_x = GPIO_Mode_Out_PP,
            GPIOSpeed_TypeDef GPIO_Speed_x = GPIO_Speed_50MHz);
    static void init_IO(uint16_t GPIO_Pin_x,
            GPIOMode_TypeDef GPIO_Mode_x = GPIO_Mode_Out_PP,
            GPIOSpeed_TypeDef GPIO_Speed_x = GPIO_Speed_50MHz);
    static GPIO_TypeDef *GPIOx(void);
    static void setBits(uint16_t GPIO_Pin_x);
    static void resetBits(uint16_t GPIO_Pin_x);
    static void writeBit(uint16_t GPIO_Pin_x, BitAction bitaction);
    static void toggleOutputBit(uint16_t GPIO_Pin_x);
    static BitAction readOutputDataBit(uint16_t GPIO_Pin_x);
    static BitAction readInputDataBit(uint16_t GPIO_Pin_x);
    static void writePort(uint16_t portVal);
    static uint16_t readOutputData(void);
    static uint16_t readInputData(void);
    static void init_EXTI(uint8_t GPIO_PinSourcex,
            EXTITrigger_TypeDef EXTI_Trigger_x = EXTI_Trigger_Falling,
            EXTIMode_TypeDef EXTI_Mode_x = EXTI_Mode_Interrupt);
    static void setNVICPriority(uint8_t GPIO_PinSourcex,
            uint8_t NVIC_IRQChannelPreemptionPriority,
            uint8_t NVIC_IRQChannelSubPriority);
    static void ITConfig(uint8_t GPIO_PinSourcex, Sm::FunctionalState NewState);
    static void setITCallback(uint8_t GPIO_PinSourcex, void (*pfun)(void *), void *arg);
};

template<GPIO_TEMPLATE_PARAMETER>
GPIO_CLASS::Sm_GPIO(uint16_t GPIO_Pin_x,
        GPIOMode_TypeDef GPIO_Mode_x,
        GPIOSpeed_TypeDef GPIO_Speed_x)
{
    init_IO(GPIO_Pin_x, GPIO_Mode_x, GPIO_Speed_x); 
}

template<GPIO_TEMPLATE_PARAMETER>
void GPIO_CLASS::init_IO(uint16_t GPIO_Pin_x,
        GPIOMode_TypeDef GPIO_Mode_x,
        GPIOSpeed_TypeDef GPIO_Speed_x)
{
    Sm_assert(IS_GPIO_PIN(GPIO_Pin_x));
    Sm_assert(IS_GPIO_MODE(GPIO_Mode_x));
    Sm_assert(IS_GPIO_SPEED(GPIO_Speed_x));

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);

    GPIO_InitTypeDef init;
    init.GPIO_Pin = GPIO_Pin_x;
    init.GPIO_Speed = GPIO_Speed_x;
    init.GPIO_Mode = GPIO_Mode_x;
    GPIO_Init(GPIOx(), &init);
}

template<GPIO_TEMPLATE_PARAMETER>
inline GPIO_TypeDef *GPIO_CLASS::GPIOx(void)
{
    return (GPIO_TypeDef *)GPIOx_BASE;
}

template<GPIO_TEMPLATE_PARAMETER>
inline void GPIO_CLASS::setBits(uint16_t GPIO_Pin_x)
{
    Sm_assert(IS_GPIO_PIN(GPIO_Pin_x));

    // GPIO_SetBits(GPIOx(), GPIO_Pin_x);
    GPIOx()->BSRR = GPIO_Pin_x;
}

template<GPIO_TEMPLATE_PARAMETER>
inline void GPIO_CLASS::resetBits(uint16_t GPIO_Pin_x)
{
    Sm_assert(IS_GPIO_PIN(GPIO_Pin_x));

    // GPIO_ResetBits(GPIOx(), GPIO_Pin_x);
    GPIOx()->BRR = GPIO_Pin_x;
}

template<GPIO_TEMPLATE_PARAMETER>
inline void GPIO_CLASS::writeBit(uint16_t GPIO_Pin_x, BitAction bitaction)
{
    Sm_assert(IS_GPIO_PIN(GPIO_Pin_x));
    Sm_assert(IS_GPIO_BIT_ACTION(bitaction));

    // BIT_ADDR(GPIOx_ODR, n) = bitVal;     //n的值为0到15
    // GPIO_WriteBit(GPIOx(), GPIO_Pin, bitVal);

    if (bitaction != Bit_RESET)
        GPIOx()->BSRR = GPIO_Pin_x;
    else
        GPIOx()->BRR = GPIO_Pin_x;
}

template<GPIO_TEMPLATE_PARAMETER>
inline void GPIO_CLASS::toggleOutputBit(uint16_t GPIO_Pin_x)
{
    GPIOx()->ODR ^= GPIO_Pin_x;
}

template<GPIO_TEMPLATE_PARAMETER>
inline BitAction GPIO_CLASS::readOutputDataBit(uint16_t GPIO_Pin_x)
{
    Sm_assert(IS_GPIO_PIN(GPIO_Pin_x));

    // return GPIO_ReadOutputDataBit(GPIOx(), GPIO_Pin_x) ? Bit_SET : Bit_RESET;
    return GPIOx()->ODR & GPIO_Pin_x ? Bit_SET : Bit_RESET;
}

template<GPIO_TEMPLATE_PARAMETER>
inline BitAction GPIO_CLASS::readInputDataBit(uint16_t GPIO_Pin_x)
{
    Sm_assert(IS_GPIO_PIN(GPIO_Pin_x));

    // return GPIO_ReadInputDataBit(GPIOx(), GPIO_Pin_x) ? Bit_SET : Bit_RESET;
    return GPIOx()->IDR & GPIO_Pin_x ? Bit_SET : Bit_RESET;
}

template<GPIO_TEMPLATE_PARAMETER>
inline void GPIO_CLASS::writePort(uint16_t portVal)
{
    // GPIO_Write(GPIOx(), portVal);
    GPIOx()->ODR = portVal;
}

template<GPIO_TEMPLATE_PARAMETER>
inline uint16_t GPIO_CLASS::readOutputData(void)
{
    return GPIOx()->ODR;
}

template<GPIO_TEMPLATE_PARAMETER>
inline uint16_t GPIO_CLASS::readInputData(void)
{
    return GPIOx()->IDR;
}

template<GPIO_TEMPLATE_PARAMETER>
void GPIO_CLASS::init_EXTI(uint8_t GPIO_PinSourcex,
        EXTITrigger_TypeDef EXTI_Trigger_x,
        EXTIMode_TypeDef EXTI_Mode_x)
{
    Sm_assert(IS_GPIO_PIN_SOURCE(GPIO_PinSourcex));
    Sm_assert(IS_EXTI_TRIGGER(EXTI_Trigger_x));
    Sm_assert(IS_EXTI_MODE(EXTI_Mode_x));

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOx, GPIO_PinSourcex);

    EXTI_InitTypeDef exti_init;
    exti_init.EXTI_Line = (1 << GPIO_PinSourcex);
    exti_init.EXTI_Mode = EXTI_Mode_x;
    exti_init.EXTI_Trigger = EXTI_Trigger_x;
    exti_init.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_init);

    setNVICPriority(GPIO_PinSourcex,
            Default_EXTI_NVIC_IRQChannelPreemptionPriority,
            Default_EXTI_NVIC_IRQChannelSubPriority);
}

template<GPIO_TEMPLATE_PARAMETER>
void GPIO_CLASS::setNVICPriority(uint8_t GPIO_PinSourcex,
        uint8_t NVIC_IRQChannelPreemptionPriority,
        uint8_t NVIC_IRQChannelSubPriority)
{
    Sm_assert(IS_GPIO_PIN_SOURCE(GPIO_PinSourcex));

    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = (GPIO_PinSourcex <= 4
            ? GPIO_PinSourcex + 6
            : (GPIO_PinSourcex <= 9 ? EXTI9_5_IRQn : EXTI15_10_IRQn));
    nvic_init.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
    nvic_init.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);                      
}

template<GPIO_TEMPLATE_PARAMETER>
void GPIO_CLASS::ITConfig(uint8_t GPIO_PinSourcex, Sm::FunctionalState NewState)
{
    Sm_assert(IS_GPIO_PIN_SOURCE(GPIO_PinSourcex));

    uint8_t IRQChannel = GPIO_PinSourcex <= 4
        ? GPIO_PinSourcex + 6
        : (GPIO_PinSourcex <= 9 ? EXTI9_5_IRQn : EXTI15_10_IRQn);

    if(NewState != DISABLE)
        NVIC->ISER[IRQChannel>>0x05] = (uint32_t)0x01<<(IRQChannel & (uint8_t)0x1F);
    else
        NVIC->ICER[IRQChannel>>0x05] = (uint32_t)0x01<<(IRQChannel & (uint8_t)0x1F);
}

template<GPIO_TEMPLATE_PARAMETER>
void GPIO_CLASS::setITCallback(uint8_t GPIO_PinSourcex, void (*pfun)(void *), void *arg)
{
    Sm_assert(IS_GPIO_PIN_SOURCE(GPIO_PinSourcex));

    EXTI_IRQ_Table[GPIO_PinSourcex].pfun = pfun;
    EXTI_IRQ_Table[GPIO_PinSourcex].arg = arg;
    //使能GPIO中断。
    ITConfig(GPIO_PinSourcex, ENABLE);
}

#endif
