#include <stdio.h>
#include <stdlib.h>
#include <string.h.>

#include "Sm_Config.h"
#include "Sm_SysTick.h"
#include "Sm_GPIOA.h"
#include "Sm_GPIOB.h"
#include "Sm_GPIOC.h"
#include "Sm_GPIOD.h"
#include "Sm_GPIOE.h"
#include "Sm_USART1.h"
#include "Sm_USART2.h"
#include "Sm_USART3.h"
#include "Sm_UART4.h"
#include "Sm_UART5.h"
#include "Sm_Debug.h"
#include "Sm_TIM1.h"
#include "Sm_TIM2.h"
#include "Sm_TIM3.h"
#include "Sm_TIM4.h"
#include "Sm_TIM5.h"
#include "Sm_TIM6.h"
#include "Sm_TIM7.h"
#include "Sm_TIM8.h"
#include "Sm_RTC.h"
#include "Sm_DAC1.h"
#include "Sm_DAC2.h"
#include "Sm_ADC1.h"
#include "Sm_ADC3.h"
#include "Sm_SPI1.h"
#include "Sm_SPI2.h"
#include "Sm_SPI3.h"
#include "Sm_Delay.h"

/*
 * //init_global_in_bss虽然定义成全局变量并给它赋值，但由于把它的地址强制放在.bss区，所以实际上它是个未赋值全局变量，初始值为空
 * char __attribute__ ((section(".bss"))) init_global_in_bss[] = "this is an init global variable, but in .bss";
 * //init_global是一个初始化全局变量，放在.data段中
 * char init_global[] = "this is an init global variable in .data";
 * //extbss_global是一个初始化全局变量，放在外部SRAM的.extbss段中，.exbss中的全局变量是不能被初始化的，所以打印不出字符串
 * char Sm_EXTBSS extbss_global[] = "a .extbss global variable, can not be init";
 */

//LED和蜂鸣器测试
void LED_Init(void)
{
    Sm_GPIOB::init_IO(GPIO_Pin_5|GPIO_Pin_8);
    Sm_GPIOE::init_IO(GPIO_Pin_5);
}
void led1On(void){Sm_GPIOB::resetBits(GPIO_Pin_5);}
void led1Off(void){Sm_GPIOB::setBits(GPIO_Pin_5);}
void led2On(void){Sm_GPIOE::resetBits(GPIO_Pin_5);}
void led2Off(void){Sm_GPIOE::setBits(GPIO_Pin_5);}
void beepOn(void){Sm_GPIOB::resetBits(GPIO_Pin_8);}
void beepOff(void){Sm_GPIOB::setBits(GPIO_Pin_8);}
void gpioe5toggle(void *){PEout(5) ^= 1;}

void keyUpCallback(void *arg)
{
    EXTI->IMR &= ~(uint32_t)arg;    //屏蔽中断

    led1On();
    Sm_Debug("Key Up\n");

    EXTI->IMR |= (uint32_t)arg;     //恢复中断
};

void keyLeftCallback(void *arg)
{
    EXTI->IMR &= ~(uint32_t)arg;    //屏蔽中断

    led1Off();
    Sm_Debug("Key Left\n");

    EXTI->IMR |= (uint32_t)arg;     //恢复中断
};

void keyDownCallback(void *arg)
{
    EXTI->IMR &= ~(uint32_t)arg;    //屏蔽中断

    led2On();
    Sm_Debug("Key Down\n");

    EXTI->IMR |= (uint32_t)arg;     //恢复中断
};

void keyRightCallback(void *arg)
{
    EXTI->IMR &= ~(uint32_t)arg;    //屏蔽中断

    led2Off();
    Sm_Debug("Key Right\n");

    EXTI->IMR |= (uint32_t)arg;     //恢复中断
};

