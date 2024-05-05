#include "nandflash.h"
#include "tim.h"

NANDFLASH_Attriute nand_flash_dev;
/**************************************
 * 驱动函数段
 */

/**
 * @brief 微秒延时函数
 */
static void delay_us(uint32_t us)
{
    uint32_t cnt = 0;
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    __HAL_TIM_SET_AUTORELOAD(&htim2, us);
    HAL_TIM_Base_Start(&htim2);
    while (cnt != us) {
        cnt = __HAL_TIM_GET_COUNTER(&htim2);
    }
    HAL_TIM_Base_Stop(&htim2);
}

/**
 * @brief   NAND 延时
 */
void NAND_Delay(volatile uint32_t i)
{
    while (i > 0) i--;
}

/**
 * @brief   读取 NAND FLASH 的 状态
 * @return  NAND状态值
 *          bit0:0,成功;1,错误(编程/擦除/READ)
 *          bit6:0,Busy;1,Ready
 */
static uint8_t NAND_ReadStatus(void)
{
    volatile uint8_t data                          = 0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_READSTA; // 发送读状态命令
    NAND_Delay(NAND_TWHR_DELAY);                                   // 等待tWHR,再读取状态寄存器
    data = *(volatile uint8_t *)NAND_ADDRESS;                      // 读取状态值
    return data;
}

/**
 * @brief   等待 NAND FLASH 准备
 * @return  NSTA_READY      准备完毕
 *          NSTA_TIMEOUT    获得最大值
 */
static uint8_t NAND_WaitForReady(void)
{
    uint8_t status         = 0;
    volatile uint32_t time = 0;
    while (1) // 等待ready
    {
        status = NAND_ReadStatus(); // 获取状态值
        if (status & NSTA_READY) break;
        time++;
        if (time >= 0X1FFFFFFF) return NSTA_TIMEOUT; // 超时
    }
    return NSTA_READY; // 准备好
}

/**
 * @brief   NAND FLASH 复位
 * @return
 */
static uint8_t NAND_Reset(void)
{
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_RESET; // 复位NAND
    if (NAND_WaitForReady() == NSTA_READY)
        return 0; // 复位成功
    else
        return 1; // 复位失败
}

/**
 * @brief   等待 RB 信号为某个电平
 * @param   rb  0,等待RB==0 1,等待RB==1
 */
static uint8_t NAND_WaitRB(volatile uint8_t rb)
{
    volatile uint32_t time = 0;
    while (time < 0X1FFFFFF) {
        time++;
        if (HAL_GPIO_ReadPin(NAND_RB_Port, NAND_RB_Pin) == rb) return 0;
    }
    return 1;
}

/**
 * @brief   设置 NAND FLASH 的模式
 */
static uint8_t NAND_ModeSet(uint8_t mode)
{
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD)  = NAND_FEATURE; // 发送设置特性命令
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = 0X01;         // 地址为0X01,设置mode
    *(volatile uint8_t *)NAND_ADDRESS               = mode;         // P1参数,设置mode
    *(volatile uint8_t *)NAND_ADDRESS               = 0;
    *(volatile uint8_t *)NAND_ADDRESS               = 0;
    *(volatile uint8_t *)NAND_ADDRESS               = 0;
    if (NAND_WaitForReady() == NSTA_READY)
        return 0; // 成功
    else
        return 1; // 失败
}

/**
 * @brief   读取 NAND FLASH 的 ID
 */
uint32_t NAND_ReadID(void)
{
    uint8_t deviceid[5];
    uint32_t id;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD)  = NAND_READID; // 发送读取ID命令
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = 0X00;
    // ID一共有5个字节
    deviceid[0] = *(volatile uint8_t *)NAND_ADDRESS;
    deviceid[1] = *(volatile uint8_t *)NAND_ADDRESS;
    deviceid[2] = *(volatile uint8_t *)NAND_ADDRESS;
    deviceid[3] = *(volatile uint8_t *)NAND_ADDRESS;
    deviceid[4] = *(volatile uint8_t *)NAND_ADDRESS;
    // 镁光的NAND FLASH的ID一共5个字节，但是为了方便我们只取4个字节组成一个32位的ID值
    // 根据NAND FLASH的数据手册，只要是镁光的NAND FLASH，那么一个字节ID的第一个字节都是0X2C
    // 所以我们就可以抛弃这个0X2C，只取后面四字节的ID值。
    id = ((uint32_t)deviceid[1]) << 24 | ((uint32_t)deviceid[2]) << 16 | ((uint32_t)deviceid[3]) << 8 | deviceid[4];
    return id;
}

