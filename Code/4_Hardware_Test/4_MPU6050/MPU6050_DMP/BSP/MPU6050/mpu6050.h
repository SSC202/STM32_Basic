/**
 * @brief   STM32 MPU6050 驱动库
 * @attention   本驱动库内含DMP，卡尔曼滤波，Mahony滤波三种算法，用户可选择其中一种，比较推荐Mahony算法进行数据处理
 *              使用前请选择驱动方式（硬件/软件IIC）
 *              使用时使用MPU_Init()进行初始化
 *              在一个循环的程序中定时运行MPU_Data_Get()函数，引出imu_data结构体数据进行数据接收
 * @author  SSC
*/
#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f1xx.h"

/************************ 数据处理方式选择 *****************************/

#define MPU_DMP           0 // 使用DMP库
#define MPU_Kalman_Filter 1 // 使用卡尔曼滤波
#define MPU_Mahony_Filter 0 // 使用Mahony互补滤波（推荐）

/*************************驱动方式选择*********************************/

#define MPU6050_SoftWare_IIC 0 // 使用软件IIC
#define MPU6050_HardWare_IIC 1 // 使用硬件IIC
/************************** 端口定义 **********************************/

#if (MPU6050_SoftWare_IIC == 1)

#define MPU_SCL_Port GPIOB
#define MPU_SDA_Port GPIOB
#define MPU_SCL_Pin  GPIO_PIN_8
#define MPU_SDA_Pin  GPIO_PIN_9

#endif

/*********************************************************************/

#if (MPU_Mahony_Filter == 1)
#define Kp    100.0f  // 比例增益支配率收敛到加速度计/磁强计
#define Ki    0.002f  // 积分增益支配率的陀螺仪偏见的衔接
#define halfT 0.0005f // 采样周期的一半

#endif

/*********************************************************************/

#if (MPU_DMP == 1)
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#endif
#if (MPU_Mahony_Filter == 1 || MPU_Kalman_Filter == 1)
#include "math.h"
#endif

/************************** 寄存器定义 ********************************/
// #define MPU_ACCEL_OFFS_REG		0X06	    //accel_offs寄存器,可读取版本号,寄存器手册未提到
// #define MPU_PROD_ID_REG			0X0C	    //prod id寄存器,在寄存器手册未提到
#define MPU_SELF_TESTX_REG   0X0D // 自检寄存器X
#define MPU_SELF_TESTY_REG   0X0E // 自检寄存器Y
#define MPU_SELF_TESTZ_REG   0X0F // 自检寄存器Z
#define MPU_SELF_TESTA_REG   0X10 // 自检寄存器A
#define MPU_SAMPLE_RATE_REG  0X19 // 采样频率分频器
#define MPU_CFG_REG          0X1A // 配置寄存器
#define MPU_GYRO_CFG_REG     0X1B // 陀螺仪配置寄存器
#define MPU_ACCEL_CFG_REG    0X1C // 加速度计配置寄存器
#define MPU_MOTION_DET_REG   0X1F // 运动检测阀值设置寄存器
#define MPU_FIFO_EN_REG      0X23 // FIFO使能寄存器
#define MPU_I2CMST_CTRL_REG  0X24 // IIC主机控制寄存器
#define MPU_I2CSLV0_ADDR_REG 0X25 // IIC从机0器件地址寄存器
#define MPU_I2CSLV0_REG      0X26 // IIC从机0数据地址寄存器
#define MPU_I2CSLV0_CTRL_REG 0X27 // IIC从机0控制寄存器
#define MPU_I2CSLV1_ADDR_REG 0X28 // IIC从机1器件地址寄存器
#define MPU_I2CSLV1_REG      0X29 // IIC从机1数据地址寄存器
#define MPU_I2CSLV1_CTRL_REG 0X2A // IIC从机1控制寄存器
#define MPU_I2CSLV2_ADDR_REG 0X2B // IIC从机2器件地址寄存器
#define MPU_I2CSLV2_REG      0X2C // IIC从机2数据地址寄存器
#define MPU_I2CSLV2_CTRL_REG 0X2D // IIC从机2控制寄存器
#define MPU_I2CSLV3_ADDR_REG 0X2E // IIC从机3器件地址寄存器
#define MPU_I2CSLV3_REG      0X2F // IIC从机3数据地址寄存器
#define MPU_I2CSLV3_CTRL_REG 0X30 // IIC从机3控制寄存器
#define MPU_I2CSLV4_ADDR_REG 0X31 // IIC从机4器件地址寄存器
#define MPU_I2CSLV4_REG      0X32 // IIC从机4数据地址寄存器
#define MPU_I2CSLV4_DO_REG   0X33 // IIC从机4写数据寄存器
#define MPU_I2CSLV4_CTRL_REG 0X34 // IIC从机4控制寄存器
#define MPU_I2CSLV4_DI_REG   0X35 // IIC从机4读数据寄存器

