#include "mpu6050.h"
/*******************宏定义部分*************************/
/* 如果使能软件IIC */
#if (MPU6050_SoftWare_IIC == 1)
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
#if (MPU6050_HardWare_IIC == 1)
#include "i2c.h"
/**
 * @attention   模块AD0默认接GND,所以转为读写地址后,为0XD1和0XD0(如果接VCC,则为0XD3和0XD2)
 */
#define MPU_READ  0XD1
#define MPU_WRITE 0XD0
#endif

IMU_Data imu_data;

/*****************IIC基础驱动函数**********************/
/* 如果使能软件IIC */
#if (MPU6050_SoftWare_IIC == 1)
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
static uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data)
{
    _IIC_Start();
    _IIC_Send_Byte((MPU_ADDR << 1) | 0); // 发送器件地址+写命令
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
static uint8_t MPU_Read_Byte(uint8_t reg)
{
    uint8_t res;
    _IIC_Start();
    _IIC_Send_Byte((MPU_ADDR << 1) | 0); // 发送器件地址+写命令
    _IIC_Wait_Ack();                     // 等待应答
    _IIC_Send_Byte(reg);                 // 写寄存器地址
    _IIC_Wait_Ack();                     // 等待应答
    _IIC_Start();
    _IIC_Send_Byte((MPU_ADDR << 1) | 1); // 发送器件地址+读命令
    _IIC_Wait_Ack();                     // 等待应答
    res = _IIC_Read_Byte(0);             // 读取数据,发送nACK
    _IIC_Stop();                         // 产生一个停止条件
    return res;
}

#endif
/* 如果使能硬件IIC */
#if (MPU6050_HardWare_IIC == 1)
/**
 * @brief   IIC连续写
 * @param   addr    器件地址
 * @param   reg     寄存器地址
 * @param   len     写入长度
 * @param   buf     数据区
 */
uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    uint8_t ret = HAL_I2C_Mem_Write(&hi2c1, MPU_WRITE, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 0xFFFF);
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
    uint8_t ret = HAL_I2C_Mem_Read(&hi2c1, MPU_READ, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 0xFFFF);
    if (ret == HAL_OK) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief   IIC写字节
 * @param   reg     寄存器地址
 * @param   data    数据
 */
static uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data)
{
    uint8_t ret = HAL_I2C_Mem_Read(&hi2c1, MPU_READ, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFFFF);
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
static uint8_t MPU_Read_Byte(uint8_t reg)
{
    uint8_t res;
    HAL_I2C_Mem_Read(&hi2c1, MPU_READ, reg, I2C_MEMADD_SIZE_8BIT, &res, 1, 0xFFFF);
    return res;
}
#endif

/***********************驱动函数**************************/
#if (MPU_Mahony_Filter == 1 || MPU_DMP == 1 || MPU_Kalman_Filter == 1)
const float k_gyro[4] = {0.00133125, 0.0026625, 0.005325, 0.001065};          // 陀螺仪单位转换
const float k_accr[4] = {0.000598163, 0.001196326, 0.002392652, 0.004785304}; // 加速度计单位转换
float k_g;
float k_a;
#endif
#if (MPU_Kalman_Filter == 1)
float timer;                         // 计时器
float rad2deg = 57.29578;            // 弧度到角度的换算系数
float roll_v = 0, pitch_v = 0;       // 换算到x,y轴上的角速度
float e_P[2][2] = {{1, 0}, {0, 1}};  // 误差协方差矩阵
float k_k[2][2] = {{0, 0}, {0, 0}};  // 卡尔曼增益
float gyro_roll = 0, gyro_pitch = 0; // 陀螺仪积分计算出的角度，先验状态
float acc_roll = 0, acc_pitch = 0;   // 加速度计观测出的角度，观测状态
float k_roll = 0, k_pitch = 0;       // 卡尔曼滤波后估计出最优角度，最优估计状态

#endif

/**
 * @brief   设置MPU6050陀螺仪传感器满量程范围
 * @param   fsr     0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
 */
static uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
    return MPU_Write_Byte(MPU_GYRO_CFG_REG, fsr << 3); // 设置陀螺仪满量程范围
}

