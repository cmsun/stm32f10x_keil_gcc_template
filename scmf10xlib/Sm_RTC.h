#ifndef SM_RTC_H
#define SM_RTC_H

#include <time.h>

#include "Sm_Config.h"

class Sm_RTC
{
public:
    static int initialize(void);
    static void setDateTime(time_t time, uint32_t prescale = 32767);
    static void setDateTime(int16_t year, int8_t month, int8_t day,
                            int8_t hour, int8_t minute, int8_t second,
                            uint32_t prescale = 32767);
    static void setAlarmTime(time_t time);
    static void setAlarmTime(int16_t year, int8_t month, int8_t day,
                             int8_t hour, int8_t minute, int8_t second);
    static time_t getDateTime(void);
    static void setNVICPriority(uint8_t PreemptionPriority = Default_RTC_NVIC_IRQChannelPreemptionPriority,
                                uint8_t SubPriority = Default_RTC_NVIC_IRQChannelSubPriority);
    static void secondITConfig(FunctionalState state);
    static void setSecCallback(void (*pfun)(void *), void *arg = 0);
    static void alarmITConfig(FunctionalState state);
    static void setAlarmCallback(void (*pfun)(void *), void *arg = 0);
};

#endif
