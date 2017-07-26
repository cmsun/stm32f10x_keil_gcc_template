#include "Sm_TIM7.h"

template<> TIM_TypeDef* const Sm_TIM7::mTIMx = TIM7;

Sm::SmCallback TIM7_Callback;   
void *TIM7_CbArg;

extern "C" void TIM7_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        if(TIM7_Callback)
            TIM7_Callback(TIM7_CbArg);
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM7_PWM_DefPin_Init(Sm::PWM_Chl)
{
    //TIM7没有PWM功能
}

void TIM7_ETR_DefPin_Init(void)
{
   //TIM7没有外部时钟功能 
}
