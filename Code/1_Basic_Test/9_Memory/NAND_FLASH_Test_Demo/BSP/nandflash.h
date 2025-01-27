#ifndef __NANDFLASH_H
#define __NANDFLASH_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "fmc.h"
#include "user_printf.h"

#define NAND_MAX_PAGE_SIZE   4096 // 定义NAND FLASH的最大的PAGE大小（不包括SPARE区），默认4096字节
#define NAND_ECC_SECTOR_SIZE 512  // 执行ECC计算的单元大小，默认512字节

// NAND FLASH操作相关延时函数
#define NAND_TADL_DELAY  30  // tADL等待延迟,最少70ns
#define NAND_TWHR_DELAY  25  // tWHR等待延迟,最少60ns
#define NAND_TRHW_DELAY  35  // tRHW等待延迟,最少100ns
#define NAND_TPROG_DELAY 200 // tPROG等待延迟,典型值200us,最大需要700us
#define NAND_TBERS_DELAY 4   // tBERS等待延迟,典型值3.5ms,最大需要10ms

// NAND Flash的闲/忙引脚
#define NAND_RB_Port GPIOB
#define NAND_RB_Pin  GPIO_PIN_6

#define NAND_ADDRESS 0X80000000 // nand flash的访问地址,接NCE3,地址为:0X8000 0000
#define NAND_CMD     1 << 16    // 发送命令
#define NAND_ADDR    1 << 17    // 发送地址

// NAND FLASH命令
#define NAND_READID        0X90 // 读ID指令
#define NAND_FEATURE       0XEF // 设置特性指令
#define NAND_RESET         0XFF // 复位NAND
#define NAND_READSTA       0X70 // 读状态
#define NAND_AREA_A        0X00
#define NAND_AREA_TRUE1    0X30
#define NAND_WRITE0        0X80
#define NAND_WRITE_TURE1   0X10
#define NAND_ERASE0        0X60
#define NAND_ERASE1        0XD0
#define NAND_MOVEDATA_CMD0 0X00
#define NAND_MOVEDATA_CMD1 0X35
#define NAND_MOVEDATA_CMD2 0X85
#define NAND_MOVEDATA_CMD3 0X10

// NAND FLASH状态
#define NSTA_READY      0X40 // nand已经准备好
#define NSTA_ERROR      0X01 // nand错误
#define NSTA_TIMEOUT    0X02 // 超时
#define NSTA_ECC1BITERR 0X03 // ECC 1bit错误
#define NSTA_ECC2BITERR 0X04 // ECC 2bit以上错误

// NAND FLASH型号和对应的ID号
#define MT29F4G08ABADA  0XDC909556 // MT29F4G08ABADA
#define MT29F16G08ABABA 0X48002689 // MT29F16G08ABABA

// NAND属性结构体
typedef struct
{
    uint16_t page_totalsize;                                       // 每页总大小，main区和spare区总和
    uint16_t page_mainsize;                                        // 每页的main区大小
    uint16_t page_sparesize;                                       // 每页的spare区大小
    uint8_t block_pagenum;                                         // 每个块包含的页数量
    uint16_t plane_blocknum;                                       // 每个plane包含的块数量
    uint16_t block_totalnum;                                       // 总的块数量
    uint16_t good_blocknum;                                        // 好块数量
    uint16_t valid_blocknum;                                       // 有效块数量(供文件系统使用的好块数量)
    uint32_t id;                                                   // NAND FLASH ID
    uint16_t *lut;                                                 // LUT表，用作逻辑块-物理块转换
    uint32_t ecc_hard;                                             // 硬件计算出来的ECC值
    uint32_t ecc_hdbuf[NAND_MAX_PAGE_SIZE / NAND_ECC_SECTOR_SIZE]; // ECC硬件计算值缓冲区
    uint32_t ecc_rdbuf[NAND_MAX_PAGE_SIZE / NAND_ECC_SECTOR_SIZE]; // ECC读取的值缓冲区
} NANDFLASH_Attriute;

extern NANDFLASH_Attriute nand_flash_dev;

uint8_t NAND_Init(void);
uint8_t NAND_ReadPage(uint32_t PageNum, uint16_t ColNum, uint8_t *pBuffer, uint16_t NumByteToRead);
uint8_t NAND_ReadPageComp(uint32_t PageNum, uint16_t ColNum, uint32_t CmpVal, uint16_t NumByteToRead, uint16_t *NumByteEqual);
uint8_t NAND_WritePage(uint32_t PageNum, uint16_t ColNum, uint8_t *pBuffer, uint16_t NumByteToWrite);
uint8_t NAND_WritePageConst(uint32_t PageNum, uint16_t ColNum, uint32_t cval, uint16_t NumByteToWrite);
uint8_t NAND_CopyPageWithoutWrite(uint32_t Source_PageNum, uint32_t Dest_PageNum);
uint8_t NAND_CopyPageWithWrite(uint32_t Source_PageNum, uint32_t Dest_PageNum, uint16_t ColNum, uint8_t *pBuffer, uint16_t NumByteToWrite);
uint8_t NAND_ReadSpare(uint32_t PageNum, uint16_t ColNum, uint8_t *pBuffer, uint16_t NumByteToRead);
uint8_t NAND_WriteSpare(uint32_t PageNum, uint16_t ColNum, uint8_t *pBuffer, uint16_t NumByteToRead);
uint8_t NAND_EraseBlock(uint32_t BlockNum);
void NAND_EraseChip(void);

uint16_t NAND_ECC_Get_OE(uint8_t oe, uint32_t eccval);
uint8_t NAND_ECC_Correction(uint8_t *data_buf, uint32_t eccrd, uint32_t ecccl);

#ifdef __cplusplus
}
#endif
#endif