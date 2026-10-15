/**
  ******************************************************************************
  * @file    eth.c
  * @brief   This file provides code for the configuration
  *          of the ETH instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "eth.h"

/* USER CODE BEGIN 0 */
#include "LAN8720.h"
#include "stdio.h"
#include "string.h"

__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;	/* Ethernet Rx MA Descriptor */
__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;	/* Ethernet Tx DMA Descriptor */
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; 	/* Ethernet Receive Buffer */
__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; 	/* Ethernet Transmit Buffer */
/* USER CODE END 0 */

ETH_HandleTypeDef heth;

/* ETH init function */
void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */
	printf("ETH Init.\r\n");
  /* USER CODE END ETH_Init 0 */

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  heth.Init.Speed = ETH_SPEED_100M;
  heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
  heth.Init.PhyAddress = PHY_USER_NAME_PHY_ADDRESS;
  heth.Init.MACAddr[0] =   0x02;
  heth.Init.MACAddr[1] =   0x00;
  heth.Init.MACAddr[2] =   0x00;
  heth.Init.MACAddr[3] =   0x00;
  heth.Init.MACAddr[4] =   0x00;
  heth.Init.MACAddr[5] =   0x00;
  heth.Init.RxMode = ETH_RXPOLLING_MODE;
  heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;

  /* USER CODE BEGIN MACADDRESS */
	// LAN8720 Address
	heth.Init.PhyAddress = 0x00;
  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    // Error_Handler();
  }
  /* USER CODE BEGIN ETH_Init 2 */
  // Initialize Tx Descriptors list: Chain Mode
  HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
     
  // Initialize Rx Descriptors list: Chain Mode
  HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);	
	
	// Enable MAC and DMA transmission and reception
	HAL_ETH_Start(&heth);
	
	// Configure PHY to generate an interrupt when Eth Link state changes 
	uint32_t regvalue = 0;
	
  // Read Register Configuration 
  HAL_ETH_ReadPHYRegister(&heth, PHY_MICR, &regvalue);
  
  regvalue |= (PHY_MICR_INT_EN | PHY_MICR_INT_OE);

  // Enable Interrupts 
  HAL_ETH_WritePHYRegister(&heth, PHY_MICR, regvalue );
  
  // Read Register Configuration
  HAL_ETH_ReadPHYRegister(&heth, PHY_MISR, &regvalue);
  
  regvalue |= PHY_MISR_LINK_INT_EN;
    
  // Enable Interrupt on change of link status */
  HAL_ETH_WritePHYRegister(&heth, PHY_MISR, regvalue);
	printf("ETH Init done.\r\n");

	// PHY Init
	printf("LAN8720 Init.\r\n");
	LAN8720_Init();
	printf("LAN8720 Init done.\r\n");
	
	// Wait for link up
	int32_t PHYLinkState;
	printf("Wait for LAN8720 link.\r\n");
	do
	{
		HAL_Delay(100);
		PHYLinkState = LAN8720_GetLinkState();
		printf("get LAN8720 state: %d.\r\n", PHYLinkState);
	}while(PHYLinkState <= LAN8720_STATUS_LINK_DOWN);	
	printf("Wait for LAN8720 link done.\r\n");
	
	// Set Speed and Mode
	uint32_t duplex, speed = 0;
	printf("Set LAN8720 Speed and Mode.\r\n");
	switch(PHYLinkState)
	{
		case LAN8720_STATUS_100MBITS_FULLDUPLEX:
			duplex = ETH_MODE_FULLDUPLEX;
			speed = ETH_SPEED_100M;
		break;
		case LAN8720_STATUS_100MBITS_HALFDUPLEX:
			duplex = ETH_MODE_HALFDUPLEX;
			speed = ETH_SPEED_100M;
			break;
		case LAN8720_STATUS_10MBITS_FULLDUPLEX:
			duplex = ETH_MODE_FULLDUPLEX;
			speed = ETH_SPEED_10M;
			break;
		case LAN8720_STATUS_10MBITS_HALFDUPLEX:
			duplex = ETH_MODE_HALFDUPLEX;
			speed = ETH_SPEED_10M;
			break;
		default:
			duplex = ETH_MODE_FULLDUPLEX;
			speed = ETH_SPEED_100M;
			break;      
	}
	heth.Init.DuplexMode = duplex;
	heth.Init.Speed = speed;
	printf("Set LAN8720 Speed and Mode done.\r\n");
	
	// ETHERNET MAC Re-Configuration
	HAL_ETH_ConfigMAC(&heth, (ETH_MACInitTypeDef *)NULL);
  /* USER CODE END ETH_Init 2 */

}

