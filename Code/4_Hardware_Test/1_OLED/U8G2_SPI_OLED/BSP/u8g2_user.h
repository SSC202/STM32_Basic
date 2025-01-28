#ifndef __U8G2_USER_H
#define __U8G2_USER_H

#include "stm32f1xx.h"
#include "u8g2.h"
#include "tim.h"
#include "delay_us.h"

/**********************驱动方式选择************************/
//#define SoftWare_SPI    //软件SPI
#define SoftWare_IIC    //软件IIC
/************************引脚定义*************************/
#ifdef SoftWare_SPI
#define SCL_GPIO_Port GPIOB
#define SCL_GPIO_Pin  GPIO_PIN_4
#define SDA_GPIO_Port GPIOB
#define SDA_GPIO_Pin  GPIO_PIN_5
#define CS_GPIO_Port GPIOB
#define CS_GPIO_Pin  GPIO_PIN_8
#define DC_GPIO_Port GPIOB
#define DC_GPIO_Pin  GPIO_PIN_7
#define RST_GPIO_Port GPIOB
#define RST_GPIO_Pin  GPIO_PIN_6
#endif
#ifdef SoftWare_IIC
#define SCL_GPIO_Port GPIOB
#define SCL_GPIO_Pin  GPIO_PIN_4
#define SDA_GPIO_Port GPIOB
#define SDA_GPIO_Pin  GPIO_PIN_5
#endif
/************************时钟定义************************/
#define delay_htim  htim4
/************************回调函数************************/
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
/************************初始化函数**********************/
void OLED_Init(u8g2_t *_u8g2);
#endif