/***********************************************
 * 用户函数段
 */

/**
 * @brief   NAND FLASH 初始化
 */
uint8_t NAND_Init(void)
{
    MX_FMC_Init();
    NAND_Reset(); // 复位NAND
    HAL_Delay(100);
    nand_flash_dev.id = NAND_ReadID();        // 读取ID
    NAND_ModeSet(4);                          // 设置为MODE4,高速模式
    if (nand_flash_dev.id == MT29F16G08ABABA) // NAND为MT29F16G08ABABA
    {
        nand_flash_dev.page_totalsize = 4320;       // nand一个page的总大小（包括spare区）
        nand_flash_dev.page_mainsize  = 4096;       // nand一个page的有效数据区大小
        nand_flash_dev.page_sparesize = 224;        // nand一个page的spare区大小
        nand_flash_dev.block_pagenum  = 128;        // nand一个block所包含的page数目
        nand_flash_dev.plane_blocknum = 2048;       // nand一个plane所包含的block数目
        nand_flash_dev.block_totalnum = 4096;       // nand的总block数目
    } else if (nand_flash_dev.id == MT29F4G08ABADA) // NAND为MT29F4G08ABADA
    {
        nand_flash_dev.page_totalsize = 2112; // nand一个page的总大小（包括spare区）
        nand_flash_dev.page_mainsize  = 2048; // nand一个page的有效数据区大小
        nand_flash_dev.page_sparesize = 64;   // nand一个page的spare区大小
        nand_flash_dev.block_pagenum  = 64;   // nand一个block所包含的page数目
        nand_flash_dev.plane_blocknum = 2048; // nand一个plane所包含的block数目
        nand_flash_dev.block_totalnum = 4096; // nand的总block数目
    } else
        return 1; // 错误，返回
    return 0;
}

/**
 * @brief   读取NAND Flash的指定页指定列的数据
 * @param   PageNum         要读取的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param   ColNum          要读取的列开始地址(也就是页内地址),范围:0~(page_totalsize-1)
 * @param   pBuffer         指向数据存储区
 * @param   NumByteToRead   读取字节数(不能跨页读)
 */
