#include "user_flash.h"

static uint16_t FlashBuffer[STM32FLASH_PAGE_SIZE >> 1];

/**
    @brief 内部Flash读取
    @param address -[in] 读取的地址
    @param pData -[out] 指向需要操作的数据
    @param dataLen -[in] 数据长度
    @return 读出成功的字节数
*/
uint32_t Internal_ReadFlash(uint32_t addrStart, void *pData, uint32_t dataLen)
{
    uint32_t nread       = dataLen;
    uint8_t *pBuffer     = (uint8_t *)pData;
    const uint8_t *pAddr = (const uint8_t *)addrStart;

    if (!pData || addrStart < STM32FLASH_BASE || addrStart > STM32FLASH_END) {
        return 0;
    }

    while (nread >= sizeof(uint32_t) && (((uint32_t)pAddr) <= (STM32FLASH_END - 4))) {
        *(uint32_t *)pBuffer = *(uint32_t *)pAddr;
        pBuffer += sizeof(uint32_t);
        pAddr += sizeof(uint32_t);
        nread -= sizeof(uint32_t);
    }

    while (nread && (((uint32_t)pAddr) < STM32FLASH_END)) {
        *pBuffer++ = *pAddr++;
        nread--;
    }

    return dataLen - nread;
}

/**
    @brief 内部Flash无检查写入
    @param address -[in] 写入的地址
    @param pData -[in] 指向需要操作的数据
    @param dataLen -[in] 数据长度
    @return 实际写入的数据量，单位：字节
*/
uint32_t Internal_WriteFlashNoCheck(uint32_t addrStart, const uint16_t *pData, uint32_t dataLen)
{
    uint32_t nwrite  = dataLen;
    uint32_t addrmax = STM32FLASH_END - 2;

    while (nwrite) {
        if (addrStart > addrmax) {
            break;
        }

        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addrStart, *pData);
        if ((*(__IO uint16_t *)addrStart) != *pData) {
            break;
        }

        nwrite--;
        pData++;
        addrStart += 2;
    }
    return (dataLen - nwrite);
}

/**
    @brief 内部Flash写入
    @param address -[in] 写入的地址
    @param pData -[in] 指向需要操作的数据
    @param dataLen -[in] 数据长度
    @return 实际写入的数据量，单位：字节
*/
uint32_t Internal_WriteFlash(uint32_t addrStart, const uint16_t *pData, uint32_t dataLen)
{
    uint32_t i              = 0;
    uint32_t pagepos        = 0;       // 页位置
    uint32_t pageoff        = 0;       // 页内偏移地址
    uint32_t pagefre        = 0;       // 页内空余空间
    uint32_t offset         = 0;       // Address在FLASH中的偏移
    uint32_t nwrite         = dataLen; // 记录剩余要写入的数据量
    const uint16_t *pBuffer = (const uint16_t *)pData;

    /* 非法地址 */
    if (addrStart < STM32FLASH_BASE || addrStart > (STM32FLASH_END - 2) || dataLen == 0 || pData == NULL) {
        return 0;
    }

    /* 解锁FLASH */
    HAL_FLASH_Unlock();

    /* 计算偏移地址 */
    offset = addrStart - STM32FLASH_BASE;
    /* 计算当前页位置 */
    pagepos = offset / STM32FLASH_PAGE_SIZE;
    /* 计算要写数据的起始地址在当前页内的偏移地址 */
    pageoff = ((offset % STM32FLASH_PAGE_SIZE) >> 1);
    /* 计算当前页内空余空间 */
    pagefre = ((STM32FLASH_PAGE_SIZE >> 1) - pageoff);
    /* 要写入的数据量低于当前页空余量 */
    if (nwrite <= pagefre) {
        pagefre = nwrite;
    }

    while (nwrite != 0) {
        /* 检查是否超页 */
        if (pagepos >= STM32FLASH_PAGE_NUM) {
            break;
        }

        /* 读取一页 */
        Internal_ReadFlash(STM32FLASH_BASE + pagepos * STM32FLASH_PAGE_SIZE, FlashBuffer, STM32FLASH_PAGE_SIZE);

        /* 检查是否需要擦除 */
        for (i = 0; i < pagefre; i++) {
            if (*(FlashBuffer + pageoff + i) != 0xFFFF) /* FLASH擦出后默认内容全为0xFF */
            {
                break;
            }
        }

        if (i < pagefre) {
            uint32_t count     = 0;
            uint32_t index     = 0;
            uint32_t PageError = 0;
            FLASH_EraseInitTypeDef pEraseInit;

            /* 擦除一页 */
            pEraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
            pEraseInit.PageAddress = STM32FLASH_BASE + pagepos * STM32FLASH_PAGE_SIZE;
            pEraseInit.Banks       = FLASH_BANK_1;
            pEraseInit.NbPages     = 1;
            if (HAL_FLASHEx_Erase(&pEraseInit, &PageError) != HAL_OK) {
                break;
            }

            /* 复制到缓存 */
            for (index = 0; index < pagefre; index++) {
                *(FlashBuffer + pageoff + index) = *(pBuffer + index);
            }

            /* 写回FLASH */
            count = Internal_WriteFlashNoCheck(STM32FLASH_BASE + pagepos * STM32FLASH_PAGE_SIZE, FlashBuffer, STM32FLASH_PAGE_SIZE >> 1);
            if (count != (STM32FLASH_PAGE_SIZE >> 1)) {
                nwrite -= count;
                break;
            }
        } else {
            /* 无需擦除，直接写 */
            uint32_t count = Internal_WriteFlashNoCheck(addrStart, pBuffer, pagefre);
            if (count != pagefre) {
                nwrite -= count;
                break;
            }
        }

        pBuffer += pagefre;          /* 读取地址递增         */
        addrStart += (pagefre << 1); /* 写入地址递增         */
        nwrite -= pagefre;           /* 更新剩余未写入数据量 */

        pagepos++;   /* 下一页           */
        pageoff = 0; /* 页内偏移地址置零  */

        /* 根据剩余量计算下次写入数据量 */
        pagefre = nwrite >= (STM32FLASH_PAGE_SIZE >> 1) ? (STM32FLASH_PAGE_SIZE >> 1) : nwrite;
    }

    /* 加锁FLASH */
    HAL_FLASH_Lock();

    return ((dataLen - nwrite) << 1);
}

/**
    @brief 内部Flash页擦除
    @param pageAddress -[in] 擦除的起始地址
    @param nbPages -[in] 擦除页数
    @return 0 - 成功；-1 - 失败
*/
int Internal_ErasePage(uint32_t pageAddress, uint32_t nbPages)
{
    uint32_t pageError = 0;
    FLASH_EraseInitTypeDef eraseInit;
    eraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = pageAddress;
    eraseInit.Banks       = FLASH_BANK_1;
    eraseInit.NbPages     = 1;
    if (HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK) {
        return -1;
    }
    return 0;
}