#include "_can.h"

CAN_TxHeaderTypeDef can_tx_buffer;                   // 发送结构体
uint8_t tx_data[5] = {0xAA, 0x55, 0x35, 0xBB, 0xCC}; // 发送数据
uint32_t tx_fifo = 0;
CAN_FilterTypeDef can_filter;

/**
 * @brief   发送报文配置函数
 * @param   _can_tx_buffer 发送报文结构体
 * @param   _StdId         发送报文标准ID
 * @param   _ExtId         发送报文扩展ID
 * @param   Size           DLC段字节数
 * @param   _IDE           标志为标准数据帧还是扩展数据帧
 * @param   _RTR           标志为遥控帧还是数据帧
 * @param   Time_tag       时间触发模式标志
 */
void _CAN_TxAddMsg(CAN_TxHeaderTypeDef *_can_tx_buffer, uint32_t _StdId, uint32_t _ExtId, uint32_t Size, uint32_t _IDE, uint32_t _RTR, FunctionalState Time_tag)
{
    _can_tx_buffer->ExtId = _ExtId;
    _can_tx_buffer->StdId = _StdId;
    _can_tx_buffer->DLC = Size;
    _can_tx_buffer->IDE = _IDE;
    _can_tx_buffer->RTR = _RTR;
    _can_tx_buffer->TransmitGlobalTime = Time_tag;
}

/**
 * @brief 过滤器配置函数
 * @param _can_filter 过滤器结构体
 * @param _filteridhigh CAN_FiR1寄存器的高16位，用于填写筛选码。
 * @param _filterlow CAN_FiR1寄存器的低16位
 * @param _filtermaskidhigh CAN_FiR2寄存器的高16位
 * @param _filtermaskidlow  CAN_FiR2寄存器的低16位
 * @param fifo 接受邮箱 CAN_FILTER_FIFO0 或 CAN_FILTER_FIFO1
 * @param bank 筛选器号 对于单CAN为 0 ~ 13；对于双CAN为 0 ~ 27
 * @param mode 筛选模式，掩码模式或列表模式。CAN_FILTERMODE_IDMASK 或 CAN_FILTERMODE_IDMASK
 * @param scale 筛选码大小 CAN_FILTERSCALE_16BIT 或 CAN_FILTERSCALE_32BIT
 * @param enable 使能或失能此筛选器。CAN_FILTER_DISABLE 或 CAN_FILTER_ENABLE
 */
void _CAN_FilterConfig(CAN_FilterTypeDef *_can_filter,uint32_t _filteridhigh,uint32_t _filteridlow,uint32_t _filtermaskidhigh,uint32_t _filtermaskidlow,uint32_t fifo,uint32_t bank,uint32_t mode,uint32_t scale,uint32_t enable)
{
    _can_filter->FilterIdHigh = _filteridhigh;
    _can_filter->FilterIdLow = _filteridlow;
    _can_filter->FilterMaskIdHigh = _filtermaskidhigh;
    _can_filter->FilterMaskIdLow = _filtermaskidhigh;
    _can_filter->FilterFIFOAssignment = fifo;
    _can_filter->FilterBank = bank;
    _can_filter->FilterMode = mode;
    _can_filter->FilterScale = scale;
    _can_filter->FilterActivation = enable;
}
