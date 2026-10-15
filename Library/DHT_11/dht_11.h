#ifndef __DHT_11_H
#define __DHT_11_H

#include "stm32f1xx.h"
/******************引脚定义段*******************/
#define     DHT11_DAT_Port      GPIOA
#define     DHT11_DAT_Pin       GPIO_PIN_6
/******************用户函数段*******************/
uint8_t DHT11_Get_Data(float *temp,float *humi);

#endif