#include "Sm_TIM3.h"

template<> TIM_TypeDef* const Sm_TIM3::mTIMx = TIM3;

template<> TIM_TypeDef* const Sm_TIM3_PartialRemap::mTIMx = TIM3;

template<> TIM_TypeDef* const Sm_TIM3_FullRemap::mTIMx = TIM3;

Sm::SmCallback TIM3_Callback;   
void *TIM3_CbArg;

extern "C" void TIM3_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        if(TIM3_Callback)
            TIM3_Callback(TIM3_CbArg);
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM3_PWM_DefPin_Init(Sm::PWM_Chl channel)
{
    GPIO_TypeDef *GPIOx;
    uint32_t RCC_APB2Periph_GPIOx;
    uint16_t GPIO_Pin_x;

    if(channel == Sm::PWM_Channel1)
    {
        GPIOx = GPIOA;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
        GPIO_Pin_x = GPIO_Pin_6;
    }
    else if(channel == Sm::PWM_Channel2)
    {
        GPIOx = GPIOA;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
        GPIO_Pin_x = GPIO_Pin_7;
    }
    else if(channel == Sm::PWM_Channel3)
    {
        GPIOx = GPIOB;
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
        GPIO_Pin_x = GPIO_Pin_0;
    }
    else /* if(channel == Sm::PWM_Channel4) */
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

void TIM3_PWM_PartialRemap_Init(Sm::PWM_Chl channel)
{
    uint16_t GPIO_Pin_x;
    uint32_t RCC_APB2Periph_GPIOx;

    if(channel == Sm::PWM_Channel1)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO;
        GPIO_Pin_x = GPIO_Pin_4;
    }
    else if(channel == Sm::PWM_Channel2)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO;
        GPIO_Pin_x = GPIO_Pin_5;
    }
    else if(channel == Sm::PWM_Channel3)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
        GPIO_Pin_x = GPIO_Pin_0;
    }
    else /* if(channel == Sm::PWM_Channel4) */
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
        GPIO_Pin_x =GPIO_Pin_1;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

    if(channel == Sm::PWM_Channel1)
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //关闭JTAG对PB4的占用

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_x;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init);
}

void TIM3_PWM_FullRemap_Init(Sm::PWM_Chl channel)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE); 
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = (uint16_t)(1 << (channel+6));
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio_init);
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

