#include "Sm_TIM4.h"

Sm::CALLBACK TIM4_Callback;   

extern "C" void TIM4_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        if(TIM4_Callback.pfun)
            TIM4_Callback.pfun(TIM4_Callback.arg);
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

//如果使用了液晶或者外部SRAM等FSMC设备，初始化FSMC时钟后会使TIM4通道2(PB7)不能输出PWM
void TIM4_PWM_DefPin_Init(Sm::PWM_Channel chl)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = (uint16_t)(1 << (chl+6));
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init);
}

//如果使用了液晶或者外部SRAM等FSMC设备，初始化FSMC时钟后会使TIM4重映射的后的通道3、4
//(PD14、PD15)不能输出PWM。
void TIM4_PWM_Remap_Init(Sm::PWM_Channel chl)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE); 
    GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = (uint16_t)(1 << (chl+12));
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &gpio_init);
}

void TIM4_Encoder_DefPin_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM4_PWM_DefPin_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM4_PWM_DefPin_Init(Sm::PWM_Channel_2);
    else
    {
        TIM4_PWM_DefPin_Init(Sm::PWM_Channel_1);
        TIM4_PWM_DefPin_Init(Sm::PWM_Channel_2);
    }
}

void TIM4_Encoder_Remap_Init(uint16_t TIM_EncoderMode_x)
{
    if(TIM_EncoderMode_x == TIM_EncoderMode_TI1)
        TIM4_PWM_Remap_Init(Sm::PWM_Channel_1);
    else if(TIM_EncoderMode_x == TIM_EncoderMode_TI2)
        TIM4_PWM_Remap_Init(Sm::PWM_Channel_2);
    else
    {
        TIM4_PWM_Remap_Init(Sm::PWM_Channel_1);
        TIM4_PWM_Remap_Init(Sm::PWM_Channel_2);
    }   
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

