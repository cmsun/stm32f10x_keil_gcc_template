/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : Sm_Delay.cpp
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/03/29 21:20:03
*  @Last Modified  : 2017/03/29 22:59:37
********************************************************************************
*/

#include "Sm_SysTick.h"

extern "C" void delay_us(uint16_t nus)
{
#ifdef Sm_UCOS_Support
    OS_ERR err;
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;        //LOAD的值	    	 
    ticks = nus*Sm_SysTick::fac_us();       //需要的节拍数 
    OSSchedLock(&err);                      //阻止OS调度，防止打断us延时
    told = SysTick->VAL;                    //刚进入时的计数器值
    while(1)
    {
        tnow = SysTick->VAL;	
        if(tnow != told)
        {	    
            tcnt += (tnow < told ? (told - tnow) : (reload - tnow + told));
            told = tnow;
            if(tcnt >= ticks)
            {
                break;                      //时间超过/等于要延迟的时间,则退出.
            }
        }  
    }
    OSSchedUnlock(&err);                    //恢复OS调度
#else
    Sm_SysTick::delay_us(nus);
#endif
}

extern "C" void delay_ms(uint16_t nms)
{
#ifdef Sm_UCOS_Support
	OS_ERR err; 
    if(OSRunning && OSIntNestingCtr == 0)   //如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度)	    
    {		 
        if(nms >= Sm_SysTick::fac_ms())     //延时的时间大于OS的最少时间周期 
        { 
            OSTimeDly(nms, OS_OPT_TIME_PERIODIC, &err);     //UCOSIII延时
        }
        nms %= Sm_SysTick::fac_ms();        //OS已经无法提供这么小的延时了,采用普通方式延时    
    }
    delay_us((u32)(nms*1000));              //普通方式延时  
#else
    Sm_SysTick::delay_ms(nms);
#endif
}
