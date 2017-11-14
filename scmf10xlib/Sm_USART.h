#ifndef SM_USART_H
#define SM_USART_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "Sm_Config.h"

namespace Sm{
    typedef void (*OnCom)(uint16_t recByte, void *arg);
    typedef struct USART_IRQ
    {
        OnCom pfProcess;
        void *pArg;
    } USART_IRQ;
}

template<uint32_t USARTx, uint8_t USARTx_IRQn,
         uint32_t GPIO_Tx, uint16_t GPIO_Pin_Tx,
         uint32_t GPIO_Rx, uint16_t GPIO_Pin_Rx,
         void (* USART_InitRCC)(void),
         Sm::USART_IRQ *IRQ_pfun>
class Sm_USART{
public:
    Sm_USART(uint32_t baudRate = 115200,
            uint16_t wordLength = USART_WordLength_8b,
            uint16_t parity = USART_Parity_No,
            uint16_t stopBits = USART_StopBits_1, 
            uint16_t hardWareFlowControl = USART_HardwareFlowControl_None)
    {
        USART_InitRCC();

        GPIO_InitTypeDef gpio_init;
        //USART TX
        gpio_init.GPIO_Pin = GPIO_Pin_Tx;
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init((GPIO_TypeDef *)GPIO_Tx, &gpio_init);
        //USART RX
        gpio_init.GPIO_Pin = GPIO_Pin_Rx;
        gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init((GPIO_TypeDef *)GPIO_Rx, &gpio_init);

        //USART初始化
        USART_InitTypeDef usart_init;
        usart_init.USART_BaudRate = baudRate;
        usart_init.USART_WordLength = wordLength;
        usart_init.USART_Parity = parity;
        usart_init.USART_StopBits = stopBits;
        usart_init.USART_HardwareFlowControl = hardWareFlowControl;
        usart_init.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
        USART_Init((USART_TypeDef *)USARTx, &usart_init);

        USART_ITConfig((USART_TypeDef *)USARTx, USART_IT_RXNE, ENABLE);  //开启串口中断
        USART_Cmd((USART_TypeDef *)USARTx, ENABLE);  //使能串口

        //初始化USART后在发送第一个字符之前要先读取USART_SR寄存器的TC位
        //清除TC位清除，否则第一个数据没发送完成就会被第二个数据覆盖。
        while( !(((USART_TypeDef *)USARTx)->SR & USART_FLAG_TC) )
            ;

        //USART NVIC中断配置
        NVIC_InitTypeDef nvic_init;
        nvic_init.NVIC_IRQChannel = USARTx_IRQn;
        nvic_init.NVIC_IRQChannelPreemptionPriority = Default_USART_NVIC_IRQChannelPreemptionPriority;
        nvic_init.NVIC_IRQChannelSubPriority = Default_USART_NVIC_IRQChannelSubPriority;
        nvic_init.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_init);
    }

    void setNVICPriority(uint8_t PreemptionPriority = Default_USART_NVIC_IRQChannelPreemptionPriority,
                        uint8_t SubPriority = Default_USART_NVIC_IRQChannelSubPriority)
    {
        NVIC_InitTypeDef nvic_init;
        nvic_init.NVIC_IRQChannel = USARTx_IRQn;
        nvic_init.NVIC_IRQChannelPreemptionPriority = PreemptionPriority ;
        nvic_init.NVIC_IRQChannelSubPriority = SubPriority;
        nvic_init.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_init);
    }

    void setComProcess(Sm::OnCom process, void *arg)
    {
        IRQ_pfun->pfProcess = process;
        IRQ_pfun->pArg = arg;
    }

    //如果使用9位字节方式传输，则位8表示当前字节是数据还是地址。
    static void send(uint16_t data)
    {
        /*
         * USART_SendData((USART_TypeDef *)USARTx, data);
         * while(USART_GetFlagStatus((USART_TypeDef *)USARTx, USART_FLAG_TC) != SET)
         *     ;
         */

        ((USART_TypeDef *)USARTx)->DR = (data & (uint16_t)0x01FF);
        while( !(((USART_TypeDef *)USARTx)->SR & USART_FLAG_TC) )
            ;
    }

    static void send(uint8_t *data, size_t length)
    {
        for(size_t i = 0; i < length; ++i)
        {
            ((USART_TypeDef *)USARTx)->DR = data[i];
            while( !(((USART_TypeDef *)USARTx)->SR & USART_FLAG_TC) )
                ;
        }
    }

    static void printf(const char *format, ...)
    {
        char buff[128];
        va_list fmt;

        va_start(fmt, format);
        vsnprintf(buff, 128, format, fmt);
        va_end(fmt);

        send((uint8_t *)buff, strlen(buff));
    }
};

#endif