uint8_t NAND_ReadPage(uint32_t PageNum, uint16_t ColNum, uint8_t *pBuffer, uint16_t NumByteToRead)
{
    volatile uint16_t i = 0;
    uint8_t res         = 0;
    uint8_t eccnum      = 0; // 需要计算的ECC个数，每NAND_ECC_SECTOR_SIZE字节计算一个ecc
    uint8_t eccstart    = 0; // 第一个ECC值所属的地址范围
    uint8_t errsta      = 0;
    uint8_t *p;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_AREA_A;
    // 发送地址
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)ColNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(ColNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)PageNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(PageNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(PageNum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD)  = NAND_AREA_TRUE1;
    // 下面两行代码是等待R/B引脚变为低电平，其实主要起延时作用的，等待NAND操作R/B引脚。因为我们是通过
    // 将STM32的NWAIT引脚(NAND的R/B引脚)配置为普通IO，代码中通过读取NWAIT引脚的电平来判断NAND是否准备
    // 就绪的。这个也就是模拟的方法，所以在速度很快的时候有可能NAND还没来得及操作R/B引脚来表示NAND的忙
    // 闲状态，结果我们就读取了R/B引脚,这个时候肯定会出错的，事实上确实是会出错!大家也可以将下面两行
    // 代码换成延时函数,只不过这里我们为了效率所以没有用延时函数。
    res = NAND_WaitRB(0);         // 等待RB=0
    if (res) return NSTA_TIMEOUT; // 超时退出
    // 下面2行代码是真正判断NAND是否准备好的
    res = NAND_WaitRB(1);                     // 等待RB=1
    if (res) return NSTA_TIMEOUT;             // 超时退出
    if (NumByteToRead % NAND_ECC_SECTOR_SIZE) // 不是NAND_ECC_SECTOR_SIZE的整数倍，不进行ECC校验
    {
        // 读取NAND FLASH中的值
        for (i = 0; i < NumByteToRead; i++) {
            *(volatile uint8_t *)pBuffer++ = *(volatile uint8_t *)NAND_ADDRESS;
        }
    } else {
        eccnum   = NumByteToRead / NAND_ECC_SECTOR_SIZE; // 得到ecc计算次数
        eccstart = ColNum / NAND_ECC_SECTOR_SIZE;
        p        = pBuffer;
        for (res = 0; res < eccnum; res++) {
            FMC_Bank2_3->PCR3 |= 1 << 6;               // 使能ECC校验
            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++) // 读取NAND_ECC_SECTOR_SIZE个数据
            {
                *(volatile uint8_t *)pBuffer++ = *(volatile uint8_t *)NAND_ADDRESS;
            }
            while (!(FMC_Bank2_3->SR3 & (1 << 6)))
                ;                                                          // 等待FIFO空
            nand_flash_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3->ECCR3; // 读取硬件计算后的ECC值
            FMC_Bank2_3->PCR3 &= ~(1 << 6);                                // 禁止ECC校验
        }
        i = nand_flash_dev.page_mainsize + 0X10 + eccstart * 4; // 从spare区的0X10位置开始读取之前存储的ecc值
        NAND_Delay(NAND_TRHW_DELAY);                            // 等待tRHW
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = 0X05;  // 随机读指令
        // 发送地址
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)i;
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(i >> 8);
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD)  = 0XE0; // 开始读数据
        NAND_Delay(NAND_TWHR_DELAY);                            // 等待tWHR
        pBuffer = (uint8_t *)&nand_flash_dev.ecc_rdbuf[eccstart];
        for (i = 0; i < 4 * eccnum; i++) // 读取保存的ECC值
        {
            *(volatile uint8_t *)pBuffer++ = *(volatile uint8_t *)NAND_ADDRESS;
        }
        for (i = 0; i < eccnum; i++) // 检验ECC
        {
            if (nand_flash_dev.ecc_rdbuf[i + eccstart] != nand_flash_dev.ecc_hdbuf[i + eccstart]) // 不相等,需要校正
            {
                printf("err hd,rd:0x%x,0x%x\r\n", nand_flash_dev.ecc_hdbuf[i + eccstart], nand_flash_dev.ecc_rdbuf[i + eccstart]);
                printf("eccnum,eccstart:%d,%d\r\n", eccnum, eccstart);
                printf("PageNum,ColNum:%d,%d\r\n", PageNum, ColNum);
                res = NAND_ECC_Correction(p + NAND_ECC_SECTOR_SIZE * i, nand_flash_dev.ecc_rdbuf[i + eccstart], nand_flash_dev.ecc_hdbuf[i + eccstart]); // ECC校验
                if (res)
                    errsta = NSTA_ECC2BITERR; // 标记2BIT及以上ECC错误
                else
                    errsta = NSTA_ECC1BITERR; // 标记1BIT ECC错误
            }
        }
    }
    if (NAND_WaitForReady() != NSTA_READY) errsta = NSTA_ERROR; // 失败
    return errsta;                                              // 成功
}

/**
 * @brief   读取NAND Flash的指定页指定列的数据(main区和spare区都可以使用此函数),并对比(FTL管理时需要)
 * @param   PageNum         要读取的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param   ColNum          要读取的列开始地址(也就是页内地址),范围:0~(page_totalsize-1)
 * @param   CmpVal          要对比的值,以uint32_t为单位
 * @param   NumByteToRead   读取字数(以4字节为单位,不能跨页读)
 * @param   NumByteEqual    从初始位置持续与CmpVal值相同的数据个数
 */
uint8_t NAND_ReadPageComp(uint32_t PageNum, uint16_t ColNum, uint32_t CmpVal, uint16_t NumByteToRead, uint16_t *NumByteEqual)
{
    uint16_t i                                     = 0;
    uint8_t res                                    = 0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_AREA_A;
    // 发送地址
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)ColNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(ColNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)PageNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(PageNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(PageNum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD)  = NAND_AREA_TRUE1;
    // 下面两行代码是等待R/B引脚变为低电平，其实主要起延时作用的，等待NAND操作R/B引脚。因为我们是通过
    // 将STM32的NWAIT引脚(NAND的R/B引脚)配置为普通IO，代码中通过读取NWAIT引脚的电平来判断NAND是否准备
    // 就绪的。这个也就是模拟的方法，所以在速度很快的时候有可能NAND还没来得及操作R/B引脚来表示NAND的忙
    // 闲状态，结果我们就读取了R/B引脚,这个时候肯定会出错的，事实上确实是会出错!大家也可以将下面两行
    // 代码换成延时函数,只不过这里我们为了效率所以没有用延时函数。
    res = NAND_WaitRB(0);         // 等待RB=0
    if (res) return NSTA_TIMEOUT; // 超时退出
    // 下面2行代码是真正判断NAND是否准备好的
    res = NAND_WaitRB(1);               // 等待RB=1
    if (res) return NSTA_TIMEOUT;       // 超时退出
    for (i = 0; i < NumByteToRead; i++) // 读取数据,每次读4字节
    {
        if (*(volatile uint32_t *)NAND_ADDRESS != CmpVal) break; // 如果有任何一个值,与CmpVal不相等,则退出.
    }
    *NumByteEqual = i;                                        // 与CmpVal值相同的个数
    if (NAND_WaitForReady() != NSTA_READY) return NSTA_ERROR; // 失败
    return 0;                                                 // 成功
}

