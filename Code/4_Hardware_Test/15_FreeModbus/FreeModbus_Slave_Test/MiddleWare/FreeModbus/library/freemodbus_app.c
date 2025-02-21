/*
 * FreeModbus Libary: BARE Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "freemodbus_app.h"

/* ----------------------- Static variables ---------------------------------*/
static USHORT usRegHoldingStart                  = REG_HOLDING_START;
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS] = {0x0, 0x2, 0x3, 0x5};

/* ----------------------- Start implementation -----------------------------*/

#if (MB_FUNC_READ_INPUT_ENABLED == 1)
eMBErrorCode
eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;

    if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS)) {
        iRegIndex = (int)(usAddress - usRegInputStart);
        while (usNRegs > 0) {
            *pucRegBuffer++ =
                (unsigned char)(usRegInputBuf[iRegIndex] >> 8);
            *pucRegBuffer++ =
                (unsigned char)(usRegInputBuf[iRegIndex] & 0xFF);
            iRegIndex++;
            usNRegs--;
        }
    } else {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}
#endif

#if (MB_FUNC_READ_HOLDING_ENABLED == 1 || MB_FUNC_WRITE_HOLDING_ENABLED == 1)
/**
 * @brief   操作保持寄存器
 * @param   usAddress 寄存器地址
 * @param   usNRegs   寄存器个数
 * @param   eMode     寄存器操作方式,读或者写
 */
eMBErrorCode
eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;
    usAddress -= 1;
    if ((usAddress >= REG_HOLDING_START) && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS)) {
        iRegIndex = (int)(usAddress - usRegHoldingStart);
        if (eMode == MB_REG_READ) {
            while (usNRegs > 0) {
                *pucRegBuffer++ =
                    (unsigned char)(usRegHoldingBuf[iRegIndex] >> 8);
                *pucRegBuffer++ =
                    (unsigned char)(usRegHoldingBuf[iRegIndex] & 0xFF);
                iRegIndex++;
                usNRegs--;
            }
        } else {
            while (usNRegs > 0) {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++;
                usRegHoldingBuf[iRegIndex] <<= 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
#endif

#if (MB_FUNC_READ_COILS_ENABLED == 1 || MB_FUNC_WRITE_COIL_ENABLED == 1)
eMBErrorCode
eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils,
              eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iNCoils          = (int)usNCoils;
    unsigned short usBitOffset;

    if ((usAddress >= REG_COILS_START) && (usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE)) {
        usBitOffset = (unsigned short)(usAddress - REG_COILS_START);
        switch (eMode) {
            case MB_REG_READ:
                while (iNCoils > 0) {
                    *pucRegBuffer++ = xMBUtilGetBits(ucRegCoilsBuf, usBitOffset, (unsigned char)(iNCoils > 8 ? 8 : iNCoils));
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;
            case MB_REG_WRITE:
                while (iNCoils > 0) {
                    xMBUtilSetBits(ucRegCoilsBuf, usBitOffset, (unsigned char)(iNCoils > 8 ? 8 : iNCoils), *pucRegBuffer++);
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
#endif

#if (MB_FUNC_READ_DISCRETE_INPUTS_ENABLED == 1)
eMBErrorCode
eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
    eMBErrorCode eStatus = MB_ENOERR;
    short iNDiscrete     = (short)usNDiscrete;
    USHORT usBitOffset;

    if ((usAddress >= REG_DISCRETE_START) && (usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE)) {
        usBitOffset = (USHORT)(usAddress - REG_DISCRETE_START);
        while (iNDiscrete > 0) {
            *pucRegBuffer++ =
                xMBUtilGetBits(usRegDiscreteBuf, usBitOffset, (UCHAR)(iNDiscrete > 8 ? 8 : iNDiscrete));
            iNDiscrete -= 8;
            usBitOffset += 8;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
#endif