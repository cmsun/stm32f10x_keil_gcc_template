#include "Sm_GPIO.h"

Sm::EXTI_IRQ EXTI_IRQ_Table[16];

extern "C" {

void EXTI0_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if((EXTI->PR & EXTI_Line0) && (EXTI->IMR & EXTI_Line0))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[0]; 
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line0;
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void EXTI1_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if((EXTI->PR & EXTI_Line1) && (EXTI->IMR & EXTI_Line1))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[1]; 
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line1;
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void EXTI2_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if((EXTI->PR & EXTI_Line2) && (EXTI->IMR & EXTI_Line2))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[2]; 
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line2;
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void EXTI3_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if((EXTI->PR & EXTI_Line3) && (EXTI->IMR & EXTI_Line3))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[3]; 
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line3;
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void EXTI4_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if((EXTI->PR & EXTI_Line4) && (EXTI->IMR & EXTI_Line4))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[4]; 
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line4;
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void EXTI9_5_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if((EXTI->PR & EXTI_Line5) && (EXTI->IMR & EXTI_Line5))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[5]; 
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line5;
    }

    if((EXTI->PR & EXTI_Line6) && (EXTI->IMR & EXTI_Line6))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[6]; 
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line6;
    }

    if((EXTI->PR & EXTI_Line7) && (EXTI->IMR & EXTI_Line7))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[7];
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line7;
    }

    if((EXTI->PR & EXTI_Line8) && (EXTI->IMR & EXTI_Line8))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[8];
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line8;
    }

    if((EXTI->PR & EXTI_Line9) && (EXTI->IMR & EXTI_Line9))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[9];
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line9;
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

void EXTI15_10_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if((EXTI->PR & EXTI_Line10) && (EXTI->IMR & EXTI_Line10))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[10];
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line10;
    }

    if((EXTI->PR & EXTI_Line11) && (EXTI->IMR & EXTI_Line11))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[11];
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line11;
    }

    if((EXTI->PR & EXTI_Line12) && (EXTI->IMR & EXTI_Line12))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[12];
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line12;
    }

    if((EXTI->PR & EXTI_Line13) && (EXTI->IMR & EXTI_Line13))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[13];
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line13;
    }

    if((EXTI->PR & EXTI_Line14) && (EXTI->IMR & EXTI_Line14))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[14];
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line14;
    }

    if((EXTI->PR & EXTI_Line15) && (EXTI->IMR & EXTI_Line15))
    {
        Sm::EXTI_IRQ &irq = EXTI_IRQ_Table[15];
        if(irq.pf_IRQ_Process != NULL)
            irq.pf_IRQ_Process(irq.p_IRQ_Argument);
        EXTI->PR = EXTI_Line15;
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

}
