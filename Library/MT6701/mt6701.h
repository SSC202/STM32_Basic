/**
 * @file    STM32 MT6701 磁编码器驱动程序
 * @brief   MT6701 是 14Bit 的单圈绝对角度检测传感器,拥有 ABZ/PWM/模拟量/I2C/SPI 等多种信息输出方式,这里采用 IIC 进行数据输出
 * @attention   使用前请选择驱动方式(硬件/软件IIC)
 *              使用时使用MT6701_Init()进行初始化
 *              在一个循环的程序中定时运行MT6701_Get_Angle()
 * @author  SSC
 */
#ifndef __MT6701_H
#define __MT6701_H

#include "stm32g4xx.h"

/************************ 驱动方式选择 ********************************/

#define MT6701_SoftWare_IIC 1 // 使用软件IIC
#define MT6701_HardWare_IIC 0 // 使用硬件IIC

/*************************** 端口定义 *********************************/

#if (MT6701_SoftWare_IIC == 1)

#define MT6701_SCL_Port GPIOA
#define MT6701_SDA_Port GPIOB
#define MT6701_SCL_Pin  GPIO_PIN_15
#define MT6701_SDA_Pin  GPIO_PIN_7

#endif

/************************** 寄存器定义 ********************************/

#define MT6701_ANGLE_REG 0x03 // 角度寄存器

#define MT6701_ADDR      0x06 // 设备地址

/*************************** 函数定义 *********************************/
void MT6701_Get_Angle(float *encoder_angle);

#endif
