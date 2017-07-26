#include <map>
#include "Sm_UART5.h"

Sm::USART_IRQ uart5_irq;

void UART5_Init_RCC(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
}

extern "C" void UART5_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        uint16_t recByte = USART_ReceiveData(UART5); 
        if(uart5_irq.pfProcess != NULL)
            uart5_irq.pfProcess(recByte, uart5_irq.pArg);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}
