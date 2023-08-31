/**
  ******************************************************************************
  * @file    Loader_Src.h
  * @author  MCD Tools Team
  * @date    October-2017
  * @brief   Loader Header file.
  ******************************************************************************
  */

#ifndef __LOADER_SRC_H
#define __LOADER_SRC_H


/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_ospi.h"
#include "stm32l4r9xx.h"
#include "stm32l4r9i_discovery_ospi_nor.h"
#include "stm32l4r9i_discovery.h"
#include "mx25lm51245g.h"

#define OSPI_CS_PIN                 GPIO_PIN_12
#define OSPI_CS_GPIO_PORT           GPIOG
#define OSPI_CLK_PIN                GPIO_PIN_6
#define OSPI_CLK_GPIO_PORT          GPIOI
#define OSPI_D0_PIN                 GPIO_PIN_11
#define OSPI_D0_GPIO_PORT           GPIOI
#define OSPI_D1_PIN                 GPIO_PIN_10
#define OSPI_D1_GPIO_PORT           GPIOI
#define OSPI_D2_PIN                 GPIO_PIN_9
#define OSPI_D2_GPIO_PORT           GPIOI
#define OSPI_D3_PIN                 GPIO_PIN_8
#define OSPI_D3_GPIO_PORT           GPIOH
#define OSPI_D4_PIN                 GPIO_PIN_9
#define OSPI_D4_GPIO_PORT           GPIOH
#define OSPI_D5_PIN                 GPIO_PIN_10
#define OSPI_D5_GPIO_PORT           GPIOH
#define OSPI_D6_PIN                 GPIO_PIN_9
#define OSPI_D6_GPIO_PORT           GPIOG
#define OSPI_D7_PIN                 GPIO_PIN_10
#define OSPI_D7_GPIO_PORT           GPIOG
#define OSPI_DQS_PIN                GPIO_PIN_15
#define OSPI_DQS_GPIO_PORT          GPIOG

/* MX25LM512ABA1G12 Macronix memory */
/* Size of the flash */
#define OSPI_FLASH_SIZE             26
#define OSPI_PAGE_SIZE              256

/* Flash commands */
#define OCTAL_IO_READ_CMD           0xEC13
#define OCTAL_PAGE_PROG_CMD         0x12ED
#define OCTAL_READ_STATUS_REG_CMD   0x05FA
#define OCTAL_SECTOR_ERASE_CMD      0x21DE
#define OCTAL_WRITE_ENABLE_CMD      0x06F9
#define READ_STATUS_REG_CMD         0x05
#define WRITE_CFG_REG_2_CMD         0x72
#define WRITE_ENABLE_CMD            0x06

/* Dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ     8
#define DUMMY_CLOCK_CYCLES_READ_REG 4

/* Auto-polling values */
#define WRITE_ENABLE_MATCH_VALUE    0x02
#define WRITE_ENABLE_MASK_VALUE     0x02

#define MEMORY_READY_MATCH_VALUE    0x00
#define MEMORY_READY_MASK_VALUE     0x01

#define AUTO_POLLING_INTERVAL       0x10

/* Memory registers address */
#define CONFIG_REG2_ADDR1           0x0000000
#define CR2_STR_OPI_ENABLE          0x01

#define CONFIG_REG2_ADDR3           0x00000300
#define CR2_DUMMY_CYCLES_66MHZ      0x07

/* Memory delay */
#define MEMORY_REG_WRITE_DELAY      40
#define MEMORY_PAGE_PROG_DELAY      2

/* End address of the OSPI memory */
#define OSPI_END_ADDR               (1 << OSPI_FLASH_SIZE)

/* Size of buffers */
#define BUFFERSIZE                  (COUNTOF(aTxBuffer) - 1)

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)         (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Definition for OSPI clock resources */
#define OSPI_CLK_ENABLE()           __HAL_RCC_OSPI2_CLK_ENABLE()
#define OSPI_CLK_DISABLE()          __HAL_RCC_OSPI2_CLK_DISABLE()
#define OSPIM_CLK_ENABLE()          __HAL_RCC_OSPIM_CLK_ENABLE()
#define OSPIM_CLK_DISABLE()         __HAL_RCC_OSPIM_CLK_DISABLE()
#define OSPI_CS_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOG_CLK_ENABLE()
#define OSPI_CLK_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOI_CLK_ENABLE()
#define OSPI_D0_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOI_CLK_ENABLE()
#define OSPI_D1_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOI_CLK_ENABLE()
#define OSPI_D2_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOI_CLK_ENABLE()
#define OSPI_D3_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define OSPI_D4_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define OSPI_D5_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define OSPI_D6_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOG_CLK_ENABLE()
#define OSPI_D7_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOG_CLK_ENABLE()
#define OSPI_DQS_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOG_CLK_ENABLE()

#define OSPI_FORCE_RESET()          __HAL_RCC_OSPI2_FORCE_RESET()
#define OSPI_RELEASE_RESET()        __HAL_RCC_OSPI2_RELEASE_RESET()

#if defined(__CC_ARM)
extern uint32_t Load$$QSPI$$Base;
extern uint32_t Load$$QSPI$$Length;
#elif defined(__ICCARM__)
#pragma section =".qspi"
#pragma section =".qspi_init"
#elif defined(__GNUC__)
extern uint32_t _qspi_init_base;
extern uint32_t _qspi_init_length;
#endif


#ifdef __ICCARM__                 //IAR
#define KeepInCompilation __root 
#elif __CC_ARM                    //MDK-ARM
#define KeepInCompilation __attribute__((used))
#else // TASKING                  //TrueStudio
#define KeepInCompilation __attribute__((used))
#endif


/* Private function prototypes -----------------------------------------------*/
int Init (uint8_t configureMemoryMappedMode);
KeepInCompilation int Write (uint32_t Address, uint32_t Size, uint8_t* buffer);
KeepInCompilation int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
KeepInCompilation uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement);
void SystemClock_Config(void);
void OSPI_OctalModeCfg(OSPI_HandleTypeDef *hospi);





#endif /* __LOADER_SRC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