/**
 * @brief   在NAND一页中写入指定个字节的数据(main区和spare区都可以使用此函数)
 * @param   PageNum         要写入的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param   ColNum          要写入的列开始地址(也就是页内地址),范围:0~(page_totalsize-1)
 * @param   pBbuffer        指向数据存储区
 * @param   NumByteToWrite  要写入的字节数，该值不能超过该页剩余字节数
 */
uint8_t NAND_WritePage(uint32_t PageNum, uint16_t ColNum, uint8_t *pBuffer, uint16_t NumByteToWrite)
{
    volatile uint16_t i = 0;
    uint8_t res         = 0;
    uint8_t eccnum      = 0; // 需要计算的ECC个数，每NAND_ECC_SECTOR_SIZE字节计算一个ecc
    uint8_t eccstart    = 0; // 第一个ECC值所属的地址范围

    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE0;
    // 发送地址
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)ColNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(ColNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)PageNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(PageNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(PageNum >> 16);
    NAND_Delay(NAND_TADL_DELAY);               // 等待tADL
    if (NumByteToWrite % NAND_ECC_SECTOR_SIZE) // 不是NAND_ECC_SECTOR_SIZE的整数倍，不进行ECC校验
    {
        for (i = 0; i < NumByteToWrite; i++) // 写入数据
        {
            *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pBuffer++;
        }
    } else {
        eccnum   = NumByteToWrite / NAND_ECC_SECTOR_SIZE; // 得到ecc计算次数
        eccstart = ColNum / NAND_ECC_SECTOR_SIZE;
        for (res = 0; res < eccnum; res++) {
            FMC_Bank2_3->PCR3 |= 1 << 6;               // 使能ECC校验
            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++) // 写入NAND_ECC_SECTOR_SIZE个数据
            {
                *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pBuffer++;
            }
            while (!(FMC_Bank2_3->SR3 & (1 << 6)))
                ;                                                          // 等待FIFO空
            nand_flash_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3->ECCR3; // 读取硬件计算后的ECC值
            FMC_Bank2_3->PCR3 &= ~(1 << 6);                                // 禁止ECC校验
        }
        i = nand_flash_dev.page_mainsize + 0X10 + eccstart * 4; // 计算写入ECC的spare区地址
        NAND_Delay(NAND_TADL_DELAY);                            // 等待tADL
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = 0X85;  // 随机写指令
        // 发送地址
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)i;
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(i >> 8);
        NAND_Delay(NAND_TADL_DELAY); // 等待tADL
        pBuffer = (uint8_t *)&nand_flash_dev.ecc_hdbuf[eccstart];
        for (i = 0; i < eccnum; i++) // 写入ECC
        {
            for (res = 0; res < 4; res++) {
                *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pBuffer++;
            }
        }
    }
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE_TURE1;
    delay_us(NAND_TPROG_DELAY);                               // 等待tPROG
    if (NAND_WaitForReady() != NSTA_READY) return NSTA_ERROR; // 失败
    return 0;                                                 // 成功
}

/**
 * @brief   在NAND一页中的指定地址开始,写入指定长度的恒定数字
 * @param   PageNum         要写入的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param   ColNum          要写入的列开始地址(也就是页内地址),范围:0~(page_totalsize-1)
 * @param   cval            要写入的指定常数
 * @param   NumByteToWrite  要写入的字节数，该值不能超过该页剩余字节数
 */
