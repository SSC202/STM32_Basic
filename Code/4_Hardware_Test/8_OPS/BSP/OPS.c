#include "OPS.h"
#include <stdio.h>

OPS_t OPS_Data = {0};
uint8_t ops_buffer[28]; // 接收缓冲区

#if (USE_POLL == 1)

#define OPS_FRAME_LEN 28
/**
 * @brief   OPS数据接收函数
 * @return  0: 成功；1: 未接收到足够字节；2: 接收到错误格式的数据
 */
uint8_t OPS_Decode(void)
{
    uint16_t size = 0;
    static union {
        uint8_t data[24];
        float ActVal[6];
    } posture;
    HAL_UARTEx_ReceiveToIdle(&OPS_UART_HANDLE, ops_buffer, OPS_FRAME_LEN, &size, 0xFF);
    if (size != 28) {
        for (int i = 0; i < 28; i++) {
            ops_buffer[i] = 0;
        }
        return 1;
    }
    if (ops_buffer[0] != 0x0D || ops_buffer[1] != 0x0A || ops_buffer[26] != 0x0A || ops_buffer[27] != 0x0D) {
        for (int i = 0; i < 28; i++) {
            ops_buffer[i] = 0;
        }
        return 2;
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

#endif
#if (USE_IT == 1)
/*************************变量及结构体定义段******************************/
typedef union {
    uint8_t data[24];
    float ActVal[6];
} Posture;

Posture posture;
static uint8_t count = 0;
uint8_t i            = 0;
uint8_t ch[1];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == OPS_UART) {
        OPS_Decode();
    }
}

/**
 * @brief		OPS数据处理函数
 */
uint8_t OPS_Decode(void)
{
    HAL_UART_Receive_IT(&OPS_UART_HANDLE, (uint8_t *)&ch, 1);
    // USART_ClearITPendingBit( USART1, USART_FLAG_RXNE);
    // HAL_UART_IRQHandler(&huart6); // 该函数会清空中断标志，取消中断使能，并间接调用回调函数
    switch (count) // uint8_t隐转为int
    {
        case 0:
            if (ch[0] == 0x0d)
                count++;
            else
                count = 0;
            break;
        case 1:
            if (ch[0] == 0x0a) {
                i = 0;
                count++;
            } else if (ch[0] == 0x0d)
                ;
            else
                count = 0;
            break;
        case 2:
            posture.data[i] = ch[0];
            i++;
            if (i >= 24) {
                i = 0;
                count++;
            }
            break;
        case 3:
            if (ch[0] == 0x0a)
                count++;
            else
                count = 0;
            break;
        case 4:
            if (ch[0] == 0x0d) {
                OPS_Data.z_angle = posture.ActVal[0];
                OPS_Data.x_angle = posture.ActVal[1];
                OPS_Data.y_angle = posture.ActVal[2];
                OPS_Data.pos_x   = posture.ActVal[3];
                OPS_Data.pos_y   = posture.ActVal[4];
                OPS_Data.w_z     = posture.ActVal[5];
            }
            count = 0;
            break;
        default:
            count = 0;
            break;
    }
    return 0;
}
#endif
