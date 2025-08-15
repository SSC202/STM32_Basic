#include "mt6701.h"

/*******************宏定义部分*************************/

/* 如果使能软件IIC */
#if (MT6701_SoftWare_IIC == 1)
#include <inttypes.h>
#include "gpio.h"
#define IIC_WR 0 /* 写控制bit */
#define IIC_RD 1 /* 读控制bit */
#define IIC_SCL(x)                                                                                                                                        \
    do {                                                                                                                                                  \
        (x == 1) ? HAL_GPIO_WritePin(MT6701_SCL_Port, MT6701_SCL_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(MT6701_SCL_Port, MT6701_SCL_Pin, GPIO_PIN_RESET); \
    } while (0)

#define IIC_SDA(x)                                                                                                                                        \
    do {                                                                                                                                                  \
        (x == 1) ? HAL_GPIO_WritePin(MT6701_SDA_Port, MT6701_SDA_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(MT6701_SDA_Port, MT6701_SDA_Pin, GPIO_PIN_RESET); \
    } while (0)

#define IIC_SDA_READ() HAL_GPIO_ReadPin(MT6701_SDA_Port, MT6701_SDA_Pin) /* 读SDA口线状态 */
#endif

/* 如果使能硬件IIC */
#if (MT6701_HardWare_IIC == 1)

#include "i2c.h"
#define MT6701_WRITE_ADDR (0x0C) // MT6701 写地址
#define MT6701_READ_ADDR  (0x0D) // MT6701 读地址

#endif

/*****************IIC基础驱动函数**********************/

/* 如果使能软件IIC */

#if (MT6701_SoftWare_IIC == 1)
/**
 * @brief               IIC 总线延迟，最快为400kHz
 * @attention           循环次数为10时，SCL频率 = 205KHz
                        循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us
                        循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us
*/
static void _IIC_Delay(void)
{
    uint8_t i;

    for (i = 0; i < 10; i++);
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
void _IIC_GPIO_Init(void)
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
    _IIC_Send_Byte(_Address | IIC_WR); /* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
    ucAck = _IIC_Wait_Ack();           /* 检测设备的ACK应答 */
    _IIC_Stop();                       /* 发送停止信号 */

    return ucAck;
}

/**
 * @brief   IIC连续写
 * @param   addr    器件地址
 * @param   reg     寄存器地址
 * @param   len     写入长度
 * @param   buf     数据区
 */
uint8_t MT6701_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    uint8_t i;
    _IIC_Start();
    _IIC_Send_Byte((addr << 1) | 0); // 发送器件地址+写命令
    if (_IIC_Wait_Ack())             // 等待应答
    {
        _IIC_Stop();
        return 1;
    }
    _IIC_Send_Byte(reg); // 写寄存器地址
    _IIC_Wait_Ack();     // 等待应答
    for (i = 0; i < len; i++) {
        _IIC_Send_Byte(buf[i]); // 发送数据
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
 * @brief   IIC连续读
 * @param   addr    器件地址
 * @param   reg     寄存器地址
 * @param   len     写入长度
 * @param   buf     数据区
 */
uint8_t MT6701_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    _IIC_Start();
    _IIC_Send_Byte((addr << 1) | 0); // 发送器件地址+写命令
    if (_IIC_Wait_Ack())             // 等待应答
    {
        _IIC_Stop();
        return 1;
    }
    _IIC_Send_Byte(reg); // 写寄存器地址
    _IIC_Wait_Ack();     // 等待应答
    _IIC_Start();
    _IIC_Send_Byte((addr << 1) | 1); // 发送器件地址+读命令
    _IIC_Wait_Ack();                 // 等待应答
    while (len) {
        if (len == 1)
            *buf = _IIC_Read_Byte(0); // 读数据,发送nACK
        else
            *buf = _IIC_Read_Byte(1); // 读数据,发送ACK
        len--;
        buf++;
    }
    _IIC_Stop(); // 产生一个停止条件
    return 0;
}

/**
 * @brief   IIC写字节
 * @param   reg     寄存器地址
 * @param   data    数据
 */
static uint8_t MT6701_Write_Byte(uint8_t reg, uint8_t data)
{
    _IIC_Start();
    _IIC_Send_Byte((MT6701_ADDR << 1) | 0); // 发送器件地址+写命令
    if (_IIC_Wait_Ack())                    // 等待应答
    {
        _IIC_Stop();
        return 1;
    }
    _IIC_Send_Byte(reg);  // 写寄存器地址
    _IIC_Wait_Ack();      // 等待应答
    _IIC_Send_Byte(data); // 发送数据
    if (_IIC_Wait_Ack())  // 等待ACK
    {
        _IIC_Stop();
        return 1;
    }
    _IIC_Stop();
    return 0;
}

/**
 * @brief   IIC读字节
 * @param   reg     寄存器地址
 */
static uint8_t MT6701_Read_Byte(uint8_t reg)
{
    uint8_t res;
    _IIC_Start();
    _IIC_Send_Byte((MT6701_ADDR << 1) | 0); // 发送器件地址+写命令
    _IIC_Wait_Ack();                        // 等待应答
    _IIC_Send_Byte(reg);                    // 写寄存器地址
    _IIC_Wait_Ack();                        // 等待应答
    _IIC_Start();
    _IIC_Send_Byte((MT6701_ADDR << 1) | 1); // 发送器件地址+读命令
    _IIC_Wait_Ack();                        // 等待应答
    res = _IIC_Read_Byte(0);                // 读取数据,发送nACK
    _IIC_Stop();                            // 产生一个停止条件
    return res;
}

#endif

/* 如果使能硬件IIC */
#if (MT6701_HardWare_IIC == 1)

/**
 * @brief   MT6701 向寄存器内写字节
 * @param   reg     寄存器地址
 * @param   value   数据
 */
static uint8_t MT6701_Write_Byte(uint8_t reg, uint8_t data)
{
    return HAL_I2C_Mem_Write(&hi2c1, MT6701_WRITE_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 50);
}

/**
 * @brief   MT6701 向寄存器内连续写
 * @param   reg     寄存器地址
 * @param   value   数据
 * @param   len     数据长度
 */
static uint8_t MT6701_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    return HAL_I2C_Mem_Write(&hi2c1, MT6701_WRITE_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 50);
}

/**
 * @brief   MT6701 从寄存器内连续读
 * @param   reg     寄存器地址
 * @param   buf     数据
 * @param   len     数据长度
 */
static uint8_t MT6701_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    return HAL_I2C_Mem_Read(&hi2c1, MT6701_READ_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 50);
}

#endif

/*********************************用户函数*******************************************/

/**
 * @brief   MT6701 初始化函数
 */
void MT6701_Init(void)
{
#if (MT6701_SoftWare_IIC == 1)
    _IIC_GPIO_Init(); // 初始化IIC总线
#endif
}

/**
 * @brief   MT6701 读取角度
 * @param   angle  绝对角度值
 */
void MT6701_Get_Angle(float *angle)
{
    uint8_t temp[2];
    int16_t encoder_angle;
    MT6701_Read_Len(MT6701_ADDR, MT6701_ANGLE_REG, 2, temp);

    encoder_angle = ((int16_t)temp[0] << 6) | (temp[1] >> 2);
    *angle        = (float)encoder_angle * (2 * 3.141592654f) / 16384.f;
}
