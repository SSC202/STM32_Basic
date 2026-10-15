/**
 * @file    wtr_can 配置
 * @note    1. 如果使用STM32H7系列的FDCAN（通常为FDCAN1），需要进行以下配置流程：
 *          (1) CubeMX 配置：
 *              Frame Format            ----        Classis Mode
 *              Mode                    ----        Normal  Mode
 *              Rx FiFO0 Elmts Nbr      ----        32
 *              Tx Fifo Queue Elmts Nbr ----        32
 *              Std Filters Nbr         ----        1
 *              其余为默认配置
 *              
 *              对于时钟，进行以下配置：
 *              主频540Mhz，预分频27，Normal Time Seg1/Seg2 为2，此时波特率为 1000000 bit/s
 *          (2) 代码配置：
 *              使用以下代码进行初始化
 *                  fdcan1.FDCAN_Rx_Filter_Init();
                    fdcan1.FDCAN_Start();
                    fdcan1.FDCAN_Interrupt_Init();
                
                注意：此库指定了FDCAN1的配置，如果使用其他型号的FDCAN,请自行修改。
            2. 如果使用STM32F4以下系列的CAN（通常为CAN1），需要进行以下配置流程：
            (1) CubeMX 配置：
                主频180Mhz，预分频9，Normal Time Seg1为3，Normal Time Seg2为1，此时波特率为 1000000 bit/s
 * 
 */
#ifndef __WTR_CAN_H
#define __WTR_CAN_H

#if (STM32F427xx) || (STM32F407xx) // 如果使用STM32F4系列

#endif
#if (STM32H723xx) || (STM32H743xx) // 如果使用STM32H7系列

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
typedef struct FDCAN_HANDLER {
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
#endif