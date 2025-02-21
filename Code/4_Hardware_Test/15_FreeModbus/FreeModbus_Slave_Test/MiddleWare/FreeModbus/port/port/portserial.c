/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "usart.h"
#include "rs485.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);

/* ----------------------- Start implementation -----------------------------*/
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
    // 启用接收中断
    if (xRxEnable) {
        __HAL_UART_ENABLE_IT(&UART_PORT, UART_IT_RXNE);
        RS485_Set(0);
    } else {
        __HAL_UART_DISABLE_IT(&UART_PORT, UART_IT_RXNE);
        RS485_Set(1);
    }
    // 启用发送完成中断
    if (xTxEnable) {
        RS485_Set(1);
        __HAL_UART_ENABLE_IT(&UART_PORT, UART_IT_TC);
    } else {
        RS485_Set(0);
        __HAL_UART_DISABLE_IT(&UART_PORT, UART_IT_TC);
    }
}

BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
    return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    // 发送一个字节
    RS485_Set(1);
    UART_PORT.Instance->DR = ucByte;
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR *pucByte)
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    // 接收一个字节
    RS485_Set(0);
    *pucByte = UART_PORT.Instance->DR;
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR(void)
{
    pxMBFrameCBTransmitterEmpty();
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR(void)
{
    pxMBFrameCBByteReceived();
}

/**
 * @brief   串口中断句柄
 */
void USART2_IRQHandler(void)
{
    // 发送中断
    if (__HAL_UART_GET_FLAG(&UART_PORT, UART_FLAG_TC) != RESET) {
        __HAL_UART_CLEAR_FLAG(&UART_PORT, UART_FLAG_TC);
        prvvUARTTxReadyISR();
    }
    // 接收中断
    if (__HAL_UART_GET_FLAG(&UART_PORT, UART_FLAG_RXNE) != RESET) {
        __HAL_UART_CLEAR_FLAG(&UART_PORT, UART_FLAG_RXNE);
        prvvUARTRxISR();
    }
}