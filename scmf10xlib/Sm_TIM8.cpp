#include "Sm_TIM8.h"

Sm::CALLBACK TIM8_Callback;   

extern "C" void TIM8_UP_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
    {
        if(TIM8_Callback.pfun)
            TIM8_Callback.pfun(TIM8_Callback.arg);
        TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM8_PWM_DefPin_Init(Sm::PWM_Channel chl)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = (uint16_t)(1 << (chl+6));
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio_init);
}

void TIM8_Encoder_DefPin_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM8_PWM_DefPin_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM8_PWM_DefPin_Init(Sm::PWM_Channel_2);
    else
    {
        TIM8_PWM_DefPin_Init(Sm::PWM_Channel_1);
        TIM8_PWM_DefPin_Init(Sm::PWM_Channel_2);
    }
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
