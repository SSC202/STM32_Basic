#include "nandftl.h"
#include "string.h"
#include "malloc.h"
#include "nandflash.h"
#include "user_printf.h"

/**
 * @brief   FTL 初始化
 */
uint8_t FTL_Init(void)
{
    uint8_t temp;
    if (NAND_Init()) return 1; // 初始化NAND FLASH
    if (nand_flash_dev.lut) myfree(SRAMIN, nand_flash_dev.lut);
    nand_flash_dev.lut = mymalloc(SRAMIN, (nand_flash_dev.block_totalnum) * 2); // 给LUT表申请内存
    memset(nand_flash_dev.lut, 0, nand_flash_dev.block_totalnum * 2);           // 全部清理
    if (!nand_flash_dev.lut) return 1;                                          // 内存申请失败
    temp = FTL_CreateLUT(1);
    if (temp) {
        printf("format nand flash...\r\n");
        temp = FTL_Format(); // 格式化NAND
        if (temp) {
            printf("format failed!\r\n");
            return 2;
        }
    } else // 创建LUT表成功
    {
        printf("total block num:%d\r\n", nand_flash_dev.block_totalnum);
        printf("good block num:%d\r\n", nand_flash_dev.good_blocknum);
        printf("valid block num:%d\r\n", nand_flash_dev.valid_blocknum);
    }
    return 0;
}

/**
 * @brief   标记某一个块为坏块
 * @param   blocknum    块编号,范围:0~(block_totalnum-1)
 */
void FTL_BadBlockMark(uint32_t blocknum)
{
    uint32_t temp = 0XAAAAAAAA;                                                           // 坏块标记mark,任意值都OK,只要不是0XFF.这里写前4个字节,方便FTL_FindUnusedBlock函数检查坏块.(不检查备份区,以提高速度)
    NAND_WriteSpare(blocknum * nand_flash_dev.block_pagenum, 0, (uint8_t *)&temp, 4);     // 在第一个page的spare区,第一个字节做坏块标记(前4个字节都写)
    NAND_WriteSpare(blocknum * nand_flash_dev.block_pagenum + 1, 0, (uint8_t *)&temp, 4); // 在第二个page的spare区,第一个字节做坏块标记(备份用,前4个字节都写)
}

/**
 * @brief   检查某一块是否是坏块
 * @param   blocknum    块编号,范围:0~(block_totalnum-1)
 */
uint8_t FTL_CheckBadBlock(uint32_t blocknum)
{
    uint8_t flag = 0;
    NAND_ReadSpare(blocknum * nand_flash_dev.block_pagenum, 0, &flag, 1); // 读取坏块标志
    if (flag == 0XFF)                                                     // 好块?,读取备份区坏块标记
    {
        NAND_ReadSpare(blocknum * nand_flash_dev.block_pagenum + 1, 0, &flag, 1); // 读取备份区坏块标志
        if (flag == 0XFF)
            return 0; // 好块
        else
            return 1; // 坏块
    }
    return 2;
}

/**
 * @brief   标记某一个块已经使用
 * @param   blocknum    块编号,范围:0~(block_totalnum-1)
 */
uint8_t FTL_UsedBlockMark(uint32_t blocknum)
{
    uint8_t Usedflag = 0XCC;
    uint8_t temp     = 0;
    temp             = NAND_WriteSpare(blocknum * nand_flash_dev.block_pagenum, 1, (uint8_t *)&Usedflag, 1); // 写入块已经被使用标志
    return temp;
}

/**
 * @brief   从给定的块开始找到往前找到一个未被使用的块(指定奇数/偶数)
 * @param   sblock  开始块,范围:0~(block_totalnum-1)
 * @param   flag    0,偶数块;1,奇数块.
 * @return  未使用块号
 */
uint32_t FTL_FindUnusedBlock(uint32_t sblock, uint8_t flag)
{
    uint32_t temp     = 0;
    uint32_t blocknum = 0;
    for (blocknum = sblock + 1; blocknum > 0; blocknum--) {
        if (((blocknum - 1) % 2) == flag) // 奇偶合格,才检测
        {
            NAND_ReadSpare((blocknum - 1) * nand_flash_dev.block_pagenum, 0, (uint8_t *)&temp, 4); // 读块是否被使用标记
            if (temp == 0XFFFFFFFF) return (blocknum - 1);                                         // 找到一个空块,返回块编号
        }
    }
    return 0XFFFFFFFF; // 未找到空余块
}

