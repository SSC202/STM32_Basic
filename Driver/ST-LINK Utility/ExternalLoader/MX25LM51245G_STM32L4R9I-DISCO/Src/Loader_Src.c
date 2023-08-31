/**
  ******************************************************************************
  * @file    Loader_Src.c
  * @author  MCD Tools Team
  * @date    October-2017
  * @brief   This file defines the operations of the external loader for
  *          MX25LM51245G OSPI memory of STM32L4R9I-DISCO.
  ******************************************************************************
  */

#include "Loader_Src.h"
#include <string.h>

 __IO uint8_t                    StatusMatch;
 extern OSPI_HandleTypeDef       OSPINORHandle;
 OSPI_RegularCmdTypeDef          sCommand;


/**
  * @brief  System initialization.
  * @param  None
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */
int Init (uint8_t configureMemoryMappedMode)
{ 
  OSPI_MemoryMappedTypeDef sMemMappedCfg;
  /* Zero Init structs */
  memset(&OSPINORHandle,0,sizeof(OSPINORHandle));
  memset(&sCommand,0,sizeof(sCommand));
  memset(&sMemMappedCfg,0,sizeof(sMemMappedCfg));
  
  HAL_Init(); 
  SystemClock_Config();
 
  /* Memory-mapped mode configuration ------------------------------- */
  sCommand.OperationType = HAL_OSPI_OPTYPE_WRITE_CFG;
  sCommand.Instruction   = OCTAL_PAGE_PROG_CMD;
  sCommand.DataMode      = HAL_OSPI_DATA_8_LINES;
  sCommand.NbData        = 1; 
  sCommand.OperationType = HAL_OSPI_OPTYPE_READ_CFG;
  sCommand.Instruction   = OCTAL_IO_READ_CMD;
  sCommand.DummyCycles   = DUMMY_CLOCK_CYCLES_READ;

  sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_ENABLE;
  sMemMappedCfg.TimeOutPeriod     = 0x20;
	
  if(BSP_OSPI_NOR_Init() != OSPI_NOR_OK)
    return 0; //fail
  
  if(!configureMemoryMappedMode)
  {
    if(BSP_OSPI_NOR_EnableMemoryMappedMode() != OSPI_NOR_OK)
      return 0; //fail
  }
  
  return 1; //success
}


/**
  * @brief   Program memory.
  * @param   Address: page address
  * @param   Size   : size of data
  * @param   buffer : pointer to data buffer
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */
KeepInCompilation int Write (uint32_t Address, uint32_t Size, uint8_t* buffer)
{
  if(BSP_OSPI_NOR_Write(buffer, Address, Size) != OSPI_NOR_OK)
    return 0;
  
  return 1;
}


/**
  * @brief   Sector erase.
  * @param   EraseStartAddress :  erase start address
  * @param   EraseEndAddress   :  erase end address
  * @retval  None
  */
KeepInCompilation int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{
  uint32_t BlockAddr = 0;	
  while (EraseEndAddress>=EraseStartAddress)
  {
    BlockAddr = EraseStartAddress & 0x0FFFFFFF;
    if(BSP_OSPI_NOR_Erase_Block(BlockAddr) != OSPI_NOR_OK)
      return 0;
    EraseStartAddress += 0x10000;
  }
  
  return 1;	
}

KeepInCompilation int MassErase()
{
  if(BSP_OSPI_NOR_Erase_Chip() != OSPI_NOR_OK)
    return 0;
  
  return 1;	
}

/**
  * Description :
  * Calculates checksum value of the memory zone
  * Inputs    :
  *      StartAddress  : Flash start address
  *      Size          : Size (in WORD)  
  *      InitVal       : Initial CRC value
  * outputs   :
  *     R0             : Checksum value
  * Note: Optional for all types of device
  */
uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal)
{
  uint8_t missalignementAddress = StartAddress%4;
  uint8_t missalignementSize = Size;
  int cnt;
  uint32_t Val;
	
  StartAddress-=StartAddress%4;
  Size += (Size%4==0)?0:4-(Size%4);
  
  for(cnt=0; cnt<Size ; cnt+=4)
  {
    Val = *(uint32_t*)StartAddress;
    if(missalignementAddress)
    {
      switch (missalignementAddress)
      {
        case 1:
          InitVal += (uint8_t) (Val>>8 & 0xff);
          InitVal += (uint8_t) (Val>>16 & 0xff);
          InitVal += (uint8_t) (Val>>24 & 0xff);
          missalignementAddress-=1;
          break;
        case 2:
          InitVal += (uint8_t) (Val>>16 & 0xff);
          InitVal += (uint8_t) (Val>>24 & 0xff);
          missalignementAddress-=2;
          break;
        case 3:   
          InitVal += (uint8_t) (Val>>24 & 0xff);
          missalignementAddress-=3;
          break;
      }  
    }
    else if((Size-missalignementSize)%4 && (Size-cnt) <=4)
    {
      switch (Size-missalignementSize)
      {
        case 1:
          InitVal += (uint8_t) Val;
          InitVal += (uint8_t) (Val>>8 & 0xff);
          InitVal += (uint8_t) (Val>>16 & 0xff);
          missalignementSize-=1;
          break;
        case 2:
          InitVal += (uint8_t) Val;
          InitVal += (uint8_t) (Val>>8 & 0xff);
          missalignementSize-=2;
          break;
        case 3:   
          InitVal += (uint8_t) Val;
          missalignementSize-=3;
          break;
      } 
    }
    else
    {
      InitVal += (uint8_t) Val;
      InitVal += (uint8_t) (Val>>8 & 0xff);
      InitVal += (uint8_t) (Val>>16 & 0xff);
      InitVal += (uint8_t) (Val>>24 & 0xff);
    }
    StartAddress+=4;
  }
  
  return (InitVal);
}


/**
  * Description :
  * Verify flash memory with RAM buffer and calculates checksum value of
  * the programmed memory
  * Inputs    :
  *      FlashAddr     : Flash address
  *      RAMBufferAddr : RAM buffer address
  *      Size          : Size (in WORD)  
  *      InitVal       : Initial CRC value
  * outputs   :
  *     R0             : Operation failed (address of failure)
  *     R1             : Checksum value
  * Note: Optional for all types of device
  */
KeepInCompilation uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement)
{
  uint32_t VerifiedData = 0, InitVal = 0;
  uint64_t checksum;
  Size*=4;
  
  if(BSP_OSPI_NOR_EnableMemoryMappedMode() != OSPI_NOR_OK)
    return 0;
  
  checksum = CheckSum((uint32_t)MemoryAddr + (missalignement & 0xF), Size - ((missalignement >> 16) & 0xF), InitVal);
  while (Size>VerifiedData)
  {
    if ( *(uint8_t*)MemoryAddr++ != *((uint8_t*)RAMBufferAddr + VerifiedData))
      return ((checksum<<32) + (MemoryAddr + VerifiedData));  
   
    VerifiedData++;  
  }
        
  return (checksum<<32);
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 110000000
  *            HCLK(Hz)                       = 110000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 55
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable MSI Oscillator and activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 55;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    return;
  
  /* To avoid undershoot due to maximum frequency, select PLL as system clock source */
  /* with AHB prescaler divider 2 as first step */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
 if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
   return;

  /* AHB prescaler divider at 1 as second step */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    return;
}


HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  return HAL_OK;
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
