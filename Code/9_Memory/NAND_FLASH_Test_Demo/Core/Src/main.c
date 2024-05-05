/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nandflash_user.h"
#include "nandflash.h"
#include "nandftl.h"
#include "malloc.h"
#include "user_printf.h"
#include "sdram_user.h"
#include "tftlcd.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define NAND_PAGE_SIZE 2048
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */
    uint32_t i = 0;
    NAND_IDTypeDef id;
    NAND_AddressTypeDef temp;
    temp.Plane = 0;
    temp.Block = 0;
    temp.Page  = 0;
    uint8_t buf[NAND_PAGE_SIZE];
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_FMC_Init();
    MX_TIM2_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
    SDRAM_Init();
    LCD_Init();
    RetargetInit(&huart1);
    HAL_Delay(100);
    printf("sudaroot\r\n");

    // 1、复位NAND芯片，读取芯片ID
    HAL_NAND_Reset(&hnand1);
    HAL_NAND_Read_ID(&hnand1, &id);
    printf("1.HAL_id = 0x%X\r\n", *((unsigned int *)&id));

    // 2、擦除第0页所在的第0块的数据，并打印HAL_NAND_Erase_Block函数执行结果
    printf("2.HAL_NAND_Erase_Block = %d\r\n", HAL_NAND_Erase_Block(&hnand1, &temp));

    // 3、读取在芯片第0页的数据，并打印HAL_NAND_Read_Page_8b函数执行结果
    memset(buf, 0, NAND_PAGE_SIZE);
    printf("3.HAL_NAND_Read_Page_8b = %d\r\n", HAL_NAND_Read_Page_8b(&hnand1, &temp, buf, 1));
    for (i = 0; i < NAND_PAGE_SIZE; i++) {
        if ((i % 5) == 0) printf("\r\n");
        printf("buf[%04d] = 0x%02X ", i, buf[i]);
    }
    printf("\r\n");

    // 4、初始化数据，把新的数据写入芯片第0页，并打印HAL_NAND_Write_Page_8b函数执行结果
    for (i = 0; i < NAND_PAGE_SIZE; i++) {
        buf[i] = i & 0x00FF;
    }
    printf("4.HAL_NAND_Write_Page_8b = %d\r\n", HAL_NAND_Write_Page_8b(&hnand1, &temp, buf, 1));

    // 5、重新读取芯片第0页数据，并打印HAL_NAND_Read_Page_8b函数执行结果
    memset(buf, 0, NAND_PAGE_SIZE);
    printf("5.HAL_NAND_Read_Page_8b = %d\r\n", HAL_NAND_Read_Page_8b(&hnand1, &temp, buf, 1));
    for (i = 0; i < NAND_PAGE_SIZE; i++) {
        if ((i % 5) == 0) printf("\r\n");
        printf("buf[%04d] = 0x%02X ", i, buf[i]);
    }
    printf("\r\n");

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 15;
    RCC_OscInitStruct.PLL.PLLN       = 216;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Activate the Over-Drive mode
     */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
