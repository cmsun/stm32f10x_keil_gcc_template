#include "Sm_TIM5.h"

template<> TIM_TypeDef* const Sm_TIM5::mTIMx = TIM5;

Sm::SmCallback TIM5_Callback;   
void *TIM5_CbArg;

extern "C" void TIM5_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        if(TIM5_Callback)
            TIM5_Callback(TIM5_CbArg);
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM5_PWM_DefPin_Init(Sm::PWM_Chl channel)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

    if(channel == Sm::PWM_Channel3)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, DISABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, DISABLE);
    }

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = (uint16_t)(1 << channel);
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);
}

void TIM5_ETR_DefPin_Init(void)
{
    //TIM5没有外部时钟功能:w
}
