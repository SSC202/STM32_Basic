/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* 互斥信号量创建 */
osMutexId xMute_Semaphore;

osThreadId Read_Task_Handle;
osThreadId Write_Task_Handle;
osThreadId LED_Task_Handle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Read_Task(void const *argument);
void Write_Task(void const *argument);
void LED_Task(void const *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer   = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
    /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    osMutexDef(mutex);
    xMute_Semaphore = osMutexCreate(osMutex(mutex));
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    osThreadDef(LEDTask, LED_Task, osPriorityNormal, 0, 128);
    LED_Task_Handle = osThreadCreate(osThread(LEDTask), NULL);
    osThreadDef(ReadTask, Read_Task, osPriorityHigh, 0, 128);
    Read_Task_Handle = osThreadCreate(osThread(ReadTask), NULL);
    osThreadDef(WriteTask, Write_Task, osPriorityLow, 0, 128);
    Write_Task_Handle = osThreadCreate(osThread(WriteTask), NULL);
    /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void LED_Task(void const *argument)
{
    for (;;) {
        OLED_Clear();
        OLED_ShowString(0, 2, (char *)("Hello World"), 6, 0);
        osDelay(1000);
    }
}

void Write_Task(void const *argument)
{
    static uint32_t t;
    osStatus xstatus;
    for (;;) {
        xstatus = osMutexWait(xMute_Semaphore, osWaitForever);
        if (xstatus == osOK) {
            OLED_Clear();
            OLED_ShowString(0, 0, (char *)("Writeing>>>"), 6, 0);
            for (t = 0; t < 200000; t++) {
                taskYIELD();
            }
        }
        osMutexRelease(xMute_Semaphore);
        osDelay(1000);
    }
}

void Read_Task(void const *argument)
{
    static uint32_t t;
    osStatus xstatus;
    for (;;) {
        xstatus = osMutexWait(xMute_Semaphore, osWaitForever);
        if (xstatus == osOK) {
            OLED_Clear();
            OLED_ShowString(0, 0, (char *)("Reading<<<"), 6, 0);
            for (t = 0; t < 200000; t++) {
                taskYIELD();
            }
        }
        osMutexRelease(xMute_Semaphore);
        osDelay(1000);
    }
}
/* USER CODE END Application */
