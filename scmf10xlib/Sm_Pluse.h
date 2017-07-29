#ifndef SM_PLUSE_H
#define SM_PLUSE_H

#include <algorithm>
#include <list>

#include "stm32f10x.h"
#include "Sm_TIM.h"

//DMA脉冲计数数的最大值
#define SM_PLUSE_DMA_BUFFER 60000

//默认起动速度100Hz/s
#define SM_STARTUP_SPEED 100

//最大脉冲速度限制在500KHz以内，大于500K会开始出现多发脉冲的情况
#define SM_MAX_SPEED 500000

//最小脉冲速度100Hz，速度模式减速时如果减到最小脉冲速度以下则停止脉冲输出
#define SM_MIN_SPEED 100

//加减速时间间隔(单位毫秒)
#define SM_VELOCITY_INTERVAL 5

//开始加速或者减速前，提前计算的最大脉冲段数量
#define SM_COMPUTE_STEP 250

//任务堆栈大小
#define PLUSE_TASK_STACK_QTY  128

namespace Sm
{

#pragma pack(1)     //1字节对齐
    //脉冲段
    typedef struct {
        uint16_t PSC;
        uint16_t ARR;
        uint16_t CCR;
        uint16_t PluseCnt;
    } Step_t;
#pragma pack()

    //加减速参数
    typedef struct ADPrm_t {
        float Acc_Hzms2;
        float Dec_Hzms2;
        float StartUp_Hzms;
        ADPrm_t()
        {
            Acc_Hzms2 = 0.5;
            Dec_Hzms2 = 0.5;
            StartUp_Hzms = SM_STARTUP_SPEED/1000.0F;
        };
    } ADPrm_t;

    typedef enum {
        Velmode,            //速度模式
        Posmode,            //点位模式
        Interpolationmode,  //插补模式
        Stopmode,
    } enPluseMode;

    typedef struct Pluse_t {
        std::list<Step_t> Step;
        uint8_t *DMABuff;
        uint16_t PreIndex;

        enPluseMode PluseMode;

        ADPrm_t VelModePrm;
        ADPrm_t PosModePrm;

        float CurStepVel_Hzms;
        float TargetVel_Hzms;

        int64_t GlobalPosition;
        int64_t CurStepPosition;
        int64_t TartgetPosition;

        Pluse_t()
        {   
            PluseMode = Sm::Stopmode;
            TargetVel_Hzms = 250.0;
            GlobalPosition = 0;
        };
    } Pluse_t;

} //namespace Sm

//判断速度方向
#define Sm_Direction(velocity) (velocity > 0 ? mForwarFlag : (BitAction)!mForwarFlag)

class Sm_Pluse_Base
{
public:
    Sm_Pluse_Base() = default;
    virtual ~Sm_Pluse_Base() = default;
    virtual int setMode(Sm::enPluseMode) = 0;
    virtual int setVelmodePrm(float, float, uint32_t) = 0;
    virtual int setPosmodePrm(float, float, uint32_t) = 0;
    virtual void forwarDirectionConfig(BitAction bitaction) = 0;
    virtual int32_t getVel(void) = 0;
    virtual int64_t getPos(void) = 0;
    virtual int setVel(int32_t) = 0;
    virtual int setAbsolutePos(int64_t) = 0;
    virtual int setRelativePos(int64_t) = 0;
    virtual int stop(bool emergency) = 0;
    virtual void limitCheck(void) = 0;
};

template<typename Sm_TIMx, Sm::PWM_Chl channel, uint32_t DMA_PeripheralBaseAddr,
    uint32_t RCC_AHBPeriph_DMAx, uint32_t DMAx_Channely, IRQn_Type DMAx_Channely_IRQn,
    typename Sm_GPIO_Direction, uint16_t GPIO_Pin_Direction,            //方向信号
    typename Sm_GPIO_Home, uint8_t GPIO_PintSource_Home,                          //原点信号(中断源)
    typename Sm_GPIO_ForwardLimit, uint16_t GPIO_Pin_ForwardLimit,//正限位信号
    typename Sm_GPIO_ReverseLimit, uint16_t GPIO_Pin_ReverseLimit,//负限位信号
    Sm::Pluse_t *Pluse>
