#ifndef __OLED_H
#define __OLED_H

#include "stm32f1xx.h"

/******************驱动方式选择**********************/
// #define OLED_HardWare_IIC        //硬件IIC
// #define OLED_SoftWare_IIC        //软件IIC
#define OLED_HardWare_SPI // 硬件SPI
// #define OLED_SoftWare_SPI        //软件SPI

// #define USE_FREERTOS             //使用FreeRTOS
/********************引脚定义************************/
#ifdef OLED_SoftWare_IIC
/* 注意：在使用软件IIC时，务必使用高速开漏输出 */
#define IIC_SCL_Port GPIOB
#define IIC_SDA_Port GPIOB
#define IIC_SCL_Pin  GPIO_PIN_4
#define IIC_SDA_Pin  GPIO_PIN_5
#endif
#ifdef OLED_HardWare_SPI
#define SPI_CS_Port  GPIOC
#define SPI_CS_Pin   GPIO_PIN_5
#define SPI_DC_Port  GPIOC
#define SPI_DC_Pin   GPIO_PIN_4
#define SPI_RST_Port GPIOB
#define SPI_RST_Pin  GPIO_PIN_0
#endif
/********************OLED全局定义********************/
#define Max_Column      144  // OLED列宽
#define OLED_BRIGHTNESS 0xFF // OLED亮度

/********************函数定义************************/
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t charSize, uint8_t Is_Reverse);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t Size, uint8_t Is_Reverse);
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t charSize, uint8_t Is_Reverse);
void OLED_ShowCHinese(unsigned char x, unsigned char y, unsigned char num);
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);
void OLED_Init(void);

#endif