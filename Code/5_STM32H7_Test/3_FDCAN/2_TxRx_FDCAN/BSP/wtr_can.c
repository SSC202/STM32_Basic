#include "wtr_can.h"
#include "fdcan.h"
#include "DJI.h"
#include "string.h"

extern FDCAN_HandleTypeDef hfdcan1;
static FDCAN_FilterTypeDef hfdcan1_rx_filter0; // FDCAN1 滤波器0 句柄
FDCAN_RxHeaderTypeDef hfdcan1_rx;              // FDCAN1 接受处理单元句柄
FDCAN_TxHeaderTypeDef hfdcan1_tx;              // FDCAN1 发送处理单元句柄
uint8_t fdcan1_rxdata[8] = {0};

FDCAN_HANDLER fdcan1 = {
    .rx_MSG                     = {0},
    .FDCAN_Start                = FDCAN1_Start,                // FDCAN1 开始方法
    .FDCAN_Rx_Filter_Init       = FDCAN1_RX_Filter_Init,       // FDCAN1 过滤器初始化方法
    .FDCAN_Interrupt_Init       = FDCAN1_Interrupt_Init,       // FDCAN1 中断初始化方法
    .FDCAN_Send_MSG             = FDCAN1_Send_Msg,             // FDCAN1 发送消息方法
    .FDCAN_Update_RXFIFO_Status = FDCAN1_Update_RXFIFO_Status, // FDCAN1 更新接收FIFO状态方法
};

/*************************************用户函数**************************************/

/**
 * @brief  设置并初始化FDCAN1滤波器
 * @note   1. FDCAN_HandleTypeDef 对象句柄的成员 RxFifo0 Elmts Nbr设置大于0时，表示启用 RXFIFO0;
 * @note   2. 如果启用 RXFIFO0 ,那么滤波器必须关联到 RXFIFO0, 即 FilterConfig 必须赋值 FDCAN_FILTER_TO_RXFIFO0;
 */
void FDCAN1_RX_Filter_Init(void)
{
    hfdcan1_rx_filter0.IdType       = FDCAN_STANDARD_ID;       // 只接收标准帧ID
    hfdcan1_rx_filter0.FilterIndex  = 0;                       // 滤波器索引0
    hfdcan1_rx_filter0.FilterType   = FDCAN_FILTER_MASK;       // 滤波器类型
    hfdcan1_rx_filter0.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // 滤波器关联到RXFIFO0
    hfdcan1_rx_filter0.FilterID1    = 0x00;                    // 滤波ID1 0x00
    hfdcan1_rx_filter0.FilterID2    = 0x00;                    // 滤波ID2 0x00

    // 设置失败进入死循环
    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &hfdcan1_rx_filter0) != HAL_OK) {
        while (1) {
            ;
        }
    }

    /**
     * @note    设置滤波器全局配置
     *          设置标准帧ID，接收的报文ID没有匹配上滤波器时，选择拒绝接收(没有匹配上时,可以选择放入FIFO0或者FIFO1)。
     *          设置拓展帧ID，接收的报文ID没有匹配上滤波器时，选择拒绝接收。
     *          设置是否拒绝远程标准帧，ENABLE代表拒绝接收。
     *          设置是否拒绝远程拓展帧，ENABLE代表拒绝接收。
     */
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, DISABLE, DISABLE) != HAL_OK) /* 设置FDCAN1滤波器0全局配置  */
    {
        while (1) {
            ;
        }
    }
}

/**
 * @brief  设置FDCAN1的中断
 * @note   1. 设置收到新的数据就产生中断。
 * @note   2. 配置水印中断防止FIFO溢出。
 * @note   3. 配置FIFO溢出中断，防止FIFO溢出导致丢失报文。
 */
void FDCAN1_Interrupt_Init(void)
{
    // 使能接受新数据中断
    if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        while (1) {
            ;
        }
    }
    // 设置FIFO0溢出中断
    if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_FULL, 0) != HAL_OK) {
        while (1) {
            ;
        }
    }
    // 设置水印中断
    if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_WATERMARK, 0) != HAL_OK) {
        while (1) {
            ;
        }
    }
}

/**
 * @brief   更新FDCAN接收FIFO的状态寄存器
 * @param   hfdcan  FDCAN句柄
 * @param   fdcan   CAN对象结构体
 * @note    FDCAN控制器必须配置RX FIFO后才能使用
 */
void FDCAN1_Update_RXFIFO_Status(FDCAN_HandleTypeDef *hfdcan, FDCAN_HANDLER *fdcan)
{
    if (fdcan == &fdcan1) {
        fdcan->RXFxS = hfdcan->Instance->RXF0S;
    }
    fdcan->FxGI = (fdcan->RXFxS >> 8) & 0x1F;
    fdcan->FxPI = (fdcan->RXFxS >> 16) & 0x1F;
    fdcan->FxFL = fdcan->RXFxS & 0x3F;
}

