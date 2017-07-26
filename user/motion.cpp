#include "Sm_TIM1_Pluse.h"
#include "Sm_TIM2_Pluse.h"
#include "Sm_TIM3_Pluse.h"
#include "Sm_TIM4_Pluse.h"
#include "Sm_TIM5_Pluse.h"
#include "Sm_TIM8_Pluse.h"
#include "Sm_RTC.h"
#include "Sm_Debug.h"

#include "includes.h"

#include "motion.h"

//需要将Sm_TIM1和Sm_TIM8类及它的派生类的全局实例定义在main.cpp中，否则stm32不能正常启动
static Sm_TIM1_Pluse Pluse1;
static Sm_TIM2_Pluse Pluse2;
static Sm_TIM3_Pluse Pluse3;
static Sm_TIM4_Pluse Pluse4;
static Sm_TIM5_Pluse Pluse5;
static Sm_TIM8_Pluse Pluse8;
static Sm_Pluse_Base *PluseArray[AXIS_QTY] = {&Pluse1, &Pluse2, &Pluse3, &Pluse4, &Pluse5, &Pluse8};

OS_TCB PluseMSGTask_TCB;
CPU_STK PluseMSGTask_STK[PLUSE_TASK_STACK_QTY];

OS_TCB CheckLimitTask_TCB;
CPU_STK CheckLimitTask_STK[PLUSE_TASK_STACK_QTY];

MotionIns_t PluseMSGTask_Ins;

int Sm_forwarDirectionCoonfig(uint8_t axis, BitAction bitaction)
{
    if(axis < 1 || axis > AXIS_QTY) 
        return -1;
    PluseArray[axis-1]->forwarDirectionConfig(bitaction);
}

int Sm_SetVelmode(uint8_t axis)
{
    if(axis < 1 || axis > AXIS_QTY) 
        return -1;
    return PluseArray[axis-1]->setMode(Sm::Velmode);
}

int Sm_SetPosmode(uint8_t axis)
{
    if(axis < 1 || axis > AXIS_QTY) 
        return -1;
    return PluseArray[axis-1]->setMode(Sm::Posmode);
}

int Sm_SetVelmodePrm(uint8_t axis, float acc, float dec, uint32_t startUp)
{
    if(axis < 1 || axis > AXIS_QTY) 
        return -1;
    return PluseArray[axis-1]->setVelmodePrm(acc, dec, startUp);
}

int Sm_SetPosmodePrm(uint8_t axis, float acc, float dec, uint32_t startUp)
{
    if(axis < 1 || axis > AXIS_QTY) 
        return -1;
    return PluseArray[axis-1]->setPosmodePrm(acc, dec, startUp);
}

int32_t Sm_GetVel(uint8_t axis)
{
    if(axis < 1 || axis > AXIS_QTY) 
        return -1;
    return PluseArray[axis-1]->getVel();
}

int64_t Sm_GetPos(uint8_t axis)
{
    if(axis < 1 || axis > AXIS_QTY) 
        return -1;
    return PluseArray[axis-1]->getPos();
}

int Sm_SetVel(uint8_t axis, int32_t vel)
{
    OS_ERR err = OS_ERR_NONE;

    if(axis < 1 || axis > AXIS_QTY) 
        return -1;

    if(PluseArray[axis-1]->setVel(vel) == OS_ERR_TASK_CREATE_ISR)
    {
        //如果当前函数在中断中被调用，则发消息
        PluseMSGTask_Ins.Ins = VelIns;
        PluseMSGTask_Ins.Axis = axis;
        PluseMSGTask_Ins.Argument = vel;
        OSTaskQPost(&PluseMSGTask_TCB, &PluseMSGTask_Ins,
                    sizeof(MotionIns_t), OS_OPT_POST_FIFO, &err); 
    }

    return err;
}

int Sm_SetAbsolutePos(uint8_t axis, int64_t absPos)
{
    OS_ERR err = OS_ERR_NONE;

    if(axis < 1 || axis > AXIS_QTY) 
        return -1;

    if(PluseArray[axis-1]->setAbsolutePos(absPos) == OS_ERR_TASK_CREATE_ISR)
    {
        //如果当前函数在中断中被调用，则发消息
        PluseMSGTask_Ins.Ins = AbsIns;
        PluseMSGTask_Ins.Axis = axis;
        PluseMSGTask_Ins.Argument = absPos;
        OSTaskQPost(&PluseMSGTask_TCB, &PluseMSGTask_Ins,
                    sizeof(MotionIns_t), OS_OPT_POST_FIFO, &err); 
    }

    return err;
}

