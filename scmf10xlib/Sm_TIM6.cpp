#include "Sm_TIM6.h"

template<> TIM_TypeDef* const Sm_TIM6::mTIMx = TIM6;

Sm::SmCallback TIM6_Callback;   
void *TIM6_CbArg;

extern "C" void TIM6_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        if(TIM6_Callback)
            TIM6_Callback(TIM6_CbArg);
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void TIM6_PWM_DefPin_Init(Sm::PWM_Chl)
{
    //TIM6没有PWM功能
}

void TIM6_ETR_DefPin_Init(void)
{
   //TIM6没有外部时钟功能 
}