/**
 * @brief       FDCAN1发送数据
 * @param       msg  发送数据结构体
 * @note        只允许报文的长度是0 ~ 8,不支持大于8的报文
 *
 */
uint8_t FDCAN1_Send_Msg(CAN_MSG *msg)
{
    // 选择数据的长度
    switch (msg->len) {
        case 0:
            hfdcan1_tx.DataLength = FDCAN_DLC_BYTES_0; /* 数据长度:0 */
            break;
        case 1:
            hfdcan1_tx.DataLength = FDCAN_DLC_BYTES_1; /* 数据长度:1 */
            break;
        case 2:
            hfdcan1_tx.DataLength = FDCAN_DLC_BYTES_2; /* 数据长度:2 */
            break;
        case 3:
            hfdcan1_tx.DataLength = FDCAN_DLC_BYTES_3; /* 数据长度:3 */
            break;
        case 4:
            hfdcan1_tx.DataLength = FDCAN_DLC_BYTES_4; /* 数据长度:4 */
            break;
        case 5:
            hfdcan1_tx.DataLength = FDCAN_DLC_BYTES_5; /* 数据长度:5 */
            break;
        case 6:
            hfdcan1_tx.DataLength = FDCAN_DLC_BYTES_6; /* 数据长度:6 */
            break;
        case 7:
            hfdcan1_tx.DataLength = FDCAN_DLC_BYTES_7; /* 数据长度:7 */
            break;
        case 8:
            hfdcan1_tx.DataLength = FDCAN_DLC_BYTES_8; /* 数据长度:8 */
            break;
        default:
            return 0;
    }

    hfdcan1_tx.Identifier          = msg->id;            // 32位ID
    hfdcan1_tx.IdType              = FDCAN_STANDARD_ID;  // 标准ID
    hfdcan1_tx.TxFrameType         = FDCAN_DATA_FRAME;   // 数据帧
    hfdcan1_tx.ErrorStateIndicator = FDCAN_ESI_ACTIVE;   // ESI位
    hfdcan1_tx.BitRateSwitch       = FDCAN_BRS_OFF;      // 关闭速率转换
    hfdcan1_tx.FDFormat            = FDCAN_CLASSIC_CAN;  // 标准CAN模式
    hfdcan1_tx.TxEventFifoControl  = FDCAN_NO_TX_EVENTS; // 无发送事件
    hfdcan1_tx.MessageMarker       = 0;

    if (REMOTE_FRAME == msg->rtr)
        hfdcan1_tx.TxFrameType = FDCAN_REMOTE_FRAME; // 远程帧
    else
        hfdcan1_tx.TxFrameType = FDCAN_DATA_FRAME; // 数据帧

    // 将需要发送的数据压入到TX FIFO
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &hfdcan1_tx, (uint8_t *)msg->buffer) == HAL_OK) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief  FDCAN1启动
 */
void FDCAN1_Start(void)
{
    HAL_FDCAN_Start(&hfdcan1);
}

/**
 * @brief FIFO0接收中断回调函数
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    fdcan1.RXFIFO_Inter_Type = RxFifo0ITs; /* 看看中断的类型 */

    // FIFO 中断类型
    if (FDCAN_IT_RX_FIFO0_WATERMARK == RxFifo0ITs ||
        FDCAN_IT_RX_FIFO0_NEW_MESSAGE == RxFifo0ITs ||
        FDCAN_IT_RX_FIFO0_FULL == RxFifo0ITs) {
        do {
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &hfdcan1_rx, fdcan1_rxdata); // 提取FIFO0中接收到的数据
            memcpy((uint8_t *)fdcan1.rx_MSG.buffer, fdcan1_rxdata, 8);                  // 保存CAN报文的内容
            fdcan1.rx_MSG.id  = hfdcan1_rx.Identifier;                                  // 保存CAN报文的ID
            fdcan1.rx_MSG.len = hfdcan1_rx.DataLength >> 16;                            // 保存CAN报文的内容长度

            // 判断远程帧或者数据帧
            if (hfdcan1_rx.RxFrameType == FDCAN_REMOTE_FRAME) {
                fdcan1.rx_MSG.rtr = REMOTE_FRAME;
            } else {
                fdcan1.rx_MSG.rtr = DATA_FRAME;
            }
            fdcan1.FDCAN_Update_RXFIFO_Status(hfdcan, &fdcan1); // 更新RXFIFO的状态（了解FIFO是否有剩余报文）
            DJI_CanMsgDecode(fdcan1.rx_MSG.id, (uint8_t *)fdcan1.rx_MSG.buffer);
        } while (fdcan1.FxFL > 0);
        // 循环直到FxFL等于0
    }
}
