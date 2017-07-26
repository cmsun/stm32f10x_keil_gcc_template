#include "Sm_TIM8.h"

template<> TIM_TypeDef* const Sm_TIM8::mTIMx = TIM8;

Sm::SmCallback TIM8_Callback;   
void *TIM8_CbArg;

extern "C" void TIM8_UP_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
    {
        if(TIM8_Callback)
            TIM8_Callback(TIM8_CbArg);
        TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM8_PWM_DefPin_Init(Sm::PWM_Chl channel)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = (uint16_t)(1 << (channel+6));
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio_init);
}

void TIM8_ETR_DefPin_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_0;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);   
}