class Sm_Pluse : public Sm_Pluse_Base, public Sm_TIMx
{
private:
    static BitAction mForwarFlag;            //正方向标志
public:
    OS_TCB TCB;                             //UCOSIII的任务控制块
    CPU_STK STACK[PLUSE_TASK_STACK_QTY];    //UCOSIII的任务堆栈

private:
    static void computeTIMRegValue(Sm::Step_t &step, float speed_Hzms)
    {
        Sm_assert(speed_Hzms > 0);

        //当前脉冲段的分频，大于50KHz/s不分频，小于则24分频
        if(speed_Hzms > 50.0F)             
            step.PSC = 1-1;
        else
            step.PSC = 24-1;

        //计算当前脉冲段的周期和比较值
        step.ARR = (uint16_t)(((SystemCoreClock/1000.0F) / (step.PSC+1) / speed_Hzms) + 0.5F) - 1;
        step.CCR = (uint16_t)((step.ARR+1)/2.0F + 0.5F) - 1;
    }

    static uint16_t computeStepTIMReg(std::list<Sm::Step_t> &stepList, float speed_Hzms)
    {
        Sm_assert(speed_Hzms > 0);

        Sm::Step_t step = {0, 0, 0, 0};

        //每SM_VELOCITY_INTERVAL毫秒加速一次
        if(speed_Hzms < 1.0F/SM_VELOCITY_INTERVAL)
            step.PluseCnt = 1;
        else
            step.PluseCnt = (uint16_t)(SM_VELOCITY_INTERVAL * speed_Hzms + 0.5F);

        computeTIMRegValue(step, speed_Hzms); 
        stepList.push_back(step);

        return step.PluseCnt;
    }

    static float computeStepTime(float speed_Hzms)
    {
        Sm_assert(speed_Hzms > 0);
        return speed_Hzms < 1.0F/SM_VELOCITY_INTERVAL
               ? 1/speed_Hzms
               : ((uint16_t)(SM_VELOCITY_INTERVAL*speed_Hzms+0.5F))/speed_Hzms;
    }

    static uint64_t computerDecPluseCnt(float speed_Hzms)
    {
        Sm_assert(speed_Hzms > 0);

        uint16_t stepPluseCnt;
        uint64_t totalPluseCnt = 0;
        float dec_Hzms2 = Pluse->PosModePrm.Dec_Hzms2;
        float stepTime_ms = computeStepTime(speed_Hzms);

        speed_Hzms = speed_Hzms - dec_Hzms2*stepTime_ms;

        while(speed_Hzms > SM_MIN_SPEED/1000.0F)
        {
            if(speed_Hzms < 1.0F/SM_VELOCITY_INTERVAL)
                stepPluseCnt = 1;
            else
                stepPluseCnt = (uint16_t)(SM_VELOCITY_INTERVAL * speed_Hzms + 0.5F);

            totalPluseCnt += stepPluseCnt;
            stepTime_ms = stepPluseCnt / speed_Hzms;          //当前脉冲段发送脉冲消耗的时间
            speed_Hzms = speed_Hzms - dec_Hzms2*stepTime_ms;  //计算下个脉冲段的速度

            //如果下个脉冲段的速度小于最小速度，则只能减速到最小速度
            if(Sm_Float_LessEqual(speed_Hzms, SM_MIN_SPEED/1000.0F)) 
            {
                if(SM_MIN_SPEED/1000.0F < 1.0F/SM_VELOCITY_INTERVAL)
                    totalPluseCnt += 1;
                else
                    totalPluseCnt += (uint16_t)(SM_VELOCITY_INTERVAL * speed_Hzms + 0.5F);
            }
        }

        return totalPluseCnt;
    }

