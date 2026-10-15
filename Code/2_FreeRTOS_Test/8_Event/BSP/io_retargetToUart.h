/**
 * @file io_retargetToUart.h
 * @author X. Y.
 * @brief io 重定向，支持输入和输出（注意不能在中断中使用）
 * @version 3.6
 * @date 2022-09-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "usart.h"

#define IORETARGET_USE_RTOS              // 是否使用 RTOS
#define IORETARGET_STDIN_BUFFER_SIZE 128 // stdin 缓冲区大小，设为 0 表示不使用 stdin 缓冲（使用缓冲区还要开启串口接收中断）

#if (defined IORETARGET_STDIN_BUFFER_SIZE) && (IORETARGET_STDIN_BUFFER_SIZE > 0)
typedef struct
{
    char buffer[IORETARGET_STDIN_BUFFER_SIZE];
    volatile int start;
    volatile int end;
    const int total_size;
} IO_BufferQueue_t;
#endif

void IORetarget_Uart_Receive_IT();

void IORetarget_Uart_RxCpltCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif
