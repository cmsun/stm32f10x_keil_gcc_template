#include "Sm_SysTick.h"

uint32_t Sm_SysTick::mFace_us;
uint32_t Sm_SysTick::mFace_ms;
uint32_t Sm_SysTick::mMax_us;
uint32_t Sm_SysTick::mMax_ms;
Sm::CALLBACK SysTickCallback;

#if !defined(Sm_UCOS_Support)
extern "C" void SysTick_Handler(void)
{
    if(SysTickCallback.pfun != NULL)
        SysTickCallback.pfun(SysTickCallback.arg);
}
#endif

/*
********************************************************************************
*                               init
* Description : 初始化计算face和max
* 
* Arguments   : face_ms：当前SysTick时钟源下延时1ms需要的计数值
*               face_us：当前SysTick时钟源下延时1us需要的计数值
*               max_ms: 当前SysTick时钟源下一次加载能延时的最大毫秒数
*               max_us: 当前SysTick时钟源下一次加载能延时的最大微秒数
*               
* Return      : none
* 
* Note(s)     : none
********************************************************************************
*/
void Sm_SysTick::init(void)
{
    RCC_ClocksTypeDef rcc_clocks;

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);    //设置成不分频，则SysTick定时器的时钟频率等于HCLK

    RCC_GetClocksFreq(&rcc_clocks);
    mFace_ms = rcc_clocks.HCLK_Frequency / 1000;         //systick延时1毫秒所需要的计数值
    mFace_us = rcc_clocks.HCLK_Frequency / 1000000;      //systick延时1微秒所需要的计数值
    mMax_ms = 0xffffff / mFace_ms;                       //一次加载最大能延时的毫秒数
    mMax_us = 0xffffff / mFace_us;                       //一次加载最大能延时的微秒数
}

/*
********************************************************************************
*                               setCLKSource 
*
* Description : 设置SysTick时钟源为HCLK或者HCLK的8分频。
*
* Arguments   : SysTick_CLKSource取值为：SysTick_CLKSource_HCLK(系统时钟) 和
*               SysTick_CLKSource_HCLK_Div8(系统时钟8分频)。
*
* Returns     : none
*
* Note(s)     : 默认情况下SysTick时钟源为HCLK的8分频。
********************************************************************************
*/
void Sm_SysTick::setCLKSource(uint32_t SysTick_CLKSource)
{
    Sm_assert(IS_SYSTICK_CLK_SOURCE(SysTick_CLKSource));

    RCC_ClocksTypeDef rcc_clocks;

    SysTick_CLKSourceConfig(SysTick_CLKSource); 

    RCC_GetClocksFreq(&rcc_clocks);
    if(SysTick_CLKSource == SysTick_CLKSource_HCLK)
        mFace_us = rcc_clocks.HCLK_Frequency / 1000000;
    else
        mFace_us = rcc_clocks.HCLK_Frequency / 8 / 1000000;

    mFace_ms = mFace_us * 1000;
    mMax_us = 0xffffff / mFace_us;
    mMax_ms = 0xffffff / mFace_ms;
}

/*
********************************************************************************
*                               delay_ms_private 
*
* Description : 毫秒延时函数。
*
* Arguments   : nms 延时时间，单位毫秒。
*
* Returns     : none
*
* Note(s)     : SysTick->LOAD为24位寄存器,所以,最大延时为:nms*mFace_ms <= 0xffffff
*               对48M条件下不分频时mFace_ms==48000，所以nms<=349
*               对48M条件下8分频时mFace_ms==6000，所以nms<=2796
*               对72M条件下不分频时mFace_ms==72000，所以nms<=233
*               对72M条件下8分频时mFace_ms==9000，所以nms<=1864
*               对168M条件下不分频时face_ms==168000，所以nms<=99
*               对168M条件下8分频时face_ms==21000，所以nms<=792
********************************************************************************
*/
void Sm_SysTick::delay_ms_private(uint16_t nms)
{
    Sm_assert(nms <= mMax_ms);

    uint32_t temp;
    SysTick->LOAD = nms * mFace_ms - 1;           //加载时间
    SysTick->VAL = 0x00;                        //清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //开始计数
    do{
        temp = SysTick->CTRL;
    }while((temp&0x01) && !(temp&(1<<16)));     //等待时间到达
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //关闭计数器
    SysTick->VAL = 0x00;                        //清空计数器
}

/*
********************************************************************************
*                               setTimeOut_ms 
*
* Description : 把SysTick当作定时器使用，每nms产生一次中断
*
* Arguments   : nms:定时的毫秒数。
*               callback:回调函数。
*               回调函数的参数。
*
* Returns     : none
*
* Note(s)     : SysTick->LOAD为24位寄存器，所以最大定时为：nms*mFace_ms <= 0xffffff
*               对48M条件下不分频时mFac_ms==48000，所以nms<=349
*               对48M条件下8分频时mFac_ms==6000，所以nms<=2796
*               对72M条件下不分频时mFace_ms==72000，所以nms<=233
*               对72M条件下8分频时mFace_ms==9000，所以nms<=1864
*               对168M条件下不分频时face_ms==168000，所以nms<=99
*               对168M条件下8分频时face_ms==21000，所以nms<=792
********************************************************************************
*/
void Sm_SysTick::setTimeOut_ms(uint16_t nms, void (*pfun)(void *), void *arg)
{
    Sm_assert(nms <= mMax_ms);

    SysTick->LOAD = mFace_ms * nms - 1;
    SysTickCallback.pfun = pfun;
    SysTickCallback.arg = arg;

    ITConfig(ENABLE);
}