    static void speedInc(float startSpeed_Hzms, float endSpeed_Hzms, uint16_t limitCnt)
    {
        Sm_assert(startSpeed_Hzms >= 0 && endSpeed_Hzms >= SM_MIN_SPEED/1000.0F);

        if(startSpeed_Hzms < Pluse->VelModePrm.StartUp_Hzms)
            startSpeed_Hzms = Pluse->VelModePrm.StartUp_Hzms;
        if(endSpeed_Hzms > SM_MAX_SPEED)
            endSpeed_Hzms = SM_MAX_SPEED/1000.0F;

        OS_ERR err;
        bool isRunning = false;
        float curSpeed_Hzms = startSpeed_Hzms;          //当前速度，单位为Hz/ms
        float acc_Hzms2 = Pluse->VelModePrm.Acc_Hzms2;  //加速度，单位为Hz/ms^2
        float stepTime_ms;

        if(Sm_Float_Equal(startSpeed_Hzms, endSpeed_Hzms))
            goto EndStep;
        while(curSpeed_Hzms < endSpeed_Hzms)
        {
            computeStepTIMReg(Pluse->Step, curSpeed_Hzms);
            stepTime_ms = computeStepTime(curSpeed_Hzms);           //当前脉冲段发送脉冲消耗的时间
            curSpeed_Hzms = curSpeed_Hzms + acc_Hzms2*stepTime_ms;  //计算下一脉冲段的速度
EndStep:
            //如果下个脉冲段的速度大于endSpeed_Hzms，则只能达到endVel_Hzms
            if(Sm_Float_GreaterEqual(curSpeed_Hzms, endSpeed_Hzms)) 
            {
                computeStepTIMReg(Pluse->Step, endSpeed_Hzms);
                if(Sm_Float_Equal(endSpeed_Hzms, fabs(Pluse->TargetVel_Hzms)))
                    curSpeed_Hzms = fabs(Pluse->TargetVel_Hzms);
            }

            while(Pluse->Step.size() >= limitCnt)
            {
                if(!isRunning)
                {
                    fillDMAPluseData();
                    Sm_TIMx::enable();
                    isRunning = true;
                }
                OSSchedRoundRobinYield(&err);
                OSTimeDly(1, OS_OPT_TIME_PERIODIC, &err);
            }
        }
        
        if(!isRunning)
        {
            fillDMAPluseData();
            Sm_TIMx::enable();
        }
    }

    static void speedDec(float startSpeed_Hzms, float endSpeed_Hzms, uint16_t limitCnt)
    {
        Sm_assert(startSpeed_Hzms >= SM_MIN_SPEED/1000.0F && endSpeed_Hzms >= 0);

        OS_ERR err;
        bool isRunning = false;
        bool stop = false; //减速到最低速度后是否停止脉冲输出，或者以最低速度输出脉冲
        float stepTime_ms = computeStepTime(startSpeed_Hzms);
        float dec_Hzms2 = Pluse->VelModePrm.Dec_Hzms2;                   //减速度，单位为Hz/ms^2
        float curSpeed_Hzms = startSpeed_Hzms - dec_Hzms2*stepTime_ms;   //当前速度，单位为Hz/ms
 
        if(endSpeed_Hzms < SM_MIN_SPEED/1000.0F)
        {
            endSpeed_Hzms = SM_MIN_SPEED/1000.0F;
            stop = true;
        }
       
        //如果一个脉冲段的时间内就能完成减速度
        if(curSpeed_Hzms < endSpeed_Hzms)
            goto EndStep;
        while(curSpeed_Hzms > endSpeed_Hzms)
        {
            computeStepTIMReg(Pluse->Step, curSpeed_Hzms);
            stepTime_ms = computeStepTime(curSpeed_Hzms);           //当前脉冲段发送脉冲消耗的时间
            curSpeed_Hzms = curSpeed_Hzms - dec_Hzms2*stepTime_ms;  //计算下个脉冲段的速度
EndStep:
            //如果下个脉冲段的速度小于目标速度，只能减速到目标速度
            if(Sm_Float_LessEqual(curSpeed_Hzms, endSpeed_Hzms)) 
                computeStepTIMReg(Pluse->Step, endSpeed_Hzms);

            while(Pluse->Step.size() >= limitCnt)
            {
                if(!isRunning)
                {
                    fillDMAPluseData();
                    Sm_TIMx::enable();
                    isRunning = true;
                }
                OSSchedRoundRobinYield(&err);
                OSTimeDly(1, OS_OPT_TIME_PERIODIC, &err);
            }
        }

        if(!isRunning)
        {
            fillDMAPluseData();
            Sm_TIMx::enable();
        }

        if(stop == true)
        {
            while(!Pluse->Step.empty())
                OSTimeDly(1, OS_OPT_TIME_PERIODIC, &err);
            Sm_TIMx::disable();
        }
    }

