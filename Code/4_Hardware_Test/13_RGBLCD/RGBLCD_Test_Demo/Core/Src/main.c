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
#include "dma2d.h"
#include "ltdc.h"
#include "tim.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rgblcd.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define KEY2_Pin           GPIO_PIN_13
#define KEY2_GPIO_Port     GPIOC
#define KEY_WkUP_Pin       GPIO_PIN_0
#define KEY_WkUP_GPIO_Port GPIOA
#define KEY1_Pin           GPIO_PIN_2
#define KEY1_GPIO_Port     GPIOH
#define KEY0_Pin           GPIO_PIN_3
#define KEY0_GPIO_Port     GPIOH
#define LED1_Pin           GPIO_PIN_0
#define LED1_GPIO_Port     GPIOB
#define LED0_Pin           GPIO_PIN_1
#define LED0_GPIO_Port     GPIOB
#define PI                 3.1415926
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

void LCD_Test(void);

static float cube[8][3] = {
    {-32, -32, -32},
    {-32, +32, -32},
    {+32, +32, -32},
    {+32, -32, -32},
    {-32, -32, +32},
    {-32, +32, +32},
    {+32, +32, +32},
    {+32, -32, +32}};

static uint8_t line_id[24] = {
    1, 2, 2, 3,
    3, 4, 4, 1,
    5, 6, 6, 7,
    7, 8, 8, 5,
    8, 4, 7, 3,
    6, 2, 5, 1};
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */

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
    MX_DMA2D_Init();
    MX_FMC_Init();
    MX_LTDC_Init();
    MX_TIM7_Init();
    /* USER CODE BEGIN 2 */
    MX_FMC_Init();
    MX_LTDC_Init();
    LCD_Init();
    LCD_Test();
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
static float *demo_matconv(float *a, float b[3][3])
{
    float res[3];
    uint8_t res_index;
    uint8_t a_index;

    for (res_index = 0; res_index < 3; res_index++) {
        res[res_index] = b[res_index][0] * a[0] + b[res_index][1] * a[1] + b[res_index][2] * a[2];
    }

    for (a_index = 0; a_index < 3; a_index++) {
        a[a_index] = res[a_index];
    }

    return a;
}

static void demo_rotate(float *point, float x, float y, float z)
{
    float rx[3][3];
    float ry[3][3];
    float rz[3][3];

    x /= PI;
    y /= PI;
    z /= PI;

    rx[0][0] = cos(x);
    rx[0][1] = 0;
    rx[0][2] = sin(x);
    rx[1][0] = 0;
    rx[1][1] = 1;
    rx[1][2] = 0;
    rx[2][0] = -sin(x);
    rx[2][1] = 0;
    rx[2][2] = cos(x);

    ry[0][0] = 1;
    ry[0][1] = 0;
    ry[0][2] = 0;
    ry[1][0] = 0;
    ry[1][1] = cos(y);
    ry[1][2] = -sin(y);
    ry[2][0] = 0;
    ry[2][1] = sin(y);
    ry[2][2] = cos(y);

    rz[0][0] = cos(z);
    rz[0][1] = -sin(z);
    rz[0][2] = 0;
    rz[1][0] = sin(z);
    rz[1][1] = cos(z);
    rz[1][2] = 0;
    rz[2][0] = 0;
    rz[2][1] = 0;
    rz[2][2] = 1;

    demo_matconv(demo_matconv(demo_matconv(point, rz), ry), rx);
}

void LCD_Test(void)
{
    uint8_t ret;
    uint16_t pid;

    ret = LCD_Init();
    pid = LCD_Get_PID();
    if ((ret != 0) || (pid != LCD_PID_4384)) {
    }
    LCD_Clear(LCD_WHITE);
    LCD_Show_String(10, 10, LCD_Get_Width(), 32, "STM32", LCD_FONT_32, LCD_RED);
    LCD_Show_String(10, 42, LCD_Get_Width(), 24, "ATK-MD0430R-800480", LCD_FONT_24, LCD_RED);
    LCD_Show_String(10, 66, LCD_Get_Width(), 16, "ATOM@ALIENTEK", LCD_FONT_16, LCD_RED);
    while (1) {
        uint8_t point_index;
        uint8_t line_index;
        static uint16_t x = 57;
        static uint16_t y = 139;
#if (LCD_USING_TOUCH != 0)
        static LCD_touch_point_t point[1];

        if (LCD_Touch_Scan(point, sizeof(point) / sizeof(point[0])) == sizeof(point) / sizeof(point[0])) {
            if ((point[0].x > 56) && (point[0].x < (LCD_Get_Width() - 56)) && (point[0].y > 138) && (point[0].y < (LCD_Get_Height() - 56))) {
                x = point[0].x;
                y = point[0].y;
            }
        }
#endif

        for (point_index = 0; point_index < 8; point_index++) {
            demo_rotate(cube[point_index], 0.05f, 0.03f, 0.02f);
        }

        for (line_index = 0; line_index < 24; line_index += 2) {
            LCD_Draw_Line(x + cube[line_id[line_index] - 1][0],
                          y + cube[line_id[line_index] - 1][1],
                          x + cube[line_id[line_index + 1] - 1][0],
                          y + cube[line_id[line_index + 1] - 1][1],
                          LCD_BLUE);
        }

        HAL_Delay(20);

        LCD_Fill(x - 56, y - 56, x + 56, y + 56, LCD_WHITE);
    }
}
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
