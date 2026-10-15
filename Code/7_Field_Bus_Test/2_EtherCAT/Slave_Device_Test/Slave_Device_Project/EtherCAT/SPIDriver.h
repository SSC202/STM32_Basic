/*******************************************************************************
 PIC32 SPI Interface Driver

  Company:
    Microchip Technology Inc.

  File Name:
    SPIDriver.h

  Summary:
    Contains the Header File of PIC32 SPI Interface Driver

  Description:
    This file contains the Header File of PIC32 SPI Interface Driver
	
  Change History:
    Version		Changes
	0.1			Initial version.
	0.2			-
	0.3			-	
	0.4 		-
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
 Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

 Microchip licenses to you the right to use, modify, copy and distribute
 Software only when embedded on a Microchip microcontroller or digital signal
 controller that is integrated into your product or third party product
 (pursuant to the sublicense terms in the accompanying license agreement).

 You should refer to the license agreement accompanying this Software for
 additional information regarding your rights and obligations.

 SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
 IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
 CONTRACT, NEGLiPMPCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
 OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
 CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
 SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "ecat_def.h"
#include "gpio.h"
#include "spi.h"
#include "main.h"
#include "GenericTypeDefs.h"

#ifndef SPIDRIVER_H
#define	SPIDRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif
	// *****************************************************************************
	// *****************************************************************************
	// Section: File Scope or Global Data Types
	// *****************************************************************************
	// *****************************************************************************
	#define CMD_SERIAL_READ 0x03
	#define CMD_FAST_READ 0x0B
	#define CMD_DUAL_OP_READ 0x3B
	#define CMD_DUAL_IO_READ 0xBB
	#define CMD_QUAD_OP_READ 0x6B
	#define CMD_QUAD_IO_READ 0xEB
	#define CMD_SERIAL_WRITE 0x02
	#define CMD_DUAL_DATA_WRITE 0x32
	#define CMD_DUAL_ADDR_DATA_WRITE 0xB2
	#define CMD_QUAD_DATA_WRITE 0x62
	#define CMD_QUAD_ADDR_DARA_WRITE 0xE2

	#define CMD_SERIAL_READ_DUMMY 0
	#define CMD_FAST_READ_DUMMY 1
	#define CMD_DUAL_OP_READ_DUMMY 1
	#define CMD_DUAL_IO_READ_DUMMY 2
	#define CMD_QUAD_OP_READ_DUMMY 1
	#define CMD_QUAD_IO_READ_DUMMY 4
	#define CMD_SERIAL_WRITE_DUMMY 0
	#define CMD_DUAL_DATA_WRITE_DUMMY 0
	#define CMD_DUAL_ADDR_DATA_WRITE_DUMMY 0
	#define CMD_QUAD_DATA_WRITE_DUMMY 0
	#define CMD_QUAD_ADDR_DARA_WRITE_DUMMY 0

	#define ESC_CSR_CMD_REG		0x304
	#define ESC_CSR_DATA_REG	0x300
	#define ESC_WRITE_BYTE 		0x80
	#define ESC_READ_BYTE 		0xC0
	#define ESC_CSR_BUSY		0x80

        #define SYS_FREQ_MHZ     80

	#define SPI_CLK_DIV(MHz) ((SYS_FREQ_MHZ/(2*MHz))-1)

	#define SPI_MODE0 0
	#define SPI_MODE1 1
	#define SPI_MODE2 2
	#define SPI_MODE3 3

	#define SPIGet()     SPI4BUF
	#define CSLOW()      HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,GPIO_PIN_RESET)
	#define CSHIGH()     HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,GPIO_PIN_SET)


	#define SPIWriteByte(UINT8) SPIWrite(UINT8)
	#define SPIReadByte() SPIRead()


	// *****************************************************************************
	// *****************************************************************************
	// Section: File Scope Functions
	// *****************************************************************************
	// *****************************************************************************

	void SPIWritePDRamRegister(UINT8 *WriteBuffer, UINT16 Address, UINT16 Count);
	void SPIReadPDRamRegister(UINT8 *ReadBuffer, UINT16 Address, UINT16 Count);
	void SPIWriteRegister( UINT8 *WriteBuffer, UINT16 Address, UINT16 Count);
	void SPIReadDRegister(UINT8 *ReadBuffer, UINT16 Address, UINT16 Count);
    void SPIReadRegUsingCSR(UINT8 *ReadBuffer, UINT16 Address, UINT8 Count);
    void SPIWriteRegUsingCSR( UINT8 *WriteBuffer, UINT16 Address, UINT8 Count);
	void SPIWriteDWord (UINT16 Address, UINT32 Val);
	UINT32 SPIReadDWord (UINT16 Address);
	UINT8 SPIRead(void);
	void SPIWrite(UINT8 data);
	void SPIPut(UINT8 data);
    void SPIWriteBurstMode (UINT32 Val);
    UINT32 SPIReadBurstMode (void);
    void SPISendAddr (UINT16 Address);
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* PMPDRIVER_H */

