#include "eeprom.h"

/****************************************************/
/* 如果使能FreeRTOS */
#if (USE_FREERTOS == 1)
#include "FreeRTOS.h"
#include "cmsis_os.h"
#endif

/* 如果使能软件IIC */
#if (EEPROM_IIC_SoftWare == 1)
#include <inttypes.h>
#include "gpio.h"
/* 读写控制定义 */
#define IIC_WR 0 /* 写控制bit */
#define IIC_RD 1 /* 读控制bit */
/* 端口状态定义 */
#define IIC_SCL(x)                                                                                                                            \
    do {                                                                                                                                      \
        (x == 1) ? HAL_GPIO_WritePin(IIC_SCL_Port, IIC_SCL_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(IIC_SCL_Port, IIC_SCL_Pin, GPIO_PIN_RESET); \
    } while (0)

#define IIC_SDA(x)                                                                                                                            \
    do {                                                                                                                                      \
        (x == 1) ? HAL_GPIO_WritePin(IIC_SDA_Port, IIC_SDA_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(IIC_SDA_Port, IIC_SDA_Pin, GPIO_PIN_RESET); \
    } while (0)
#define IIC_SDA_READ() HAL_GPIO_ReadPin(IIC_SDA_Port, IIC_SDA_Pin)
#endif

/* 如果使能硬件IIC */
#if (EEPROM_IIC_HardWare == 1)
#include "i2c.h"
#endif
/****************************************************/
/* 如果使能硬件IIC */
#if (EEPROM_IIC_HardWare == 1)
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
/* 如果使能软件IIC */
#if (EEPROM_IIC_SoftWare == 1)
/* IIC 基础驱动函数 */

/**
 * @brief               IIC 总线延迟，最快为400kHz
 * @attention           循环次数为10时，SCL频率 = 205KHz
                        循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us
                        循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us
*/
static void _IIC_Delay(void)
{
    __IO uint8_t i;
    for (i = 0; i < 10; i++)
        ;
}

/**
 * @brief       CPU发起IIC总线启动信号
 * @attention   当SCL高电平时，SDA出现一个下跳沿表示IIC总线启动信号
 */
static void _IIC_Start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    _IIC_Delay();
    IIC_SDA(0);
    _IIC_Delay();
    IIC_SCL(0);
    _IIC_Delay();
}

/**
 * @brief       CPU发起IIC总线停止信号
 * @attention   当SCL高电平时，SDA出现一个上跳沿表示IIC总线停止信号
 */
static void _IIC_Stop(void)
{
    IIC_SDA(0);
    IIC_SCL(1);
    _IIC_Delay();
    IIC_SDA(1);
}

/**
 * @brief   CPU产生一个ACK信号
 */
static void _IIC_Ack(void)
{
    IIC_SDA(0); /* CPU驱动SDA = 0 */
    _IIC_Delay();
    IIC_SCL(1); /* CPU产生1个时钟 */
    _IIC_Delay();
    IIC_SCL(0);
    _IIC_Delay();
    IIC_SDA(1); /* CPU释放SDA总线 */
}

/**
 * @brief   CPU产生1个NACK信号
 */
static void _IIC_NAck(void)
{
    IIC_SDA(1); /* CPU驱动SDA = 1 */
    _IIC_Delay();
    IIC_SCL(1); /* CPU产生1个时钟 */
    _IIC_Delay();
    IIC_SCL(0);
    _IIC_Delay();
}

/**
 * @brief       CPU向IIC总线设备发送8bit数据
 * @param       byte    发送的8字节数据
 */
static void _IIC_Send_Byte(uint8_t byte)
{
    uint8_t i;

    /* 先发送字节的高位bit7 */
    for (i = 0; i < 8; i++) {
        if (byte & 0x80) {
            IIC_SDA(1);
        } else {
            IIC_SDA(0);
        }
        _IIC_Delay();
        IIC_SCL(1);
        _IIC_Delay();
        IIC_SCL(0);
        if (i == 7) {
            IIC_SDA(1); // 释放总线
        }
        byte <<= 1; // 左移一位
        _IIC_Delay();
    }
}

/**
 * @brief   CPU从IIC总线设备读取8bit数据
 * @return  读取的数据
 */
static uint8_t _IIC_Read_Byte(uint8_t ack)
{
    uint8_t i;
    uint8_t value;

    /* 读到第1个bit为数据的bit7 */
    value = 0;
    for (i = 0; i < 8; i++) {
        value <<= 1;
        IIC_SCL(1);
        _IIC_Delay();
        if (IIC_SDA_READ()) {
            value++;
        }
        IIC_SCL(0);
        _IIC_Delay();
    }
    if (ack == 0)
        _IIC_NAck();
    else
        _IIC_Ack();
    return value;
}

/**
 * @brief   CPU产生一个时钟，并读取器件的ACK应答信号
 * @return  返回0表示正确应答，1表示无器件响应
 */
static uint8_t _IIC_Wait_Ack(void)
{
    uint8_t re;

    IIC_SDA(1); /* CPU释放SDA总线 */
    _IIC_Delay();
    IIC_SCL(1); /* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
    _IIC_Delay();
    if (IIC_SDA_READ()) /* CPU读取SDA口线状态 */
    {
        re = 1;
    } else {
        re = 0;
    }
    IIC_SCL(0);
    _IIC_Delay();
    return re;
}

/**
 * @brief   配置IIC总线的GPIO，采用模拟IO的方式实现
 * @attention   在CubeMX里实现，选择高速开漏输出
 */
static void _IIC_GPIO_Init(void)
{
    MX_GPIO_Init();
    _IIC_Stop();
}

/**
 * @brief   检测IIC总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
 * @param   _Address 设备的IIC总线地址
 * @return  0表示正确,1表示未探测到
 */
static uint8_t _IIC_CheckDevice(uint8_t _Address)
{
    uint8_t ucAck;

    _IIC_GPIO_Init();                  /* 配置GPIO */
    _IIC_Start();                      /* 发送启动信号 */
    _IIC_Send_Byte(EEPROM_ADDR_Write); /* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
    ucAck = _IIC_Wait_Ack();           /* 检测设备的ACK应答 */
    _IIC_Stop();                       /* 发送停止信号 */

    return ucAck;
}

/**
 * @brief IIC写字节函数
 * @param addr EEPROM的写入地址
 * @param send_buf  发送缓冲区
 * @param size      发送大小
 */
static HAL_StatusTypeDef EEPROM_IIC_MemWrite(uint8_t addr, uint8_t *send_buf, uint16_t size)
{
    uint8_t i;
    _IIC_Start();
    _IIC_Send_Byte(EEPROM_ADDR_Write); // 发送器件地址+写命令
    if (_IIC_Wait_Ack())               // 等待应答
    {
        _IIC_Stop();
        return 1;
    }
    _IIC_Send_Byte(addr); // 写寄存器地址
    _IIC_Wait_Ack();      // 等待应答
    for (i = 0; i < size; i++) {
        _IIC_Send_Byte(send_buf[i]); // 发送数据
        if (_IIC_Wait_Ack())    // 等待ACK
        {
            _IIC_Stop();
            return 1;
        }
    }
    _IIC_Stop();
    return 0;
}

/**
 * @brief IIC读字节函数
 * @param addr EEPROM的读取地址
 * @param send_buf  接收缓冲区
 * @param size      读取数据大小
 */
static HAL_StatusTypeDef EEPROM_IIC_MemRead(uint8_t addr, uint8_t *rev_buf, uint16_t size)
{
    _IIC_Start();
    _IIC_Send_Byte(EEPROM_ADDR_Write); // 发送器件地址+写命令
    if (_IIC_Wait_Ack())               // 等待应答
    {
        _IIC_Stop();
        return 1;
    }
    _IIC_Send_Byte(addr); // 写寄存器地址
    _IIC_Wait_Ack();      // 等待应答
    _IIC_Start();
    _IIC_Send_Byte(EEPROM_ADDR_Read); // 发送器件地址+读命令
    _IIC_Wait_Ack();                  // 等待应答
    while (size) {
        if (size == 1)
            *rev_buf = _IIC_Read_Byte(0); // 读数据,发送nACK
        else
            *rev_buf = _IIC_Read_Byte(1); // 读数据,发送ACK
        size--;
        rev_buf++;
    }
    _IIC_Stop(); // 产生一个停止条件
    return 0;
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