uint8_t NAND_WritePageConst(uint32_t PageNum, uint16_t ColNum, uint32_t cval, uint16_t NumByteToWrite)
{
    uint16_t i                                     = 0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE0;
    // 发送地址
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)ColNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(ColNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)PageNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(PageNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(PageNum >> 16);
    NAND_Delay(NAND_TADL_DELAY);         // 等待tADL
    for (i = 0; i < NumByteToWrite; i++) // 写入数据,每次写4字节
    {
        *(volatile uint32_t *)NAND_ADDRESS = cval;
    }
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE_TURE1;
    delay_us(NAND_TPROG_DELAY);                               // 等待tPROG
    if (NAND_WaitForReady() != NSTA_READY) return NSTA_ERROR; // 失败
    return 0;                                                 // 成功
}

/**
 * @brief   将一页数据拷贝到另一页,不写入新数据
 * @attention               源页和目的页要在同一个Plane内
 * @param   Source_PageNo   源页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param   Dest_PageNo     目的页地址,范围:0~(block_pagenum*block_totalnum-1)
 */
uint8_t NAND_CopyPageWithoutWrite(uint32_t Source_PageNum, uint32_t Dest_PageNum)
{
    uint8_t res           = 0;
    uint16_t source_block = 0, dest_block = 0;
    // 判断源页和目的页是否在同一个plane中
    source_block = Source_PageNum / nand_flash_dev.block_pagenum;
    dest_block   = Dest_PageNum / nand_flash_dev.block_pagenum;
    if ((source_block % 2) != (dest_block % 2)) return NSTA_ERROR;       // 不在同一个plane内
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD0; // 发送命令0X00
    // 发送源页地址
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)Source_PageNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(Source_PageNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(Source_PageNum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD)  = NAND_MOVEDATA_CMD1; // 发送命令0X35
    // 下面两行代码是等待R/B引脚变为低电平，其实主要起延时作用的，等待NAND操作R/B引脚。因为我们是通过
    // 将STM32的NWAIT引脚(NAND的R/B引脚)配置为普通IO，代码中通过读取NWAIT引脚的电平来判断NAND是否准备
    // 就绪的。这个也就是模拟的方法，所以在速度很快的时候有可能NAND还没来得及操作R/B引脚来表示NAND的忙
    // 闲状态，结果我们就读取了R/B引脚,这个时候肯定会出错的，事实上确实是会出错!大家也可以将下面两行
    // 代码换成延时函数,只不过这里我们为了效率所以没有用延时函数。
    res = NAND_WaitRB(0);         // 等待RB=0
    if (res) return NSTA_TIMEOUT; // 超时退出
    // 下面2行代码是真正判断NAND是否准备好的
    res = NAND_WaitRB(1);                                                // 等待RB=1
    if (res) return NSTA_TIMEOUT;                                        // 超时退出
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD2; // 发送命令0X85
    // 发送目的页地址
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)Dest_PageNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(Dest_PageNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(Dest_PageNum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD)  = NAND_MOVEDATA_CMD3; // 发送命令0X10
    delay_us(NAND_TPROG_DELAY);                                           // 等待tPROG
    if (NAND_WaitForReady() != NSTA_READY) return NSTA_ERROR;             // NAND未准备好
    return 0;                                                             // 成功
}

/**
 * @brief   将一页数据拷贝到另一页,并且可以写入数据
 * @attention               源页和目的页要在同一个Plane内
 * @param   Source_PageNo   源页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param   Dest_PageNo     目的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param   ColNo           页内列地址,范围:0~(page_totalsize-1)
 * @param   pBuffer         要写入的数据
 * @param   NumByteToWrite  要写入的数据个数
 */
