/**
 * @file ADS1256.h
 * @author TITH (1023515576@qq.com)
 * @brief ADS1256的驱动程序，请include此文件
 * @version 1.1
 * @date 2022-01-11
 */

#pragma once

#include <stdio.h>
#include "gpio.h"
#include "spi.h"
#include "bsp_delay.h"
extern int32_t ADS1256_diff_data[4];

//ADS1256 初始化

typedef struct ADS1256_register {
    uint8_t STATUS;
    uint8_t MUX;
    uint8_t ADCON;
    uint8_t DRATE;
    uint8_t IO;
} ADS1256_REG;

extern int32_t ADS1256_diff_data[4];

// ADS1256 初始化
void ADS1256_Init(void);

/**
 * @brief 读取指定通道的数据 (0-7 for AIN0-AIN7, 8 for AINCOM)
 * @param pIChannel Positive Input Channel
 * @param nIChannel Negative Input Channel
 * @return uint32_t 读到的值，实际位数是24位
 */
int32_t ADS1256ReadData(uint8_t pIChannel, uint8_t nIChannel);

uint8_t ADS1256_RREG(uint8_t regaddr);

ADS1256_REG ADS1256_RREG_All(void);

void ADS1256_UpdateDiffData(void);
