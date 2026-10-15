#include "u8g2_user.h"

/* 如果使用软件SPI */
#ifdef SoftWare_SPI
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
    switch (msg) {
        // Initialize SPI peripheral
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            /* HAL initialization contains all what we need so we can skip this part. */
            break;

            // Function which implements a delay, arg_int contains the amount of ms
        case U8X8_MSG_DELAY_MILLI:
            HAL_Delay(arg_int);
            break;
            // Function which delays 10us
        case U8X8_MSG_DELAY_10MICRO:
            delay_us(&delay_htim, 10);
            break;
            // Function which delays 100ns
        case U8X8_MSG_DELAY_100NANO:
            __NOP();
            break;
            // Function to define the logic level of the clockline
        case U8X8_MSG_GPIO_SPI_CLOCK:
            if (arg_int)
                HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_GPIO_Pin, GPIO_PIN_SET);
            else
                HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_GPIO_Pin, GPIO_PIN_RESET);

            break;
            // Function to define the logic level of the data line to the display
        case U8X8_MSG_GPIO_SPI_DATA:
            if (arg_int)
                HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_GPIO_Pin, GPIO_PIN_SET);
            else
                HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_GPIO_Pin, GPIO_PIN_RESET);

            break;
            // Function to define the logic level of the CS line
        case U8X8_MSG_GPIO_CS:
            if (arg_int)
                HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_SET);
            else
                HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_RESET);

            break;
            // Function to define the logic level of the Data/ Command line
        case U8X8_MSG_GPIO_DC:
            if (arg_int)
                HAL_GPIO_WritePin(DC_GPIO_Port, DC_GPIO_Pin, GPIO_PIN_SET);
            else
                HAL_GPIO_WritePin(DC_GPIO_Port, DC_GPIO_Pin, GPIO_PIN_RESET);

            break;
            // Function to define the logic level of the RESET line
        case U8X8_MSG_GPIO_RESET:
            if (arg_int)
                HAL_GPIO_WritePin(RST_GPIO_Port, RST_GPIO_Pin, GPIO_PIN_SET);
            else
                HAL_GPIO_WritePin(RST_GPIO_Port, RST_GPIO_Pin, GPIO_PIN_RESET);

            break;
        default:
            return 0; // A message was received which is not implemented, return 0 to indicate an error
    }

    return 1; // command processed successfully.
}
#endif
/* 如果使用软件IIC */
#ifdef SoftWare_IIC
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
    switch (msg) {
        case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
            __NOP();
            break;
        case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
            delay_us(&delay_htim, 10);
            break;
        case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
            HAL_Delay(1);
            break;
        case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
            delay_us(&delay_htim, 5);
            break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
        case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
            if (arg_int == 1)         // arg_int=1: Input dir with pullup high for I2C clock pin
                HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_GPIO_Pin, GPIO_PIN_SET);
            else if (arg_int == 0)
                HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_GPIO_Pin, GPIO_PIN_RESET);
            break;
        case U8X8_MSG_GPIO_I2C_DATA: // arg_int=0: Output low at I2C data pin
            if (arg_int == 1)        // arg_int=1: Input dir with pullup high for I2C data pin
                HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_GPIO_Pin, GPIO_PIN_SET);
            else if (arg_int == 0)
                HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_GPIO_Pin, GPIO_PIN_RESET);
            break;
        case U8X8_MSG_GPIO_MENU_SELECT:
            u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_NEXT:
            u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_PREV:
            u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_HOME:
            u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
            break;
        default:
            u8x8_SetGPIOResult(u8x8, 1); // default return value
            break;
    }
    return 1;
}
#endif

/***************************用户函数*******************************/
void OLED_Init(u8g2_t *_u8g2)
{
#ifdef SoftWare_SPI
    u8g2_Setup_ssd1306_128x64_noname_f(_u8g2, U8G2_R0, u8x8_byte_4wire_sw_spi, u8g2_gpio_and_delay_stm32);
#endif
#ifdef SoftWare_IIC
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(_u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8g2_gpio_and_delay_stm32);
#endif
    u8g2_InitDisplay(_u8g2); // send init sequence to the display, display is in sleep mode after this,
    HAL_Delay(10);
    u8g2_SetPowerSave(_u8g2, 0); // wake up display
    u8g2_ClearBuffer(_u8g2);
    HAL_Delay(100);
}