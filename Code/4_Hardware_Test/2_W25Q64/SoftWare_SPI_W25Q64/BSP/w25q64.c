#include "w25q64.h"

/****************************************************/
/* 如果使能FreeRTOS */
#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "cmsis_os.h"
#endif

/* 如果使能软件SPI */
#ifdef SoftWare_SPI

#endif

/* 如果使能硬件SPI */
#ifdef HardWare_SPI
#include "spi.h"
#endif
/****************************************************/
/* SPI基础驱动函数 */
/* 如果使能软件SPI */
#ifdef SoftWare_SPI
/* 引脚定义 */
#define W25Q64_DI_Port  GPIOA
#define W25Q64_DI_Pin   GPIO_PIN_6
#define W25Q64_DO_Port  GPIOA
#define W25Q64_DO_Pin   GPIO_PIN_7
#define W25Q64_SCL_Port GPIOA
#define W25Q64_SCL_Pin  GPIO_PIN_5
/* 引脚状态宏定义 */
#define DO_H()                                                          \
    do {                                                                \
        HAL_GPIO_WritePin(W25Q64_DO_Port, W25Q64_DO_Pin, GPIO_PIN_SET); \
    } while (0)
#define DO_L()                                                            \
    do {                                                                  \
        HAL_GPIO_WritePin(W25Q64_DO_Port, W25Q64_DO_Pin, GPIO_PIN_RESET); \
    } while (0)

#define SCL_H()                                                           \
    do {                                                                  \
        HAL_GPIO_WritePin(W25Q64_SCL_Port, W25Q64_SCL_Pin, GPIO_PIN_SET); \
    } while (0)
#define SCL_L()                                                             \
    do {                                                                    \
        HAL_GPIO_WritePin(W25Q64_SCL_Port, W25Q64_SCL_Pin, GPIO_PIN_RESET); \
    } while (0)

#define DI_STATE() HAL_GPIO_ReadPin(W25Q64_DI_Port, W25Q64_DI_Pin)

/**
 * @brief   微秒延时函数
 */
static void delay_us(__IO uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++) {
        uint8_t uc = 12;
        while (uc--)
            ;
    }
}

/**
 * @brief   SPI写字节函数
 */
static void W25Q64_SPI_WriteByte(uint8_t data)
{
    unsigned char i;
    SCL_L();
    for (i = 0; i < 8; i++) {
        if (data & 0x80)
            DO_H();
        else
            DO_L();
        delay_us(4);
        SCL_H();
        data <<= 1;
        delay_us(4);
        SCL_L();
        delay_us(4);
    }
}

/**
 * @brief   SPI读字节函数
 */
static uint8_t W25Q64_SPI_ReadByte(void)
{
    unsigned char i, out;
    out = 0;
    SCL_L();
    for (i = 0; i < 8; i++) {
        delay_us(4);
        SCL_H();
        out <<= 1;
        delay_us(4);
        if (DI_STATE())
            out |= 0x01;
        delay_us(4);
        SCL_L();
        delay_us(4);
    }
    return out;
}

/**
 * @brief   SPI发送函数
 * @param   send_buf    发送缓冲区地址
 * @param   size        发送字节大小
 */
static HAL_StatusTypeDef W25Q64_SPI_Tramsmit(uint8_t *send_buf, uint16_t size)
{
    for (int i = 0; i < size; i++) {
        W25Q64_SPI_WriteByte(send_buf[i]);
    }
    return HAL_OK;
}

/**
 * @brief   SPI接收函数
 * @param   rev_buf     接收缓冲区地址
 * @param   size        接收字节大小
 */
static HAL_StatusTypeDef W25Q64_SPI_Receive(uint8_t *rev_buf, uint16_t size)
{
    for (int i = 0; i < size; i++) {
        rev_buf[i] = W25Q64_SPI_ReadByte();
    }
    return HAL_OK;
}

#endif
/* 如果使能硬件SPI */
#ifdef HardWare_SPI
/**
 * @brief   SPI发送函数
 * @param   send_buf    发送缓冲区地址
 * @param   size        发送字节大小
 */
static HAL_StatusTypeDef W25Q64_SPI_Tramsmit(uint8_t *send_buf, uint16_t size)
{
    return HAL_SPI_Transmit(&hspi1, send_buf, size, 0xFFFF);
}

/**
 * @brief   SPI接收函数
 * @param   rev_buf     接收缓冲区地址
 * @param   size        接收字节大小
 */
static HAL_StatusTypeDef W25Q64_SPI_Receive(uint8_t *rev_buf, uint16_t size)
{
    return HAL_SPI_Receive(&hspi1, rev_buf, size, 0xFFFF);
}

/**
 * @brief   SPI发送接收函数
 * @param   send_buf    发送缓冲区地址
 * @param   rev_buf     接收缓冲区地址
 * @param   size        发送字节大小
 */
static HAL_StatusTypeDef W25Q64_SPI_TransmitReceive(uint8_t *send_buf, uint8_t *rev_buf, uint16_t size)
{
    return HAL_SPI_TransmitReceive(&hspi1, send_buf, rev_buf, size, 0xFFFF);
}
#endif

/********************W25Q64基本驱动函数********************/
/**
 * @brief ID读取函数
 * @retval 设备ID（第一个字节为厂商ID，第二个字节为设备ID）
 */
