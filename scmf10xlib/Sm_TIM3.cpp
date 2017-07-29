#include "Sm_TIM3.h"

Sm::CALLBACK TIM3_Callback;   

extern "C" void TIM3_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        if(TIM3_Callback.pfun)
            TIM3_Callback.pfun(TIM3_Callback.arg);
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM3_PWM_DefPin_Init(Sm::PWM_Channel chl)
{
    GPIO_TypeDef *GPIOx;
    uint32_t RCC_APB2Periph_GPIOx;
    uint16_t GPIO_Pin_x;

    if(chl == Sm::PWM_Channel_1)
    {
        GPIOx = GPIOA;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
        GPIO_Pin_x = GPIO_Pin_6;
    }
    else if(chl == Sm::PWM_Channel_2)
    {
        GPIOx = GPIOA;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
        GPIO_Pin_x = GPIO_Pin_7;
    }
    else if(chl == Sm::PWM_Channel_3)
    {
        GPIOx = GPIOB;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
        GPIO_Pin_x = GPIO_Pin_0;
    }
    else /* if(chl == Sm::PWM_Channel_4) */
    {
        GPIOx = GPIOB;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
        GPIO_Pin_x = GPIO_Pin_1;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_x;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &gpio_init);
}

void TIM3_PWM_PartialRemap_Init(Sm::PWM_Channel chl)
{
    uint16_t GPIO_Pin_x;
    uint32_t RCC_APB2Periph_GPIOx;

    if(chl == Sm::PWM_Channel_1)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO;
        GPIO_Pin_x = GPIO_Pin_4;
    }
    else if(chl == Sm::PWM_Channel_2)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO;
        GPIO_Pin_x = GPIO_Pin_5;
    }
    else if(chl == Sm::PWM_Channel_3)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
        GPIO_Pin_x = GPIO_Pin_0;
    }
    else /* if(chl == Sm::PWM_Channel_4) */
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
        GPIO_Pin_x =GPIO_Pin_1;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

    if(chl == Sm::PWM_Channel_1)
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //关闭JTAG对PB4的占用

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_x;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init);
}

void TIM3_PWM_FullRemap_Init(Sm::PWM_Channel chl)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE); 
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = (uint16_t)(1 << (chl+6));
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio_init);
}

void TIM3_Encoder_DefPin_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM3_PWM_DefPin_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM3_PWM_DefPin_Init(Sm::PWM_Channel_2);
    else
    {
        TIM3_PWM_DefPin_Init(Sm::PWM_Channel_1);
        TIM3_PWM_DefPin_Init(Sm::PWM_Channel_2);
    }
}

void TIM3_Encoder_FullRemap_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM3_PWM_FullRemap_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM3_PWM_FullRemap_Init(Sm::PWM_Channel_2);
    else
    {
        TIM3_PWM_FullRemap_Init(Sm::PWM_Channel_1);
        TIM3_PWM_FullRemap_Init(Sm::PWM_Channel_2);
    }   
}

void TIM3_Encoder_PartialRemap_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM3_PWM_PartialRemap_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM3_PWM_PartialRemap_Init(Sm::PWM_Channel_2);
    else
    {
        TIM3_PWM_PartialRemap_Init(Sm::PWM_Channel_1);
        TIM3_PWM_PartialRemap_Init(Sm::PWM_Channel_2);
    }   
}

void TIM3_ETR_DefPin_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_2;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &gpio_init);
}

