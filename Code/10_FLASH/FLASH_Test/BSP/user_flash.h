#include "stm32f1xx.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"

/* FLASH大小：STM32F103C8T6:64K */
#define STM32FLASH_SIZE 0x00010000UL
/* FLASH起始地址 */
#define STM32FLASH_BASE FLASH_BASE
/* FLASH结束地址 */
#define STM32FLASH_END (STM32FLASH_BASE | STM32FLASH_SIZE)
/* FLASH页大小：1K */
#define STM32FLASH_PAGE_SIZE FLASH_PAGE_SIZE
/* FLASH总页数 */
#define STM32FLASH_PAGE_NUM (STM32FLASH_SIZE / STM32FLASH_PAGE_SIZE)

#define WRITE_START_ADDR    ((uint32_t)0x08010000)
#define WRITE_END_ADDR      ((uint32_t)0x0801FFFF)

uint32_t Internal_ReadFlash(uint32_t addrStart, void *pData, uint32_t dataLen);
uint32_t Internal_WriteFlashNoCheck(uint32_t addrStart, const uint16_t *pData, uint32_t dataLen);
uint32_t Internal_WriteFlash(uint32_t addrStart, const uint16_t *pData, uint32_t dataLen);
int Internal_ErasePage(uint32_t pageAddress, uint32_t nbPages);