/**
  ******************************************************************************
  * @file    Loader_Src.c
  * @author  MCD Tools Team
  * @date    October-2015
  * @brief   This file defines the operations of the external loader for
  *          N25Q256A QSPI memory of STM32L476G-EVAL.
  ******************************************************************************
  */

#include "Loader_Src.h"
#include "stm32f769i_eval_qspi.h"
#include <string.h>


/* Private variables ---------------------------------------------------------*/
//QSPI structure
extern QSPI_HandleTypeDef QSPIHandle; 
QSPI_CommandTypeDef sCommand;
QSPI_AutoPollingTypeDef sConfig;



/**
  * @brief  System initialization.
  * @param  None
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */
int Init (uint8_t configureMemoryMappedMode)
{
   SystemInit();
  SystemClock_Config();
  
  /* Zero Init structs */
  memset(&QSPIHandle, 0, sizeof(QSPIHandle));
  memset(&sCommand, 0, sizeof(sCommand));
  memset(&sConfig, 0, sizeof(sConfig));
 
  /* Initialize QuadSPI ------------------------------------------------------ */
  QSPIHandle.Instance = QUADSPI;	  
  HAL_QSPI_DeInit(&QSPIHandle) ;
  
  QSPIHandle.Init.ClockPrescaler     = 2;
  QSPIHandle.Init.FifoThreshold      = 1;
  QSPIHandle.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_NONE; 
  QSPIHandle.Init.FlashSize          = QSPI_FLASH_SIZE;
  QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_3_CYCLE; 
  QSPIHandle.Init.ClockMode          = QSPI_CLOCK_MODE_0;
  QSPIHandle.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;
  QSPIHandle.Init.FlashID            = QSPI_FLASH_ID_1;
  
  if (HAL_QSPI_Init(&QSPIHandle) != HAL_OK)
  {
    return 0;
  }
  
  ResetMemory(&QSPIHandle);
  
  /* Enable 32-Bit address mode */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.AlternateBytesSize= QSPI_ALTERNATE_BYTES_8_BITS;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  sCommand.DummyCycles       = 0;
  
  sCommand.Instruction       = ENTER_4_BYTE_ADDR_MODE_CMD ;
  QSPIHandle.State           = HAL_QSPI_STATE_READY;
  HAL_QSPI_Command(&QSPIHandle, &sCommand, 0x10); 
  
  /* Configure automatic polling mode to wait the memory is ready */
  if(QSPI_AutoPollingMemReady(&QSPIHandle) != 1)
  {
    return 0;
  }
  if(!configureMemoryMappedMode)
  {
    if(QUADSPI_MappedMode() !=1)
      return 0; //fail
  }
  return 1; 
}