/*
********************************************************************************
*                               setTimeOut_us 
*
* Description : 把SysTick当作定时器使用，每nus产生一次中断
*
* Arguments   : nus:定时的微秒数。
*               callback:回调函数。
*               回调函数的参数。
*
* Returns     : none
*
* Note(s)     : SysTick->LOAD为24位寄存器，所以最大定时为：nus*mFace_us <= 0xffffff
*               对48M条件下不分频时mFac_us==48，所以nus<=349525
*               对48M条件下8分频时mFac_us==6，所以nus<=2796202
*               对72M条件下不分频时mFace_us==72,所以nus<=233016
*               对72M条件下8分步时mFace_us==9,所以nus<=1864135
*               对168M条件下不分频时face_us==168,所以nus<=99864
*               对168M条件下8分步时face_us==21,所以nus<=798915
********************************************************************************
*/
void Sm_SysTick::setTimeOut_us(uint16_t nus, void (*pfun)(void *), void *arg)
{
    Sm_assert(nus <= mMax_us);

    SysTick->LOAD = mFace_us * nus - 1;
    SysTickCallback.pfun = pfun;
    SysTickCallback.arg = arg;

    ITConfig(ENABLE);
}

/*
********************************************************************************
*                               delay_ms 
*
* Description : 毫秒延时函数。
*
* Arguments   : nms延时毫秒数。
*
* Returns     : none
*
* Note(s)     : 可延时非常长的时间。
********************************************************************************
*/
void Sm_SysTick::delay_ms(uint32_t nms)
{
    if(nms <= mMax_ms)
    {
        delay_ms_private((uint16_t)nms);
    }
    else
    {
        while(nms--)
            delay_ms_private(1);
    }
}

/*
********************************************************************************
*                               delay_us 
*
* Description : 微秒延时函数。
*
* Arguments   : nus延时微秒数。
*
* Returns     : none
*
* Note(s)     : SysTick->LOAD为24位寄存器，所以最大延时为：nus*mFace_us <= 0xffffff
*               对48M条件下不分频时mFac_us==48，所以nus<=349525
*               对48M条件下8分频时mFac_us==6，所以nus<=2796202
*               对72M条件下不分频时mFace_us==72,所以nus<=233016
*               对72M条件下8分步时mFace_us==9,所以nus<=1864135
*               对168M条件下不分频时face_us==168,所以nus<=99864
*               对168M条件下8分步时face_us==21,所以nus<=798915
********************************************************************************
*/
void Sm_SysTick::delay_us(uint32_t nus)
{
    Sm_assert(nus <= mMax_us);

    uint32_t temp;
    SysTick->LOAD = nus * mFace_us - 1;           //加载时间
    SysTick->VAL = 0x00;                        //清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //开始计数
    do{
        temp = SysTick->CTRL;
    }while((temp&0x01) && !(temp&(1<<16)));     //等待时间到达
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //关闭计数器
    SysTick->VAL = 0x00;                        //清空计数器
}

/*
********************************************************************************
*                               setITConfig 
*
* Description : 开启或者关闭SysTick中断。
*
* Arguments   : NewState: ENABLE 开启中断，当SysTick计数值为0时产生SysTick中断。
*                         DISABLE 关闭中断。
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
void Sm_SysTick::ITConfig(Sm::FunctionalState NewState)
{
    if(NewState != DISABLE)
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    else
        SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

/*
********************************************************************************
*                               setReload 
*
* Description : 设置SysTick的自动重装值。
*
* Arguments   : reloadVal
*
* Returns     : none
*
* Note(s)     : 自动重装值应该小于0xffffff。
********************************************************************************
*/
void Sm_SysTick::setReload(uint32_t reloadVal)
{
    Sm_assert(reloadVal <= 0xffffff);

    SysTick->LOAD = reloadVal;
}

/*
********************************************************************************
*                               value 
*
* Description : 读取SysTick的当前计数值。
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : 此函数可以用来计算某些代码块的运行时间。使用方法是setReload(0),
*               然后调用enable()开始计时，用value()读出计数值。如果SysTick时钟源
*               不分频，则value()/HCLK/1000000为代码块执行的us数。如果8分频则
*               value()/HCLK/8/1000000为代码块执行的us数。
********************************************************************************
*/
uint32_t Sm_SysTick::value(void)
{
    return SysTick->VAL;
}

/*
********************************************************************************
*                               enable 
*
* Description : 启动SysTick定时器。
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : 对计数值清0，然后硬件将从预装载寄存器加载计数值进行递减计数。
********************************************************************************
*/
void Sm_SysTick::enable(void)
{
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   
}

/*
********************************************************************************
*                               disable 
*
* Description : 停止SysTick计数，但并不清除当前的计数值。
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
void Sm_SysTick::disable(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/*
********************************************************************************
*                               face_us 
*
* Description : 获取当前SysTick时钟分频下定时1us的计数值
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
uint32_t Sm_SysTick::face_us(void)
{
    return mFace_us;
}

/*
********************************************************************************
*                               face_ms 
*
* Description : 获取当前SysTick时钟分频下定时1ms的计数值
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
uint32_t Sm_SysTick::face_ms(void)
{
    return mFace_ms;
}