uint8_t NAND_CopyPageWithWrite(uint32_t Source_PageNum, uint32_t Dest_PageNum, uint16_t ColNum, uint8_t *pBuffer, uint16_t NumByteToWrite)
{
    uint8_t res           = 0;
    volatile uint16_t i   = 0;
    uint16_t source_block = 0, dest_block = 0;
    uint8_t eccnum   = 0; // 需要计算的ECC个数，每NAND_ECC_SECTOR_SIZE字节计算一个ecc
    uint8_t eccstart = 0; // 第一个ECC值所属的地址范围
    // 判断源页和目的页是否在同一个plane中
    source_block = Source_PageNum / nand_flash_dev.block_pagenum;
    dest_block   = Dest_PageNum / nand_flash_dev.block_pagenum;
    if ((source_block % 2) != (dest_block % 2)) return NSTA_ERROR;       // 不在同一个plane内
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD0; // 发送命令0X00
    // 发送源页地址
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)Source_PageNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(Source_PageNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(Source_PageNum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD)  = NAND_MOVEDATA_CMD1; // 发送命令0X35

    // 下面两行代码是等待R/B引脚变为低电平，其实主要起延时作用的，等待NAND操作R/B引脚。因为我们是通过
    // 将STM32的NWAIT引脚(NAND的R/B引脚)配置为普通IO，代码中通过读取NWAIT引脚的电平来判断NAND是否准备
    // 就绪的。这个也就是模拟的方法，所以在速度很快的时候有可能NAND还没来得及操作R/B引脚来表示NAND的忙
    // 闲状态，结果我们就读取了R/B引脚,这个时候肯定会出错的，事实上确实是会出错!大家也可以将下面两行
    // 代码换成延时函数,只不过这里我们为了效率所以没有用延时函数。
    res = NAND_WaitRB(0);         // 等待RB=0
    if (res) return NSTA_TIMEOUT; // 超时退出
    // 下面2行代码是真正判断NAND是否准备好的
    res = NAND_WaitRB(1);                                                // 等待RB=1
    if (res) return NSTA_TIMEOUT;                                        // 超时退出
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD2; // 发送命令0X85
    // 发送目的页地址
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)ColNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(ColNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)Dest_PageNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(Dest_PageNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(Dest_PageNum >> 16);
    // 发送页内列地址
    NAND_Delay(NAND_TADL_DELAY);               // 等待tADL
    if (NumByteToWrite % NAND_ECC_SECTOR_SIZE) // 不是NAND_ECC_SECTOR_SIZE的整数倍，不进行ECC校验
    {
        for (i = 0; i < NumByteToWrite; i++) // 写入数据
        {
            *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pBuffer++;
        }
    } else {
        eccnum   = NumByteToWrite / NAND_ECC_SECTOR_SIZE; // 得到ecc计算次数
        eccstart = ColNum / NAND_ECC_SECTOR_SIZE;
        for (res = 0; res < eccnum; res++) {
            FMC_Bank2_3->PCR3 |= 1 << 6;               // 使能ECC校验
            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++) // 写入NAND_ECC_SECTOR_SIZE个数据
            {
                *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pBuffer++;
            }
            while (!(FMC_Bank2_3->SR3 & (1 << 6)))
                ;                                                          // 等待FIFO空
            nand_flash_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3->ECCR3; // 读取硬件计算后的ECC值
            FMC_Bank2_3->PCR3 &= ~(1 << 6);                                // 禁止ECC校验
        }
        i = nand_flash_dev.page_mainsize + 0X10 + eccstart * 4; // 计算写入ECC的spare区地址
        NAND_Delay(NAND_TADL_DELAY);                            // 等待tADL
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = 0X85;  // 随机写指令
        // 发送地址
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)i;
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(i >> 8);
        NAND_Delay(NAND_TADL_DELAY); // 等待tADL
        pBuffer = (uint8_t *)&nand_flash_dev.ecc_hdbuf[eccstart];
        for (i = 0; i < eccnum; i++) // 写入ECC
        {
            for (res = 0; res < 4; res++) {
                *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pBuffer++;
            }
        }
    }
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD3; // 发送命令0X10
    delay_us(NAND_TPROG_DELAY);                                          // 等待tPROG
    if (NAND_WaitForReady() != NSTA_READY) return NSTA_ERROR;            // 失败
    return 0;                                                            // 成功
}

/**
 * @brief   读取spare区中的数据
 * @param   PageNum         要写入的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param   ColNum          要写入的spare区地址(spare区中哪个地址),范围:0~(page_sparesize-1)
 * @param   pBuffer         接收数据缓冲区
 * @param   NumByteToRead   要读取的字节数(不大于page_sparesize)
 */
uint8_t NAND_ReadSpare(uint32_t PageNum, uint16_t ColNum, uint8_t *pBuffer, uint16_t NumByteToRead)
{
    uint8_t temp       = 0;
    uint8_t remainbyte = 0;
    remainbyte         = nand_flash_dev.page_sparesize - ColNum;
    if (NumByteToRead > remainbyte) NumByteToRead = remainbyte;                                   // 确保要写入的字节数不大于spare剩余的大小
    temp = NAND_ReadPage(PageNum, ColNum + nand_flash_dev.page_mainsize, pBuffer, NumByteToRead); // 读取数据
    return temp;
}

