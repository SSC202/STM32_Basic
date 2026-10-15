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
#include "pcf8574.h"
#include "string.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "user_printf.h"
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
osThreadId_t netconn_tcp_client_TaskHandle;
const osThreadAttr_t netconn_tcp_client_Task_attributes = {
    .name       = "tcp_client_Task",
    .stack_size = 128 * 24,
    .priority   = (osPriority_t)(osPriorityNormal - 1),
};
void netconn_tcp_client_Task(void *argument);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

extern void MX_LWIP_Init(void);
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
    /* init code for LWIP */
    MX_LWIP_Init();
    /* USER CODE BEGIN StartDefaultTask */
    taskENTER_CRITICAL();
    netconn_tcp_client_TaskHandle = osThreadNew(netconn_tcp_client_Task, NULL, &netconn_tcp_client_Task_attributes);
    taskEXIT_CRITICAL();
    /* Infinite loop */
    for (;;) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
        osDelay(1000);
    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void netconn_tcp_client_Task(void *argument)
{

#define PORT    8087
#define IP_ADDR "192.168.137.1"

    int sock = -1;
    struct sockaddr_in client_addr;
    uint8_t send_buf[] = "This is a TCP Client test...\n";
    while (1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            printf("Socket error\n");
            vTaskDelay(10);
            continue;
        }
        client_addr.sin_family      = AF_INET;
        client_addr.sin_port        = htons(PORT);
        client_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
        memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));
        if (connect(sock,
                    (struct sockaddr *)&client_addr,
                    sizeof(struct sockaddr)) == -1) {
            printf("Connect failed!\n");
            closesocket(sock);
            vTaskDelay(10);
            continue;
        }
        printf("Connect to iperf server successful!\n");
        while (1) {
            if (write(sock, send_buf, sizeof(send_buf)) < 0)
                break;
            vTaskDelay(1000);
        }
        closesocket(sock);
    }
}

/* USER CODE END Application */
