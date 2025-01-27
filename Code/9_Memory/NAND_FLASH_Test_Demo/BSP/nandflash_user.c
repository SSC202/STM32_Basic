#include "nandflash_user.h"
#include "nandflash.h"
#include "nandftl.h"
#include "string.h"
#include "usart.h"
#include "malloc.h"

/**
 * @brief   向NAND某一页写入指定大小的数据
 * @param   pagenum     要写入的页地址
 * @param   colnum      要写入的开始列地址(页内地址)
 * @param   writebytes  要写入的数据大小，MT29F16G最大为4320，MT29F4G最大为2112
 * @return 
 */
uint8_t test_writepage(uint32_t pagenum, uint16_t colnum, uint16_t writebytes)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint16_t i  = 0;
    pbuf        = mymalloc(SRAMIN, 5000);
    for (i = 0; i < writebytes; i++) // 准备要写入的数据,填充数据,从0开始增大
    {
        pbuf[i] = i;
    }
    sta = NAND_WritePage(pagenum, colnum, pbuf, writebytes); // 向nand写入数据
    myfree(SRAMIN, pbuf);                                    // 释放内存
    return sta;
}

/**
 * @brief   读取NAND某一页指定大小的数据
 * @param   pagenum     要读取的页地址
 * @param   colnum      要读取的开始列地址(页内地址)
 * @param   readbytes   要读取的数据大小，MT29F16G最大为4320，MT29F4G最大为2112
 * @return 
 */
uint8_t test_readpage(uint32_t pagenum, uint16_t colnum, uint16_t readbytes)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint16_t i  = 0;
    pbuf        = mymalloc(SRAMIN, 5000);
    sta         = NAND_ReadPage(pagenum, colnum, pbuf, readbytes);    // 读取数据
    if (sta == 0 || sta == NSTA_ECC1BITERR || sta == NSTA_ECC2BITERR) // 读取成功
    {
        printf("read page data is:\r\n");
        for (i = 0; i < readbytes; i++) {
            printf("%x ", pbuf[i]); // 串口打印读取到的数据
        }
        printf("\r\nend\r\n");
    }
    myfree(SRAMIN, pbuf); // 释放内存
    return sta;
}

/**
 * @brief   将一页数据拷贝到另外一页,并写入一部分内容
 * @attention           源页和目标页要在同一个Plane内(同为奇数/同为偶数)
 * @param   spnum       源页地址
 * @param   epnum       目标页地址
 * @param   colnum      要写入的开始列地址(页内地址)
 * @param   writebytes  要写入的数据大小，不能超过页大小
 * @return 
 */
uint8_t test_copypageandwrite(uint32_t spnum, uint32_t dpnum, uint16_t colnum, uint16_t writebytes)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint16_t i  = 0;
    pbuf        = mymalloc(SRAMIN, 5000);
    for (i = 0; i < writebytes; i++) // 准备要写入的数据,填充数据,从0X80开始增大
    {
        pbuf[i] = i + 0X80;
    }
    sta = NAND_CopyPageWithWrite(spnum, dpnum, colnum, pbuf, writebytes); // 向nand写入数据
    myfree(SRAMIN, pbuf);                                                 // 释放内存
    return sta;
}

/**
 * @brief   读取NAND某一页Spare区指定大小的数据
 * @attention           源页和目标页要在同一个Plane内(同为奇数/同为偶数)
 * @param   spnum       源页地址
 * @param   epnum       目标页地址
 * @param   colnum      要写入的开始列地址(页内地址)
 * @param   writebytes  要写入的数据大小，不能超过页大小
 * @return 
 */
uint8_t test_readspare(uint32_t pagenum, uint16_t colnum, uint16_t readbytes)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint16_t i  = 0;
    pbuf        = mymalloc(SRAMIN, 512);
    sta         = NAND_ReadSpare(pagenum, colnum, pbuf, readbytes); // 读取数据
    if (sta == 0)                                                   // 读取成功
    {
        printf("read spare data is:\r\n");
        for (i = 0; i < readbytes; i++) {
            printf("%x ", pbuf[i]); // 串口打印读取到的数据
        }
        printf("\r\nend\r\n");
    }
    myfree(SRAMIN, pbuf); // 释放内存
    return sta;
}

/**
 * @brief   从指定位置开始,读取整个NAND,每个BLOCK的第一个page的前5个字节
 * @attention           源页和目标页要在同一个Plane内(同为奇数/同为偶数)
 * @param   sblock      指定开始的block编号
 * @return 
 */
void test_readallblockinfo(uint32_t sblock)
{
    uint8_t j  = 0;
    uint32_t i = 0;
    uint8_t sta;
    uint8_t buffer[5];
    for (i = sblock; i < nand_flash_dev.block_totalnum; i++) {
        printf("block %d info:", i);
        sta = NAND_ReadSpare(i * nand_flash_dev.block_pagenum, 0, buffer, 5); // 读取每个block,第一个page的前5个字节
        if (sta) printf("failed\r\n");
        for (j = 0; j < 5; j++) {
            printf("%x ", buffer[j]);
        }
        printf("\r\n");
    }
}

/**
 * @brief   从某个扇区开始,写入seccnt个扇区的数据
 * @param   secx    开始的扇区编号
 * @param   secsize 扇区大小
 * @param   seccnt  要写入的扇区个数
 * @return 
 */
uint8_t test_ftlwritesectors(uint32_t secx, uint16_t secsize, uint16_t seccnt)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint32_t i  = 0;
    pbuf        = mymalloc(SRAMIN, secsize * seccnt);
    for (i = 0; i < secsize * seccnt; i++) // 准备要写入的数据,填充数据,从0开始增大
    {
        pbuf[i] = i;
    }
    sta = FTL_WriteSectors(pbuf, secx, secsize, seccnt); // 向nand写入数据
    myfree(SRAMIN, pbuf);                                // 释放内存
    return sta;
}

/**
 * @brief   从某个扇区开始,读出seccnt个扇区的数据
 * @param   secx    开始的扇区编号
 * @param   secsize 扇区大小
 * @param   seccnt:要读取的扇区个数
 * @return 
 */
uint8_t test_ftlreadsectors(uint32_t secx, uint16_t secsize, uint16_t seccnt)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint32_t i  = 0;
    pbuf        = mymalloc(SRAMIN, secsize * seccnt);
    sta         = FTL_ReadSectors(pbuf, secx, secsize, seccnt); // 读取数据
    if (sta == 0) {
        printf("read sec %d data is:\r\n", secx);
        for (i = 0; i < secsize * seccnt; i++) // 准备要写入的数据,填充数据,从0开始增大
        {
            printf("%x ", pbuf[i]); // 串口打印读取到的数据
        }
        printf("\r\nend\r\n");
    }
    myfree(SRAMIN, pbuf); // 释放内存
    return sta;
}