/**
 * @brief   设置MPU6050加速度传感器满量程范围
 * @param   fsr     0,±2g;1,±4g;2,±8g;3,±16g
 */
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
    return MPU_Write_Byte(MPU_ACCEL_CFG_REG, fsr << 3); // 设置加速度传感器满量程范围
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
    return MPU_Write_Byte(MPU_CFG_REG, data); // 设置数字低通滤波器
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
    data = MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data); // 设置数字低通滤波器
    return MPU_Set_LPF(rate / 2);                     // 自动设置LPF为采样率的一半
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
    MPU_Read_Len(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf);
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
    res = MPU_Read_Len(MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
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
    res = MPU_Read_Len(MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
    if (res == 0) {
        *ax = ((uint16_t)buf[0] << 8) | buf[1];
        *ay = ((uint16_t)buf[2] << 8) | buf[3];
        *az = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
    ;
}

/*********************************用户函数*******************************************/
/**
 * @brief 初始化MPU6050
 */
uint8_t MPU_Init(void)
{
    uint8_t res;
#if (MPU6050_SoftWare_IIC == 1)
    _IIC_GPIO_Init(); // 初始化IIC总线
#endif
    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80); // 复位MPU6050
    HAL_Delay(100);
    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00); // 唤醒MPU6050
    MPU_Set_Gyro_Fsr(3);                     // 陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                    // 加速度传感器,±2g
#if (MPU_Mahony_Filter == 1 || MPU_DMP == 1 || MPU_Kalman_Filter == 1)
    k_g = k_gyro[3];
    k_a = k_accr[0];
#endif
    MPU_Set_Rate(50);                        // 设置采样率50Hz
    MPU_Write_Byte(MPU_INT_EN_REG, 0X00);    // 关闭所有中断
    MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00); // I2C主模式关闭
    MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);   // 关闭FIFO
    MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80); // INT引脚低电平有效
    res = MPU_Read_Byte(MPU_DEVICE_ID_REG);
    if (res == MPU_ADDR) // 器件ID正确
    {
        MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01); // 设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00); // 加速度与陀螺仪都工作
        MPU_Set_Rate(100);                       // 设置采样率为100Hz
    } else
        return 1;
#if (MPU_DMP == 1)
    while (mpu_dmp_init() != 0)
        ;
#endif
#if (MPU_Mahony_Filter == 1)
    imu_data.q0    = 1.0;
    imu_data.q1    = 0.0;
    imu_data.q2    = 0.0;
    imu_data.q3    = 0.0;
    imu_data.exInt = 0.0;
    imu_data.eyInt = 0.0;
    imu_data.ezInt = 0.0;
#endif
    return 0;
}

/**
 * @brief MPU6050 数据采样
 */
void MPU_Data_Get(void)
{
#if (MPU_DMP == 1)
    short accx, accy, accz;
    short gyrox, gyroy, gyroz;
    float pitch, roll, yaw;
    MPU_Get_Accelerometer(&accx, &accy, &accz);
    MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);
    while (mpu_dmp_get_data(&pitch, &roll, &yaw)) {
#if (USE_FREERTOS == 1)
        osDelay(1);
#endif
    }
    imu_data.accel_x    = accx * k_a;
    imu_data.accel_y    = accy * k_a;
    imu_data.accel_z    = accz * k_a;
    imu_data.gyro_x     = gyrox * k_g;
    imu_data.gyro_y     = gyroy * k_g;
    imu_data.gyro_z     = gyroz * k_g;
    imu_data.pitch      = pitch;
    imu_data.roll       = roll;
    imu_data.yaw        = yaw;
    imu_data.tempreture = (float)MPU_Get_Temperature();
#endif
#if (MPU_Mahony_Filter == 1)
    short accx, accy, accz;
    short gyrox, gyroy, gyroz;
    MPU_Get_Accelerometer(&accx, &accy, &accz);
    MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);

    // 单位转换
    float ax, ay, az, gx, gy, gz, norm;
    imu_data.gyro_x  = gyrox * k_g;
    imu_data.gyro_y  = gyroy * k_g;
    imu_data.gyro_z  = gyroz * k_g;
    imu_data.accel_x = accx * k_a;
    imu_data.accel_y = accy * k_a;
    imu_data.accel_z = accz * k_a;

    // 单位化
    norm = sqrt(imu_data.accel_x * imu_data.accel_x + imu_data.accel_y * imu_data.accel_y + imu_data.accel_z * imu_data.accel_z);
    ax   = imu_data.accel_x / norm;
    ay   = imu_data.accel_y / norm;
    az   = imu_data.accel_z / norm;

    // 重力估计
    float g_vx, g_vy, g_vz;
    g_vx = 2 * (imu_data.q1 * imu_data.q3 - imu_data.q0 * imu_data.q2);
    g_vy = 2 * (imu_data.q0 * imu_data.q1 - imu_data.q2 * imu_data.q3);
    g_vz = imu_data.q0 * imu_data.q0 - imu_data.q1 * imu_data.q1 - imu_data.q2 * imu_data.q2 + imu_data.q3 * imu_data.q3;

    // 叉积求得误差
    float ex, ey, ez;
    ex = (ay * g_vz - az * g_vy);
    ey = (az * g_vx - ax * g_vz);
    ez = (ax * g_vy - ay * g_vx);

    // 积分误差比例积分增益
    imu_data.exInt = imu_data.exInt + ex * Ki;
    imu_data.eyInt = imu_data.eyInt + ey * Ki;
    imu_data.ezInt = imu_data.ezInt + ez * Ki;

    // 调整后的陀螺仪测量
    gx = imu_data.gyro_x + Kp * ex + imu_data.exInt;
    gy = imu_data.gyro_y + Kp * ey + imu_data.eyInt;
    gz = imu_data.gyro_z + Kp * ez + imu_data.ezInt;

    // 整合四元数率和正常化
    imu_data.q0 = imu_data.q0 + (-imu_data.q1 * gx - imu_data.q2 * gy - imu_data.q3 * gz) * halfT;
    imu_data.q1 = imu_data.q1 + (imu_data.q0 * gx + imu_data.q2 * gz - imu_data.q3 * gy) * halfT;
    imu_data.q2 = imu_data.q2 + (imu_data.q0 * gy - imu_data.q1 * gz + imu_data.q3 * gx) * halfT;
    imu_data.q3 = imu_data.q3 + (imu_data.q0 * gz + imu_data.q1 * gy - imu_data.q2 * gx) * halfT;

    // 正常化四元
    norm        = sqrt(imu_data.q0 * imu_data.q0 + imu_data.q1 * imu_data.q1 + imu_data.q2 * imu_data.q2 + imu_data.q3 * imu_data.q3);
    imu_data.q0 = imu_data.q0 / norm;
    imu_data.q1 = imu_data.q1 / norm;
    imu_data.q2 = imu_data.q2 / norm;
    imu_data.q3 = imu_data.q3 / norm;

    // 求欧拉角
    imu_data.pitch      = asin(-2 * imu_data.q1 * imu_data.q3 + 2 * imu_data.q0 * imu_data.q2) * 57.3;
    imu_data.roll       = atan2(2 * imu_data.q2 * imu_data.q3 + 2 * imu_data.q0 * imu_data.q1, -2 * imu_data.q1 * imu_data.q1 - 2 * imu_data.q2 * imu_data.q2 + 1) * 57.3;
    imu_data.yaw        = atan2(2 * (imu_data.q1 * imu_data.q2 + imu_data.q0 * imu_data.q3), imu_data.q0 * imu_data.q0 + imu_data.q1 * imu_data.q1 - imu_data.q2 * imu_data.q2 - imu_data.q3 * imu_data.q3) * 57.3;
    imu_data.tempreture = (float)MPU_Get_Temperature();
