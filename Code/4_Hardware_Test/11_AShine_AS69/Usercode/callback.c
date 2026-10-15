#include "usermain.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART8) {
        wtrMavlink_UARTRxCpltCallback(huart, MAVLINK_COMM_0);
    }
}

void wtrMavlink_MsgRxCpltCallback(mavlink_message_t *msg)
{
    HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
    int debug_msgid = msg->msgid;
    switch (msg->msgid) {
        case 209:
            mavlink_msg_joystick_air_decode(msg, &msg_joystick_air);
            break;
        default:
            break;
    }
}