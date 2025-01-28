#include "dht_11.h"

static uint8_t DHT11_Data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

/**************************DHT11 宏定义段****************************/
#define DHT11_L                                                           \
    do {                                                                  \
        HAL_GPIO_WritePin(DHT11_DAT_Port, DHT11_DAT_Pin, GPIO_PIN_RESET); \
    } while (0)
#define DHT11_H                                                         \
    do {                                                                \
        HAL_GPIO_WritePin(DHT11_DAT_Port, DHT11_DAT_Pin, GPIO_PIN_SET); \
    } while (0)
#define DHT11_Read  HAL_GPIO_ReadPin(DHT11_DAT_Port, DHT11_DAT_Pin)

#define DHT11_OK    1
#define DHT11_ERROR 0
/**************************DHT11 驱动函数段***************************/

/**
 *@brief 微秒延时函数
 *@param us：微秒数
 */
static void delay_us(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (delay--) {
        ;
    }
}

/**
 *@brief 毫秒延时函数
 *@param us：微秒数
 */
static void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++) {
        delay_us(1000);
    }
}

/**
 * @brief   DHT11 GPIO输入输出模式设置
 * @param   mode：输出模式（0）/输入模式（1）
 * @retval  无
 */
static void DHT11_ModeSet(uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStuct = {0};
    if (mode == 0) {
        GPIO_InitStuct.Pin   = DHT11_DAT_Pin;
        GPIO_InitStuct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStuct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DHT11_DAT_Port, &GPIO_InitStuct);
    } else if (mode == 1) {
        GPIO_InitStuct.Pin   = DHT11_DAT_Pin;
        GPIO_InitStuct.Mode  = GPIO_MODE_INPUT;
        GPIO_InitStuct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DHT11_DAT_Port, &GPIO_InitStuct);
    }
}

/**
 * @brief   DHT11 DAT读取1字节
 * @param   无
 * @retval  返回的数据（8 bit）
 */
static uint8_t DHT11_Read_Byte(void)
{
    uint8_t ret = 0;
    uint8_t temp;
    uint8_t delay;
    for (int i = 0; i < 8; i++) {
        while (DHT11_Read == 0 && delay < 100) {
            delay_us(1);
            delay++;
        }
        delay = 0;
        delay_us(40);
        if (DHT11_Read == 1) {
            temp = 1;
        } else {
            temp = 0;
        }
        while (DHT11_Read == 1 && delay < 100) {
            delay_us(1);
            delay++;
        }
        delay = 0;
        ret <<= 1;
        ret |= temp;
    }
    return ret;
}

/************************DHT11 用户函数段***************************/
/**
 * @brief   DHT11读取温湿度数据
 * @param   temp:温度
 * @param   humi:湿度
 * @retval  无
 */
uint8_t DHT11_Get_Data(float *temp, float *humi)
{
    uint8_t delay = 0;

    DHT11_ModeSet(0);
    DHT11_L;
    delay_ms(18);
    DHT11_H;
    delay_us(20);

    DHT11_ModeSet(1);
    delay_us(20);
    if (DHT11_Read == 0) {
        while (DHT11_Read == 0 && delay < 100) {
            delay_us(1);
            delay++;
        }
        delay = 0;
        while (DHT11_Read == 1 && delay < 100) {
            delay_us(1);
            delay++;
        }
        delay = 0;
        for (int i = 0; i < 5; i++) {
            DHT11_Data[i] = DHT11_Read_Byte();
        }
        delay_us(50);
    }
    if (DHT11_Data[0] + DHT11_Data[1] + DHT11_Data[2] + DHT11_Data[3] == DHT11_Data[4]) {
        *temp = (float)(DHT11_Data[2] + DHT11_Data[3] * 0.1);
        *humi = (float)(DHT11_Data[0] + DHT11_Data[1] * 0.1);
        return DHT11_OK;
    } else {
        return DHT11_ERROR;
    }
}
