#ifndef SM_TIM_H
#define SM_TIM_H

#include "Sm_Config.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "Sm_Debug.h"

namespace Sm{

typedef enum {
    PWM_Channel1 = 0,
    PWM_Channel2 = 1,
    PWM_Channel3 = 2,
    PWM_Channel4 = 3
} PWM_Chl;

}

template<uint32_t TIMX,
         Sm::RCC_ClockCmd RCC_APBxPeriphClockCmd,
         uint32_t RCC_APBxPeriph_TIMx,
         void (* Init_PWM_GPIO)(Sm::PWM_Chl),
         void (* Init_ETR_GPIO)(void),
         // void (* Init_Endcoder_GPIO)(uint16_t),
         IRQn_Type TIMx_IRQn,
         Sm::SmCallback *Time_Callback,
         void **Time_CbArg>
class Sm_TIM{
public:
    static TIM_TypeDef* const mTIMx;

public:
    Sm_TIM(uint16_t PSC = 1-1, uint16_t ARR = 72-1,            //定时预分频和定时周期
           uint16_t TIM_CounterMode_x = TIM_CounterMode_Up,    //计数方向
           uint16_t TIM_CKD_DIVx = TIM_CKD_DIV1,               //时钟分割
           uint8_t TIM_RepetitionCounter = 0)               
    {
        initialize(PSC, ARR, TIM_CounterMode_x, TIM_CKD_DIVx, TIM_RepetitionCounter);
    }

    static void initialize(uint16_t PSC = 1-1, uint16_t ARR = 72-1, //定时预分频和定时周期
            uint16_t TIM_CounterMode_x = TIM_CounterMode_Up,        //计数方向
            uint16_t TIM_CKD_DIVx = TIM_CKD_DIV1,                   //时钟分割
            uint8_t TIM_RepetitionCounter = 0)               
    {
        Sm_assert(IS_TIM_COUNTER_MODE(TIM_CounterMode_x));
        Sm_assert(IS_TIM_CKD_DIV(TIM_CKD_DIVx));

        RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE);

        TIM_TimeBaseInitTypeDef tim_init;
        tim_init.TIM_Prescaler = PSC;
        tim_init.TIM_Period = ARR;
        tim_init.TIM_ClockDivision = TIM_CKD_DIVx;
        tim_init.TIM_CounterMode = TIM_CounterMode_x;
        //设置RCR寄存器，只对高级定时器1和8有用。作用是每(1+TIM_RepetitionCounter)次arr溢出才调用一次中断函数
        tim_init.TIM_RepetitionCounter = TIM_RepetitionCounter;
        TIM_TimeBaseInit(mTIMx, &tim_init);

        //设置定时器更新中断的默认中断优先级，并使能中断线。
        setNVICPriority(TIMx_IRQn);
        //打开定时器更新中断线，对于高级定时器的其它中断线，在需要时使用setNVICPriority()打开。
        NVIC_EnableIRQ(TIMx_IRQn);

        //把TIMx_CR1的URS位置1，使UG=1产生更新事件时不产生更新中断。而溢出更新事件可以正常产生更新中断。
        mTIMx->CR1 |= TIM_CR1_URS;

