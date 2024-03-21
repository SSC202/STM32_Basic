/**
 * @file wtr_mavlink.c
 * @author X. Y.
 * @brief 移植到 stm32 的 mavlink
 * @version 1.2
 * @date 2022-11-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "wtr_mavlink.h"

wtrMavlink_handle_t hMAVLink[MAVLINK_COMM_NUM_BUFFERS] = {0};

void wtrMavlink_BindChannel(UART_HandleTypeDef *huart, mavlink_channel_t chan)
{
    if (chan < MAVLINK_COMM_NUM_BUFFERS) {
        hMAVLink[chan].huart = huart;
    } else {
        // 如果卡在这里，说明 chan 超过了最大值
        // 请检查通道号是否给错
        // 如果当前通道数量不够用，请增大 wtr_mavlink.h 中 MAVLINK_COMM_NUM_BUFFERS 的值
        while (1) {}
    }
}

int wtrMavlink_StartReceiveIT(mavlink_channel_t chan)
{
    if (chan < MAVLINK_COMM_NUM_BUFFERS) {
        if (hMAVLink[chan].huart == NULL) {
            return -1;
        }

        return WTR_UART_Receive_IT(hMAVLink[chan].huart, &(hMAVLink[chan].rx_buffer), 1);
    } else {
        // 如果卡在这里，说明 chan 超过了最大值
        // 请检查通道号是否给错
        // 如果当前通道数量不够用，请增大 wtr_mavlink.h 中 MAVLINK_COMM_NUM_BUFFERS 的值
        while (1) {}
        return -1;
    }
}

__weak void wtrMavlink_MsgRxCpltCallback(mavlink_message_t *msg)
{
    switch (msg->msgid) {
        case 1:
            // id = 1 的消息对应的解码函数(mavlink_msg_xxx_decode)
            break;
        case 2:
            // id = 2 的消息对应的解码函数(mavlink_msg_xxx_decode)
            break;
        // ......
        default:
            break;
    }
}