uint16_t W25Q64_ReadID(void)
{
    uint8_t rev_buf[2]   = {0x00};
    uint16_t device_id   = 0x00;
    uint8_t send_data[4] = {W25Q64_DEVICEID_CMD, 0x00, 0x00, 0x00};

    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_RESET);
    if (W25Q64_SPI_Tramsmit(send_data, 4) == HAL_OK) {
        if (W25Q64_SPI_Receive(rev_buf, 2) == HAL_OK) {
            device_id = (rev_buf[0] << 8) | rev_buf[1];
        }
    }
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_SET);
    return device_id;
}

/**
 * @brief   状态寄存器读取状态函数
 * @param   reg 状态寄存器编号
 * @retval  状态寄存器值
 */
static uint8_t W25Q64_ReadSR(uint8_t reg)
{
    uint8_t result       = 0;
    uint8_t send_data[4] = {0x00, 0x00, 0x00, 0x00};
    switch (reg) {
        case 1:
            send_data[0] = W25Q64_READ_STATUS_REG1_CMD;
            break;
        case 2:
            send_data[0] = W25Q64_READ_STATUS_REG2_CMD;
            break;
        case 3:
            send_data[0] = W25Q64_READ_STATUS_REG3_CMD;
            break;
        default:
            send_data[0] = W25Q64_READ_STATUS_REG1_CMD;
            break;
    }
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_RESET);
    if (W25Q64_SPI_Tramsmit(send_data, 4) == HAL_OK) {
        if (W25Q64_SPI_Receive(&result, 1) == HAL_OK) {
            HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_SET);
            return result;
        }
    }
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_SET);
    return 0;
}

/**
 * @brief   等待Flash处于空闲
 */
static void W25Q64_Wait_Busy(void)
{
    /* 等待Busy位清空 */
    while ((W25Q64_ReadSR(1) & 0x01) == 0x01)
        ;
}

/**
 * @brief   读取Flash数据函数
 * @param buffer    读出数据
 * @param addr      起始读出地址
 * @param bytes     读出字节数
 */
int W25Q64_ReadFlash(uint8_t *buffer, uint32_t addr, uint16_t bytes)
{
    uint8_t send_data[4] = {0x00, 0x00, 0x00, 0x00};

    send_data[0] = W25Q64_READ_CMD;
    send_data[1] = (uint8_t)(addr >> 16);
    send_data[2] = (uint8_t)(addr >> 8);
    send_data[3] = (uint8_t)(addr);

    W25Q64_Wait_Busy();
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_RESET);
    if (W25Q64_SPI_Tramsmit(send_data, 4) == HAL_OK) {
        if (W25Q64_SPI_Receive(buffer, bytes) != HAL_OK) {
            return HAL_ERROR;
        }
    }
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_SET);
    return HAL_OK;
}

/**
 * @brief 写使能函数
 */
void W25Q64_Write_Enable(void)
{
    uint8_t send_data = W25Q64_WRITE_ENABLE_CMD;
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_RESET);
    if (W25Q64_SPI_Tramsmit(&send_data, 1) == HAL_OK) {
        ;
    }
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_SET);
    W25Q64_Wait_Busy();
}

/**
 * @brief 写失能函数
 */
void W25Q64_Write_Disable(void)
{
    uint8_t send_data = W25Q64_WRITE_DISABLE_CMD;
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_RESET);
    if (W25Q64_SPI_Tramsmit(&send_data, 1) == HAL_OK) {
        ;
    }
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_SET);
    W25Q64_Wait_Busy();
}

/**
 * @brief   扇区擦除函数
 * @param   sector_addr 擦除的扇区地址
 */
void W25Q64_Erase_Sector(uint32_t sector_addr)
{
    uint8_t send_data[4] = {0x00, 0x00, 0x00, 0x00};

    send_data[0] = W25Q64_SECTOR_ERASE_CMD;
    send_data[1] = (uint8_t)(sector_addr >> 16);
    send_data[2] = (uint8_t)(sector_addr >> 8);
    send_data[3] = (uint8_t)(sector_addr);

    W25Q64_Write_Enable();
    W25Q64_Wait_Busy();
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_RESET);
    if (W25Q64_SPI_Tramsmit(send_data, 4) == HAL_OK) {
        ;
    }
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_SET);
    W25Q64_Wait_Busy();
}

/**
 * @brief   页写入函数
 * @param data  写入数据
 * @param addr  起始写入地址
 * @param bytes 写入字节数
 */
void W25Q64_Write_Page(uint8_t *data, uint32_t addr, uint16_t bytes)
{
    uint8_t send_data[4] = {0x00, 0x00, 0x00, 0x00};

    send_data[0] = W25Q64_PAGE_PROG_CMD;
    send_data[1] = (uint8_t)(addr >> 16);
    send_data[2] = (uint8_t)(addr >> 8);
    send_data[3] = (uint8_t)(addr);

    W25Q64_Write_Enable();
    W25Q64_Wait_Busy();
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_RESET);
    if (W25Q64_SPI_Tramsmit(send_data, 4) == HAL_OK) {
        if (W25Q64_SPI_Tramsmit(data, bytes) == HAL_OK) {
            ;
        }
    }
    HAL_GPIO_WritePin(W25Q64_CS_Port, W25Q64_CS_Pin, GPIO_PIN_SET);
    W25Q64_Wait_Busy();
}