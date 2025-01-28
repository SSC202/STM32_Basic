/**
 * @file    DJI 遥控器解码文件
 * @note    使用遥控器时，应当将USART1 RX DMA使能并在接收中断回调函数中解码
 *          波特率为100000，DMA 为 Circular 模式
*/
#ifndef __AS69_H
#define __AS69_H

#include "usart.h"
#include "stm32f4xx.h"

/********************** 结构体定义 ***************************/
typedef struct
{
    int16_t ch0;
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;
    int8_t left;
    int8_t right;
    int16_t wheel;
} Remote_t;

/************************ 用户变量 ****************************/
extern Remote_t raw_data;
extern uint8_t as69_buffer[18];

/************************* 用户函数 *****************************/
int AS69_Decode(void);
#endif
