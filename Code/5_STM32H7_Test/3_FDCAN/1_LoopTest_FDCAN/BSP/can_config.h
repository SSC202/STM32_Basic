#ifndef __CAN_CONFIG_H
#define __CAN_CONFIG_H

#include "stm32h7xx.h"

/***********************数据类型定义**********************/

// 帧类型定义
typedef enum {
    DATA_FRAME   = 0, // 数据帧
    REMOTE_FRAME = 1, // 远程帧
} CAN_FRAME_TYPE;

// 接收消息结构体定义
typedef struct {
    __IO uint16_t id;       // CANID
    CAN_FRAME_TYPE rtr;     // 远程帧，数据帧
    __IO uint8_t len;       // CAN报文长度
    __IO uint8_t buffer[8]; // CAN报文内容
} CAN_MSG;

// FDCAN 对象结构体定义
typedef struct FDCAN_HANDLER
{
    // 成员变量
    CAN_MSG rx_MSG;                  // CAN报文结构体
    __IO uint32_t RXFIFO_Inter_Type; // RXFIFO中断的类型（新数据，水印，FIFO溢出)
    __IO uint32_t RXFxS;             // FDCAN_RXF0C 或 FDCAN_RXF1C
    __IO uint8_t FxGI;               // FDCAN_F0GI 或 FDCAN_F1GI
    __IO uint8_t FxFL;               // FDCAN_F0FL 或 FDCAN_F1FL
    __IO uint8_t FxPI;               // FDCAN_F0PI 或 FDCAN_F1PI

    // 方法函数
    void (*FDCAN_Start)(void);
    void (*FDCAN_Rx_Filter_Init)(void);
    void (*FDCAN_Interrupt_Init)(void);
    uint8_t (*FDCAN_Send_MSG)(CAN_MSG *);
    void (*FDCAN_Update_RXFIFO_Status)(FDCAN_HandleTypeDef *, struct FDCAN_HANDLER *);
} FDCAN_HANDLER;

/************************变量定义***********************/

extern FDCAN_RxHeaderTypeDef hfdcan1_rx; // FDCAN1 接受处理单元句柄
extern FDCAN_TxHeaderTypeDef hfdcan1_tx; // FDCAN1 发送处理单元句柄
extern FDCAN_HANDLER fdcan1;
extern uint8_t fdcan1_rxdata[8];

/************************函数定义***********************/

void FDCAN1_RX_Filter_Init(void);
void FDCAN1_Interrupt_Init(void);
void FDCAN1_Update_RXFIFO_Status(FDCAN_HandleTypeDef *hfdcan, FDCAN_HANDLER *fdcan);
uint8_t FDCAN1_Send_Msg(CAN_MSG *msg);
void FDCAN1_Start(void);

#endif