        //对TIMx_EGR的UG位软件置1会产生一次更新事件。
        //产生更新事件时会把新写入的ARR和PSC的值写入影子寄存器里。
        //所有的寄存器会更新：计数器会被清0，同时预分频器的计数也被请0(但预分频系数不变)。
        //如果设置了TIMx_CR1的URS位为1，则UG=1时只产生更新事件，不产生更新中断。
        //UG=1产生更新事件后硬件会自动把UG位清0。
        mTIMx->EGR |= TIM_EGR_UG;
    }

    //设置中断优先级，并使能中断线。
    static void setNVICPriority(IRQn_Type timx_irqn,
            uint8_t PreemptionPriority = Default_TIM_NVIC_IRQChannelPreemptionPriority,
            uint8_t SubPriority = Default_TIM_NVIC_IRQChannelSubPriority)
    {
        NVIC_InitTypeDef nvic_init;
        nvic_init.NVIC_IRQChannel = timx_irqn;
        nvic_init.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
        nvic_init.NVIC_IRQChannelSubPriority = SubPriority;
        nvic_init.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_init);
    }

    static void ITConfig(uint16_t TIM_IT_x, Sm::FunctionalState NewState = ENABLE)
    {
        Sm_assert(IS_TIM_IT(TIM_IT_x));
        Sm_assert(IS_FUNCTIONAL_STATE(NewState));

        TIM_ITConfig(mTIMx, TIM_IT_x, NewState);
    }

    //设置定时器时钟分频
    static inline void setPSC(uint16_t PSC, bool Immediately = true)
    {
        mTIMx->PSC = PSC;
        if(Immediately)
            mTIMx->EGR |= TIM_EGR_UG;
    }

    //设置ARR值被修改后是否立即更新影子寄存器。
    static void setARRPreload(Sm::FunctionalState NewState)
    {
        // TIM_ARRPreloadConfig((TIM_TypeDef *)mTIMx, NewState);

        if (NewState != DISABLE)
            mTIMx->CR1 |= TIM_CR1_ARPE;
        else
            mTIMx->CR1 &= (uint16_t)~((uint16_t)TIM_CR1_ARPE);
    }

    //设置定时器溢出值
    //是否产即更新影子寄存器由CR1的ARPE寄存器决定
    static inline void setARR(uint16_t ARR)
    {
        mTIMx->ARR = ARR;
    }

    static void setCallback(Sm::SmCallback callback, void *arg = 0)
    {
        //设置回调函数
        *Time_Callback = callback;
        *Time_CbArg = arg;

        //必须先清除中断标志位再使能定时器更新中断，否则不管有没有使能定时器都会产即进入中断。
        TIM_ClearITPendingBit(mTIMx, TIM_IT_Update);

        //使能定时器更新中断
        TIM_ITConfig(mTIMx, TIM_IT_Update, ENABLE);
    }

    //可以定时在65535/2=32767ms以内(也就是32秒以内)
    static void init_ms(uint16_t nms)
    {
        // mTIMx->PSC = 7200 - 1;      //定时器每秒计数：72000000(次/sec) / 7200 = 10000(次/sec)
        // mTIMx->ARR = nms*10 - 1;    //10000(次/sec) / 1000(ms/sec) = 10(次/ms); 所以定时器计数10次为1ms
        
        Sm_assert(nms > 0 && nms <= 32767);

        mTIMx->PSC = 36000 - 1; //定时器每秒计数：72000000(次/sec) / 36000 = 2000(次/sec)
        mTIMx->ARR = nms*2 - 1; //2000(次/sec) / 1000(ms/sec) = 2(次/ms); 所以定时器计数2次为1ms
        mTIMx->EGR = 0x0001;    //UG位置1，产生更新事件，计数清0并把ARR和PSC的值写入影子寄存器
    }

    //可以定时在65535/2=32767微秒以内(也就是32ms)
    static void init_us(uint16_t nus)
    {
        Sm_assert(nus > 0 && nus <= 32767);

        mTIMx->PSC = 36 - 1;    //定时器每秒计数：72000000(次/sec) / 36 = 2000000(次/sec)
        mTIMx->ARR = nus*2 - 1; //2000000(次/sec) / 1000000(us/sec) = 2(次/us); 所以定时器计数2次为1us
        mTIMx->EGR = 0x0001;    //UG位置1，产生更新事件，计数清0并把ARR和PSC的值写入影子寄存器
    }

    static inline void enable(void)
    {
        // TIM_Cmd(mTIMx, ENABLE);
        mTIMx->CR1 |= TIM_CR1_CEN;

#ifdef ADVANCED_TIMER  //如果是高级定时器
        // TIM_CtrlPWMOutputs(mTIMx, ENABLE);
        mTIMx->BDTR |= TIM_BDTR_MOE;
#endif
    }

    //如果compare=true，则停止后清空计数值，并强制产生一次比较
    static inline void disable(bool compare = true)
    {
#ifdef ADVANCED_TIMER  //如果是高级定时器
        // TIM_CtrlPWMOutputs(mTIMx, DISABLE);
        mTIMx->BDTR &= (uint16_t)(~((uint16_t)TIM_BDTR_MOE));
#endif

        // TIM_Cmd(mTIMx, DISABLE);
        mTIMx->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));

        if(compare)
            mTIMx->EGR |= TIM_EGR_UG;
    }

    //TIM_OCMode_PWM1:计数值小于比较值时为有效值。
    //TIM_OCMode_PWM2:计数值大于比较值时为有效值。
    //TIM_OCPolarity_High:计数值为有效值时输出高电平，非有效值时输出低电平
    //TIM_OCPolarity_Low: 计数值为有效值时输出低电平，非有效值时输出高电平
    static void PWMConfig(Sm::PWM_Chl channel,
            uint16_t TIM_OCMode = TIM_OCMode_PWM2,
            uint16_t TIM_OutputState = TIM_OutputState_Enable,
            uint16_t TIM_OCPolarity = TIM_OCPolarity_High)
    {
        Init_PWM_GPIO(channel);

        TIM_OCInitTypeDef tim_ocinit;
        tim_ocinit.TIM_OCMode = TIM_OCMode;                     //选择PWM模式
        tim_ocinit.TIM_OutputState = TIM_OutputState;           //比较输出使能
        tim_ocinit.TIM_OCPolarity = TIM_OCPolarity;             //输出极性高
#ifdef ADVANCED_TIMER  //如果是高级定时器
        tim_ocinit.TIM_OutputNState = TIM_OutputNState_Disable;
        tim_ocinit.TIM_OCNPolarity =  TIM_OCNPolarity_High;
        tim_ocinit.TIM_OCIdleState = TIM_OCIdleState_Reset;
        tim_ocinit.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
#endif

        if(channel == Sm::PWM_Channel1)
        {
            tim_ocinit.TIM_Pulse = mTIMx->CCR1;
            TIM_OC1Init(mTIMx, &tim_ocinit);
            TIM_OC1PreloadConfig(mTIMx, TIM_OCPreload_Enable); //使能预装载寄存器
        }
        else if(channel == Sm::PWM_Channel2)
        {
            tim_ocinit.TIM_Pulse = mTIMx->CCR2;
            TIM_OC2Init(mTIMx, &tim_ocinit);
            TIM_OC2PreloadConfig(mTIMx, TIM_OCPreload_Enable); //使能预装载寄存器
        }
        else if(channel == Sm::PWM_Channel3)
        {
            tim_ocinit.TIM_Pulse = mTIMx->CCR3;
            TIM_OC3Init(mTIMx, &tim_ocinit);    
            TIM_OC3PreloadConfig(mTIMx, TIM_OCPreload_Enable); //使能预装载寄存器
        }
        else if(channel == Sm::PWM_Channel4)
        {
            tim_ocinit.TIM_Pulse = mTIMx->CCR4;
            TIM_OC4Init(mTIMx, &tim_ocinit);
            TIM_OC4PreloadConfig(mTIMx, TIM_OCPreload_Enable); //使能预装载寄存器
        }
    }

    //初使化PWM后，PWM通道的初始电平是不确定的。所以初始化PWM并设置比较值后应立
    //即设置UG位产生一次更新事件，由此发生一次比效，使PWM初始电平处于正确的电平。
    //所以使用PWMConfig()后第一次使用setCompare()时应使Immediately=true。
    //在PWM输出过程中重新设置比较值时，如果设置UG位则立即更新比较值，否则在下个周期更新比较值
    static void setCompare(Sm::PWM_Chl channel, uint16_t compareVal, bool Immediately = true)
    {
        static volatile uint16_t *compareReg[] = {&mTIMx->CCR1, &mTIMx->CCR2,
                                                  &mTIMx->CCR3, &mTIMx->CCR4};
        *compareReg[channel] = compareVal;
        if(Immediately)
            mTIMx->EGR |= TIM_EGR_UG;//设置UG位，产生更新比较值，并产生一次比较
    }

