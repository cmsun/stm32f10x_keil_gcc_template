#include "Sm_TIM4.h"

template<> TIM_TypeDef* const Sm_TIM4::mTIMx = TIM4;

template<> TIM_TypeDef* const Sm_TIM4_Remap::mTIMx = TIM4;

Sm::SmCallback TIM4_Callback;   
void *TIM4_CbArg;

extern "C" void TIM4_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        if(TIM4_Callback)
            TIM4_Callback(TIM4_CbArg);
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

//如果使用了液晶或者外部SRAM等FSMC设备，初始化FSMC时钟后会使TIM4通道2(PB7)不能输出PWM
void TIM4_PWM_DefPin_Init(Sm::PWM_Chl channel)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = (uint16_t)(1 << (channel+6));
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init);
}

//如果使用了液晶或者外部SRAM等FSMC设备，初始化FSMC时钟后会使TIM4重映射的后的通道3、4
//(PD14、PD15)不能输出PWM。
void TIM4_PWM_Remap_Init(Sm::PWM_Chl channel)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE); 
    GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = (uint16_t)(1 << (channel+12));
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &gpio_init);
}

//TIM4的ETR管脚和FSMC的NBL0管脚是同一个管脚，如果使用了外部SRAM则TIM4无法使用外部时钟功能 
void TIM4_ETR_DefPin_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_0;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &gpio_init);
}