    //速度模式下改变速度
    static void VelocityChange(void *)
    {
        float targetSpeed = fabs(Pluse->TargetVel_Hzms);
        float curStepSpeed = fabs(Pluse->CurStepVel_Hzms);
        OS_ERR err;

        if(getVelocity() != 0 &&
           Sm_Direction(Pluse->TargetVel_Hzms) != Sm_Direction(Pluse->CurStepVel_Hzms))
        {
            speedDec(curStepSpeed, 0, SM_COMPUTE_STEP);
            curStepSpeed = 0;
        }

        setDirection(Sm_Direction(Pluse->TargetVel_Hzms));

        if(curStepSpeed < targetSpeed)
            speedInc(curStepSpeed, targetSpeed, SM_COMPUTE_STEP);
        else
            speedDec(curStepSpeed, targetSpeed, SM_COMPUTE_STEP);

        OSTaskDel(NULL, &err);
    }

    //位置模式下改变位置
    static void PositionChange(void *)
    {
        uint64_t remainPluseCnt;        //剩下的要发送的脉冲数
        uint64_t constSpeedPluseCnt;    //匀速运动的脉冲数
        uint64_t decPluseCnt;           //从当前速度减速到最低速度所需要的脉冲数
        uint16_t stepPluseCnt;
        float stepTime_ms;
        float nextStepSpeed_Hzms;
        Sm::Step_t step = {0, 0, 0, 0};
        bool isRunning = false;
        OS_ERR err;

        remainPluseCnt = abs(Pluse->TartgetPosition - Pluse->CurStepPosition);

        while(remainPluseCnt > 0)
        {
            if(Pluse->CurStepVel_Hzms < 1.0F/SM_VELOCITY_INTERVAL)
                stepPluseCnt = 1;
            else
                stepPluseCnt = (uint16_t)(SM_VELOCITY_INTERVAL * Pluse->CurStepVel_Hzms + 0.5F);
            stepTime_ms = stepPluseCnt / Pluse->CurStepVel_Hzms;

            if(Sm_Float_Equal(Pluse->CurStepVel_Hzms, Pluse->TargetVel_Hzms))   //已经加速到最高速度
            {
                decPluseCnt = computerDecPluseCnt(Pluse->CurStepVel_Hzms);

                if(remainPluseCnt - decPluseCnt >= stepPluseCnt) //匀速运动
                {
                    constSpeedPluseCnt = remainPluseCnt - decPluseCnt;
                    if(constSpeedPluseCnt < SM_PLUSE_DMA_BUFFER)
                        step.PluseCnt = (uint16_t)constSpeedPluseCnt;
                    else
                        step.PluseCnt = SM_PLUSE_DMA_BUFFER/2U;
                    computeTIMRegValue(step, Pluse->CurStepVel_Hzms);
                    Pluse->Step.push_back(step);
                    remainPluseCnt -= step.PluseCnt;
                }
                else
                {
                    Pluse->CurStepVel_Hzms -= Pluse->PosModePrm.Dec_Hzms2*stepTime_ms;
                    goto Dec;
                }
            }
            else //if(Pluse->CurStepVel_Hzms < Pluse->TargetVel_Hzms)
            {
                nextStepSpeed_Hzms = Pluse->CurStepVel_Hzms +
                    Pluse->PosModePrm.Acc_Hzms2*stepTime_ms;
                if(nextStepSpeed_Hzms > Pluse->TargetVel_Hzms)
                    nextStepSpeed_Hzms = Pluse->TargetVel_Hzms;
                decPluseCnt = computerDecPluseCnt(nextStepSpeed_Hzms);

                if(remainPluseCnt - stepPluseCnt >= decPluseCnt) //加速运动
                {
                    step.PluseCnt = stepPluseCnt;
                    computeTIMRegValue(step, Pluse->CurStepVel_Hzms);
                    Pluse->Step.push_back(step);
                    Pluse->CurStepVel_Hzms = nextStepSpeed_Hzms;
                    remainPluseCnt -= stepPluseCnt;
                }
                else                                             //减速运动
                {
Dec:
                    step.PluseCnt = (uint16_t)(remainPluseCnt -
                            computerDecPluseCnt(Pluse->CurStepVel_Hzms));
                    computeTIMRegValue(step, Pluse->CurStepVel_Hzms);
                    Pluse->Step.push_back(step);
                    Pluse->CurStepVel_Hzms -= Pluse->PosModePrm.Dec_Hzms2*stepTime_ms;
                    remainPluseCnt -= step.PluseCnt;
                }
            }

            if(!isRunning && (Pluse->Step.size() >= SM_COMPUTE_STEP || remainPluseCnt == 0))
            {
                if(Pluse->TartgetPosition - Pluse->CurStepPosition > 0)
                    setDirection(mForwarFlag);
                else
                    setDirection((BitAction)!mForwarFlag);
                fillDMAPluseData();
                isRunning = true;
            }
            else if(isRunning)
            {
                while(Pluse->Step.size() >= SM_COMPUTE_STEP)
                {
                    OSSchedRoundRobinYield(&err);
                    OSTimeDly(1, OS_OPT_TIME_PERIODIC, &err);
                }
            }
        }

        OSTaskDel(NULL, &err); //删除任务本身
    }