#ifdef ADVANCED_TIMER  //如果是高级定时器
    static inline void setRCR(uint8_t RCR, bool Immediately = true)
    {
        Sm_assert(mTIMx == TIM1  ||
                  mTIMx == TIM8  ||
                  mTIMx == TIM15 ||
                  mTIMx == TIM16 ||
                  mTIMx == TIM17);

        mTIMx->RCR = RCR;
        if(Immediately)
            mTIMx->EGR |= TIM_EGR_UG;
    }
#endif  //ADVANCED_TIMER

#ifdef EXTRA_CLOCK_MODE2 //如果定时器有外部时钟源模式2功能
    //设置外部时钟模式2，从ETR管脚接入外部脉冲时行计数
    static void initETRClockMode2(uint16_t PSC = 1-1, uint16_t ARR = 0xffff-1)
    {
        //初始化ETR管脚
        Init_ETR_GPIO(); 

        //参数1：mTIMx为定时器TIM1、TIM2、TIM3、TIM4、TIM8
        //参数2：TIM_ExtTRGPrescaler外部触发预分频
        //参数3：TIM_ExtTRGPolarity外部时钟极性
        //参数4：外部触发滤波，取值为0x0到0xf之间
        TIM_ETRClockMode2Config(mTIMx, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);

        mTIMx->PSC = PSC;
        mTIMx->ARR = ARR;
        mTIMx->EGR |= TIM_EGR_UG;
    }
#endif //EXTRA_CLOCK_MODE2

    static void selectOutputTrigger(uint16_t TIM_TRGOSource_x)
    {
        Sm_assert(IS_TIM_TRGO_SOURCE(TIM_TRGOSource_x));

        TIM_SelectOutputTrigger(mTIMx, TIM_TRGOSource_x);
    }

    static void encoderConfig(uint16_t TIM_EncoderMode_x = TIM_EncoderMode_TI12,
            uint16_t TIM_IC1Polarity_x = TIM_ICPolarity_Rising,
            uint16_t TIM_IC2Polarity_x = TIM_ICPolarity_Rising)
    {
        // Init_Endcoder_GPIO(TIM_EncoderMode_x);

        TIM_EncoderInterfaceConfig(mTIMx,
                                   TIM_EncoderMode_x,
                                   TIM_IC1Polarity_x,
                                   TIM_IC2Polarity_x);
        TIM_ICInitTypeDef tim_icinit;
        TIM_ICStructInit(&tim_icinit);
        tim_icinit.TIM_ICFilter = 6;
        TIM_ICInit(mTIMx, &tim_icinit);
    }
};

#endif
