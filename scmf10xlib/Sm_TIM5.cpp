#include "Sm_TIM5.h"

Sm::CALLBACK TIM5_Callback;   

extern "C" void TIM5_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        if(TIM5_Callback.pfun)
            TIM5_Callback.pfun(TIM5_Callback.arg);
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM5_PWM_DefPin_Init(Sm::PWM_Channel chl)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

    if(chl == Sm::PWM_Channel_3)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, DISABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, DISABLE);
    }

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = (uint16_t)(1 << chl);
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);
}

void TIM5_Encoder_DefPin_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM5_PWM_DefPin_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM5_PWM_DefPin_Init(Sm::PWM_Channel_2);
    else
    {
        TIM5_PWM_DefPin_Init(Sm::PWM_Channel_1);
        TIM5_PWM_DefPin_Init(Sm::PWM_Channel_2);
    }
}

void TIM5_ETR_DefPin_Init(void)
{
    //TIM5没有外部时钟功能:w
}