    //设置运动正方向和 GPIO BitAction 的对应关系。
    void forwarDirectionConfig(BitAction bitaction)
    {
        Sm_assert(IS_GPIO_BIT_ACTION(bitaction));

        mForwarFlag = bitaction;
    }

    //设置运动方向
    static inline void setDirection(BitAction bitaction)
    {
        Sm_assert(IS_GPIO_BIT_ACTION(bitaction));

        Sm_GPIO_Direction::writeBit(GPIO_Pin_Direction, bitaction);
    }

    //获取运动方向
    static inline BitAction getDirection(void)
    {
        return Sm_GPIO_Direction::readOutputDataBit(GPIO_Pin_Direction);
    }

    static inline Sm::enPluseMode getMode(void)
    {
        return Pluse->PluseMode; 
    }

public:
    Sm_Pluse(uint16_t PSC = 1-1, uint16_t ARR = 144-1)
    {
        Pluse->DMABuff = new uint8_t[SM_PLUSE_DMA_BUFFER];
        std::fill(Pluse->DMABuff,
                  Pluse->DMABuff + SM_PLUSE_DMA_BUFFER-1,
                  TIM_CR1_URS|TIM_CR1_CEN);
 
        Sm_TIMx::setPSC(PSC);
        Sm_TIMx::setARR(ARR);
        //关闭定时器更新中断
        Sm_TIMx::ITConfig(TIM_IT_Update, DISABLE);
        //Sm_TIM基类初始化PWM模式
        Sm_TIMx::PWMConfig(channel);
        Sm_TIMx::setCompare(channel, (uint16_t)((ARR+1)/2.0F - 1), true);
 
        //开始DMA时钟
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMAx, ENABLE);

