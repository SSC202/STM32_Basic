#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#include "stm32f1xx.h"
/**************结构体定义****************/

typedef struct Ring_Buffer{
    uint8_t *buf;    // 环形缓冲区指针
    int32_t size; // 环形缓冲区大小

    uint16_t read_mirror : 1;  // 1位表示读指针是否超出缓冲区
    uint16_t read_pos : 15;    // 15位表示读指针位置
    uint16_t write_mirror : 1; // 1位表示写指针是否超出缓冲区
    uint16_t write_pos : 15;   // 15位表示写指针位置
} Ring_Buffer;

typedef struct Ring_Buffer* Ring_Buffer_t;

typedef enum {
    RINGBUFFER_EMPTY,    // 环形缓冲区为空
    RINGBUFFER_HALFFULL, // 环形缓冲区为非空非满
    RINGBUFFER_FULL      // 环形缓冲区为满
} Ring_Buffer_State;

int32_t Ring_Buffer_Read_OneByte(Ring_Buffer_t pbuf, uint8_t *ch);
int32_t Ring_Buffer_Write_OneByte(Ring_Buffer_t pbuf, const uint8_t ch);
int32_t Ring_Buffer_Peek_OneByte(Ring_Buffer_t pbuf, uint8_t *ch);

#endif