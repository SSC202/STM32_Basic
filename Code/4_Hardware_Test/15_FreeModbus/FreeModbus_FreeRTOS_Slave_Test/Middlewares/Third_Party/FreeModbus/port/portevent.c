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

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* ----------------------- Variables ----------------------------------------*/
static osMessageQId xQueueHdl; // 队列句柄

/* ----------------------- Start implementation -----------------------------*/
BOOL bMBPortIsWithinException(void) {
    return (BOOL)xPortIsInsideInterrupt();
}

BOOL xMBPortEventInit(void)
{
    BOOL bStatus = FALSE;
    xQueueHdl    = osMessageQueueNew(1, sizeof(eMBEventType), NULL);
    if (xQueueHdl != NULL) {
        bStatus = TRUE;
    }
    return bStatus;
}

void vMBPortEventClose(void)
{
    if (xQueueHdl != NULL) {
        osMessageQueueDelete(xQueueHdl);
        xQueueHdl = NULL;
    }
}

BOOL xMBPortEventPost(eMBEventType eEvent)
{
    BOOL bStatus = TRUE;
    if (bMBPortIsWithinException()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (xQueueSendFromISR((QueueHandle_t)xQueueHdl, (const void *)&eEvent, &xHigherPriorityTaskWoken) == pdPASS) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            bStatus = FALSE;
        }
    } else {
        osStatus_t status = osMessageQueuePut(xQueueHdl, &eEvent, 0, 0);
        if (status != osOK) {
            bStatus = FALSE;
        }
    }
    return bStatus;
}

BOOL xMBPortEventGet(eMBEventType *peEvent)
{
    BOOL xEventHappened = FALSE;
    uint32_t timeout = osKernelGetTickCount() + 50;
    osStatus_t status   = osMessageQueueGet(xQueueHdl, peEvent, NULL, timeout);
    if (status == osOK) {
        xEventHappened = TRUE;
    }

    return xEventHappened;
}