        DMA_InitTypeDef dma_init;
        dma_init.DMA_PeripheralBaseAddr = DMA_PeripheralBaseAddr ;
        dma_init.DMA_MemoryBaseAddr = (uint32_t)Pluse->DMABuff ;
        dma_init.DMA_DIR = DMA_DIR_PeripheralDST;
        dma_init.DMA_BufferSize = 1; //如果设置寄存器CNDTR的值为0则使用DMA_Init()函数后程序崩溃
        dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
        dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        dma_init.DMA_Mode = DMA_Mode_Normal;
        dma_init.DMA_Priority = DMA_Priority_High;
        dma_init.DMA_M2M = DMA_M2M_Disable;
        //初始化DMA通道
        DMA_Init((DMA_Channel_TypeDef *)DMAx_Channely, &dma_init);
        ((DMA_Channel_TypeDef *)DMAx_Channely)->CNDTR = 0; //恢复寄存器CNDTR的值为0
        
        NVIC_InitTypeDef nvic_init;
        nvic_init.NVIC_IRQChannel = DMAx_Channely_IRQn;
        nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
        nvic_init.NVIC_IRQChannelSubPriority = 0;
        nvic_init.NVIC_IRQChannelCmd = ENABLE;
        //使能DMA中断
        NVIC_Init(&nvic_init);

        //开启DMA传输完成中断
        DMA_ITConfig((DMA_Channel_TypeDef *)DMAx_Channely, DMA_IT_TC, ENABLE);
        //定时器更新时产生DMA请求
        TIM_DMACmd(Sm_TIMx::TIMx(), TIM_DMA_Update, ENABLE);

        //初始化方向信号
        Sm_GPIO_Direction::init_IO(GPIO_Pin_Direction);

        //初始化原点信号
        Sm_GPIO_Home::init_IO(1<<(GPIO_PintSource_Home+1), GPIO_Mode_IPU);
        Sm_GPIO_Home::init_EXTI(GPIO_PintSource_Home);
        // Sm_GPIO_Home::setCallback();

