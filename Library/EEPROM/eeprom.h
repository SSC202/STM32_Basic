/**
 * @brief   外置 EEPROM 的 STM32 驱动库
 * @attention   使用前，需要选择硬件或者软件IIC，选用硬件IIC将默认使用IIC 1，选用软件IIC后需要自行确定软件IIC对应的引脚并设置为高速开漏输出
 *              使用EEPROM_Read()进行读取，EEPROM_Write()进行写入
 */
#ifndef __EEPROM_H
#define __EEPROM_H

#include "stm32f1xx.h"

/****************配置选择*********************/

#define EEPROM_IIC_HardWare 0 /* 是否使能硬件IIC */
#define EEPROM_IIC_SoftWare 1 /* 是否使能软件IIC */
#ifndef USE_FREERTOS
#define USE_FREERTOS 0        /* 是否使用FreeRTOS */
#endif
/****************芯片定义*********************/
#define EEPROM_ADDR_Write   0xA0    // 写地址
#define EEPROM_ADDR_Read    0xA1    // 读地址
/****************端口定义*********************/
#if (EEPROM_IIC_SoftWare == 1)
#define IIC_SCL_Port GPIOB
#define IIC_SDA_Port GPIOB
#define IIC_SCL_Pin GPIO_PIN_6
#define IIC_SDA_Pin GPIO_PIN_7
#endif
/****************函数定义*********************/
void EEPROM_Read(uint8_t addr, uint8_t *rev_buf, uint16_t size);
void EEPROM_Write(uint8_t addr, uint8_t *send_buf, uint16_t size);
#endif