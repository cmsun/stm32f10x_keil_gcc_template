#include "Sm_USART3.h"

Sm::USART_IRQ usart3_irq;

void USART3_Init_DefPinRCC(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
}

void USART3_Init_PartialRemapPinRCC(void)
{
    //USART3的半复用Tx引脚为GPIOC的10脚，Rx为GPIOC的11脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
}

void USART3_Init_FullRemapPinRCC(void)
{
    //USART3的全复用Tx引脚为GPIOD的8脚，Rx为GPIOD的9脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
}

extern "C" void USART3_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        uint16_t recByte = USART_ReceiveData(USART3); 
        if(usart3_irq.pfProcess != NULL)
            usart3_irq.pfProcess(recByte, usart3_irq.pArg);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}