#define MPU_I2CMST_STA_REG   0X36 // IIC主机状态寄存器
#define MPU_INTBP_CFG_REG    0X37 // 中断/旁路设置寄存器
#define MPU_INT_EN_REG       0X38 // 中断使能寄存器
#define MPU_INT_STA_REG      0X3A // 中断状态寄存器

#define MPU_ACCEL_XOUTH_REG  0X3B // 加速度值,X轴高8位寄存器
#define MPU_ACCEL_XOUTL_REG  0X3C // 加速度值,X轴低8位寄存器
#define MPU_ACCEL_YOUTH_REG  0X3D // 加速度值,Y轴高8位寄存器
#define MPU_ACCEL_YOUTL_REG  0X3E // 加速度值,Y轴低8位寄存器
#define MPU_ACCEL_ZOUTH_REG  0X3F // 加速度值,Z轴高8位寄存器
#define MPU_ACCEL_ZOUTL_REG  0X40 // 加速度值,Z轴低8位寄存器

#define MPU_TEMP_OUTH_REG    0X41 // 温度值高八位寄存器
#define MPU_TEMP_OUTL_REG    0X42 // 温度值低8位寄存器

#define MPU_GYRO_XOUTH_REG   0X43 // 陀螺仪值,X轴高8位寄存器
#define MPU_GYRO_XOUTL_REG   0X44 // 陀螺仪值,X轴低8位寄存器
#define MPU_GYRO_YOUTH_REG   0X45 // 陀螺仪值,Y轴高8位寄存器
#define MPU_GYRO_YOUTL_REG   0X46 // 陀螺仪值,Y轴低8位寄存器
#define MPU_GYRO_ZOUTH_REG   0X47 // 陀螺仪值,Z轴高8位寄存器
#define MPU_GYRO_ZOUTL_REG   0X48 // 陀螺仪值,Z轴低8位寄存器

#define MPU_I2CSLV0_DO_REG   0X63 // IIC从机0数据寄存器
#define MPU_I2CSLV1_DO_REG   0X64 // IIC从机1数据寄存器
#define MPU_I2CSLV2_DO_REG   0X65 // IIC从机2数据寄存器
#define MPU_I2CSLV3_DO_REG   0X66 // IIC从机3数据寄存器

#define MPU_I2CMST_DELAY_REG 0X67 // IIC主机延时管理寄存器
#define MPU_SIGPATH_RST_REG  0X68 // 信号通道复位寄存器
#define MPU_MDETECT_CTRL_REG 0X69 // 运动检测控制寄存器
#define MPU_USER_CTRL_REG    0X6A // 用户控制寄存器
#define MPU_PWR_MGMT1_REG    0X6B // 电源管理寄存器1
#define MPU_PWR_MGMT2_REG    0X6C // 电源管理寄存器2
#define MPU_FIFO_CNTH_REG    0X72 // FIFO计数寄存器高八位
#define MPU_FIFO_CNTL_REG    0X73 // FIFO计数寄存器低八位
#define MPU_FIFO_RW_REG      0X74 // FIFO读写寄存器
#define MPU_DEVICE_ID_REG    0X75 // 器件ID寄存器

/**
 * @attention   如果接V3.3,则IIC地址为0X69(不包含最低位).
 *              如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
 */
#define MPU_ADDR 0X68

/**
 * @attention   模块AD0默认接GND,所以转为读写地址后,为0XD1和0XD0(如果接VCC,则为0XD3和0XD2)
 */
// #define MPU_READ    0XD1
// #define MPU_WRITE   0XD0
/******************************************************************************/
typedef struct
{
    float pitch; // 三轴航向角
    float roll;
    float yaw;
    float accel_x; // 三轴加速度
    float accel_y;
    float accel_z;
    float gyro_x; // 三轴角速度
    float gyro_y;
    float gyro_z;
    float tempreture; // 温度（摄氏度）
#if (MPU_Mahony_Filter == 1)
    float q0, q1, q2, q3; // 四元数
    float exInt, eyInt, ezInt;
#endif
} IMU_Data; // IMU数据结构定义

extern IMU_Data imu_data; // 用户读取数据函数

/**************************辅助函数，用户不使用**********************************/

uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
void _IIC_GPIO_Init(void);

/***********************************函数定义************************************/

uint8_t MPU_Init(void);
void MPU_Data_Get(void);

#endif