/**
 * @brief   查找与给定块在同一个plane内的未使用的块
 * @param   sblock  开始块,范围:0~(block_totalnum-1)
 * @return  未使用块号
 */
uint32_t FTL_FindSamePlaneUnusedBlock(uint32_t sblock)
{
    static uint32_t curblock = 0XFFFFFFFF;
    uint32_t unusedblock     = 0;
    if (curblock > (nand_flash_dev.block_totalnum - 1)) curblock = nand_flash_dev.block_totalnum - 1; // 超出范围了,强制从最后一个块开始
    unusedblock = FTL_FindUnusedBlock(curblock, sblock % 2);                                          // 从当前块,开始,向前查找空余块
    if (unusedblock == 0XFFFFFFFF && curblock < (nand_flash_dev.block_totalnum - 1))                  // 未找到,且不是从最末尾开始找的
    {
        curblock    = nand_flash_dev.block_totalnum - 1;         // 强制从最后一个块开始
        unusedblock = FTL_FindUnusedBlock(curblock, sblock % 2); // 从最末尾开始,重新找一遍
    }
    if (unusedblock == 0XFFFFFFFF) return 0XFFFFFFFF; // 找不到空闲block
    curblock = unusedblock;                           // 当前块号等于未使用块编号.下次则从此处开始查找
    return unusedblock;                               // 返回找到的空闲block
}

/**
 * @brief   将一个块的数据拷贝到另一块,并且可以写入数据
 * @param   Source_PageNo   要写入数据的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param   ColNum          要写入的列开始地址(也就是页内地址),范围:0~(page_totalsize-1)
 * @param   pBuffer         要写入的数据
 * @param   NumByteToWrite  要写入的字节数，该值不能超过块内剩余容量大小
 */
uint8_t FTL_CopyAndWriteToBlock(uint32_t Source_PageNum, uint16_t ColNum, uint8_t *pBuffer, uint32_t NumByteToWrite)
{
    uint16_t i = 0, temp = 0, wrlen;
    uint32_t source_block = 0, pageoffset = 0;
    uint32_t unusedblock = 0;
    source_block         = Source_PageNum / nand_flash_dev.block_pagenum; // 获得页所在的块号
    pageoffset           = Source_PageNum % nand_flash_dev.block_pagenum; // 获得页在所在块内的偏移
retry:
    unusedblock = FTL_FindSamePlaneUnusedBlock(source_block);  // 查找与源块在一个plane的未使用块
    if (unusedblock > nand_flash_dev.block_totalnum) return 1; // 当找到的空余块号大于块总数量的话肯定是出错了
    for (i = 0; i < nand_flash_dev.block_pagenum; i++)         // 将一个块的数据复制到找到的未使用块中
    {
        if (i >= pageoffset && NumByteToWrite) // 数据要写入到当前页
        {
            if (NumByteToWrite > (nand_flash_dev.page_mainsize - ColNum)) // 要写入的数据,超过了当前页的剩余数据
            {
                wrlen = nand_flash_dev.page_mainsize - ColNum; // 写入长度等于当前页剩余数据长度
            } else
                wrlen = NumByteToWrite; // 写入全部数据
            temp   = NAND_CopyPageWithWrite(source_block * nand_flash_dev.block_pagenum + i, unusedblock * nand_flash_dev.block_pagenum + i, ColNum, pBuffer, wrlen);
            ColNum = 0;              // 列地址归零
            pBuffer += wrlen;        // 写地址偏移
            NumByteToWrite -= wrlen; // 写入数据减少
        } else                       // 无数据写入,直接拷贝即可
        {
            temp = NAND_CopyPageWithoutWrite(source_block * nand_flash_dev.block_pagenum + i, unusedblock * nand_flash_dev.block_pagenum + i);
        }
        if (temp) // 返回值非零,当坏块处理
        {
            FTL_BadBlockMark(unusedblock); // 标记为坏块
            FTL_CreateLUT(1);              // 重建LUT表
            goto retry;
        }
    }
    if (i == nand_flash_dev.block_pagenum) // 拷贝完成
    {
        FTL_UsedBlockMark(unusedblock); // 标记块已经使用
        NAND_EraseBlock(source_block);  // 擦除源块
        // printf("\r\ncopy block %d to block %d\r\n",source_block,unusedblock);//打印调试信息
        for (i = 0; i < nand_flash_dev.block_totalnum; i++) // 修正LUT表，用unusedblock替换source_block
        {
            if (nand_flash_dev.lut[i] == source_block) {
                nand_flash_dev.lut[i] = unusedblock;
                break;
            }
        }
    }
    return 0; // 成功
}

