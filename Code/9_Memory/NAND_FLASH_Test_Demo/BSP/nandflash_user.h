#ifndef __NANDFLASH_USER_H
#define __NANDFLASH_USER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

uint8_t test_writepage(uint32_t pagenum, uint16_t colnum, uint16_t writebytes);
uint8_t test_readpage(uint32_t pagenum, uint16_t colnum, uint16_t readbytes);
uint8_t test_copypageandwrite(uint32_t spnum, uint32_t dpnum, uint16_t colnum, uint16_t writebytes);
uint8_t test_readspare(uint32_t pagenum, uint16_t colnum, uint16_t readbytes);
void test_readallblockinfo(uint32_t sblock);
uint8_t test_ftlwritesectors(uint32_t secx, uint16_t secsize, uint16_t seccnt);
uint8_t test_ftlreadsectors(uint32_t secx, uint16_t secsize, uint16_t seccnt);

#ifdef __cplusplus
}
#endif
#endif