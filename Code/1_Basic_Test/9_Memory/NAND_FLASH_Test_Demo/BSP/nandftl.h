#ifndef __NANDFTL_H
#define __NANDFTL_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

#define FTL_USE_BAD_BLOCK_SEARCH 0 // 定义是否使用坏块搜索

uint8_t FTL_Init(void);
void FTL_BadBlockMark(uint32_t blocknum);
uint8_t FTL_CheckBadBlock(uint32_t blocknum);
uint8_t FTL_UsedBlockMark(uint32_t blocknum);
uint32_t FTL_FindUnusedBlock(uint32_t sblock, uint8_t flag);
uint32_t FTL_FindSamePlaneUnusedBlock(uint32_t sblock);
uint8_t FTL_CopyAndWriteToBlock(uint32_t Source_PageNum, uint16_t ColNum, uint8_t *pBuffer, uint32_t NumByteToWrite);
uint16_t FTL_LBNToPBN(uint32_t LBNNum);
uint8_t FTL_WriteSectors(uint8_t *pBuffer, uint32_t SectorNo, uint16_t SectorSize, uint32_t SectorCount);
uint8_t FTL_ReadSectors(uint8_t *pBuffer, uint32_t SectorNo, uint16_t SectorSize, uint32_t SectorCount);
uint8_t FTL_CreateLUT(uint8_t mode);
uint8_t FTL_BlockCompare(uint32_t blockx, uint32_t cmpval);
uint32_t FTL_SearchBadBlock(void);
uint8_t FTL_Format(void);

#ifdef __cplusplus
}
#endif
#endif