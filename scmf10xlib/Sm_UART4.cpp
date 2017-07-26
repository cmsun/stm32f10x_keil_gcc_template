#include <map>
#include "Sm_UART4.h"

Sm::USART_IRQ uart4_irq;

void UART4_Init_RCC(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
}

extern "C" void UART4_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        uint16_t recByte = USART_ReceiveData(UART4); 
        if(uart4_irq.pfProcess != NULL)
            uart4_irq.pfProcess(recByte, uart4_irq.pArg);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}
