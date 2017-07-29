#ifndef SM_CONFIGURE_H
#define SM_CONFIGURE_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define DEBUG_VIA_SWO       //通过SWO打印debug信息 
// #define DEBUG_VIA_OPENOCD   //通过Openocd打印debug信息  
// #define DEBUG_VIA_USART     //通过串口打印debug信息 
#if defined(DEBUG_VIA_SWO) && defined(DEBUG_VIA_OPENOCD)
#error "There is a conflic between SWO and openocd!"
#endif

// #define Sm_UCOS_Support //是否支持UCOS系统
#ifdef Sm_UCOS_Support
#include "includes.h"
#endif

#ifdef __cplusplus
namespace Sm{

typedef void (*RCC_ClockCmd)(uint32_t, FunctionalState);

typedef struct {
    void (*pfun)(void *);
    void *arg;
} CALLBACK;

using ::FunctionalState;

}
#endif

//定义未初始化全局变量时使用Sm_EXTBSS前缀可以把变量定义在外部SRAM中
//如：char Sm_EXTBSS extVar; 变量extVar就被定义在外部SRAM中
#define Sm_EXTBSS __attribute__ ((section(".extbss")))  

/**************************浮点数比较是否相等**********************************/
#define Sm_Float_Equal(f1, f2) (fabs(f1-f2) < 0.0001F)
#define Sm_Float_GreaterEqual(f1, f2) (f1 > f2 || fabs(f1-f2) < 0.0001F)
#define Sm_Float_LessEqual(f1, f2) (f1 < f2 || fabs(f1-f2) < 0.0001F)

/************************外设默认抢断优先级和响应优先级************************/
//GPIO外部中断默认抢断优先级和响应优先级
#define Default_EXTI_NVIC_IRQChannelPreemptionPriority 0
#define Default_EXTI_NVIC_IRQChannelSubPriority 0
//定时器中断默认抢断优先级和响应优先级
#define Default_TIM_NVIC_IRQChannelPreemptionPriority 1
#define Default_TIM_NVIC_IRQChannelSubPriority 0
//串口中断默认抢断优先级和响应优先级
#define Default_USART_NVIC_IRQChannelPreemptionPriority 2
#define Default_USART_NVIC_IRQChannelSubPriority 0
//RTC时钟中断默认推断优先级和响应优先级
#define Default_RTC_NVIC_IRQChannelPreemptionPriority 3
#define Default_RTC_NVIC_IRQChannelSubPriority 3

/**********************位带操作,实现类似51的GPIO控制功能***********************/
/***************具体实现思想,参考<<CM3权威指南>>第五章(87页~92页)**************/
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr&0xF0000000)+0x2000000+((addr&0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr) *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr, n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr, n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr, n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr, n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr, n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr, n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr, n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr, n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr, n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr, n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr, n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr, n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr, n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr, n)  //输入

#endif //SM_CONFIGURE_H
