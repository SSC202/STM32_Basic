/**
 * @file wtr_ads1256.c
 * @author TITH (1023515576@qq.com)
 * @test  Liam (quan.2003@outlook.com)  添加了相应注释
 * @brief ADS1256的驱动程序
 * @version 1.2
 * @date 2023-10-10
 * 
 * @copyright Copyright (c) 2023
 * 
 * 本驱动在 ADS1256 时钟频率为 7.68 MHz 下测试
 * (片选功能虽然写了，但没测试)
 * 
 * 以下内容为写驱动时的笔记，使用者可以不必深究
 * 在 f = 7.68 MHz (T = 130 ns) 下：
 * 
 * (手册 page 5)
 * SPI T_min = 4T = 521 ns
 * 输入命令到读取数据的延时 T_min = 50T = 6510 ns
 * 命令之间延时各不相同，最长的需要 24T = 3125 ns
 * 片选使能后可以立即进行操作，但失能需要与最后一个读写操作相隔至少 8T = 1042 ns
 * 
 * Settling Time Using the Input Multiplexer读取数据时，
 * 需要在发送 SYNC 命令后在 SETTLING TIME 内完成
 * SETTLING TIME 长度随着 DATA RATE 变化（见手册Table 13, page 20）
 * 摘取几个值：
 * DATA RATE		SETTLING TIME(ms)
 * 
 * 30 000			0.21
 * 2000				0.68
 * 1000				1.18                (本驱动使用1000SPS samples per second)
 * 500				2.18.m          
 * 100				10.18
 * 
 * 关于CubeMX SPI的配置
 * 选择Band Rate <= 1,919,385.7965Bits/s  在F407VET6下 168MHz SPI1 分频64 Band Rate 1.3125MBits/s
 * CPOL      Low
 * CPHA      2 Edge   即第二个跳变沿数据被采集
 * 
 * 引脚配置
 * DRDY      GPIO_Input
 * Reset     GPIO_Output
 * .
 * 延时函数
 * delay_us()
 * delay_ms()
 */

#include "wtr_ads1256.h"
#include "wtr_ads1256_Config.h"

#include "time.h"



// 一些命令
#define ADS1256_CMD_WREG 0x50
#define ADS1256_CMD_RREG 0x10
#define ADS1256_CMD_SYNC 0xFC
#define ADS1256_CMD_WAKEUP 0xFF
#define ADS1256_CMD_RDATA 0x01

// 寄存器地址
#define ADS1256_STATUS 0x00
#define ADS1256_MUX 0x01
#define ADS1256_ADCON 0x02
#define ADS1256_DRATE 0x03
#define ADS1256_IO 0x04
#define ADS1256_OFC0 0x05
#define ADS1256_OFC1 0x06
#define ADS1256_OFC2 0x07
#define ADS1256_FSC0 0x08
#define ADS1256_FSC1 0x09
#define ADS1256_FSC2 0x0A

// A/D Data Rate
#define ADS1256_DRATE_2_5	0x03
#define ADS1256_DRATE_5		0x13
#define ADS1256_DRATE_10	0x23
#define ADS1256_DRATE_15	0x33
#define ADS1256_DRATE_25	0x43
#define ADS1256_DRATE_30	0x53
#define ADS1256_DRATE_50	0x63
#define ADS1256_DRATE_60	0x72
#define ADS1256_DRATE_100	0x82
#define ADS1256_DRATE_500	0x92
#define ADS1256_DRATE_1000	0xA1
#define ADS1256_DRATE_2000	0xB0
#define ADS1256_DRATE_3750	0xC0
#define ADS1256_DRATE_7500	0xD0
#define ADS1256_DRATE_15000	0xE0
#define ADS1256_DRATE_30000	0xF0

int32_t ADS1256_diff_data[4];

/**
 * @brief 向 ADS1256 同时读写数据（双工）
 * 
 * @param TxData 要写入的数据
 * @return uint8_t 读到的数据
 */
uint8_t ADS1256_SPI_RW(uint8_t TxData)
{
	uint8_t RxData;
	HAL_SPI_TransmitReceive(&hspi1, &TxData, &RxData, 1, HAL_MAX_DELAY);
	delay_us(5);
	// printf("ADS1256: W %X\t R %X\n",TxData, RxData);
	return RxData;
}

