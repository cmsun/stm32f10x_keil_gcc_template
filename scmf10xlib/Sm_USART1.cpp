#include "Sm_USART1.h"

Sm::USART_IRQ usart1_irq;

void USART1_Init_DefPinRCC(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

void USART1_Init_RemapPinRCC(void)
{
    //USART1的复用Tx引脚为GPIOB的6脚，Rx为GPIOB的7脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
}

extern "C" void USART1_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint16_t recByte = USART_ReceiveData(USART1); 
        if(usart1_irq.pfProcess != NULL)
            usart1_irq.pfProcess(recByte, usart1_irq.pArg);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}
