/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : Sm_Delay.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/03/29 21:20:09
*  @Last Modified  : 2017/03/29 21:37:20
********************************************************************************
*/

#ifndef SM_DELAY_H
#define SM_DELAY_H

#include "Sm_Config.h"

#ifdef __cplusplus
extern "C" {
#endif

void delay_us(uint16_t nus);
void delay_ms(uint16_t nms);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: SM_DELAY_H */