/**
 * @brief   逻辑块号转换为物理块号
 * @param   LBNNum          逻辑块编号
 * @return  物理块编号
 */
uint16_t FTL_LBNToPBN(uint32_t LBNNum)
{
    uint16_t PBNNo = 0;
    // 当逻辑块号大于有效块数的时候返回0XFFFF
    if (LBNNum > nand_flash_dev.valid_blocknum) return 0XFFFF;
    PBNNo = nand_flash_dev.lut[LBNNum];
    return PBNNo;
}

/**
 * @brief   写扇区(支持多扇区写)，FATFS文件系统使用
 * @param   pBuffer     数据缓存区
 * @param   SectorNo    起始扇区号
 * @param   SectorSize  扇区大小
 * @param   SectorCount 要写入的扇区数量
 * @return 
 */
uint8_t FTL_WriteSectors(uint8_t *pBuffer, uint32_t SectorNo, uint16_t SectorSize, uint32_t SectorCount)
{
    uint8_t flag = 0;
    uint16_t temp;
    uint32_t i = 0;
    uint16_t wsecs;                 // 写页大小
    uint32_t wlen;                  // 写入长度
    uint32_t LBNNo;                 // 逻辑块号
    uint32_t PBNNo;                 // 物理块号
    uint32_t PhyPageNo;             // 物理页号
    uint32_t PageOffset;            // 页内偏移地址
    uint32_t BlockOffset;           // 块内偏移地址
    uint32_t markdpbn = 0XFFFFFFFF; // 标记了的物理块编号
    for (i = 0; i < SectorCount; i++) {
        LBNNo = (SectorNo + i) / (nand_flash_dev.block_pagenum * (nand_flash_dev.page_mainsize / SectorSize));                      // 根据逻辑扇区号和扇区大小计算出逻辑块号
        PBNNo = FTL_LBNToPBN(LBNNo);                                                                                                // 将逻辑块转换为物理块
        if (PBNNo >= nand_flash_dev.block_totalnum) return 1;                                                                       // 物理块号大于NAND FLASH的总块数,则失败.
        BlockOffset = ((SectorNo + i) % (nand_flash_dev.block_pagenum * (nand_flash_dev.page_mainsize / SectorSize))) * SectorSize; // 计算块内偏移
        PhyPageNo   = PBNNo * nand_flash_dev.block_pagenum + BlockOffset / nand_flash_dev.page_mainsize;                            // 计算出物理页号
        PageOffset  = BlockOffset % nand_flash_dev.page_mainsize;                                                                   // 计算出页内偏移地址
        temp        = nand_flash_dev.page_mainsize - PageOffset;                                                                    // page内剩余字节数
        temp /= SectorSize;                                                                                                         // 可以连续写入的sector数
        wsecs = SectorCount - i;                                                                                                    // 还剩多少个sector要写
        if (wsecs >= temp) wsecs = temp;                                                                                            // 大于可连续写入的sector数,则写入temp个扇区
        wlen = wsecs * SectorSize;                                                                                                  // 每次写wsecs个sector
        // 读出写入大小的内容判断是否全为0XFF
        flag = NAND_ReadPageComp(PhyPageNo, PageOffset, 0XFFFFFFFF, wlen / 4, &temp); // 读一个wlen/4大小个数据,并与0XFFFFFFFF对比
        if (flag) return 2;                                                           // 读写错误，坏块
        if (temp == (wlen / 4))
            flag = NAND_WritePage(PhyPageNo, PageOffset, pBuffer, wlen); // 全为0XFF,可以直接写数据
        else
            flag = 1;                         // 不全是0XFF,则另作处理
        if (flag == 0 && (markdpbn != PBNNo)) // 全是0XFF,且写入成功,且标记了的物理块与当前物理块不同
        {
            flag     = FTL_UsedBlockMark(PBNNo); // 标记此块已经使用
            markdpbn = PBNNo;                    // 标记完成,标记块=当前块,防止重复标记
        }
        if (flag) // 不全为0XFF/标记失败，将数据写到另一个块
        {
            temp  = ((uint32_t)nand_flash_dev.block_pagenum * nand_flash_dev.page_mainsize - BlockOffset) / SectorSize; // 计算整个block还剩下多少个SECTOR可以写入
            wsecs = SectorCount - i;                                                                                    // 还剩多少个sector要写
            if (wsecs >= temp) wsecs = temp;                                                                            // 大于可连续写入的sector数,则写入temp个扇区
            wlen = wsecs * SectorSize;                                                                                  // 每次写wsecs个sector
            flag = FTL_CopyAndWriteToBlock(PhyPageNo, PageOffset, pBuffer, wlen);                                       // 拷贝到另外一个block,并写入数据
            if (flag) return 3;                                                                                         // 失败
        }
        i += wsecs - 1;
        pBuffer += wlen; // 数据缓冲区指针偏移
    }
    return 0;
}

