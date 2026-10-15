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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR(void);

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)
{
    // 配置 50us 的定时器
    TIM_PORT.Instance->ARR = usTim1Timerout50us - 1;
    return TRUE;
}

inline void
vMBPortTimersEnable()
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
    __HAL_TIM_SET_COUNTER(&TIM_PORT, 0);
    HAL_TIM_Base_Start_IT(&TIM_PORT);
}

inline void
vMBPortTimersDisable()
{
    /* Disable any pending timers. */
    HAL_TIM_Base_Stop_IT(&TIM_PORT);
    __HAL_TIM_SET_COUNTER(&TIM_PORT, 0);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR(void)
{
    (void)pxMBPortCBTimerExpired();
}

void TIM2_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&TIM_PORT, TIM_FLAG_UPDATE) != RESET) {
        __HAL_UART_CLEAR_FLAG(&TIM_PORT, TIM_FLAG_UPDATE);
        prvvTIMERExpiredISR();
    }
}
