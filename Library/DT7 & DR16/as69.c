#include "as69.h"
#include "string.h"
#include "stdlib.h"
#include "usart.h"
#include "main.h"

/************************************宏定义段*************************************/
#define AS69_FRAME_LEN 18
#define CH0_BIAS       1024
#define CH1_BIAS       1024
#define CH2_BIAS       1024
#define CH3_BIAS       1024
#define WHEEL_BIAS     1024
#define CH_RANGE       660.0

Remote_t raw_data;
uint8_t as69_buffer[18];
/************************************用户函数段*************************************/
/**
 * @brief   AS69 解码函数
 */
int AS69_Decode(void)
{
    raw_data.ch0 = ((int16_t)as69_buffer[0] | ((int16_t)as69_buffer[1] << 8)) & 0x07FF;
    raw_data.ch1 = (((int16_t)as69_buffer[1] >> 3) | ((int16_t)as69_buffer[2] << 5)) & 0x07FF;
    raw_data.ch2 = (((int16_t)as69_buffer[2] >> 6) | ((int16_t)as69_buffer[3] << 2) |
                    ((int16_t)as69_buffer[4] << 10)) &
                   0x07FF;
    raw_data.ch3   = (((int16_t)as69_buffer[4] >> 1) | ((int16_t)as69_buffer[5] << 7)) & 0x07FF;
    raw_data.left  = ((as69_buffer[5] >> 4) & 0x000C) >> 2;
    raw_data.right = ((as69_buffer[5] >> 4) & 0x0003);
    raw_data.wheel = ((int16_t)as69_buffer[16]) | ((int16_t)as69_buffer[17] << 8);
    return 0;
}

/**
 * @brief   串口1回调函数
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        AS69_Decode();
    }
}