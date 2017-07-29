#include "Sm_TIM7.h"

Sm::CALLBACK TIM7_Callback;   

extern "C" void TIM7_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        if(TIM7_Callback.pfun)
            TIM7_Callback.pfun(TIM7_Callback.arg);
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM7_PWM_DefPin_Init(Sm::PWM_Channel)
{
    //TIM7没有PWM功能
}

void TIM7_Encoder_DefPin_Init(uint16_t)
{
    //TIM7没有编码器功能
}

void TIM7_ETR_DefPin_Init(void)
{
   //TIM7没有外部时钟功能 
}
