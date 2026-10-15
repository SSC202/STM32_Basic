/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "freemodbus_app.h"
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
osThreadId_t MasterTaskHandle;
const osThreadAttr_t MasterTask_attributes = {
    .name       = "MasterTask",
    .priority   = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};
osThreadId_t SlaveTaskHandle;
const osThreadAttr_t SlaveTask_attributes = {
    .name       = "SlaveTask",
    .priority   = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void MasterTask(void *argument);
void SlaveTask(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

    // eMBInit(MB_RTU, 0x01, 2, 115200, MB_PAR_NONE);
    // eMBEnable();
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

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
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
    uint16_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    eMBMasterInit(MB_RTU, 2, 9600, MB_PAR_NONE);
    eMBMasterEnable();
    MasterTaskHandle = osThreadNew(MasterTask, NULL, &MasterTask_attributes);
    // SlaveTaskHandle  = osThreadNew(SlaveTask, NULL, &SlaveTask_attributes);
    /* Infinite loop */
    for (;;) {
        eMBMasterReqWriteMultipleHoldingRegister(1, 0, 10, data, portMAX_DELAY);
        for (uint8_t i = 0; i < sizeof(data) / sizeof(uint16_t); i++)
            data[i]++;
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
        osDelay(100);
    }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void MasterTask(void *argument)
{
    for (;;) {
        eMBMasterPoll();
        osDelay(1);
    }
}

void SlaveTask(void *argument)
{
    for (;;) {
        // eMBPoll();
        osDelay(1);
    }
}
/* USER CODE END Application */

