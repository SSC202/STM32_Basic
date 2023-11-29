/**
 * @brief   串口设备管理函数
 * @note    借用的RoboMaster机器人的源码并添加注释，源码地址https://github.com/RoboMaster/RoboRTS-Firmware
 */

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include "fifo.h"
#include "stm32f1xx.h"

#define USART1_RX_BUFFER_SIZE   (512)
#define USART1_TX_BUFFER_SIZE   (512)
#define USART1_TX_FIFO_SIZE     (2048)

#define USART1_PRINTF_BUFF_SIZE (128)

typedef uint32_t (*usart_call_back)(uint8_t *buf, uint16_t len);

typedef struct
{
    UART_HandleTypeDef *uart_h;  // 串口句柄
    DMA_HandleTypeDef *dma_h;    // 串口对应的DMA句柄
    uint16_t rx_buffer_size;     // 接收缓冲区大小
    uint8_t *rx_buffer;          // 接收缓冲区
    uint16_t read_start_index;   // 接收缓冲区的读指针索引
    usart_call_back call_back_f; //  串口回调函数，接收数据时调用

    uint8_t *tx_buffer;      // 发送缓冲区
    uint16_t tx_buffer_size; // 发送缓冲区大小
    fifo_s_t tx_fifo;        // 发送缓冲区FIFO
    uint8_t *tx_fifo_buffer; // 发送缓冲区FIFO的指针
    uint16_t tx_fifo_size;   // 发送FIFO缓冲区的大小
    uint8_t is_sending;      // 发送状态
} usart_manage_obj_t;

typedef enum {
    INTERRUPT_TYPE_UART     = 0,    // 空闲中断
    INTERRUPT_TYPE_DMA_HALF = 1,    // 半传输中断
    INTERRUPT_TYPE_DMA_ALL  = 2     // 全传输中断
} interrput_type;

void usart_rx_callback_register(usart_manage_obj_t *m_obj, usart_call_back fun);
int32_t usart_transmit(usart_manage_obj_t *m_obj, uint8_t *buf, uint16_t len);

void usart1_manage_init(void);
void usart1_transmit(uint8_t *buff, uint16_t len);
void usart1_idle_callback(void);
void usart1_rx_callback_register(usart_call_back fun);

void debug_raw_printf(char *fmt, ...);

#endif