void Key_Init(void)
{
    Sm_GPIOA::init_IO(GPIO_Pin_0, GPIO_Mode_IPD);
    Sm_GPIOA::init_EXTI(GPIO_PinSource0, EXTI_Trigger_Rising);
    Sm_GPIOA::setITCallback(GPIO_PinSource0, keyUpCallback, (void *)EXTI_Line0);

    Sm_GPIOE::init_IO(GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4, GPIO_Mode_IPU);
    Sm_GPIOE::init_EXTI(GPIO_PinSource2);
    Sm_GPIOE::setITCallback(GPIO_PinSource2, keyLeftCallback, (void *)EXTI_Line2);

    Sm_GPIOE::init_EXTI(GPIO_PinSource3);
    Sm_GPIOE::setITCallback(GPIO_PinSource3, keyDownCallback, (void *)EXTI_Line3);

    Sm_GPIOE::init_EXTI(GPIO_PinSource4, EXTI_Trigger_Rising);
    Sm_GPIOE::setITCallback(GPIO_PinSource4, keyRightCallback, (void *)EXTI_Line4);
}

void showRTCTime(void *arg)
{
    char buf[1024];//当运行ucosiii后，buf的空间最小要1024个字节，否则程序执行strftime后会挂掉
    time_t time;
    struct tm loc_tm;

    time = Sm_RTC::getDateTime();
    localtime_r(&time, &loc_tm);
    strftime(buf, 1024, "%c\n", &loc_tm);
    Sm_Debug(buf);
};

void RTC_Init(void)
{

    //时间设置
    int ret = Sm_RTC::initialize();
    if(ret > -1)
    {
        if(ret == 0)
        {
            Sm_RTC::setDateTime(2016, 11, 17, 15, 50, 10);
        }
        Sm_RTC::secondITConfig(ENABLE);
        Sm_RTC::setSecCallback(showRTCTime);
    }
}

//USART测试
Sm_USART1 usart1;
Sm_USART2 usart2;
Sm_USART3 usart3;
//在gcc编译器中把Sm_UART4和Sm_UART5定义成全局变量能正常运行。但是在keil自带编译器中会会程序崩溃，原因还未找到。
// Sm_UART4 uart4;
// Sm_UART5 uart5;
void usart1_sendback(uint16_t recByte, void *){usart1.send(recByte);}
void usart2_sendback(uint16_t recByte, void *){usart2.send(recByte);}
void usart3_sendback(uint16_t recByte, void *){usart3.send(recByte);}
// void uart4_sendback(uint16_t recByte, void *){uart4.send(recByte);}
// void uart5_sendback(uint16_t recByte, void *){uart5.send(recByte);}
void USART_Init(void)
{
    //设置每一个串口接收到数据后将数据原样发送回去
    usart1.setComProcess(usart1_sendback, NULL);
    usart2.setComProcess(usart2_sendback, NULL);
    usart3.setComProcess(usart3_sendback, NULL);
    // uart4.setComProcess(uart4_sendback, NULL);
    // uart5.setComProcess(uart5_sendback, NULL);
}

uint16_t WaveBuff12Bit[32];
uint8_t WaveBuff8Bit[32];

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    Sm_SysTick::setCLKSource(SysTick_CLKSource_HCLK_Div8);

    Sm_Debug("...Is Running...\n");

    //设置4个抢断优先级和4个响应优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    LED_Init();
    // led1Off(); //LED1灭
    // led2Off(); //LED2灭
    // Key_Init(); //初始化上、下、左、右4个按键
    // RTC_Init();
    // USART_Init();
    
    /*
     * //测试malloc和new申请的内存是否在外部SRAM中
     * int *heap_test = new int;
     * *heap_test = 10086;
     * //如果要用st-link进行仿真，则要注释掉下面的4个函数，否则仿真失败
     * Sm_Debug("init_global_in_bss should be in .bss:%p %s\n", init_global_in_bss, init_global_in_bss);
     * Sm_Debug("init_global should be in .data:%p %s\n", init_global, init_global);
     * Sm_Debug("extbss_global should be in .extbss:%p %s\n", extbss_global, extbss_global);
     * Sm_Debug("heap_test:%p %d\n", heap_test, *heap_test);
     */

    //高级定时器RCR测试
    /*
     * Sm_TIM1 tim1(72-1, 1000-1); //PA8输出1KHZ的PWM波形
     * tim1.setCallback([&](void *){PEout(5) = ~PEout(5);});
     * tim1.PWMConfig(Sm::PWM_Channel1);
     * tim1.setCompare(Sm::PWM_Channel1, (TIM1->ARR+1)/2-1, true);
     * tim1.setRCR(10-1, true); //设置高级定时器的RCR寄存器,每10个更新事件产生一个更新中断，PE5输出50HZ的PWM波形。
     * tim1.enable();
     */

    //TIM1外部脉冲计数测试，TIM1的ETR管脚为PA12
    /*
     * Sm_TIM1 tim1;
     * tim1.initETRClockMode2(1-1, 10000-1);
     * tim1.setCallback([&](void *){Sm_Debug("Timer1 Exti Pluse:%d\n", TIM1->ARR+1);});
     * tim1.enable();
     */

    //定时器编码器模式测试
    /*
     * Sm_TIM2::initialize(0, 0xffff);
     * Sm_TIM2::setCallback([&](void *){trace_printf("encoder update...\n");});
     * Sm_TIM2::encoderConfig();
     * Sm_TIM2::enable();
     */

    //通用定时器PWM测试
    Sm_TIM3::initialize(2-1, 7200-1);
    Sm_TIM3::PWMConfig(Sm::PWM_Channel_1);    //PA6
    Sm_TIM3::PWMConfig(Sm::PWM_Channel_2);    //PA7
    Sm_TIM3::PWMConfig(Sm::PWM_Channel_3);    //PB0
    Sm_TIM3::PWMConfig(Sm::PWM_Channel_4);    //PB1
    Sm_TIM3::setCompare(Sm::PWM_Channel_1, 7200/2-1, true);
    Sm_TIM3::setCompare(Sm::PWM_Channel_2, 7200/3-1, true);
    Sm_TIM3::setCompare(Sm::PWM_Channel_3, 7200/4-1, true);
    Sm_TIM3::setCompare(Sm::PWM_Channel_4, 7200/5-1, true);
    Sm_TIM3::enable();

    //SysTick定时测试。SysTick定时和SysTick延时不能同时使用，会相互干扰
    /*
     * Sm_SysTick::init();
     * Sm_SysTick::ITConfig(ENABLE);
     * Sm_SysTick::setTimeOut_ms(2, gpioe5toggle);
     * Sm_SysTick::enable();
     */

    //DAC 通道1(PA4)测试--------------------------------------------------------
/*
 *     Sm_TIM6::initialize();
 *     Sm_TIM6::selectOutputTrigger(TIM_TRGOSource_Update);
 *     Sm_TIM6::enable();
 * 
 *     Sm_DAC1::initialize();
 *     // Sm_DAC1::setData(0x0fff);                           //输出参考最大电压值，测量输出的实际电压值
 *     Sm_DAC1::setRefVoltage(3.30f);                      //用实际输出的最大电压值进行校准
 *     Sm_DAC1::triggerConfig(DAC_Trigger_T6_TRGO);        //设置触发源
 *     // uint16_t *WaveBuff12Bit = new uint16_t[32];         //申请12位波形数据缓存空间
 *     // uint16_t *WaveBuff12Bit = (uint16_t *)malloc(sizeof(uint16_t)*32);         //申请12位波形数据缓存空间
 * 
 *     //右对齐方法生成正弦波: 
 *     //必须先设置对齐方式再生成波形数据和设置DMA，因为createSine()、createEscalator()
 *     //和DMAConfig()等函数根据不同的数据对齐方式作不同处理。一但重新设置对齐方式就必须
 *     //重新生成波形数据和重新设置DMA。
 *     Sm_DAC1::setAlign(DAC_Align_12b_R);                 //设置12位数据右对齐
 *     Sm_DAC1::createSine(WaveBuff12Bit, 32, 3.3f);       //生成12位数据右对齐的正弦波波形数据
 *     Sm_DAC1::DMAConfig(WaveBuff12Bit, 32);              //正弦波输出
 */

    //左对齐方法生成正弦波: 
    //必须先设置对齐方式再生成波形数据和设置DMA，因为createSine()、createEscalator()
    //和DMAConfig()等函数根据不同的数据对齐方式作不同处理。一但重新设置对齐方式就必须
    //重新生成波形数据和重新设置DMA。
    // Sm_DAC1::setAlign(DAC_Align_12b_L);                 //设置12位数据左对齐。
    // Sm_DAC1::createSine(WaveBuff12Bit, 32, 3.3f);       //生成12位数据左对齐的正弦波波形数据
    // Sm_DAC1::DMAConfig(WaveBuff12Bit, 32);              //正弦波输出
    //--------------------------------------------------------------------------

    //DAC 通道2(PA5)测试--------------------------------------------------------
