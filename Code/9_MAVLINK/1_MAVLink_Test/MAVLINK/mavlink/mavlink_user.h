#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "mavlink_types.h"
#include "usart.h"

// 系统 ID 和 组件 ID
static mavlink_system_t mavlink_system = {
    1,                                      // System ID (1-255)
    1                                       // Component ID (1-255)
};

// MavLink 能使用的最大通道数
#define MAVLINK_COMM_NUM_BUFFERS 4

// mavlink 句柄
typedef struct
{
    UART_HandleTypeDef *huart; // 该通道对应的串口句柄
    uint8_t rx_buffer;         // 接收缓冲
    mavlink_message_t msg;     // the message data
    mavlink_status_t status;   // 统计信息
} _Mavlink_handle_t;

// mavlink 支持多个通道，使用全局句柄方便对每一个通道配置（通常，一个通道对应一个串口）
extern _Mavlink_handle_t hMAVLink[MAVLINK_COMM_NUM_BUFFERS];

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS

#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS
static inline void _Mavlink_SEND_UART_BYTES(mavlink_channel_t chan, const uint8_t *buf, uint16_t len)
{
    if (chan < MAVLINK_COMM_NUM_BUFFERS) {
        while (HAL_UART_Transmit(hMAVLink[chan].huart, (uint8_t *)buf, len, HAL_MAX_DELAY) == HAL_BUSY)
            ;
    } else {
        // 如果卡在这里，说明 chan 超过了最大值
        // 请检查通道号是否给错
        // 如果当前通道数量不够用，请增大 wtr_mavlink.h 中 MAVLINK_COMM_NUM_BUFFERS 的值
        while (1) {}
    }
}
#define MAVLINK_SEND_UART_BYTES(chan, buf, len) _Mavlink_SEND_UART_BYTES(chan, buf, len)
#endif // MAVLINK_USE_CONVENIENCE_FUNCTIONS

#define MAVLINK_GET_CHANNEL_STATUS
static inline mavlink_status_t *mavlink_get_channel_status(uint8_t chan)
{
    if (chan < MAVLINK_COMM_NUM_BUFFERS) {
        return &hMAVLink[chan].status;
    } else {
            // 如果卡在这里，说明 chan 超过了最大值
            // 请检查通道号是否给错
            // 如果当前通道数量不够用，请增大 wtr_mavlink.h 中 MAVLINK_COMM_NUM_BUFFERS 的值
        while (1) {}
        return NULL;
    }
}

#define MAVLINK_GET_CHANNEL_BUFFER
static inline mavlink_message_t *mavlink_get_channel_buffer(uint8_t chan)
{
    if (chan < MAVLINK_COMM_NUM_BUFFERS) {
        return &hMAVLink[chan].msg;
    } else {
        // 如果卡在这里，说明 chan 超过了最大值
        // 请检查通道号是否给错
        // 如果当前通道数量不够用，请增大 wtr_mavlink.h 中 MAVLINK_COMM_NUM_BUFFERS 的值
        while (1) {}
        return NULL;
    }
}

#include "mavlink.h"

/**
 * @brief 将串口和 mavlink 的通道绑定
 *
 * @param huart 串口句柄
 * @param chan  通道号
 */
void _Mavlink_BindChannel(UART_HandleTypeDef *huart, mavlink_channel_t chan);

/**
 * @brief 开启接收中断（记得先绑定通道和串口）
 *
 * @param chan 通道号
 * @return 0：成功；-1：未绑定串口；HAL_BUSY：串口忙；HAL_ERROR：中断开启出错
 */
int _Mavlink_StartReceiveIT(mavlink_channel_t chan);

/**
 * @brief 接收到完整消息且校验通过后会调用这个函数。在这个函数里调用解码函数就可以向结构体写入收到的数据
 *
 * @param msg 接收到的消息
 * @return
 */
void _Mavlink_MsgRxCpltCallback(mavlink_message_t *msg);

/**
 * @brief 使用方法同 HAL_UART_Receive_IT。HAL 库中的这个函数在同时收发时会出现锁死现象，这个函数取消了加锁解锁操作，修复了锁死 Bug
 *
 * @param huart
 * @param pData
 * @param Size
 * @return HAL_StatusTypeDef
 */
static inline HAL_StatusTypeDef __UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    /* Check that a Rx process is not already ongoing */
    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        /* Process Locked */
        // __HAL_LOCK(huart);

        /* Set Reception type to Standard reception */
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        huart->pRxBuffPtr    = pData;
        huart->RxXferSize    = Size;
        huart->RxXferCount   = Size;

        huart->ErrorCode     = HAL_UART_ERROR_NONE;
        huart->RxState       = HAL_UART_STATE_BUSY_RX;

        /* Process Unlocked */
        //   __HAL_UNLOCK(huart);

        /* Enable the UART Parity Error Interrupt */
        __HAL_UART_ENABLE_IT(huart, UART_IT_PE);

        /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
        __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

        /* Enable the UART Data Register not empty Interrupt */
        __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

/**
 * @brief 这个函数需要在 HAL_UART_RxCpltCallback 中调用
 *
 * @param huart 收到数据的串口（函数内部有判断，只有这个 huart 是 channel 对应的 huart 时，该函数才会处理）
 * @param chan
 */
static inline void _Mavlink_UARTRxCpltCallback(UART_HandleTypeDef *huart, mavlink_channel_t chan)
{
    if (chan < MAVLINK_COMM_NUM_BUFFERS) {
        if (huart == hMAVLink[chan].huart) {
            if (mavlink_parse_char(chan, hMAVLink[chan].rx_buffer, &(hMAVLink[chan].msg), &(hMAVLink[chan].status))) {
                _Mavlink_MsgRxCpltCallback(&(hMAVLink[chan].msg));
            }
            __UART_Receive_IT(huart, &(hMAVLink[chan].rx_buffer), 1);
        }
    } else {
        // 如果卡在这里，说明 chan 超过了最大值
        // 请检查通道号是否给错
        // 如果当前通道数量不够用，请增大 wtr_mavlink.h 中 MAVLINK_COMM_NUM_BUFFERS 的值
        while (1) {}
    }
}

#ifdef __cplusplus
}
#endif