/**
 * @brief   读扇区(支持多扇区读)，FATFS文件系统使用
 * @param   pBuffer     数据缓存区
 * @param   SectorNo    起始扇区号
 * @param   SectorSize  扇区大小
 * @param   SectorCount 要写入的扇区数量
 * @return 
 */
uint8_t FTL_ReadSectors(uint8_t *pBuffer, uint32_t SectorNo, uint16_t SectorSize, uint32_t SectorCount)
{
    uint8_t flag = 0;
    uint16_t rsecs; // 单次读取页数
    uint32_t i = 0;
    uint32_t LBNNo;       // 逻辑块号
    uint32_t PBNNo;       // 物理块号
    uint32_t PhyPageNo;   // 物理页号
    uint32_t PageOffset;  // 页内偏移地址
    uint32_t BlockOffset; // 块内偏移地址
    for (i = 0; i < SectorCount; i++) {
        LBNNo = (SectorNo + i) / (nand_flash_dev.block_pagenum * (nand_flash_dev.page_mainsize / SectorSize));                      // 根据逻辑扇区号和扇区大小计算出逻辑块号
        PBNNo = FTL_LBNToPBN(LBNNo);                                                                                                // 将逻辑块转换为物理块
        if (PBNNo >= nand_flash_dev.block_totalnum) return 1;                                                                       // 物理块号大于NAND FLASH的总块数,则失败.
        BlockOffset = ((SectorNo + i) % (nand_flash_dev.block_pagenum * (nand_flash_dev.page_mainsize / SectorSize))) * SectorSize; // 计算块内偏移
        PhyPageNo   = PBNNo * nand_flash_dev.block_pagenum + BlockOffset / nand_flash_dev.page_mainsize;                            // 计算出物理页号
        PageOffset  = BlockOffset % nand_flash_dev.page_mainsize;                                                                   // 计算出页内偏移地址
        rsecs       = (nand_flash_dev.page_mainsize - PageOffset) / SectorSize;                                                     // 计算一次最多可以读取多少页
        if (rsecs > (SectorCount - i)) rsecs = SectorCount - i;                                                                     // 最多不能超过SectorCount-i
        flag = NAND_ReadPage(PhyPageNo, PageOffset, pBuffer, rsecs * SectorSize);                                                   // 读取数据
        if (flag == NSTA_ECC1BITERR)                                                                                                // 对于1bit ecc错误,可能为坏块
        {
            flag = NAND_ReadPage(PhyPageNo, PageOffset, pBuffer, rsecs * SectorSize); // 重读数据,再次确认
            if (flag == NSTA_ECC1BITERR) {
                FTL_CopyAndWriteToBlock(PhyPageNo, PageOffset, pBuffer, rsecs * SectorSize);   // 搬运数据
                flag = FTL_BlockCompare(PhyPageNo / nand_flash_dev.block_pagenum, 0XFFFFFFFF); // 全1检查,确认是否为坏块
                if (flag == 0) {
                    flag = FTL_BlockCompare(PhyPageNo / nand_flash_dev.block_pagenum, 0X00); // 全0检查,确认是否为坏块
                    NAND_EraseBlock(PhyPageNo / nand_flash_dev.block_pagenum);               // 检测完成后,擦除这个块
                }
                if (flag) // 全0/全1检查出错,肯定是坏块了.
                {
                    FTL_BadBlockMark(PhyPageNo / nand_flash_dev.block_pagenum); // 标记为坏块
                    FTL_CreateLUT(1);                                           // 重建LUT表
                }
                flag = 0;
            }
        }
        if (flag == NSTA_ECC2BITERR) flag = 0; // 2bit ecc错误,不处理(可能是初次写入数据导致的)
        if (flag) return 2;                    // 失败
        pBuffer += SectorSize * rsecs;         // 数据缓冲区指针偏移
        i += rsecs - 1;
    }
    return 0;
}

