#ifndef __USART1_UNPACK_H
#define __USART1_UNPACK_H

#include "drv_uart.h"

#define REF_FRAME_MAX_SIZE 128

typedef void (*ref_rx_complete_callabck_t)(uint8_t *buf, uint16_t len);

// 解包步骤枚举类型
typedef enum {
    STEP_HEADER_SOF1   = 0, // 帧头起始标志1解析步骤
    STEP_HEADER_SOF2   = 1, // 帧头起始标志2解析步骤
    STEP_TAIL_END1     = 2, // 帧尾结束标志1解析步骤
    STEP_TAIL_END2     = 3, // 帧尾结束标志2解析步骤
    STEP_LENGTH_UNPACK = 5, // 数据长度低字节解析步骤
} unpack_step_e;

typedef struct
{
    uint8_t len;
    uint8_t data[4];
} frame_data_t;

// 数据解包结构体
typedef struct
{
    fifo_s_t *data_fifo;                         // 数据FIFO
    uint16_t data_len;                           // 指定数据长度
    uint8_t protocol_packet[REF_FRAME_MAX_SIZE]; // 存储协议数据缓冲区
    unpack_step_e unpack_step;                   // 解包步骤的枚举类型
    uint16_t index;                              // 索引，用于处理解包过程中的位置信息
} unpack_data_t;

void usart1_unpack_init(ref_rx_complete_callabck_t rx_callback);
uint32_t usart1_rx_data_handle(uint8_t *data, uint16_t len);
uint32_t usart1_unpack_fifo(void);

#endif