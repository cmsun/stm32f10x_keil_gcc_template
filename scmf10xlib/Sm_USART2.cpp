#include "Sm_USART2.h"

Sm::USART_IRQ usart2_irq;

void USART2_Init_DefPinRCC(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

void USART2_Init_RemapPinRCC(void)
{
    //USART2的复用Tx引脚为GPIOD的5脚，Rx为GPIOD的6脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
}

extern "C" void USART2_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint16_t recByte = USART_ReceiveData(USART2); 
        if(usart2_irq.pfProcess != NULL)
            usart2_irq.pfProcess(recByte, usart2_irq.pArg);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}
