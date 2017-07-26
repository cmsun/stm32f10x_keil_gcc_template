#include "Sm_TIM1_Pluse.h"
#include "Sm_TIM2_Pluse.h"
#include "Sm_TIM3_Pluse.h"
#include "Sm_TIM4_Pluse.h"
#include "Sm_TIM5_Pluse.h"
#include "Sm_TIM8_Pluse.h"
#include "Sm_Debug.h"

template<> BitAction Sm_TIM1_Pluse::mForwarFlag = Bit_SET;
template<> BitAction Sm_TIM2_Pluse::mForwarFlag = Bit_SET;
template<> BitAction Sm_TIM3_Pluse::mForwarFlag = Bit_SET;
template<> BitAction Sm_TIM4_Pluse::mForwarFlag = Bit_SET;
template<> BitAction Sm_TIM5_Pluse::mForwarFlag = Bit_SET;
template<> BitAction Sm_TIM8_Pluse::mForwarFlag = Bit_SET;

Sm::Pluse_t TIM1_Pluse;
Sm::Pluse_t TIM2_Pluse;
Sm::Pluse_t TIM3_Pluse;
Sm::Pluse_t TIM4_Pluse;
Sm::Pluse_t TIM5_Pluse;
Sm::Pluse_t TIM8_Pluse;

#define Sm_Pluse_IRQHandler(DMAx, DMAx_IT_TCy, TIMx_Pluse, Sm_TIMx_Pluse) do{ \
    OSIntEnter(); \
    /*if(DMA_GetITStatus(DMAx_IT_TCy))*/ \
    if(DMAx->ISR & DMAx_IT_TCy) \
    { \
        /*DMA_ClearITPendingBit(DMA1_IT_TC3)*/ \
        /*IFCR寄存器的位写0无效，写1有效*/ \
        DMAx->IFCR = DMAx_IT_TCy; \
        if(TIMx_Pluse.Step.empty()) \
            goto out; \
        Sm_TIMx_Pluse::fillDMAPluseData(); \
    } \
out: \
    OSIntExit(); \
}while(0)

extern "C"
{
    //Sm_TIM1_Pluse
    void DMA1_Channel5_IRQHandler(void)
    {
        Sm_Pluse_IRQHandler(DMA1, DMA1_IT_TC5, TIM1_Pluse, Sm_TIM1_Pluse);
    }    

    //Sm_TIM2_Pluse
    void DMA1_Channel2_IRQHandler(void)
    {
        Sm_Pluse_IRQHandler(DMA1, DMA1_IT_TC2, TIM2_Pluse, Sm_TIM2_Pluse);
    }

    //Sm_TIM3_Pluse
    void DMA1_Channel3_IRQHandler(void)
    {
        Sm_Pluse_IRQHandler(DMA1, DMA1_IT_TC3, TIM3_Pluse, Sm_TIM3_Pluse);
    }

    //Sm_TIM4_Pluse
    void DMA1_Channel7_IRQHandler(void)
    {
        Sm_Pluse_IRQHandler(DMA1, DMA1_IT_TC7, TIM4_Pluse, Sm_TIM4_Pluse);
    }

    //Sm_TIM5_Pluse
    void DMA2_Channel2_IRQHandler(void)
    {
        Sm_Pluse_IRQHandler(DMA2, DMA2_IT_TC2, TIM5_Pluse, Sm_TIM5_Pluse);
    }

    //Sm_TIM8_Pluse
    void DMA2_Channel1_IRQHandler(void)
    {
        Sm_Pluse_IRQHandler(DMA2, DMA2_IT_TC1, TIM8_Pluse, Sm_TIM8_Pluse);
    }
}