/*
 *     Sm_TIM7::initialize();
 *     Sm_TIM7::selectOutputTrigger(TIM_TRGOSource_Update);
 *     Sm_TIM7::enable();
 * 
 *     Sm_DAC2::initialize();
 *     // Sm_DAC2::setData(0x0fff);                           //输出参考最大电压值，测量输出的实际电压值
 *     Sm_DAC2::setRefVoltage(3.30f);                      //用实际输出的最大电压值进行校准
 *     Sm_DAC2::triggerConfig(DAC_Trigger_T7_TRGO);        //设置触发源
 *     
 *     // uint8_t *WaveBuff8Bit = new uint8_t[32];            //申请8位数据对齐波形数据缓存空间
 *     // uint8_t *WaveBuff8Bit = (uint8_t *)malloc(sizeof(uint8_t)*32);         //申请8位波形数据缓存空间
 *     Sm_DAC2::setAlign(DAC_Align_8b_R);                  //设置8位数据右对齐
 *     Sm_DAC2::createEscalator(WaveBuff8Bit, 32, 2.5f);   //生成8位级数对齐的梯形波形数据
 *     Sm_DAC2::DMAConfig(WaveBuff8Bit, 32);               //梯形波输出
 */
    //--------------------------------------------------------------------------

    //DAC 双通道(PA4 PA5)测试---------------------------------------------------
