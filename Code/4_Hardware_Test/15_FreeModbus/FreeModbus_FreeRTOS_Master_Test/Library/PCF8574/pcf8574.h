#ifndef __PCF8574_H
#define __PCF8574_H

#include "stm32f4xx.h"
/****************配置选择*********************/
/* 是否使能硬件IIC */
// #define PCF8574_IIC_HardWare
/* 是否使能软件IIC */
#define PCF8574_IIC_SoftWare
/* 是否使用FreeRTOS */
// #define USE_FREERTOS
/****************芯片定义*********************/
#define PCF8574_ADDR_Write 0x40 // 写地址
#define PCF8574_ADDR_Read  0x41 // 读地址

/****************函数定义*********************/
uint8_t PCF8574_Read_OneByte(void);
void PCF8574_Write_OneByte(uint8_t io_state);
void PCF8574_Write_Bit(uint8_t bit, uint8_t state);
uint8_t PCF8574_Read_Bit(uint8_t bit);
void PCF8574_Init(void);
#endif