/**
 * @brief   向spare区中写数据
 * @param   PageNum         要写入的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param   ColNum          要写入的spare区地址(spare区中哪个地址),范围:0~(page_sparesize-1)
 * @param   pBuffer         要写入的数据首地址
 * @param   NumByteToWrite  要写入的字节数(不大于page_sparesize)
 */
uint8_t NAND_WriteSpare(uint32_t PageNum, uint16_t ColNum, uint8_t *pBuffer, uint16_t NumByteToWrite)
{
    uint8_t temp       = 0;
    uint8_t remainbyte = 0;
    remainbyte         = nand_flash_dev.page_sparesize - ColNum;
    if (NumByteToWrite > remainbyte) NumByteToWrite = remainbyte;                                   // 确保要读取的字节数不大于spare剩余的大小
    temp = NAND_WritePage(PageNum, ColNum + nand_flash_dev.page_mainsize, pBuffer, NumByteToWrite); // 读取
    return temp;
}

/**
 * @brief   擦除一个块
 * @param   BlockNum        要擦除的BLOCK编号,范围:0-(block_totalnum-1)
 */
uint8_t NAND_EraseBlock(uint32_t BlockNum)
{
    if (nand_flash_dev.id == MT29F16G08ABABA)
        BlockNum <<= 7; // 将块地址转换为页地址
    else if (nand_flash_dev.id == MT29F4G08ABADA)
        BlockNum <<= 6;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_ERASE0;
    // 发送块地址
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)BlockNum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(BlockNum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(BlockNum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD)  = NAND_ERASE1;
    HAL_Delay(NAND_TBERS_DELAY);                              // 等待擦除成功
    if (NAND_WaitForReady() != NSTA_READY) return NSTA_ERROR; // 失败
    return 0;                                                 // 成功
}

/**
 * @brief   擦除 NAND FLASH
 */
void NAND_EraseChip(void)
{
    uint8_t status;
    uint16_t i = 0;
    for (i = 0; i < nand_flash_dev.block_totalnum; i++) // 循环擦除所有的块
    {
        status = NAND_EraseBlock(i);
        if (status) printf("Erase %d block fail!!，错误码为%d\r\n", i, status); // 擦除失败
    }
}

/**
 * @brief   获取ECC的奇数位/偶数位
 * @param   oe      0,偶数位        1,奇数位
 * @param   eccval  输入的ecc值
 * @return  计算后的ecc值(最多16位)
 */
uint16_t NAND_ECC_Get_OE(uint8_t oe, uint32_t eccval)
{
    uint8_t i;
    uint16_t ecctemp = 0;
    for (i = 0; i < 24; i++) {
        if ((i % 2) == oe) {
            if ((eccval >> i) & 0X01) ecctemp += 1 << (i >> 1);
        }
    }
    return ecctemp;
}

/**
 * @brief   ECC校正函数
 * @param   eccrd   读取出来,原来保存的ECC值
 * @param   ecccl   读取数据时,硬件计算的ECC只
 */
uint8_t NAND_ECC_Correction(uint8_t *data_buf, uint32_t eccrd, uint32_t ecccl)
{
    uint16_t eccrdo, eccrde, eccclo, ecccle;
    uint16_t eccchk   = 0;
    uint16_t errorpos = 0;
    uint32_t bytepos  = 0;
    eccrdo            = NAND_ECC_Get_OE(1, eccrd); // 获取eccrd的奇数位
    eccrde            = NAND_ECC_Get_OE(0, eccrd); // 获取eccrd的偶数位
    eccclo            = NAND_ECC_Get_OE(1, ecccl); // 获取ecccl的奇数位
    ecccle            = NAND_ECC_Get_OE(0, ecccl); // 获取ecccl的偶数位
    eccchk            = eccrdo ^ eccrde ^ eccclo ^ ecccle;
    if (eccchk == 0XFFF) // 全1,说明只有1bit ECC错误
    {
        errorpos = eccrdo ^ eccclo;
        printf("errorpos:%d\r\n", errorpos);
        bytepos = errorpos / 8;
        data_buf[bytepos] ^= 1 << (errorpos % 8);
    } else // 不是全1,说明至少有2bit ECC错误,无法修复
    {
        printf("2bit ecc error or more\r\n");
        return 1;
    }
    return 0;
}