/**
 * @brief   FTL整个Block与某个数据对比
 * @param   mode        0,仅检查第一个坏块标记     1,两个坏块标记都要检查(备份区也要检查)
 * @return 
 */
uint8_t FTL_CreateLUT(uint8_t mode)
{
    uint32_t i;
    uint8_t buf[4];
    uint32_t LBNnum = 0;                                // 逻辑块号
    for (i = 0; i < nand_flash_dev.block_totalnum; i++) // 复位LUT表，初始化为无效值，也就是0XFFFF
    {
        nand_flash_dev.lut[i] = 0XFFFF;
    }
    nand_flash_dev.good_blocknum = 0;
    for (i = 0; i < nand_flash_dev.block_totalnum; i++) {
        NAND_ReadSpare(i * nand_flash_dev.block_pagenum, 0, buf, 4);                                 // 读取4个字节
        if (buf[0] == 0XFF && mode) NAND_ReadSpare(i * nand_flash_dev.block_pagenum + 1, 0, buf, 1); // 好块,且需要检查2次坏块标记
        if (buf[0] == 0XFF)                                                                          // 是好块
        {
            LBNnum = ((uint16_t)buf[3] << 8) + buf[2];  // 得到逻辑块编号
            if (LBNnum < nand_flash_dev.block_totalnum) // 逻辑块号肯定小于总的块数量
            {
                nand_flash_dev.lut[LBNnum] = i; // 更新LUT表，写LBNnum对应的物理块编号
            }
            nand_flash_dev.good_blocknum++;
        } else
            printf("bad block index:%d\r\n", i);
    }
    // LUT表建立完成以后检查有效块个数
    for (i = 0; i < nand_flash_dev.block_totalnum; i++) {
        if (nand_flash_dev.lut[i] >= nand_flash_dev.block_totalnum) {
            nand_flash_dev.valid_blocknum = i;
            break;
        }
    }
    if (nand_flash_dev.valid_blocknum < 100) return 2; // 有效块数小于100,有问题.需要重新格式化
    return 0;                                          // LUT表创建完成
}

/**
 * @brief   FTL整个Block与某个数据对比
 * @param   blockx      block编号
 * @param   cmpval      要与之对比的值
 * @return 
 */
uint8_t FTL_BlockCompare(uint32_t blockx, uint32_t cmpval)
{
    uint8_t res;
    uint16_t i, j, k;
    for (i = 0; i < 3; i++) // 允许3次机会
    {
        for (j = 0; j < nand_flash_dev.block_pagenum; j++) {
            NAND_ReadPageComp(blockx * nand_flash_dev.block_pagenum, 0, cmpval, nand_flash_dev.page_mainsize / 4, &k); // 检查一个page,并与0XFFFFFFFF对比
            if (k != (nand_flash_dev.page_mainsize / 4)) break;
        }
        if (j == nand_flash_dev.block_pagenum) return 0; // 检查合格,直接退出
        res = NAND_EraseBlock(blockx);
        if (res)
            printf("error erase block:%d\r\n", i);
        else {
            if (cmpval != 0XFFFFFFFF) // 不是判断全1,则需要重写数据
            {
                for (k = 0; k < nand_flash_dev.block_pagenum; k++) {
                    NAND_WritePageConst(blockx * nand_flash_dev.block_pagenum + k, 0, 0, nand_flash_dev.page_mainsize / 4); // 写PAGE
                }
            }
        }
    }
    printf("bad block checked:%d\r\n", blockx);
    return 1;
}