/*******************************************************************************
Description    :	         erase sector in the device										        
Inputs 	:																        
sector address																		
outputs 	:																				
"1" 		: Operation succeeded											
********************************************************************************/
int EraseSector (unsigned long adr) 
{
  if(QSPI_WriteEnable(&QSPIHandle) != 1)
  {
    return 0;
  }
  
  /* Erasing Sequence -------------------------------------------------- */
  sCommand.Instruction       = SECTOR_ERASE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
  sCommand.Address           = adr ;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;

  if(HAL_QSPI_Command( &QSPIHandle, &sCommand,HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  
  /* Configure automatic polling mode to wait for end of erase ------- */  
  if (QSPI_AutoPollingMemReady(&QSPIHandle) != 1)
  {
    return 0;
  }
  
  return 1 ;	
}

/*******************************************************************************
Description    :	         Erase a full sector in the device 										        
Inputs 	:		 StartSectrorAddress	: Start of sector 														        
EndSectrorAddress	: End of sector 																	
outputs 	:																				
"1" 		: Operation succeeded	
Note : Not Mandatory for SRAM PSRAM and NOR_FLASH
********************************************************************************/
KeepInCompilation int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{      
  uint32_t BlockAddr;
  EraseStartAddress = EraseStartAddress - (EraseStartAddress % 0x10000);
  
  while (EraseEndAddress >= EraseStartAddress)
  {      
    BlockAddr = EraseStartAddress & 0x0FFFFFFF;
    if(EraseSector(BlockAddr) != 1)
    {
      return 0;
    }
    EraseStartAddress += 0x10000;
  }
  
  return 1;	
}

/*******************************************************************************
Description :	                Write data to QSPI 														       
Inputs :																					           
Address 	: Write location  										     
Size 		: Length in bytes 										     
buffer 		: Address where to get the data to write	 
outputs :																				           
"1" 	        : Operation succeeded								       
Info :																						           
Note : Mandatory for all types except SRAM and PSRAM			   
********************************************************************************/
KeepInCompilation int Write(uint32_t Address, uint32_t Size, uint8_t* buffer)
{
  uint32_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  uint32_t   QSPI_DataNum = 0;
  
  Addr = Address % QSPI_PAGE_SIZE;
  count = QSPI_PAGE_SIZE - Addr;
  NumOfPage =  Size / QSPI_PAGE_SIZE;
  NumOfSingle = Size % QSPI_PAGE_SIZE;
  
  if (Addr == 0) /*!< Address is QSPI_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < QSPI_PAGESIZE */
    {
      QSPI_DataNum = Size;      
      QSPI_WritePage(Address, QSPI_DataNum, buffer);
    }
    else /*!< Size > QSPI_PAGESIZE */
    {
      while (NumOfPage--)
      {
        QSPI_DataNum = QSPI_PAGE_SIZE;
        QSPI_WritePage(Address, QSPI_DataNum, buffer);
        Address +=  QSPI_PAGE_SIZE;
        buffer += QSPI_PAGE_SIZE;
      }
      
      QSPI_DataNum = NumOfSingle;
      if(QSPI_DataNum > 0)
        QSPI_WritePage(Address, QSPI_DataNum, buffer);
    }
  }
  else /*!< Address is not QSPI_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< Size < QSPI_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (Size + Address) > QSPI_PAGESIZE */
      {
        temp = NumOfSingle - count;
        QSPI_DataNum = count;
        QSPI_WritePage(Address, QSPI_DataNum, buffer);
        Address +=  count;
        buffer += count;
        QSPI_DataNum = temp;
        QSPI_WritePage(Address, QSPI_DataNum, buffer);
      }
      else
      {
        QSPI_DataNum = Size; 
        QSPI_WritePage(Address, QSPI_DataNum, buffer);
      }
    }
    else /*!< Size > QSPI_PAGESIZE */
    {
      Size -= count;
      NumOfPage =  Size / QSPI_PAGE_SIZE;
      NumOfSingle = Size % QSPI_PAGE_SIZE;
      QSPI_DataNum = count;
      QSPI_WritePage(Address, QSPI_DataNum, buffer);
      Address +=  count;
      buffer += count;
      
      while (NumOfPage--)
      {
        QSPI_DataNum = QSPI_PAGE_SIZE;
        QSPI_WritePage(Address, QSPI_DataNum, buffer);
        Address +=  QSPI_PAGE_SIZE;
        buffer += QSPI_PAGE_SIZE;
      }
      
      if (NumOfSingle != 0)
      {
        QSPI_DataNum = NumOfSingle;
        QSPI_WritePage(Address, QSPI_DataNum, buffer);
      }
    }
  }
  
  return 1;
}

/*******************************************************************************
Description :		        Write pages to QSPI 														       
Inputs :																					           
Adr	        : Write location  										     
Size 		: Length in bytes 										     
buf 		: Address where to get the data to write	 
outputs :																				           
"1" 	        : Operation succeeded								       			   
********************************************************************************/
int  QSPI_WritePage(unsigned long adr, unsigned long sz, unsigned char *buf) 
{
  if(QSPI_WriteEnable(&QSPIHandle) != 1)
  {
    return 0;
  }
  
  /* Writing Sequence ------------------------------------------------ */
  sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
  sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
  sCommand.AddressSize = QSPI_ADDRESS_32_BITS;
  sCommand.Address     = adr & 0x0FFFFFFF;
  sCommand.DataMode    = QSPI_DATA_4_LINES;
  sCommand.NbData      = sz;
  
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  
  if (HAL_QSPI_Transmit(&QSPIHandle, buf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  
  /* Configure automatic polling mode to wait for end of program ----- */  
  if(QSPI_AutoPollingMemReady(&QSPIHandle) != 1)
  {
    return 0;
  }
  
  return 1;
}

/*******************************************************************************
Description :			This function send a Write Enable and wait it is effective.														       
Inputs :																					           
hqspi    : QSPI handle										     

outputs :																				           
"1" 	        : Operation succeeded	
********************************************************************************/
static int QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
  /* Enable write operations ------------------------------------------ */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = WRITE_ENABLE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  
  /* Configure automatic polling mode to wait for write enabling ---- */    
  sConfig.Match           = 0x02;
  sConfig.Mask            = 0x02;
  sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
  sConfig.StatusBytesSize = 1;
  sConfig.Interval        = 0x10;
  sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
  
  sCommand.Instruction    = READ_STATUS_REG_CMD;
  sCommand.DataMode       = QSPI_DATA_1_LINE  ;
  
  if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  
  return 1;
}

/*******************************************************************************
Description :		        This function wait for end of operation (erase,write,read).														       
Inputs :																					           
hqspi    : QSPI handle										     

outputs :																				           
NONE
********************************************************************************/
int QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi)
{
  /* Configure automatic polling mode to wait for memory ready ------ */  
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = READ_STATUS_REG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
  sConfig.Match           = 0x00;
  sConfig.Mask            = 0x01;
  sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
  sConfig.StatusBytesSize = 1;
  sConfig.Interval        = 0x10;
  sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
  
  if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  
  return 1;
}

