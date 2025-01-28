#ifndef _W25Q64_H

#include "stm32f1xx.h"
/*************************驱动方式选择*****************************/
/* 是否使用软件SPI */
// #define SoftWare_SPI
/* 是否使用硬件SPI */
#define HardWare_SPI
/* 是否使能FreeRTOS */
// #define USE_FREERTOS
/*************************片选引脚定义*****************************/
#define W25Q64_CS_Port GPIOA
#define W25Q64_CS_Pin  GPIO_PIN_4
/*************************设备命令定义*****************************/
#define W25Q64_DEVICEID_CMD             0x90 // 读取ID命令

#define W25Q64_READ_STATUS_REG1_CMD     0x05 // 状态寄存器1
#define W25Q64_READ_STATUS_REG2_CMD     0x35 // 状态寄存器2
#define W25Q64_READ_STATUS_REG3_CMD     0x15 // 状态寄存器3

#define W25Q64_READ_CMD                 0x03 // 读取命令
#define W25Q64_FAST_READ_CMD            0x0B
#define W25Q64_DUAL_OUT_FAST_READ_CMD   0x3B
#define W25Q64_DUAL_INOUT_FAST_READ_CMD 0xBB
#define W25Q64_QUAD_OUT_FAST_READ_CMD   0x6B
#define W25Q64_QUAD_INOUT_FAST_READ_CMD 0xEB

#define W25Q64_WRITE_ENABLE_CMD         0x06 // 写使能
#define W25Q64_WRITE_DISABLE_CMD        0x04

#define W25Q64_SECTOR_ERASE_CMD         0x20 // 页擦除
#define W25Q64_CHIP_ERASE_CMD           0xC7

#define W25Q64_PROG_ERASE_RESUME_CMD    0x7A
#define W25Q64_PROG_ERASE_SUSPEND_CMD   0x75

#define W25Q64_PAGE_PROG_CMD            0x02 // 页写入
#define W25Q64_QUAD_INPUT_PAGE_PROG_CMD 0x32
/************************* 函数声明 ********************************/
uint16_t W25Q64_ReadID(void);
int W25Q64_ReadFlash(uint8_t *buffer, uint32_t addr, uint16_t bytes);
void W25Q64_Write_Enable(void);
void W25Q64_Write_Disable(void);
void W25Q64_Write_Page(uint8_t *data, uint32_t addr, uint16_t bytes);
void W25Q64_Erase_Sector(uint32_t sector_addr);

#endif