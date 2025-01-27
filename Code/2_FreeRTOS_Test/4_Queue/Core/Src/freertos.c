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
#include "key.h"
#include "oled.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct Message {
    uint8_t value;
    char buf[15];
} Msg;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* 存放队列句柄 */
osMessageQId xmsgqID;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* 任务句柄 */
osThreadId OLED_Task_Handle;
osThreadId LED_Task_Handle;
osThreadId Key_Task_Handle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* 自定义任务 */
void OLED_Task(void const *argument);
void LED_Task(void const *argument);
void Key_Task(void const *argument);
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
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    osMessageQDef(msgq, 10, Msg *);
    xmsgqID = osMessageCreate(osMessageQ(msgq), NULL);
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* 自定义任务 */
    osThreadDef(OLEDTask, OLED_Task, osPriorityNormal, 0, 128);
    OLED_Task_Handle = osThreadCreate(osThread(OLEDTask), NULL);

    osThreadDef(LEDTask, LED_Task, osPriorityNormal, 0, 128);
    LED_Task_Handle = osThreadCreate(osThread(LEDTask), NULL);

    osThreadDef(KeyTask, Key_Task, osPriorityNormal, 0, 128);
    Key_Task_Handle = osThreadCreate(osThread(KeyTask), NULL);
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
/**
 * @brief 按键任务---消息队列发送
 */
void Key_Task(void const *argument)
{
    osStatus ret_status;
    Msg msg;
    uint8_t key_value;
    for (;;) {
        key_value = Key_Scan();
        if (key_value != 0) {
            msg.value = key_value;
            strcpy(msg.buf, "key1");
            ret_status = osMessagePut(xmsgqID, (uint32_t)&msg, 0);
            if (ret_status != osOK) {
                OLED_ShowString(0, 7, (char *)("Error!"), 6, 0);
            }
        }
        osDelay(200);
    }
}

/**
 * @brief LED任务---消息队列接收
 */
void LED_Task(void const *argument)
{
    osEvent event;
    static uint8_t flag = 0;
    Msg *rx_msg         = NULL;
    for (;;) {
        event = osMessageGet(xmsgqID, osWaitForever);
        if (event.status == osEventMessage) {
            rx_msg = (Msg *)event.value.p;
            OLED_ShowString(0, 3, (char *)(&(rx_msg->value)), 6, 0);
            if (rx_msg->value == 1) {
                if (flag == 0) {
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
                    flag = 1;
                } else if (flag == 1) {
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
                    flag = 0;
                }
            }
        }
    }
    osDelay(200);
}

void OLED_Task(void const *argument)
{
    for (;;) {
        OLED_ShowString(0, 0, (char *)("Running"), 6, 0);
        osDelay(200);
    }
}
/* USER CODE END Application */