int Sm_SetRelativePos(uint8_t axis, int64_t relPos)
{
    OS_ERR err = OS_ERR_NONE;

    if(axis < 1 || axis > AXIS_QTY) 
        return -1;

    if(PluseArray[axis-1]->setRelativePos(relPos) == OS_ERR_TASK_CREATE_ISR)
    {
        //如果当前函数在中断中被调用，则发消息
        PluseMSGTask_Ins.Ins = RelIns;
        PluseMSGTask_Ins.Axis = axis;
        PluseMSGTask_Ins.Argument = relPos;
        OSTaskQPost(&PluseMSGTask_TCB, &PluseMSGTask_Ins,
                    sizeof(MotionIns_t), OS_OPT_POST_FIFO, &err); 
    }

    return err;
}

//Sm_SetVel、Sm_SetAbsolutePos、setRelativePos这三个函数会创建任务，而UCOS不能在中断中创建创建。
//如果是在中断中调用这三个函数，则发送消息，在中断结束后通过PluseMSGTask这个任务重新调用原来的运动函数。
static void pluseMSGTask(void *)
{
    OS_ERR err;
    OS_MSG_SIZE size;
    MotionIns_t *instruction;

    while(1)
    {
        instruction = (MotionIns_t *)OSTaskQPend((OS_TICK)      0,
                                                 (OS_OPT)       OS_OPT_PEND_BLOCKING,
                                                 (OS_MSG_SIZE *)&size,
                                                 (CPU_TS *)     0,
                                                 (OS_ERR *)     &err);

        switch(instruction->Ins)
        {
            case VelIns:
                Sm_SetVel(instruction->Axis, instruction->Argument); break;
            case AbsIns:
                Sm_SetAbsolutePos(instruction->Axis, instruction->Argument); break;
            case RelIns:
                Sm_SetRelativePos(instruction->Axis, instruction->Argument); break;
        }
    }
}

static void chkeckLimtTask(void *)
{
    OS_ERR err;

    while(1)
    {
        for(int i = 0; i < 6; ++i)
        {
            PluseArray[i]->limitCheck();
        }

        OSSchedRoundRobinYield(&err);
        OSTimeDly(1, OS_OPT_TIME_PERIODIC, &err);
    }
}

#include "Sm_GPIOE.h"
#include "diag/Trace.h"

OS_TCB KeyScanTask_TCB;
//把keyScanTask任务的任务堆栈设置成8字节对齐，就可以在该任务中使用printf()家族函数来格式化浮点数。
__attribute__ ((aligned(8))) CPU_STK KeyTask_STK[PLUSE_TASK_STACK_QTY];

int64_t ExtPluseCnt = 0; //TIM1外部脉冲计数测试，用来存放接收到的脉冲数

#define AXIS 3

static void keyScanTask(void *)
{
    OS_ERR err;
    static uint8_t keyLeft_up = 1, keyRight_up = 1, keyDown_up = 1;

    while(1)
    {
        if(Sm_GPIOE::readInputDataBit(GPIO_Pin_2) == Bit_RESET && keyLeft_up) //key left
        {
            keyLeft_up = 0;
            Sm_Debug("Key Left\n");

            Sm_Debug("Position:%d\n", (int32_t)Sm_GetPos(AXIS));
            // Sm_Debug("Velocity:%d\n", Sm_GetVel(AXIS));

            // Sm_Debug("RecPluseCount:%d\n", (int32_t)(ExtPluseCnt + TIM1->CNT));
            // Sm_Debug("RecPluseCount:%d\n", (int32_t)(TIM2->CNT));
            // trace_printf("RecPluseCount:%d\n", (int32_t)(TIM4->CNT));
        }
        else if(Sm_GPIOE::readInputDataBit(GPIO_Pin_2) == Bit_SET)
        {
            keyLeft_up = 1;
        }

        if(Sm_GPIOE::readInputDataBit(GPIO_Pin_4) == Bit_RESET && keyRight_up) //key right
        {
            keyRight_up = 0;
            Sm_Debug("Key Right\n");

            Sm_SetAbsolutePos(AXIS, -1000000);
            // Sm_SetVel(AXIS, -150000);
        }
        else if(Sm_GPIOE::readInputDataBit(GPIO_Pin_4) == Bit_SET)
        {
            keyRight_up = 1;
        }

        if(Sm_GPIOE::readInputDataBit(GPIO_Pin_3) == Bit_RESET && keyDown_up) //key down
        {
            keyDown_up = 0;
            Sm_Debug("Key Down\n");

            Sm_SetAbsolutePos(AXIS, 1000000);
            // Sm_SetVel(AXIS, 150000);
        }
        else if(Sm_GPIOE::readInputDataBit(GPIO_Pin_3) == Bit_SET)
        {
            keyDown_up = 1;
        }

        OSTimeDly(2, OS_OPT_TIME_PERIODIC, &err);
    }
}

