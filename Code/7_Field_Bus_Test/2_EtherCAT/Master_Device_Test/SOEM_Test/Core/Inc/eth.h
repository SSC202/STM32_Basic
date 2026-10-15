/**
  ******************************************************************************
  * @file    eth.h
  * @brief   This file contains all the function prototypes for
  *          the eth.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ETH_H__
#define __ETH_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ETH_HandleTypeDef heth;

/* USER CODE BEGIN Private defines */
#ifndef PHY_SR
#define PHY_SR                          ((uint16_t)0x10)    /*!< PHY status register Offset                      */
#endif
#ifndef PHY_MICR
#define PHY_MICR                        ((uint16_t)0x11)    /*!< MII Interrupt Control Register                  */
#endif
#ifndef PHY_MISR 
#define PHY_MISR                        ((uint16_t)0x12)    /*!< MII Interrupt Status and Misc. Control Register */
#endif

#ifndef PHY_LINK_STATUS 
#define PHY_LINK_STATUS                 ((uint16_t)0x0001)  /*!< PHY Link mask                                   */
#endif
#ifndef PHY_SPEED_STATUS 
#define PHY_SPEED_STATUS                ((uint16_t)0x0002)  /*!< PHY Speed mask                                  */
#endif
#ifndef PHY_DUPLEX_STATUS 
#define PHY_DUPLEX_STATUS               ((uint16_t)0x0004)  /*!< PHY Duplex mask                                 */
#endif

#ifndef PHY_MICR_INT_EN 
#define PHY_MICR_INT_EN                 ((uint16_t)0x0002)  /*!< PHY Enable interrupts                           */
#endif
#ifndef PHY_MICR_INT_OE
#define PHY_MICR_INT_OE                 ((uint16_t)0x0001)  /*!< PHY Enable output interrupt events              */
#endif

#ifndef PHY_MISR_LINK_INT_EN 
#define PHY_MISR_LINK_INT_EN            ((uint16_t)0x0020)  /*!< Enable Interrupt on change of link status       */
#endif
#ifndef PHY_LINK_INTERRUPT
#define PHY_LINK_INTERRUPT              ((uint16_t)0x2000)
#endif

#ifndef ETH_TX_DESC_CNT
#define ETH_TX_DESC_CNT         4 
#endif
	 
#ifndef ETH_RX_DESC_CNT
#define ETH_RX_DESC_CNT         4 
#endif
/* USER CODE END Private defines */

void MX_ETH_Init(void);

/* USER CODE BEGIN Prototypes */
int bfin_EMAC_send(void *packet, int length);
int bfin_EMAC_recv(uint8_t *packet, size_t size);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ETH_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
