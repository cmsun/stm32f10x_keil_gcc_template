#include "Sm_GPIOA.h"
#include "Sm_DAC1.h"

template<> uint32_t Sm_DAC1::mDataAlign = DAC_Align_12b_R;
template<> uint8_t Sm_DAC1::mBitShift = 0;
template<> float Sm_DAC1::mRefVoltage = 3.30f;
template<> float Sm_DAC1::mVoltageRate = 0x0fff / mRefVoltage; 

void initDAC2GPIOA5(void);
typedef Sm_DAC<DAC_Channel_2, DHR12R2_OFFSET, initDAC2GPIOA5, DMA2_Channel4_BASE> Sm_DAC2_Base;
template<> uint32_t Sm_DAC2_Base::mDataAlign = DAC_Align_12b_R;
template<> uint8_t Sm_DAC2_Base::mBitShift = 0;
template<> float Sm_DAC2_Base::mRefVoltage = 3.30f;
template<> float Sm_DAC2_Base::mVoltageRate = 0x0fff / mRefVoltage; 

void initDAC1GPIOA4(void)
{
    Sm_GPIOA::init_IO(GPIO_Pin_4, GPIO_Mode_AIN);
}

void initDAC2GPIOA5(void)
{
    Sm_GPIOA::init_IO(GPIO_Pin_5, GPIO_Mode_AIN);
}
