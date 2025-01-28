#include "stm32f1xx_hal.h"
#include "mpu9250.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include <stdio.h>
/*******************宏定义部分*************************/
/* 如果使能软件IIC */
#ifdef MPU9250_SoftWare_IIC
#include <inttypes.h>
#include "gpio.h"
#define IIC_WR 0 /* 写控制bit */
#define IIC_RD 1 /* 读控制bit */
#define IIC_SCL(x)                                                                                                                            \
    do {                                                                                                                                      \
        (x == 1) ? HAL_GPIO_WritePin(MPU_SCL_Port, MPU_SCL_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(MPU_SCL_Port, MPU_SCL_Pin, GPIO_PIN_RESET); \
    } while (0)

#define IIC_SDA(x)                                                                                                                            \
    do {                                                                                                                                      \
        (x == 1) ? HAL_GPIO_WritePin(MPU_SDA_Port, MPU_SDA_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(MPU_SDA_Port, MPU_SDA_Pin, GPIO_PIN_RESET); \
    } while (0)

#define IIC_SDA_READ() HAL_GPIO_ReadPin(MPU_SDA_Port, MPU_SDA_Pin) /* 读SDA口线状态 */

#endif
/* 如果使能硬件IIC */
#ifdef MPU9250_HardWare_IIC
#include "i2c.h"

#endif
/*****************IIC基础驱动函数**********************/
/* 如果使能软件IIC */
#ifdef MPU9250_SoftWare_IIC
/**
 * @brief               IIC 总线延迟，最快为400kHz
 * @attention           循环次数为10时，SCL频率 = 205KHz
                        循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us
                        循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us
*/
static void _IIC_Delay(void)
{
    uint8_t i;

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
uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
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
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
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
static uint8_t MPU_Write_Byte(uint8_t addr, uint8_t reg, uint8_t data)
{
    _IIC_Start();
    _IIC_Send_Byte((addr << 1) | 0); // 发送器件地址+写命令
    if (_IIC_Wait_Ack())                 // 等待应答
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
static uint8_t MPU_Read_Byte(uint8_t addr, uint8_t reg)
{
    uint8_t res;
    _IIC_Start();
    _IIC_Send_Byte((addr << 1) | 0); // 发送器件地址+写命令
    _IIC_Wait_Ack();                     // 等待应答
    _IIC_Send_Byte(reg);                 // 写寄存器地址
    _IIC_Wait_Ack();                     // 等待应答
    _IIC_Start();
    _IIC_Send_Byte((addr << 1) | 1); // 发送器件地址+读命令
    _IIC_Wait_Ack();                     // 等待应答
    res = _IIC_Read_Byte(0);             // 读取数据,发送nACK
    _IIC_Stop();                         // 产生一个停止条件
    return res;
}

#endif
/* 如果使能硬件IIC */
#ifdef MPU9250_HardWare_IIC
/**
 * @brief   IIC连续写
 * @param   addr    器件地址
 * @param   reg     寄存器地址
 * @param   len     写入长度
 * @param   buf     数据区
 */
uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    uint8_t ret = HAL_I2C_Mem_Write(&hi2c1, (addr << 1), reg, I2C_MEMADD_SIZE_8BIT, buf, len, 0xFFFF);
    if (ret == HAL_OK) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief   IIC连续读
 * @param   addr    器件地址
 * @param   reg     寄存器地址
 * @param   len     写入长度
 * @param   buf     数据区
 */
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    uint8_t ret = HAL_I2C_Mem_Read(&hi2c1, (addr << 1), reg, I2C_MEMADD_SIZE_8BIT, buf, len, 0xFFFF);
    if (ret == HAL_OK) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief   IIC写字节
 * @param   addr    器件地址
 * @param   reg     寄存器地址
 * @param   data    数据
 */
static uint8_t MPU_Write_Byte(uint8_t addr, uint8_t reg, uint8_t data)
{
    uint8_t ret = HAL_I2C_Mem_Read(&hi2c1, (addr << 1), reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFFFF);
    if (ret == HAL_OK) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief   IIC读字节
 * @param   reg     寄存器地址
 */
static uint8_t MPU_Read_Byte(uint8_t addr, uint8_t reg)
{
    uint8_t res;
    HAL_I2C_Mem_Read(&hi2c1, (addr << 1), reg, I2C_MEMADD_SIZE_8BIT, &res, 1, 0xFFFF);
    return res;
}
#endif

/***********************驱动函数**************************/
/**
 * @brief   设置MPU6050陀螺仪传感器满量程范围
 * @param   fsr     0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
 */
static uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
    return MPU_Write_Byte(MPU9250_ADDR, MPU_GYRO_CFG_REG, fsr << 3); // 设置陀螺仪满量程范围
}

/**
 * @brief   设置MPU6050加速度传感器满量程范围
 * @param   fsr     0,±2g;1,±4g;2,±8g;3,±16g
 */
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
    return MPU_Write_Byte(MPU9250_ADDR, MPU_ACCEL_CFG_REG, fsr << 3); // 设置加速度传感器满量程范围
}

/**
 * @brief   设置MPU6050加速度传感器满量程范围
 * @param   lpf     数字低通滤波频率(Hz)
 */
static uint8_t MPU_Set_LPF(uint16_t lpf)
{
    uint8_t data = 0;
    if (lpf >= 188)
        data = 1;
    else if (lpf >= 98)
        data = 2;
    else if (lpf >= 42)
        data = 3;
    else if (lpf >= 20)
        data = 4;
    else if (lpf >= 10)
        data = 5;
    else
        data = 6;
    return MPU_Write_Byte(MPU9250_ADDR, MPU_CFG_REG, data); // 设置数字低通滤波器
}

/**
 * @brief   设置MPU6050的采样率(假定Fs=1KHz)
 * @param   rate     4~1000(Hz)
 */
static uint8_t MPU_Set_Rate(uint16_t rate)
{
    uint8_t data;
    if (rate > 1000)
        rate = 1000;
    if (rate < 4)
        rate = 4;
    data = 1000 / rate - 1;
    data = MPU_Write_Byte(MPU9250_ADDR, MPU_SAMPLE_RATE_REG, data); // 设置数字低通滤波器
    return MPU_Set_LPF(rate / 2);                                   // 自动设置LPF为采样率的一半
}

/**
 * @brief   得到温度
 * @return  温度值(扩大了100倍)
 */
static short MPU_Get_Temperature(void)
{
    uint8_t buf[2];
    short raw;
    float temp;
    MPU_Read_Len(MPU9250_ADDR, MPU_TEMP_OUTH_REG, 2, buf);
    raw  = ((uint16_t)buf[0] << 8) | buf[1];
    temp = 36.53 + ((double)raw) / 340;
    return temp;
}

/**
 * @brief   得到陀螺仪值(原始值)
 * @param   gx  陀螺仪x轴原始读数(带符号)
 * @param   gy  陀螺仪y轴原始读数(带符号)
 * @param   gz  陀螺仪z轴原始读数(带符号)
 */
static int8_t MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
{
    uint8_t buf[6], res;
    res = MPU_Read_Len(MPU9250_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
    if (res == 0) {
        *gx = ((uint16_t)buf[0] << 8) | buf[1];
        *gy = ((uint16_t)buf[2] << 8) | buf[3];
        *gz = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
    ;
}

/**
 * @brief   得到加速度值(原始值)
 * @param   ax  加速度计x轴原始读数(带符号)
 * @param   ay  加速度计y轴原始读数(带符号)
 * @param   az  加速度计z轴原始读数(带符号)
 */
static int8_t MPU_Get_Accelerometer(short *ax, short *ay, short *az)
{
    uint8_t buf[6], res;
    res = MPU_Read_Len(MPU9250_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
    if (res == 0) {
        *ax = ((uint16_t)buf[0] << 8) | buf[1];
        *ay = ((uint16_t)buf[2] << 8) | buf[3];
        *az = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
    ;
}

/**
 * @brief   得到磁力计值(原始值)
 * @param   mx  磁力计x轴原始读数(带符号)
 * @param   my  磁力计y轴原始读数(带符号)
 * @param   mz  磁力计z轴原始读数(带符号)
 */
uint8_t MPU_Get_Magnetometer(short *mx, short *my, short *mz)
{
    uint8_t buf[6], res;
    res = MPU_Read_Len(AK8963_ADDR, MAG_XOUT_L, 6, buf);
    if (res == 0) {
        *mx = ((uint16_t)buf[1] << 8) | buf[0];
        *my = ((uint16_t)buf[3] << 8) | buf[2];
        *mz = ((uint16_t)buf[5] << 8) | buf[4];
    }
    MPU_Write_Byte(AK8963_ADDR, MAG_CNTL1, 0X11); // AK8963每次读完以后都需要重新设置为单次测量模式
    return res;
}
/*********************************用户函数*******************************************/

IMU_Data_t mpu_data;
/**
 * @brief 初始化MPU6050
 */
uint8_t MPU_Init(void)
{
    uint8_t res = 0;
#ifdef MPU6050_SoftWare_IIC
    _IIC_GPIO_Init(); // 初始化IIC总线
#endif
    MPU_Write_Byte(MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0X80); // 复位MPU9250
    HAL_Delay(100);                                        // 延时100ms
    MPU_Write_Byte(MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0X00); // 唤醒MPU9250
    MPU_Set_Gyro_Fsr(3);                                   // 陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                                  // 加速度传感器,±2g
    MPU_Set_Rate(50);                                      // 设置采样率50Hz
    MPU_Write_Byte(MPU9250_ADDR, MPU_INT_EN_REG, 0X00);    // 关闭所有中断
    MPU_Write_Byte(MPU9250_ADDR, MPU_USER_CTRL_REG, 0X00); // I2C主模式关闭
    MPU_Write_Byte(MPU9250_ADDR, MPU_FIFO_EN_REG, 0X00);   // 关闭FIFO
    MPU_Write_Byte(MPU9250_ADDR, MPU_INTBP_CFG_REG, 0X82); // INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
    res = MPU_Read_Byte(MPU9250_ADDR, MPU_DEVICE_ID_REG);  // 读取MPU6500的ID
    // printf("res = %x\r\n",res);
    if (res == MPU6500_ID) // 器件ID正确
    {
        MPU_Write_Byte(MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0X01); // 设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU9250_ADDR, MPU_PWR_MGMT2_REG, 0X00); // 加速度与陀螺仪都工作
        MPU_Set_Rate(50);                                      // 设置采样率为50Hz
    } else
        return 1;
    // printf("init start\r\n");
    res = MPU_Read_Byte(AK8963_ADDR, MAG_WIA); // 读取AK8963 ID
    printf("res = %d\r\n",res);
    if (res == AK8963_ID) {
        MPU_Write_Byte(AK8963_ADDR, MAG_CNTL1, 0X11); // 设置AK8963为单次测量模式
    } else
        return 1;
    printf("init start\r\n");
    uint8_t err = mpu_dmp_init();
    while (err) {
        printf("mpu_init_err:%d\r\n", err);
        mpu_dmp_init();
    }
    printf("mpu9250 Ok\r\n");
    return 0;
}

/**
 * @brief MPU6050 数据采样并滤波
 * @param _imu_data IMU 数据结构体
 */
void MPU_Data_Get(IMU_Data_t *uimu_data)
{
    short uaccx, uaccy, uaccz;
    short ugyrox, ugyroy, ugyroz;
    MPU_Get_Accelerometer(&uaccx, &uaccy, &uaccz);
    MPU_Get_Gyroscope(&ugyrox, &ugyroy, &ugyroz);

    uimu_data->accel_x = uaccx;
    uimu_data->accel_y = uaccy;
    uimu_data->accel_z = uaccz;
    uimu_data->gyro_x  = ugyrox;
    uimu_data->gyro_y  = ugyroy;
    uimu_data->gyro_z  = ugyroz;

    float upitch, uroll, uyaw;
    while (mpu_mpl_get_data(&upitch, &uroll, &uyaw))
        ;
    uimu_data->pitch = upitch;
    uimu_data->roll  = uroll;
    uimu_data->yaw   = uyaw;

    uimu_data->tempreture = (float)MPU_Get_Temperature();
}
