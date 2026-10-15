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
osThreadId_t netconn_tcp_server_TaskHandle;
const osThreadAttr_t netconn_tcp_server_Task_attributes = {
    .name       = "tcp_server_Task",
    .stack_size = 128 * 24,
    .priority   = (osPriority_t)(osPriorityNormal - 1),
};
void netconn_tcp_server_Task(void *argument);

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
    netconn_tcp_server_TaskHandle = osThreadNew(netconn_tcp_server_Task, NULL, &netconn_tcp_server_Task_attributes);
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
void netconn_tcp_server_Task(void *argument)
{
#define PORT      8087
#define RECV_DATA (1024)

    int sock = -1, connected;
    char *recv_data;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;
    int recv_data_len;
    recv_data = (char *)pvPortMalloc(RECV_DATA);
    if (recv_data == NULL) {
        printf("No memory\n");
        goto __exit;
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket error\n");
        goto __exit;
    }
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(PORT);
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("Unable to bind\n");
        goto __exit;
    }
    if (listen(sock, 5) == -1) {
        printf("Listen error\n");
        goto __exit;
    }
    while (1) {
        sin_size  = sizeof(struct sockaddr_in);
        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
        printf("new client connected from (%s, %d)\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        {
            int flag = 1;

            setsockopt(connected,
                       IPPROTO_TCP,   /* set option at TCP level */
                       TCP_NODELAY,   /* name of option */
                       (void *)&flag, /* the cast is historical cruft */
                       sizeof(int));  /* length of option value */
        }
        while (1) {
            recv_data_len = recv(connected, recv_data, RECV_DATA, 0);
            if (recv_data_len <= 0)
                break;
            printf("recv %d len data\n", recv_data_len);
            write(connected, recv_data, recv_data_len);
        }
        if (connected >= 0)
            closesocket(connected);
        connected = -1;
    }
__exit:
    if (sock >= 0) closesocket(sock);
    if (recv_data) free(recv_data);
}

/* USER CODE END Application */