/*
 *     Sm_TIM6::initialize();
 *     Sm_TIM6::selectOutputTrigger(TIM_TRGOSource_Update);
 *     Sm_TIM6::enable();
 * 
 *     Sm_DAC1::initialize();
 *     Sm_DAC1::setRefVoltage(3.47f);
 *     Sm_DAC1::triggerConfig(DAC_Trigger_T6_TRGO);
 *     uint8_t *SineBuff = new uint8_t[32];
 *     Sm_DAC1::setAlign(DAC_Align_8b_R);
 *     Sm_DAC1::createSine(SineBuff, 32, 3.3f);
 * 
 *     Sm_DAC2::initialize();
 *     Sm_DAC2::setRefVoltage(3.47f);
 *     Sm_DAC2::triggerConfig(DAC_Trigger_T6_TRGO);
 *     uint8_t *EscalatorBuff = new uint8_t[32];
 *     Sm_DAC2::setAlign(DAC_Align_8b_R);
 *     Sm_DAC2::createEscalator(EscalatorBuff, 32, 3.3f);
 * 
 *     uint32_t *DualBuff = new uint32_t[32];
 *     for(int i = 0; i < 32; ++i)
 *         DualBuff[i] = (EscalatorBuff[i] << 8) | SineBuff[i];
 *     Sm_DAC2::DualDMAConfig(DAC_Align_8b_R, DualBuff, 32);
 */
    //--------------------------------------------------------------------------
    
    //ADC测试-------------------------------------------------------------------
    /*
     * uint16_t *adc_buff = new uint16_t[16];
     * Sm_ADC1 adc1(16, ENABLE, ENABLE); //使用16个规则通道，多通道扫描，循环扫描。
     * adc1.regularChannelConfig(ADC_Channel_0, 1);    //A0
     * adc1.regularChannelConfig(ADC_Channel_1, 2);    //A1
     * adc1.regularChannelConfig(ADC_Channel_2, 3);    //A2
     * adc1.regularChannelConfig(ADC_Channel_3, 4);    //A3
     * adc1.regularChannelConfig(ADC_Channel_4, 5);    //A4
     * adc1.regularChannelConfig(ADC_Channel_5, 6);    //A5
     * adc1.regularChannelConfig(ADC_Channel_6, 7);    //A6
     * adc1.regularChannelConfig(ADC_Channel_7, 8);    //A7
     * adc1.regularChannelConfig(ADC_Channel_8, 9);    //B0
     * adc1.regularChannelConfig(ADC_Channel_9, 10);   //B1
     * adc1.regularChannelConfig(ADC_Channel_10, 11);  //C0
     * adc1.regularChannelConfig(ADC_Channel_11, 12);  //C1
     * adc1.regularChannelConfig(ADC_Channel_12, 13);  //C2
     * adc1.regularChannelConfig(ADC_Channel_13, 14);  //C3
     * adc1.regularChannelConfig(ADC_Channel_14, 15);  //C4
     * adc1.regularChannelConfig(ADC_Channel_15, 16);  //C5
     * adc1.regularDMAConfig(adc_buff, 16);
     * adc1.softwareStartConvCmd(ENABLE);
     */

    /*
     * Sm_DAC1::initialize();
     * Sm_DAC1::setData(3567);
     * Sm_ADC3::initialize(1, DISABLE, DISABLE);
     * Sm_ADC3::regularChannelConfig(ADC_Channel_1, 1);
     * Sm_Debug("ADC Data: %d\n", Sm_ADC3::softwareGetValue());
     */
    //--------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------
/*
 *     Sm_SPI2::SPI_Init(SPI_Direction_1Line_Tx, SPI_Mode_Master, SPI_FirstBit_MSB, SPI_CPOL_High, SPI_CPHA_1Edge);
 *     Sm_SPI2::dataSizeConfig(SPI_DataSize_16b);
 *     Sm_SPI2::speedConfig(SPI_BaudRatePrescaler_2);
 * 
 *     while(Sm_SPI2::getFlagStatus(SPI_I2S_FLAG_TXE) == RESET);
 *     Sm_SPI2::sendData(0x8000);
 *     while(Sm_SPI2::getFlagStatus(SPI_I2S_FLAG_TXE) == RESET);
 *     Sm_SPI2::sendData(0x0001<<1);
 *     while(Sm_SPI2::getFlagStatus(SPI_I2S_FLAG_BSY) == SET);
 *     PEout(5) = 0;
 *     PEout(5) = 1;
 */

    //--------------------------------------------------------------------------
    /*
     * Sm_GPIOB::init_IO(GPIO_Pin_13|GPIO_Pin_15);
     * auto sendSerialData = [&](uint16_t data){
     *     for(int i = 0x8000; i > 0; i >>= 1)
     *     {
     *         PBout(13) = 0;
     *         data & i ? PBout(15) = 1 : PBout(15) = 0;
     *         PBout(13) = 1;
     *     }
     * };
     * sendSerialData(0x8000);
     * sendSerialData(0x0001);
     * PEout(5) = 0;
     * PEout(5) = 1;
     */

    int value;
    while(1)
    {
        // scanf("%d", &value);
        // Sm_Debug("%d", value * 10);
        // Sm_Debug("%d %d %d %d %d %d\n", adc_buff[11], adc_buff[12], adc_buff[13], adc_buff[14], adc_buff[15], adc_buff[16]);
        Sm_SysTick::delay_ms(10);
        PBout(5) ^= 1;
        PEout(5) ^= 1;
    }

    return 0;
}

// ----------------------------------------------------------------------------
