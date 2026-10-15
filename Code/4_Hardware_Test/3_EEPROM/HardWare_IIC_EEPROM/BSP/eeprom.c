#include "eeprom.h"

/****************************************************/
/* 如果使能FreeRTOS */
#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "cmsis_os.h"
#endif

/* 如果使能软件IIC */
#ifdef EEPROM_IIC_SoftWare

#endif

/* 如果使能硬件IIC */
#ifdef EEPROM_IIC_HardWare
#include "i2c.h"
#endif
/****************************************************/
/* 如果使能硬件IIC */
#ifdef EEPROM_IIC_HardWare
/* IIC 基础驱动函数 */
/**
 * @brief IIC写字节函数
 * @param addr EEPROM的写入地址
 * @param send_buf  发送缓冲区
 * @param size      发送大小
 */
static HAL_StatusTypeDef EEPROM_IIC_MemWrite(uint8_t addr, uint8_t *send_buf, uint16_t size)
{
    return HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDR_Write, addr, I2C_MEMADD_SIZE_8BIT, send_buf, size, 0xFFFF);
}

/**
 * @brief IIC读字节函数
 * @param addr EEPROM的读取地址
 * @param send_buf  接收缓冲区
 * @param size      读取数据大小
 */
static HAL_StatusTypeDef EEPROM_IIC_MemRead(uint8_t addr, uint8_t *rev_buf, uint16_t size)
{
    return HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDR_Read, addr, I2C_MEMADD_SIZE_8BIT, rev_buf, size, 0xFFFF);
}
#endif
/****************************************************/
/**
 * @brief EEPROM 写入函数
 * @param addr EEPROM的读取地址
 * @param send_buf  接收缓冲区
 * @param size      读取数据大小
 */
void EEPROM_Read(uint8_t addr, uint8_t *rev_buf, uint16_t size)
{
    if (EEPROM_IIC_MemRead(addr, rev_buf, size) == HAL_OK) {
        ;
    }
}

/**
 * @brief EEPROM 写入函数
 * @param addr EEPROM的读取地址
 * @param send_buf  接收缓冲区
 * @param size      读取数据大小
 */
void EEPROM_Write(uint8_t addr, uint8_t *send_buf, uint16_t size)
{
    if (EEPROM_IIC_MemWrite(addr, send_buf, size) == HAL_OK) {
        ;
    }
}