void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspInit 0 */

  /* USER CODE END ETH_MspInit 0 */
    /* ETH clock enable */
    __HAL_RCC_ETH_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**ETH GPIO Configuration
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PA2     ------> ETH_MDIO
    PA7     ------> ETH_CRS_DV
    PC4     ------> ETH_RXD0
    PC5     ------> ETH_RXD1
    PG11     ------> ETH_TX_EN
    PG13     ------> ETH_TXD0
    PG14     ------> ETH_TXD1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN ETH_MspInit 1 */

  /* USER CODE END ETH_MspInit 1 */
  }
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{

  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspDeInit 0 */

  /* USER CODE END ETH_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ETH_CLK_DISABLE();

    /**ETH GPIO Configuration
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PA2     ------> ETH_MDIO
    PA7     ------> ETH_CRS_DV
    PC4     ------> ETH_RXD0
    PC5     ------> ETH_RXD1
    PG11     ------> ETH_TX_EN
    PG13     ------> ETH_TXD0
    PG14     ------> ETH_TXD1
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14);

  /* USER CODE BEGIN ETH_MspDeInit 1 */

  /* USER CODE END ETH_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
 * @brief  Send Ethernet packet
 * @param  packet: 	Pointer to data buffer to transmit
 * @param  length: 	Length of data in bytes
 * @retval 0: 			Success
 * @retval -1: 			Error
 */
int32_t bfin_EMAC_send(void *packet, int length)
{
    HAL_StatusTypeDef state;
    __IO ETH_DMADescTypeDef *DmaTxDesc;
    
    if(packet == NULL || length <= 0 || length > ETH_TX_BUF_SIZE) {
        return -1;
    }

    DmaTxDesc = heth.TxDesc;

    // Check descriptor available
    if((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
        return -1;
    }

    // Copy to DMA buffer and transmit
    memcpy((uint8_t *)(DmaTxDesc->Buffer1Addr), packet, length);
    while(heth.Lock == HAL_LOCKED) 
		{
		
		}
    state = HAL_ETH_TransmitFrame(&heth, length);
    
    // Handle underflow if failed
    if(state != HAL_OK) {
        if((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET) {
            heth.Instance->DMASR = ETH_DMASR_TUS;
            heth.Instance->DMATPDR = 0;
        }
        return -1;
    }
    return 0;
}

uint32_t current_pbuf_idx = 0;

/**
 * @brief  Receive Ethernet packet
 * @param  packet: 	Pointer to buffer for storing received data
 * @param  size:   	Size of the provided buffer in bytes
 * @retval 0:    		Number of bytes received (frame length)
 * @retval -1:    	No frame received or error
 */
int32_t bfin_EMAC_recv(uint8_t *packet, size_t size)
{
    uint32_t framelength = 0;
    __IO ETH_DMADescTypeDef *dmarxdesc;
    uint32_t i = 0;
    HAL_StatusTypeDef status;

    // Check if frame received
    status = HAL_ETH_GetReceivedFrame(&heth);
    if(status != HAL_OK) {
        return -1;
    }

    // Get received frame length
    framelength = heth.RxFrameInfos.length;

    // Copy frame to user buffer
    memcpy(packet, Rx_Buff[current_pbuf_idx], framelength);

    // Advance to next Rx buffer index
    if(current_pbuf_idx < (ETH_RX_DESC_CNT - 1)) {
        current_pbuf_idx++;
    } else {
        current_pbuf_idx = 0;
    }

    // Release descriptors back to DMA
    dmarxdesc = heth.RxFrameInfos.FSRxDesc;
    for(i = 0; i < heth.RxFrameInfos.SegCount; i++) {  
        dmarxdesc->Status |= ETH_DMARXDESC_OWN;
        dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }

    // Clear segment count
    heth.RxFrameInfos.SegCount = 0;

    // Handle Rx Buffer Unavailable condition
    if((heth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET) {
        // Clear RBUS flag
        heth.Instance->DMASR = ETH_DMASR_RBUS;
        // Resume DMA reception
        heth.Instance->DMARPDR = 0;
    }
		
    return framelength;
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
