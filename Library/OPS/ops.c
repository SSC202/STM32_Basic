#include "ops.h"
#include <stdio.h>

OPS_t OPS_Data = {0};
uint8_t ops_buffer[28]; // 接收缓冲区

/**
 * @brief   码盘解码函数
 */
uint8_t OPS_Decode(void)
{
    static union {
        uint8_t data[24];
        float ActVal[6];
    } posture;
    if (ops_buffer[0] != 0x0D || ops_buffer[1] != 0x0A || ops_buffer[26] != 0x0A || ops_buffer[27] != 0x0D) {
        for (int i = 0; i < 28; i++) {
            ops_buffer[i] = 0;
        }
        return 1;
    }
    for (int i = 0; i < 24; i++) {
        posture.data[i] = ops_buffer[i + 2];
    }
    OPS_Data.z_angle = posture.ActVal[0];
    OPS_Data.x_angle = posture.ActVal[1];
    OPS_Data.y_angle = posture.ActVal[2];
    OPS_Data.pos_x   = posture.ActVal[3];
    OPS_Data.pos_y   = posture.ActVal[4];
    OPS_Data.w_z     = posture.ActVal[5];
    return 0;
}

/**
 * @brief   串口回调函数
 */
__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == OPS_UART) {
        OPS_Decode();
    }
}