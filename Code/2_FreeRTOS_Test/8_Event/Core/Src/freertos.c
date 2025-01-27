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
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define KEY1_EVENT (0x01 << 0)
#define KEY2_EVENT (0x01 << 1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* 事件标志组定�? */
osEventFlagsId_t event;
const osEventFlagsAttr_t event_attributes = {
    .name = "event",
};
/* 线程定义 */
osThreadId_t Key_Task_Handle;
const osThreadAttr_t KeyTask_attributes = {
    .name       = "KeyTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
osThreadId_t Read_Task_Handle;
const osThreadAttr_t ReadTask_attributes = {
    .name       = "ReadTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name       = "defaultTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Read_Task(void *argument);
void Key_Task(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    Key_Task_Handle  = osThreadNew(Key_Task, NULL, &KeyTask_attributes);
    Read_Task_Handle = osThreadNew(Read_Task, NULL, &ReadTask_attributes);
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    event = osEventFlagsNew(&event_attributes);
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
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
void Key_Task(void *argument)
{
    for (;;) {
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == 0) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
            printf("Key1 Down!\r\n");
            osEventFlagsSet(event, KEY1_EVENT);
        }
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == 0) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
            printf("Key2 Down!\r\n");
            osEventFlagsSet(event, KEY2_EVENT);
        }
        osDelay(100);
    }
}

void Read_Task(void *argument)
{
    uint32_t rx_event;
    for (;;) {
        rx_event = osEventFlagsWait(event, KEY1_EVENT | KEY2_EVENT, osFlagsWaitAll, osWaitForever);
        if ((rx_event & (KEY1_EVENT | KEY2_EVENT)) == (KEY1_EVENT | KEY2_EVENT)) {
            printf("Key1 and Key2 pressed!\r\n");
        } else {
            printf("error!\r\n");
        }
    }
}
/* USER CODE END Application */
