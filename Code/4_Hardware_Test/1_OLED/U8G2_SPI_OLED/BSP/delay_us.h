#ifndef __DELAY_US_H
#define __DELAY_US_H
#include "tim.h"

void delay_us(TIM_HandleTypeDef *htimx,uint16_t us);

#endif
