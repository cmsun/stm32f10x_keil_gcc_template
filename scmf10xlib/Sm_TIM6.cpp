#include "Sm_TIM6.h"

Sm::CALLBACK TIM6_Callback;   

extern "C" void TIM6_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        if(TIM6_Callback.pfun)
            TIM6_Callback.pfun(TIM6_Callback.arg);
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM6_PWM_DefPin_Init(Sm::PWM_Channel)
{
    //TIM6没有PWM功能
}

void TIM6_Encoder_DefPin_Init(uint16_t)
{
    //TIM6没有编码器功能
}

void TIM6_ETR_DefPin_Init(void)
{
   //TIM6没有外部时钟功能 
}
