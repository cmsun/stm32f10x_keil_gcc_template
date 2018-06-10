#ifndef SM_SYSTICK_H
#define SM_SYSTICK_H

#include "Sm_Config.h"
#include "Sm_Debug.h"

class Sm_SysTick
{
private:
    static uint32_t mFace_us;
    static uint32_t mFace_ms;
    static uint32_t mMax_us;
    static uint32_t mMax_ms;
    static void delay_ms_private(uint16_t nms);
    Sm_SysTick(){}; //构造函数定义成私有函数，不允许生成实例。

public:
    static void init(void);
    static void setCLKSource(uint32_t SysTick_CLKSource = SysTick_CLKSource_HCLK_Div8);
    static void setTimeOut_ms(uint16_t nms, void (*pfun)(void *), void *arg = 0);
    static void setTimeOut_us(uint16_t nus, void (*pfun)(void *), void *arg = 0);
    static void delay_ms(uint32_t nms);
    static void delay_us(uint32_t nus);
    static void ITConfig(Sm::FunctionalState NewState);
    static void setReload(uint32_t reloadVal);
    static uint32_t value(void);
    static void enable(void);
    static void disable(void);
    static uint32_t face_us(void);
    static uint32_t face_ms(void);
};

#endif
