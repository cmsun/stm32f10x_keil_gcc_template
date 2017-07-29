#include "Sm_TIM2.h"

Sm::CALLBACK TIM2_Callback;

extern "C" void TIM2_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        if(TIM2_Callback.pfun)
            TIM2_Callback.pfun(TIM2_Callback.arg);
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM2_PWM_DefPin_Init(Sm::PWM_Channel chl)
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

void TIM2_PWM_PartialRemap1_Init(Sm::PWM_Channel chl)
{
    GPIO_TypeDef *GPIOx;
    uint32_t RCC_APB2Periph_GPIOx;
    uint16_t GPIO_Pin_x;

    if(chl == Sm::PWM_Channel_1)
    {
        GPIOx = GPIOA;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO;
        GPIO_Pin_x = GPIO_Pin_15;
    }
    else if(chl == Sm::PWM_Channel_2)
    {
        GPIOx = GPIOB;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO;
        GPIO_Pin_x = GPIO_Pin_3;
    }
    else if(chl == Sm::PWM_Channel_3)
    {
        GPIOx = GPIOA;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
        GPIO_Pin_x = GPIO_Pin_2;
    }
    else /* if(chl == Sm::PWM_Channel_4) */
    {
        GPIOx = GPIOA;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
        GPIO_Pin_x =GPIO_Pin_3;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);

    if(chl == Sm::PWM_Channel_1 || chl == Sm::PWM_Channel_2)
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //关闭JTAG对PA15和PB3的占用
    if(chl == Sm::PWM_Channel_3)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, DISABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, DISABLE);
    }

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_x;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &gpio_init);
}

void TIM2_PWM_PartialRemap2_Init(Sm::PWM_Channel chl)
{
    GPIO_TypeDef *GPIOx;
    uint32_t RCC_APB2Periph_GPIOx;
    uint16_t GPIO_Pin_x;

    if(chl == Sm::PWM_Channel_1)
    {
        GPIOx = GPIOA;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
        GPIO_Pin_x = GPIO_Pin_0;
    }
    else if(chl == Sm::PWM_Channel_2)
    {
        GPIOx = GPIOA;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
        GPIO_Pin_x = GPIO_Pin_1;
    }
    else if(chl == Sm::PWM_Channel_3)
    {
        GPIOx = GPIOB;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO;
        GPIO_Pin_x = GPIO_Pin_10;
    }
    else /* if(chl == Sm::PWM_Channel_4) */
    {
        GPIOx = GPIOB;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO;
        GPIO_Pin_x =GPIO_Pin_11;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_x;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &gpio_init);
}

void TIM2_PWM_FullRemap_Init(Sm::PWM_Channel chl)
{
    GPIO_TypeDef *GPIOx;
    uint32_t RCC_APB2Periph_GPIOx;
    uint16_t GPIO_Pin_x;

    if(chl == Sm::PWM_Channel_1)
    {
        GPIOx = GPIOA;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
        GPIO_Pin_x = GPIO_Pin_15;
    }
    else if(chl == Sm::PWM_Channel_2)
    {
        GPIOx = GPIOB;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
        GPIO_Pin_x = GPIO_Pin_3;
    }
    else if(chl == Sm::PWM_Channel_3)
    {
        GPIOx = GPIOB;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
        GPIO_Pin_x = GPIO_Pin_10;
    }
    else /* if(chl == Sm::PWM_Channel_4) */
    {
        GPIOx = GPIOB;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
        GPIO_Pin_x =GPIO_Pin_11;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);

    if(chl == Sm::PWM_Channel_1 || chl == Sm::PWM_Channel_2)
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //关闭JTAG对PA15或者PB3的占用

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_x;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &gpio_init);   
}

void TIM2_Encoder_DefPin_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM2_PWM_DefPin_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM2_PWM_DefPin_Init(Sm::PWM_Channel_2);
    else
    {
        TIM2_PWM_DefPin_Init(Sm::PWM_Channel_1);
        TIM2_PWM_DefPin_Init(Sm::PWM_Channel_2);
    }
}

void TIM2_Encoder_FullRemap_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM2_PWM_FullRemap_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM2_PWM_FullRemap_Init(Sm::PWM_Channel_2);
    else
    {
        TIM2_PWM_FullRemap_Init(Sm::PWM_Channel_1);
        TIM2_PWM_FullRemap_Init(Sm::PWM_Channel_2);
    }   
}

void TIM2_Encoder_PartialRemap1_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM2_PWM_PartialRemap1_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM2_PWM_PartialRemap1_Init(Sm::PWM_Channel_2);
    else
    {
        TIM2_PWM_PartialRemap1_Init(Sm::PWM_Channel_1);
        TIM2_PWM_PartialRemap1_Init(Sm::PWM_Channel_2);
    }   
}

void TIM2_Encoder_PartialRemap2_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM2_PWM_PartialRemap2_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM2_PWM_PartialRemap2_Init(Sm::PWM_Channel_2);
    else
    {
        TIM2_PWM_PartialRemap2_Init(Sm::PWM_Channel_1);
        TIM2_PWM_PartialRemap2_Init(Sm::PWM_Channel_2);
    }   
}

void TIM2_ETR_DefPin_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_0;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);
}

void TIM2_ETR_FullRemap_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_15;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);
}

void TIM2_ETR_PartialRemap1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_15;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);
}

void TIM2_ETR_PartialRemap2_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_0;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);
}

