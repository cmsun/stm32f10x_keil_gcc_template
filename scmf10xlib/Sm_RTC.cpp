#include "Sm_RTC.h"
#include "Sm_SysTick.h"
#include "Sm_Debug.h"

static Sm::CALLBACK Second_Callback;
static Sm::CALLBACK Alarm_Callback;

/*
********************************************************************************
*                               RTC_IRQHandler 
*
* Description : stm32的RTC时钟的秒中断和闹钟中断共用函数
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
extern "C" void RTC_IRQHandler(void)
{
#ifdef Sm_UCOS_Support 
    OSIntEnter();
#endif

    if(RTC_GetITStatus(RTC_IT_SEC) != RESET)    //秒中断
    {
        if(Second_Callback.pfun != NULL)
            Second_Callback.pfun(Second_Callback.arg);
        RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
    }

    if(RTC_GetITStatus(RTC_IT_ALR) != RESET)    //闹钟中断
    {
        RTC_ClearITPendingBit(RTC_IT_ALR);
        if(Alarm_Callback.pfun != NULL)
            Alarm_Callback.pfun(Alarm_Callback.arg);
    }

#ifdef Sm_UCOS_Support 
    OSIntExit();
#endif
}

/*
********************************************************************************
*                               Init 
*
* Description : 初始化时实时钟
*
* Arguments   : none
*
* Returns     : 返回值-1：RTC时钟初始化失败。
*               返回值0 ：RTC初始化成功，但是还没有设置过时间，需要设置时间。
*               返回值1 ：RTC时钟已经初始化过且已经设置过时间，不需要重新设置时间。
*
* Note(s)     : none
********************************************************************************
*/
int Sm_RTC::initialize(void)
{
    int ret = 1, loop = 0;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);                    //允许访问备份寄存器
    if(BKP_ReadBackupRegister(BKP_DR1) != 0x5050)   //如果没有设置过RTC时钟
    {
        RCC_LSEConfig(RCC_LSE_ON);                  //初始化外部低速晶振

        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
        {
            if(++loop >= 250)
            {
                Sm_Debug("Init RTC failed!");
                return -1;                          //初始化RTC失败，晶振有问题
            }
#ifdef Sm_UCOS_Support
            OS_ERR err;
            OSTimeDly(2, OS_OPT_TIME_PERIODIC, &err);
#else
            Sm_SysTick::delay_ms(10);
#endif
        }

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);     //选择LSE为RTC的时钟源
        RCC_RTCCLKCmd(ENABLE);                      //使能RTC时钟
        RTC_WaitForLastTask();                      //等待最近一次对 RTC 寄存器的写操作完成
        RTC_WaitForSynchro();                       //等待 RTC 寄存器同步

        BKP_WriteBackupRegister(BKP_DR1, 0x5050);   //往备份寄存器DKP_DR1中写入0x5050，表明已经设置过RTC

        ret = 0;                                    //RTC初始化成功，但未设置时间
    }

    setNVICPriority();
    return ret;
}

/*
********************************************************************************
*                               setDateTime 
*
* Description : 设置时间
*
* Arguments   : time:从1970年1月1日0时0分0秒到当前时间经过的秒数。
*               prescale:RTC时钟的预分频，默认值32767。 
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
void Sm_RTC::setDateTime(time_t time, uint32_t prescale)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RTC_EnterConfigMode();                      //进入RTC设置模式
    RTC_SetPrescaler(prescale);                 //设置RTC预分频
    RTC_WaitForLastTask();
    RTC_SetCounter(time);                       //设置时间
    RTC_WaitForLastTask();
    RTC_ExitConfigMode();                       //退出RTC设置模式
}

/*
********************************************************************************
*                               setDateTime 
*
* Description : 设置RTC时间。
*
* Arguments   : year month day:要设置的年月日。
*               hour minute second:为要设置的小时分钟秒。
*               prescale:RTC时钟的预分频数，默认值32767。
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
void Sm_RTC::setDateTime(int16_t year, int8_t month, int8_t day,
                         int8_t hour, int8_t minute, int8_t second,
                         uint32_t prescale)
{
    struct tm t;
    time_t timeStamp;

    t.tm_sec = second;      //范围0-59
    t.tm_min = minute;      //范围0-59
    t.tm_hour = hour;       //范围0-23
    t.tm_mday = day;        //一个月中的第几天，范围1-31
    t.tm_mon = month-1;     //月份，范围0-11，0代表1月
    t.tm_year = year-1900;  //实际年分减去1900
    t.tm_wday = 0;          //星期几，范围0-6，0代表星期天
    t.tm_yday = 0;          //一年中的第几天, 范围0-364
    t.tm_isdst = -1;

    timeStamp = mktime(&t);
    setDateTime(timeStamp, prescale);
}

/*
********************************************************************************
*                               setAlarmTime 
*
* Description : 设置闹钟
*
* Arguments   : time:从1970年1月1日0时0分0秒到闹钟时间经过的秒数。
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
void Sm_RTC::setAlarmTime(time_t time)
{
    RTC_SetAlarm(time);
}

void Sm_RTC::setAlarmTime(int16_t year, int8_t month, int8_t day,
                          int8_t hour, int8_t minute, int8_t second)
{
    struct tm t;
    time_t timeStamp;

    t.tm_sec = second;      //范围0-59
    t.tm_min = minute;      //范围0-59
    t.tm_hour = hour;       //范围0-23
    t.tm_mday = day;        //一个月中的第几天，范围1-31
    t.tm_mon = month-1;     //月份，范围0-11，0代表1月
    t.tm_year = year-1900;  //实际年分减去1900
    t.tm_wday = 0;          //星期几，范围0-6，0代表星期天
    t.tm_yday = 0;          //一年中的第几天, 范围0-364
    t.tm_isdst = -1;        //夏令时

    timeStamp = mktime(&t);
    RTC_SetAlarm(timeStamp);
}

time_t Sm_RTC::getDateTime(void)
{
    return RTC_GetCounter(); 
}

void Sm_RTC::setNVICPriority(uint8_t PreemptionPriority, uint8_t SubPriority)
{
    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = RTC_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
    nvic_init.NVIC_IRQChannelSubPriority = SubPriority;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);
}

void Sm_RTC::secondITConfig(FunctionalState NewState)
{
    RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
    RTC_ITConfig(RTC_IT_SEC, NewState);
    RTC_WaitForLastTask();
}

void Sm_RTC::setSecCallback(void (*pfun)(void *), void *arg)
{
    Second_Callback.pfun = pfun;
    Second_Callback.arg = arg;
}

void Sm_RTC::alarmITConfig(FunctionalState state)
{
    RTC_ITConfig(RTC_IT_ALR, state);           //允许闹钟中断
    RTC_WaitForLastTask();
}

void Sm_RTC::setAlarmCallback(void (*pfun)(void *), void *arg)
{
    Alarm_Callback.pfun = pfun;
    Alarm_Callback.arg = arg;
}