/**
 * @brief 根据 DRDY 引脚电平判断芯片是否准备就绪
 * 
 * @return uint8_t 1 就绪；0 未就绪
 */
uint8_t ADS1256_IsReady()
{
	return !HAL_GPIO_ReadPin(ADS1256_DRDY_GPIOx, ADS1256_DRDY_GPIO_Pin);
}

/**
 * @brief 重置ADS1256
 * 
 * @return void 
 */
void ADS1256_Reset()
{
	HAL_GPIO_WritePin(ADS1256_Reset_GPIOx, ADS1256_Reset_GPIO_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(ADS1256_Reset_GPIOx, ADS1256_Reset_GPIO_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
}

/**
 * @brief 片选，要对ADS1256通信，必须开启ADS1256的片选，没有片选引脚可忽略此函数
 * @param value 1 片选开启，0 片选关闭
 */
void ADS1256_NSS(uint8_t value)
{
#ifdef ADS1256_NSS_Enable
	if (value == 0)
	{
		delay_us(2);
		HAL_GPIO_WritePin(ADS1256_NSS_GPIOx, ADS1256_NSS_GPIO_Pin, GPIO_PIN_SET);// 高电平为关
	}
	else
	{
		HAL_GPIO_WritePin(ADS1256_NSS_GPIOx, ADS1256_NSS_GPIO_Pin, GPIO_PIN_RESET);// 低电平为开
	}
#endif
}

/**
 * @brief 写入寄存器
 * 
 * @param regaddr 寄存器地址
 * @param databyte 要写入的数据
 */
void ADS1256_WREG(uint8_t regaddr, uint8_t databyte)
{
	ADS1256_NSS(1);

	while (!ADS1256_IsReady());
	ADS1256_SPI_RW(ADS1256_CMD_WREG | (regaddr & 0x0F));
	ADS1256_SPI_RW(0x00);
	ADS1256_SPI_RW(databyte);

	ADS1256_NSS(0);
}

/**
 * @brief 读入寄存器
 * 
 * @param regaddr 寄存器地址
 * @return uint8_t 返回读出的数据
 */
uint8_t ADS1256_RREG(uint8_t regaddr)
{
	ADS1256_NSS(1);

	uint8_t result;
	while (!ADS1256_IsReady());
	ADS1256_SPI_RW(ADS1256_CMD_RREG | (regaddr & 0x0F));
	ADS1256_SPI_RW(0x00);
	delay_us(5);
	result = ADS1256_SPI_RW(0xff);

	ADS1256_NSS(0);
	return result;
}

ADS1256_REG ADS1256_RREG_All(void)
{
	ADS1256_NSS(1);

	ADS1256_REG result;
	while (!ADS1256_IsReady());
	ADS1256_SPI_RW(ADS1256_CMD_RREG | 0x00);
	ADS1256_SPI_RW(0x04);
	delay_us(5);
	result.STATUS = ADS1256_SPI_RW(0xff);
	result.MUX = ADS1256_SPI_RW(0xff);
	result.ADCON = ADS1256_SPI_RW(0xff);
	result.DRATE = ADS1256_SPI_RW(0xff);
	result.IO = ADS1256_SPI_RW(0xff);

	ADS1256_NSS(0);
	return result;
}

void ADS1256_Init(void)
{
	ADS1256_NSS(1);

	ADS1256_Reset();

	HAL_Delay(100);

	/**
	 * @brief STATUS REGISTER
	 * ORDER: Most Significant Bit First (default)
	 * 自校准: 开启
	 * 0 = Auto-Calibration Disabled (default)
     * 1 = Auto-Calibration Enabled
	 * 
	 * BUFEN: Analog Input Buffer Enable
	 * 0 = Buffer Disabled (default)
	 * 1 = Buffer Enabled
	 */
	ADS1256_WREG(ADS1256_STATUS, 0x06);

	/**
	 * @brief Input Multiplexer Control Register
	 * Positive Input Channel: AIN0 (default)
	 * Negative Input Channel: AINCOM  负的默认连接了GND，因为信号源N 为GND  使用0x00也行
	 * Positive Input Channel(前八位)：
	 * 0000    AIN
	 * 0001 = AIN1
	 * 0010 = AIN2 (ADS1256 only)
	 * 0011 = AIN3 (ADS1256 only)
	 * 0100 = AIN4 (ADS1256 only)
	 * 0101 = AIN5 (ADS1256 only)
	 * 0110 = AIN6 (ADS1256 only)
	 * 0111 = AIN7 (ADS1256 only)
	 * 1xxx = AINCOM (when PSEL3 = 1, PSEL2, PSEL1, PSEL0 are “don’t care”)
	 * 
	 *  Negative Input Channel（后八位）：
	 * 0000 = AIN0
	 * 0001 = AIN1 (default)
	 * 0010 = AIN2 (ADS1256 only)
	 * 0011 = AIN3 (ADS1256 only)
	 * 0100 = AIN4 (ADS1256 only)
	 * 0101 = AIN5 (ADS1256 only)
	 * 0110 = AIN6 (ADS1256 only)
	 * 0111 = AIN7 (ADS1256 only)
	 * 1xxx = AINCOM (when NSEL3 = 1, NSEL2, NSEL1, NSEL0 are “don’t care”)
	 */
	ADS1256_WREG(ADS1256_MUX, 0x08);

	/**
	 * @brief  A/D Control Register
	 * Clock Out OFF
	 * Sensor Detect OFF (default)（可用来检测外部传感器是否完好）
	 * Programmable Gain Amplifier Setting = 1
	 */
	ADS1256_WREG(ADS1256_ADCON, 0x00);

	/**
	 * @brief A/D Data Rate
	 * 默认值：30,000SPS
	 * 本驱动采用 1000SPS
	 */
	ADS1256_WREG(ADS1256_DRATE,ADS1256_DRATE_1000);

	/**
	 * @brief GPIO Control Register
	 * 全部设为 Output
	 * 引脚电平全部设为 0  
	 */
	ADS1256_WREG(ADS1256_IO, 0x00);

	ADS1256_NSS(0);
}

/**
 * @brief 发送读取数据命令并对读取的数据解码
 * 
 * @return int32_t 解码后的数据
 */
int32_t ADS1256_RDATA()
{
	ADS1256_NSS(1);

	ADS1256_SPI_RW(ADS1256_CMD_RDATA);
	int32_t result = 0;
	uint8_t RxGroup[3] = {0};
	uint8_t TxGroup[3] = {0};

	delay_us(5); // 发出 RDATA 需要等待一段时间才能正常接收

	HAL_SPI_TransmitReceive(&hspi1, TxGroup, RxGroup, 3, HAL_MAX_DELAY);

	// 将读取的三段数据拼接
	result |= ((int32_t)RxGroup[0] << 16);
	result |= ((int32_t)RxGroup[1] << 8);
	result |= (int32_t)RxGroup[2];

	// 处理负数
	if( result & 0x800000 )
	{
		result = ~(unsigned long)result;
		result &= 0x7fffff;
		result += 1;
		result = -result;
	}

	ADS1256_NSS(0);
	return result;
}

int32_t ADS1256ReadData(uint8_t pIChannel, uint8_t nIChannel)
{
	ADS1256_NSS(1);

	int32_t result;
	static uint8_t channel = 0;

	while (!ADS1256_IsReady());

	if (channel != ((pIChannel << 4) | nIChannel))
	{
		channel = (pIChannel << 4) | nIChannel;
		ADS1256_WREG(ADS1256_MUX,channel); // 选择通道
	}

	ADS1256_SPI_RW(ADS1256_CMD_SYNC);
	ADS1256_SPI_RW(ADS1256_CMD_WAKEUP);

	while (!ADS1256_IsReady());
	result = ADS1256_RDATA();

	ADS1256_NSS(0);
	return result;
}

void ADS1256_UpdateDiffData(void)
{
	ADS1256_NSS(1);

	int8_t channel;
	int8_t nIChannel;
	int8_t pIChannel;

	for (int8_t i = 0; i < 4; i++)
	{
		pIChannel = 2 * i;
		nIChannel = 2 * i + 1;
		channel = (pIChannel << 4) | nIChannel;

		while (!ADS1256_IsReady());

		ADS1256_WREG(ADS1256_MUX,channel); // 选择通道
		ADS1256_SPI_RW(ADS1256_CMD_SYNC);
		ADS1256_SPI_RW(ADS1256_CMD_WAKEUP);
		ADS1256_diff_data[(i + 3) % 4] = ADS1256_RDATA(); // 此次读取的是上次的数据
	}

	ADS1256_NSS(0);
}