/*******************************************************************************
Description :			Reset memory.														       
Inputs :																					           
hqspi    : QSPI handle										     

outputs :																				           
None	
********************************************************************************/
void ResetMemory(QSPI_HandleTypeDef *hqspi)
{
  /* Reset memory config, Cmd in 1 line */
  /* Send RESET ENABLE command (0x66) to be able to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2166;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB(); 
  
  /* Send RESET command (0x99) to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2199;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB();
  
  /* Reset memory config, Cmd in 2 lines*/
  /* Send RESET ENABLE command (0x66) to be able to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2266;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB(); 
  
  /* Send RESET command (0x99) to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2299;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB();
  
  /* Reset memory config, Cmd in 4 lines*/
  /* Send RESET ENABLE command (0x66) to be able to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2366;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB(); 
  
  /* Send RESET command (0x99) to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2399;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB();
}

/*******************************************************************************
Description :                  This function configure the dummy cycle to read.														       
Inputs :																					           
hqspi    : QSPI handle										     

outputs :																				           
"1" 	 : Operation succeeded	
********************************************************************************/
static int QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi)
{  
  uint8_t reg;  
  
  /* Read Volatile Configuration register --------------------------- */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = READ_VOL_CFG_REG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  sCommand.NbData            = 1;
  
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  
  if (HAL_QSPI_Receive(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  
  /* Enable write operations ---------------------------------------- */
  if (!QSPI_WriteEnable(&QSPIHandle))
  {
    return 0;
  }
  
  /* Write Volatile Configuration register (with new dummy cycles) -- */  
  sCommand.Instruction = WRITE_VOL_CFG_REG_CMD;
  MODIFY_REG(reg, 0xF0, (DUMMY_CLOCK_CYCLES_READ_QUAD << POSITION_VAL(0xF0)));
  
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  
  if (HAL_QSPI_Transmit(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  
  return 1;
}

/*******************************************************************************
Description :			This function configure the memory mapped mode.														       
Inputs :																					           
hqspi    : QSPI handle										     

outputs :																				           
"1" 	 : Operation succeeded	
********************************************************************************/
int QUADSPI_MappedMode(void)
{
  char MemoryMappedMode = 0; 
  
  if (!MemoryMappedMode)	
  {	
    QSPI_MemoryMappedTypeDef sMemMappedCfg;
    
    if( QSPI_DummyCyclesCfg(&QSPIHandle) != 1)
    {
      return 0;
    }
    
    sCommand.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize        = QSPI_ADDRESS_WIDTH;
    sCommand.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.DdrMode            = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
    
    sCommand.Instruction        = QUAD_IN_FAST_PROG_CMD;
    sCommand.AddressMode        = QSPI_ADDRESS_4_LINES;
    sCommand.Address            = 0;
    sCommand.DataMode           = QSPI_DATA_4_LINES;
    sCommand.NbData             = 0; // undefined length until end of memory
    sCommand.Instruction        = QUAD_INOUT_FAST_READ_CMD;
    sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_READ_QUAD;
    
    sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    
    if (HAL_QSPI_MemoryMapped(&QSPIHandle, &sCommand, &sMemMappedCfg) != HAL_OK)
    {
      return 0;
    }
    
    MemoryMappedMode = 1;
  }
  
  return 1;
}

/*******************************************************************************
Description :		       Initialisation of ticks counter 														       
Inputs :						     
TickPriority : Set tick priority 
outputs :																				           
NONE
********************************************************************************/
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  return HAL_OK;
}

/*******************************************************************************
Description :		       Delay in mS.														       
Inputs :						     
Delay : Time in mS	
outputs :																				           
NONE
********************************************************************************/
void HAL_Delay(__IO uint32_t Delay)
{
  for(int i=0;i<Delay*216000;i++);
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 7;
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Activate the OverDrive to reach the 216 MHz Frequency */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }  
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
  
  if(QUADSPI_MappedMode()!=1)
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



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