static void createTask(void)
{
    OS_ERR err;
    CPU_SR_ALLOC();

    OS_CRITICAL_ENTER();

    OSTaskCreate((OS_TCB *)     &PluseMSGTask_TCB,
                 (CPU_CHAR *)   "pluseMSGTask",
                 (OS_TASK_PTR)  pluseMSGTask,
                 (void *)       0,
                 (OS_PRIO)      6,
                 (CPU_STK *)    PluseMSGTask_STK,
                 (CPU_STK_SIZE) PLUSE_TASK_STACK_QTY/10,
                 (CPU_STK_SIZE) PLUSE_TASK_STACK_QTY,
                 (OS_MSG_QTY)   10,
                 (OS_TICK)      1,
                 (void *)       0,
                 (OS_OPT)       OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR *)     &err);

    OSTaskCreate((OS_TCB *)     &CheckLimitTask_TCB,
                 (CPU_CHAR *)   "checkLimtTask",
                 (OS_TASK_PTR)  chkeckLimtTask,
                 (void *)       0,
                 (OS_PRIO)      3,
                 (CPU_STK *)    CheckLimitTask_STK,
                 (CPU_STK_SIZE) PLUSE_TASK_STACK_QTY/10,
                 (CPU_STK_SIZE) PLUSE_TASK_STACK_QTY,
                 (OS_MSG_QTY)   0,
                 (OS_TICK)      1,
                 (void *)       0,
                 (OS_OPT)       OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR *)     &err);

    OSTaskCreate((OS_TCB *)     &KeyScanTask_TCB,
                 (CPU_CHAR *)   "keyScanTask",
                 (OS_TASK_PTR)  keyScanTask,
                 (void *)       0,
                 (OS_PRIO)      6,
                 (CPU_STK *)    KeyTask_STK,
                 (CPU_STK_SIZE) PLUSE_TASK_STACK_QTY/10,
                 (CPU_STK_SIZE) PLUSE_TASK_STACK_QTY,
                 (OS_MSG_QTY)   0,
                 (OS_TICK)      1,
                 (void *)       0,
                 (OS_OPT)       OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR *)     &err);

    OS_CRITICAL_EXIT();
}

int main(int argc, const char *argv[])
{
    Sm_Debug("motion running...\n");

    //key initialize
    Sm_GPIOE::init_IO(GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4, GPIO_Mode_IPU);

    //设置4个抢断优先级和4个响应优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    Sm_SetPosmode(AXIS);
    // Sm_forwarDirectionCoonfig(AXIS, Bit_RESET);
    Sm_SetPosmodePrm(AXIS, 0.5, 0.5, 1000);
    Sm_SetVel(AXIS, 150000);

    /*
     * Sm_SetVelmode(AXIS);
     * Sm_SetVelmodePrm(AXIS, 0.25F, 0.25F, 1000);
     * Sm_SetVel(AXIS, 1000);
     */

    //第一个任务
    auto start_task = [&](void *)
    {
        OS_ERR err;

        CPU_Init();
        OS_CPU_SysTickInit(5*72000);                //运行SysTick，5ms调度一次任务
        OSSchedRoundRobinCfg(DEF_ENABLED, 1, &err); //允许时间片

        createTask();                               //创建其它任务

        OSTaskDel(NULL, &err);                      //删除当前任务
    };

    OS_ERR err;
    OSInit(&err);
    CPU_SR_ALLOC();
    OS_CRITICAL_ENTER();
    OSTaskCreate((OS_TCB *)     new OS_TCB ,    //任务控制块
                 (CPU_CHAR *)   "start_task",   //任务名字
                 (OS_TASK_PTR)  start_task,     //任务函数
                 (void *)       0,              //传递给任务函数的参数
                 (OS_PRIO)      6,              //任务优先级
                 (CPU_STK *)    new CPU_STK[64],//任务堆栈基地址
                 (CPU_STK_SIZE) 64/10,          //任务堆栈深度限位
                 (CPU_STK_SIZE) 64,             //任务堆栈大小
                 (OS_MSG_QTY)   0,              //任务的消息队列能够接收的消息数量
                 (OS_TICK)      1,              //时间片轮转的时间长度
                 (void *)       0,              //用户补充的存储区
                 (OS_OPT)       OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR *)     &err);          //错误返回值
    OS_CRITICAL_EXIT();
    OSStart(&err);

    while(1)    
    {

    }

    return 0;
}

