#include "LAN8720.h"
#include "eth.h"
#include "stdio.h"

uint32_t lan8720addr;

int LAN8720_Init(void)
{
		uint32_t regvalue = 0, addr = 0;
		HAL_StatusTypeDef rtn;
	
		HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_SET);
		HAL_Delay(100);
	
		lan8720addr = 32;
		for(addr = 0; addr <= lan8720addr; addr++)
		{
				rtn = HAL_ETH_ReadPHYRegister(&heth, LAN8720_SMR, &regvalue);
				if(rtn != HAL_OK)
				{
						printf("Read Address %d error.\r\n", addr);
						continue;
				}
				if((regvalue & LAN8720_SMR_PHY_ADDR) == addr)
				{
						lan8720addr = addr;
						break;
				}
		}
		printf("LAN8720 addr = %d.\r\n",addr);
		if(lan8720addr > 31)
				return -1;

		rtn = HAL_ETH_WritePHYRegister(&heth,LAN8720_BCR,LAN8720_BCR_SOFT_RESET);
		if(rtn != HAL_OK)
		{
				return -1;
		}
		do
		{
				rtn = HAL_ETH_ReadPHYRegister(&heth, LAN8720_BCR, &regvalue);
				if(rtn != HAL_OK)
				{
						printf("read LAN8720 BCR Error.\r\n");
						return -1;
				}
		}while(regvalue & LAN8720_BCR_SOFT_RESET);
	
		LAN8720_StartAutoNego();	 

		return 0;
}

/**
  * @brief	LAN8720 Get Link State
	* @return	Link State
  */
int LAN8720_GetLinkState(void)
{
		uint32_t readval = 0;
		int32_t rtn;
	
		// Read BSR
		rtn = HAL_ETH_ReadPHYRegister(&heth, LAN8720_BSR, &readval);
		if( rtn != HAL_OK )
		{
					printf("read LAN8720 BSR Error.\r\n");
					return LAN8720_STATUS_READ_ERROR;
		}
	
		// Read BSR Again
		rtn = HAL_ETH_ReadPHYRegister(&heth, LAN8720_BSR, &readval);
		if( rtn != HAL_OK )
		{
					printf("read LAN8720 BSR Error.\r\n");
					return LAN8720_STATUS_READ_ERROR;
		}
	
		if((readval & LAN8720_BSR_LINK_STATUS) == 0)
		{
					return LAN8720_STATUS_LINK_DOWN;    
		}
	
		// Read BCR - Check Auto negotiaition 
		rtn = HAL_ETH_ReadPHYRegister(&heth,  LAN8720_BCR, &readval);
		if( rtn != HAL_OK )
		{
					return LAN8720_STATUS_READ_ERROR;
		}
		
		if((readval & LAN8720_BCR_AUTONEGO_EN) != LAN8720_BCR_AUTONEGO_EN)
		{
					if(((readval & LAN8720_BCR_SPEED_SELECT) == LAN8720_BCR_SPEED_SELECT) && ((readval & LAN8720_BCR_DUPLEX_MODE) == LAN8720_BCR_DUPLEX_MODE)) 
					{	
								return LAN8720_STATUS_100MBITS_FULLDUPLEX;
					}
					else if ((readval & LAN8720_BCR_SPEED_SELECT) == LAN8720_BCR_SPEED_SELECT)
					{
								return LAN8720_STATUS_100MBITS_HALFDUPLEX;
					}        
					else if ((readval & LAN8720_BCR_DUPLEX_MODE) == LAN8720_BCR_DUPLEX_MODE)
					{
								return LAN8720_STATUS_10MBITS_FULLDUPLEX;
					}
					else
					{
								return LAN8720_STATUS_10MBITS_HALFDUPLEX;
					}  		
		}
		else
		{
					// Read PHYSCSR
					rtn = HAL_ETH_ReadPHYRegister(&heth,  LAN8720_PHYSCSR, &readval);
					if( rtn != HAL_OK )
					{
								return LAN8720_STATUS_READ_ERROR;
					}
		
		// Check if auto nego not done
		if((readval & LAN8720_PHYSCSR_AUTONEGO_DONE) == 0)
		{
					return LAN8720_STATUS_AUTONEGO_NOTDONE;
		}
		
		// Read PHYSCSR
		if((readval & LAN8720_PHYSCSR_HCDSPEEDMASK) == LAN8720_PHYSCSR_100BTX_FD)
		{
					return LAN8720_STATUS_100MBITS_FULLDUPLEX;
		}
		else if ((readval & LAN8720_PHYSCSR_HCDSPEEDMASK) == LAN8720_PHYSCSR_100BTX_HD)
		{
					return LAN8720_STATUS_100MBITS_HALFDUPLEX;
		}
		else if ((readval & LAN8720_PHYSCSR_HCDSPEEDMASK) == LAN8720_PHYSCSR_10BT_FD)
		{
					return LAN8720_STATUS_10MBITS_FULLDUPLEX;
		}
		else
		{
					return LAN8720_STATUS_10MBITS_HALFDUPLEX;
		}				
	}
}

/**
 * @brief  Read PHY register value
 * @param  reg: 	Register address to read
 * @retval 0: 		Read successful
 * @retval -1: 		Read failed
 */
int32_t ETH_PHY_ReadReg(uint16_t reg,uint32_t *regval)
{
		if(HAL_ETH_ReadPHYRegister(&heth,reg,regval)!=HAL_OK)
					return -1;
    return 0;
}

/**
 * @brief  Write value to PHY register
 * @param  reg:   Register address to write to
 * @param  value: Value to write
 * @retval 0:     Write successful
 * @retval -1:    Write failed
 */
int32_t ETH_PHY_WriteReg(uint16_t reg,uint16_t value)
{
    uint32_t temp = value;
    if(HAL_ETH_WritePHYRegister(&heth,reg,temp)!=HAL_OK)
					return -1;
    return 0;
}

/**
 * @brief  Enable LAN8720A auto-negotiation
 */
void LAN8720_StartAutoNego(void)
{
    uint32_t readval = 0;
    ETH_PHY_ReadReg(LAN8720_BCR, &readval);
    readval |= LAN8720_BCR_AUTONEGO_EN;
    ETH_PHY_WriteReg(LAN8720_BCR, readval);
}
