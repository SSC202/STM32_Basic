/**
 * @brief   OPS定位系统解码
 * @author  SSC
 * @note    注意，OPS需要使用RS232转TTL模块且在TTL端使用TX--TX RX--RX接线方法
 */
#ifndef __OPS_H
#define __OPS_H

#include "stm32f4xx.h"
#include "usart.h"
/************************ 用户定义 ***************************/
#define OPS_UART_HANDLE huart8 // 串口选项
#define OPS_UART        UART8  // 串口选项

/************************ 结构体定义 ***************************/

typedef struct OPS_t {
    float pos_x;   // x坐标
    float pos_y;   // y坐标
    float z_angle; // 俯仰角
    float x_angle; // 横滚角
    float y_angle; // 偏航角
    float w_z;     // 角速度
} OPS_t;           // OPS数据结构体

extern OPS_t OPS_Data;
extern uint8_t ops_buffer[28];

uint8_t OPS_Decode(void);

#endif