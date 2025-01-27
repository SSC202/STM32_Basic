#ifndef __CAN_H
#define __CAN_H

#include "stm32f4xx.h"
#include "can.h"

extern CAN_TxHeaderTypeDef can_tx_buffer;
extern uint8_t tx_data[5];
extern uint32_t tx_fifo;
extern CAN_FilterTypeDef can_filter;

void _CAN_TxAddMsg(CAN_TxHeaderTypeDef *_can_tx_buffer,uint32_t _StdId,uint32_t _ExtId,uint32_t Size,uint32_t _IDE,uint32_t _RTR,FunctionalState Time_tag);
void _CAN_FilterConfig(CAN_FilterTypeDef *_can_filter,uint32_t _filteridhigh,uint32_t _filteridlow,uint32_t _filtermaskidhigh,uint32_t _filtermaskidlow,uint32_t fifo,uint32_t bank,uint32_t mode,uint32_t scale,uint32_t enable);
#endif