/**
 * @brief   FTL初始化时，搜寻所有坏块,使用:擦-写-读 方式
 * @return  好块的数量
 */
uint32_t FTL_SearchBadBlock(void)
{
    uint8_t *blktbl;
    uint8_t res;
    uint32_t i, j;
    uint32_t goodblock = 0;
    blktbl             = mymalloc(SRAMIN, nand_flash_dev.block_totalnum); // 申请block坏块表内存,对应项:0,好块;1,坏块;
    NAND_EraseChip();                                                     // 全片擦除
    for (i = 0; i < nand_flash_dev.block_totalnum; i++)                   // 第一阶段检查,检查全1
    {
        res = FTL_BlockCompare(i, 0XFFFFFFFF); // 全1检查
        if (res)
            blktbl[i] = 1; // 坏块
        else {
            blktbl[i] = 0;                                     // 好块
            for (j = 0; j < nand_flash_dev.block_pagenum; j++) // 写block为全0,为后面的检查准备
            {
                NAND_WritePageConst(i * nand_flash_dev.block_pagenum + j, 0, 0, nand_flash_dev.page_mainsize / 4);
            }
        }
    }
    for (i = 0; i < nand_flash_dev.block_totalnum; i++) // 第二阶段检查,检查全0
    {
        if (blktbl[i] == 0) // 在第一阶段,没有被标记坏块的,才可能是好块
        {
            res = FTL_BlockCompare(i, 0); // 全0检查
            if (res)
                blktbl[i] = 1; // 标记坏块
            else
                goodblock++;
        }
    }
    NAND_EraseChip();                                   // 全片擦除
    for (i = 0; i < nand_flash_dev.block_totalnum; i++) // 第三阶段检查,标记坏块
    {
        if (blktbl[i]) FTL_BadBlockMark(i); // 是坏块
    }
    return goodblock; // 返回好块的数量
}

/**
 * @brief   格式化NAND 重建LUT表
 */
uint8_t FTL_Format(void)
{
    uint8_t temp;
    uint32_t i, n;
    uint32_t goodblock           = 0;
    nand_flash_dev.good_blocknum = 0;
#if FTL_USE_BAD_BLOCK_SEARCH == 1                        // 使用擦-写-读的方式,检测坏块
    nand_flash_dev.good_blocknum = FTL_SearchBadBlock(); // 搜寻坏块.耗时很久
#else                                                    // 直接使用NAND FLASH的出厂坏块标志(其他块,默认是好块)
    for (i = 0; i < nand_flash_dev.block_totalnum; i++) {
        temp = FTL_CheckBadBlock(i); // 检查一个块是否为坏块
        if (temp == 0)               // 好块
        {
            temp = NAND_EraseBlock(i);
            if (temp) // 擦除失败,认为坏块
            {
                printf("Bad block:%d\r\n", i);
                FTL_BadBlockMark(i); // 标记是坏块
            } else
                nand_flash_dev.good_blocknum++; // 好块数量加一
        }
    }
#endif
    printf("good_blocknum:%d\r\n", nand_flash_dev.good_blocknum);
    if (nand_flash_dev.good_blocknum < 100) return 1;      // 如果好块的数量少于100，则NAND Flash报废
    goodblock = (nand_flash_dev.good_blocknum * 93) / 100; //%93的好块用于存储数据
    n         = 0;
    for (i = 0; i < nand_flash_dev.block_totalnum; i++) // 在好块中标记上逻辑块信息
    {
        temp = FTL_CheckBadBlock(i); // 检查一个块是否为坏块
        if (temp == 0)               // 好块
        {
            NAND_WriteSpare(i * nand_flash_dev.block_pagenum, 2, (uint8_t *)&n, 2); // 写入逻辑块编号
            n++;                                                                    // 逻辑块编号加1
            if (n == goodblock) break;                                              // 全部标记完了
        }
    }
    if (FTL_CreateLUT(1)) return 2; // 重建LUT表失败
    return 0;
}
