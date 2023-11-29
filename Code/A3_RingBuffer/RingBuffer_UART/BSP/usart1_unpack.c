#include "usart1_unpack.h"

#define USART1_FIFO_BUFLEN 512
static fifo_s_t usart1_rx_fifo;                      // 串口1接收FIFO
static uint8_t usart1_rx_buffer[USART1_FIFO_BUFLEN]; // 串口1数据接收缓冲区
static ref_rx_complete_callabck_t ref_rx_complete;   // 接收完成回调函数
static unpack_data_t usart1_unpack_obj;              // 串口1数据解包结构体

/**
 * @brief   解包初始化函数
 */
void usart1_unpack_init(ref_rx_complete_callabck_t rx_callback)
{
    fifo_s_init(&usart1_rx_fifo, usart1_rx_buffer, USART1_FIFO_BUFLEN);

    usart1_unpack_obj.data_fifo   = &usart1_rx_fifo;
    usart1_unpack_obj.index       = 0;
    usart1_unpack_obj.data_len    = 0;
    usart1_unpack_obj.unpack_step = STEP_HEADER_SOF1;

    ref_rx_complete = rx_callback;
}

/**
 * @brief   接收数据处理函数
 */
uint32_t usart1_rx_data_handle(uint8_t *data, uint16_t len)
{
    fifo_s_puts(&usart1_rx_fifo, (char *)data, len);

    if (ref_rx_complete != NULL) {
        ref_rx_complete(data, len);
    }
    return 0;
}

/**
 * @brief   解包数据处理函数
 */
void usart1_data_handler(uint8_t *p_frame)
{
    uint8_t data[3];
    for (int i = 0; i < 3; i++) {
        data[i] = p_frame[i + 2];
    }
    if (data[1] == 0) {
        usart1_transmit((uint8_t *)("OK"), 3);
    }
}

/**
 * @brief   解包函数，需要进行轮询
 */
uint32_t usart1_unpack_fifo(void)
{
    uint8_t byte         = 0;
    uint8_t sof[2]       = {0x0A, 0x0C};
    uint8_t end[2]       = {0x0C, 0x0A};
    unpack_data_t *p_obj = &usart1_unpack_obj;

    while (fifo_s_used(p_obj->data_fifo)) {
        byte = fifo_s_get(p_obj->data_fifo);
        switch (p_obj->unpack_step) {
            case STEP_HEADER_SOF1: {
                if (byte == sof[0]) {
                    p_obj->protocol_packet[p_obj->index++] = byte;
                    p_obj->unpack_step                     = STEP_HEADER_SOF2;
                } else {
                    p_obj->index = 0;
                }
            } break;
            case STEP_HEADER_SOF2: {
                if (byte == sof[1]) {
                    p_obj->protocol_packet[p_obj->index++] = byte;
                    p_obj->unpack_step                     = STEP_LENGTH_UNPACK;
                } else {
                    p_obj->index = 0;
                }
            } break;
            case STEP_LENGTH_UNPACK: {
                if (p_obj->data_len <= 4) {
                    p_obj->protocol_packet[p_obj->index++] = byte;
                    p_obj->data_len++;
                } else {
                    p_obj->unpack_step = STEP_TAIL_END1;
                }
            } break;
            case STEP_TAIL_END1: {
                if (byte == end[0]) {
                    p_obj->protocol_packet[p_obj->index++] = byte;
                    p_obj->unpack_step                     = STEP_TAIL_END2;
                } else {
                    p_obj->index = 0;
                }
            } break;
            case STEP_TAIL_END2: {
                if (byte == end[1]) {
                    p_obj->protocol_packet[p_obj->index++] = byte;
                    p_obj->index                           = 0;
                    p_obj->data_len                        = 0;
                    p_obj->unpack_step                     = STEP_HEADER_SOF1;
                    usart1_data_handler(p_obj->protocol_packet);
                } else {
                    p_obj->index = 0;
                }
            } break;
            default: {
                p_obj->unpack_step = STEP_HEADER_SOF1;
                p_obj->index       = 0;
            } break;
        }
    }
}