#endif
#if (MPU_Kalman_Filter == 1)
    // 获取时间间隔
    float dt = (double)(HAL_GetTick() - timer) / 1000;
    timer    = HAL_GetTick();
    // 数据读取并进行单位转换
    short accx, accy, accz;
    short gyrox, gyroy, gyroz;
    MPU_Get_Accelerometer(&accx, &accy, &accz);
    MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);
    imu_data.gyro_x  = gyrox * k_g;
    imu_data.gyro_y  = gyroy * k_g;
    imu_data.gyro_z  = gyroz * k_g;
    imu_data.accel_x = accx * k_a;
    imu_data.accel_y = accy * k_a;
    imu_data.accel_z = accz * k_a;
    // 计算先验估计值
    roll_v     = (imu_data.gyro_x) + ((sin(k_pitch) * sin(k_roll)) / cos(k_pitch)) * (imu_data.gyro_y) + ((sin(k_pitch) * cos(k_roll)) / cos(k_pitch)) * imu_data.gyro_z; // roll轴的角速度
    pitch_v    = cos(k_roll) * (imu_data.gyro_y) - sin(k_roll) * imu_data.gyro_z;                                                                                         // pitch轴的角速度
    gyro_roll  = k_roll + dt * roll_v;                                                                                                                                    // 先验roll角度
    gyro_pitch = k_pitch + dt * pitch_v;
    // 误差更新
    e_P[0][0] = e_P[0][0] + 0.0025;
    e_P[0][1] = e_P[0][1] + 0;
    e_P[1][0] = e_P[1][0] + 0;
    e_P[1][1] = e_P[1][1] + 0.0025;
    // 计算卡尔曼增益
    k_k[0][0] = e_P[0][0] / (e_P[0][0] + 0.3);
    k_k[0][1] = 0;
    k_k[1][0] = 0;
    k_k[1][1] = e_P[1][1] / (e_P[1][1] + 0.3);
    // 计算最优估计
    acc_roll  = atan((imu_data.accel_y) / (imu_data.accel_z));
    acc_pitch = -1 * atan(imu_data.accel_x) / sqrt(imu_data.accel_y * imu_data.accel_y + imu_data.accel_z * imu_data.accel_z);
    k_roll    = gyro_roll + k_k[0][0] * (acc_roll - gyro_roll);
    k_pitch   = gyro_pitch + k_k[1][1] * (acc_pitch - gyro_pitch);
    // 协方差矩阵更新
    e_P[0][0] = (1 - k_k[0][0]) * e_P[0][0];
    e_P[0][1] = 0;
    e_P[1][0] = 0;
    e_P[1][1] = (1 - k_k[1][1]) * e_P[1][1];
    // 数据处理
    imu_data.yaw        = 0;
    imu_data.tempreture = (float)MPU_Get_Temperature();
    imu_data.pitch      = k_pitch * rad2deg;
    imu_data.roll       = k_roll * rad2deg;
#endif
}