        //初始化限位信号
        Sm_GPIO_ForwardLimit::init_IO(GPIO_Pin_ForwardLimit, GPIO_Mode_IPU);
        Sm_GPIO_ReverseLimit::init_IO(GPIO_Pin_ReverseLimit, GPIO_Mode_IPU);
    }

    static inline void fillDMAPluseData(void)
    {
        Sm::Step_t &step = Pluse->Step.front();

        Sm_TIMx::TIMx()->PSC = step.PSC;
        Sm_TIMx::TIMx()->ARR = step.ARR;
        Sm_TIMx::TIMx()->CCR1 = step.CCR;
        Sm_TIMx::TIMx()->EGR |= TIM_EGR_UG;

        ((DMA_Channel_TypeDef *)DMAx_Channely)->CMAR = (uint32_t)Pluse->DMABuff;
        ((DMA_Channel_TypeDef *)DMAx_Channely)->CCR &= (uint16_t)(~DMA_CCR1_EN);    //关闭DMA通道
        ((DMA_Channel_TypeDef *)DMAx_Channely)->CNDTR = step.PluseCnt;              //设置DMA传输数量
        ((DMA_Channel_TypeDef *)DMAx_Channely)->CCR |= DMA_CCR1_EN;                 //打开DMA通道

        if(getMode() != Sm::Velmode)
        {
            Pluse->DMABuff[Pluse->PreIndex] = TIM_CR1_URS|TIM_CR1_CEN;
            Pluse->PreIndex = (uint16_t)(step.PluseCnt - 1);
            Pluse->DMABuff[Pluse->PreIndex] = TIM_CR1_URS;
            Sm_TIMx::enable();   //开始发送PWM脉冲
            Pluse->GlobalPosition += (getDirection() == mForwarFlag ? step.PluseCnt : -step.PluseCnt);
        }

        Pluse->Step.pop_front();
    }

    virtual inline int setMode(Sm::enPluseMode newMode)
    {
        if(getVel() != 0)
            return -1;
        Pluse->PluseMode = newMode;
        return 0;
    }

    static int32_t getVelocity(void)
    {
        uint16_t PSC = Sm_TIMx::TIMx()->PSC;
        uint16_t ARR = Sm_TIMx::TIMx()->ARR;
        uint16_t CCRx = (&Sm_TIMx::TIMx()->CCR1)[2*channel];

        if( !(Sm_TIMx::TIMx()->CR1 & TIM_CR1_CEN) || CCRx == 0 || CCRx >= ARR
#ifdef ADVANCED_TIMER  //如果是高级定时器
            || !(Sm_TIMx::TIMx()->BDTR & TIM_BDTR_MOE)
#endif
          )
        { return 0; }

        int32_t speed_Hzs = (float)SystemCoreClock / (PSC+1) / (ARR+1) + 0.5F; //单位是(Hz/s)
        return getDirection() == mForwarFlag ? speed_Hzs : -speed_Hzs; 
    }

    virtual int32_t getVel(void)
    {
        return getVelocity();
    }
    
    virtual int64_t getPos(void)
    {
        if(getMode() == Sm::Velmode)
            return 0;
        uint16_t DMARemain = (uint16_t)((DMA_Channel_TypeDef *)DMAx_Channely)->CNDTR;
        return Pluse->GlobalPosition + (getDirection() == mForwarFlag ? -DMARemain : DMARemain);
    }

    //设置速度模式加速度、减速参数，以及起动速度
    //acc和dec的单位为:Hz/ms^2,startUp单位为Hz/s
    virtual int setVelmodePrm(float acc, float dec, uint32_t startUp = SM_STARTUP_SPEED)
    {
        if(Pluse->PluseMode != Sm::Velmode || !Pluse->Step.empty())
            return -1;
        Pluse->VelModePrm.Acc_Hzms2 = fabs(acc);
        Pluse->VelModePrm.Dec_Hzms2 = fabs(dec);
        Pluse->VelModePrm.StartUp_Hzms = fabs(startUp/1000.0F);
        return 0;
    }

    //设置点位模式加速度、减速参数，以及起动速度
    //acc和dec的单位为:Hz/ms^2,startUp单位为Hz/s
    virtual int setPosmodePrm(float acc, float dec, uint32_t startUp = SM_STARTUP_SPEED)
    {
        //点位运动模式如果不是静止状态则不允许更改加减速参数
        if(Pluse->PluseMode != Sm::Posmode || !Pluse->Step.empty())
            return -1;
        Pluse->PosModePrm.Acc_Hzms2 = fabs(acc);
        Pluse->PosModePrm.Dec_Hzms2 = fabs(dec);
        Pluse->PosModePrm.StartUp_Hzms = fabs(startUp/1000.0F);
        return 0;
    }

    //速度模式
    virtual int setVel(int32_t targetVel)
    {
        OS_ERR err = OS_ERR_NONE;
        CPU_SR_ALLOC();

        int32_t curVel = getVel();    //当前速度
        if(curVel == targetVel)
            return -1;

        //如果速度模式还未完成加速，以及点位模式还未静止
        if(!Pluse->Step.empty() && curVel != 0)
            return -1;

        Pluse->CurStepVel_Hzms = curVel/1000.0F;
        Pluse->TargetVel_Hzms = targetVel/1000.0F;

        if(getMode() == Sm::Velmode || getMode() == Sm::Stopmode)
        {
            OS_CRITICAL_ENTER();
            OSTaskCreate((OS_TCB *)     &TCB,               //任务控制块
                         (CPU_CHAR *)   "VelocityChange",   //任务名字
                         (OS_TASK_PTR)  VelocityChange,     //任务函数
                         (void *)       0,                  //传递给任务函数的参数
                         (OS_PRIO)      3,                  //任务优先级
                         (CPU_STK *)    STACK,              //任务堆栈基地址
                         (CPU_STK_SIZE) PLUSE_TASK_STACK_QTY/10,    //任务堆栈深度限位
                         (CPU_STK_SIZE) PLUSE_TASK_STACK_QTY,       //任务堆栈大小
                         (OS_MSG_QTY)   0,                  //任务的消息队列能够接收的消息数量
                         (OS_TICK)      1,                  //时间片轮转的时间长度
                         (void *)       0,                  //用户补充的存储区
                         (OS_OPT)       OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR ,
                         (OS_ERR *)     &err);              //错误返回值
            OS_CRITICAL_EXIT();
        }

        return err;
    }

    virtual int setAbsolutePos(int64_t absPos)
    {
        OS_ERR err = OS_ERR_NONE;
        CPU_SR_ALLOC();

        if(getMode() != Sm::Posmode || getVel() != 0)
            return -1;

        Pluse->TartgetPosition = absPos;
        Pluse->CurStepPosition = getPos();
        Pluse->CurStepVel_Hzms = Pluse->PosModePrm.StartUp_Hzms;
        Pluse->TargetVel_Hzms = fabs(Pluse->TargetVel_Hzms);

        OS_CRITICAL_ENTER();
        OSTaskCreate((OS_TCB *)     &TCB,                       //任务控制块
                     (CPU_CHAR *)   "PositionChange",           //任务名字
                     (OS_TASK_PTR)  PositionChange,             //任务函数
                     (void *)       0,                          //传递给任务函数的参数
                     (OS_PRIO)      3,                          //任务优先级
                     (CPU_STK *)    STACK,                      //任务堆栈基地址
                     (CPU_STK_SIZE) PLUSE_TASK_STACK_QTY/10,    //任务堆栈深度限位
                     (CPU_STK_SIZE) PLUSE_TASK_STACK_QTY,       //任务堆栈大小
                     (OS_MSG_QTY)   0,                          //任务的消息队列能够接收的消息数量
                     (OS_TICK)      1,                          //时间片轮转的时间长度
                     (void *)       0,                          //用户补充的存储区
                     (OS_OPT)       OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR ,
                     (OS_ERR *)     &err);                      //错误返回值
        OS_CRITICAL_EXIT();
        return err;
    }

    virtual int setRelativePos(int64_t relPos)
    {
        return setAbsolutePos(getPos() + relPos);
    }

    virtual int stop(bool emergency = false)
    {
        OS_ERR err;

        if(emergency) 
        {
            Sm_TIMx::disable();

            if(getMode() != Sm::Velmode)
                Pluse->GlobalPosition = getPos();

            Pluse->DMABuff[Pluse->PreIndex] = TIM_CR1_URS|TIM_CR1_CEN;
            ((DMA_Channel_TypeDef *)DMAx_Channely)->CCR &= (uint16_t)(~DMA_CCR1_EN);    //关闭DMA通道
            ((DMA_Channel_TypeDef *)DMAx_Channely)->CNDTR = 0;                          //设置DMA传输数量
            ((DMA_Channel_TypeDef *)DMAx_Channely)->CCR |= DMA_CCR1_EN;                 //打开DMA通道

            OSTaskDel(&TCB, &err);

            Pluse->Step.clear();
        }
        else
        {
            if(getMode() == Sm::Velmode)
            {
                Pluse->Step.clear();
                setVel(0);
            }
            else
            {
                
            }
        }
    }

    virtual void limitCheck(void)
    {
        int32_t vel = getVelocity();
        BitAction forwardBit = Sm_GPIO_ForwardLimit::readInputDataBit(GPIO_Pin_ForwardLimit);
        BitAction reverseBit = Sm_GPIO_ReverseLimit::readInputDataBit(GPIO_Pin_ReverseLimit);

        if((vel >= SM_MIN_SPEED && forwardBit == Bit_RESET)          //正向运行超限
            || (vel <= -SM_MIN_SPEED && reverseBit == Bit_RESET))    //反向运行超限
        {
            stop(true); 
        }
    }
};

#endif
