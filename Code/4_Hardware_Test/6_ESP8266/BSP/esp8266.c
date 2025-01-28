#include "esp8266.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
/*********************************** 宏定义段 ********************************************/
/* 错误代码 */
#define ESP8266_OK      0 /* 没有错误 */
#define ESP8266_ERROR   1 /* 通用错误 */
#define ESP8266_TIMEOUT 2 /* 超时错误 */
#define ESP8266_INVAL   3 /* 参数错误 */
/* 收发缓冲区大小 */
#define ESP8266_RX_BUF_SIZE 128
#define ESP8266_TX_BUF_SIZE 64
/* 复位 */
#define ESP8266_RST(x)                                                                                                                                 \
    do {                                                                                                                                               \
        x ? HAL_GPIO_WritePin(ESP8266_RST_PORT, ESP8266_RST_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(ESP8266_RST_PORT, ESP8266_RST_PIN, GPIO_PIN_RESET); \
    } while (0)
/* 中断服务函数选择 */
#if (ESP8266_USART == 1)
#define ESP8266_USART_Handle &huart1
#define ESP8266_IRQHandler   USART1_IRQHandler
#elif (ESP8266_USART == 2)
#define ESP8266_USART_Handle &huart2
#define ESP8266_IRQHandler   USART2_IRQHandler
#elif (ESP8266_USART == 3)
#define ESP8266_USART_Handle huart3
#define ESP8266_IRQHandler   USART3_IRQHandler
#endif
/***********************************ESP8266 串口配置**************************************/

static struct
{
    uint8_t buf[ESP8266_RX_BUF_SIZE]; /* 帧接收缓冲 */
    struct
    {
        uint16_t len : 15;  /* 帧接收长度，sta[14:0] */
        uint16_t finsh : 1; /* 帧接收完成标志，sta[15] */
    } sta;                  /* 帧状态信息 */
} rx_frame = {0};           /* ESP8266 UART接收帧缓冲信息结构体 */

static uint8_t esp8266_tx_buffer[ESP8266_TX_BUF_SIZE]; /* ESP8266 UART发送缓冲 */

/**
 * @brief       ESP8266重定向
 * @param       fmt: 待打印的数据
 * @retval      无
 */
static void ESP8266_Printf(char *fmt, ...)
{
    va_list ap;
    uint16_t len;

    va_start(ap, fmt);
    vsprintf((char *)esp8266_tx_buffer, fmt, ap);
    va_end(ap);

    len = strlen((const char *)esp8266_tx_buffer);
    HAL_UART_Transmit(&ESP8266_USART_Handle, esp8266_tx_buffer, len, HAL_MAX_DELAY);
}

/**
 * @brief       ESP8266重新开始接收数据
 * @param       无
 * @retval      无
 */
static void ESP8266_Receive_Restart(void)
{
    rx_frame.sta.len   = 0;
    rx_frame.sta.finsh = 0;
}

/**
 * @brief       获取ESP8266接收到的一帧数据
 * @param       无
 * @retval      NULL: 未接收到一帧数据
 *              其他: 接收到的一帧数据
 */
static uint8_t *ESP8266_Receive_Frame(void)
{
    if (rx_frame.sta.finsh == 1) {
        rx_frame.buf[rx_frame.sta.len] = '\0';
        return rx_frame.buf;
    } else {
        return NULL;
    }
}

/**
 * @brief       获取ESP8266接收到的一帧数据的长度
 * @param       无
 * @retval      0   : 未接收到一帧数据
 *              其他: 接收到的一帧数据的长度
 */
static uint16_t ESP8266_Get_FrameLen(void)
{
    if (rx_frame.sta.finsh == 1) {
        return rx_frame.sta.len;
    } else {
        return 0;
    }
}

/**
 * @brief       ESP8266中断服务函数
 * @param       无
 * @retval      无
 */
void ESP8266_IRQHandler(void)
{
    uint8_t tmp;

    if (__HAL_UART_GET_FLAG(&ESP8266_USART_Handle, UART_FLAG_ORE) != RESET) /* UART接收过载错误中断 */
    {
        __HAL_UART_CLEAR_OREFLAG(&ESP8266_USART_Handle); /* 清除接收过载错误中断标志 */
        (void)ESP8266_USART_Handle.Instance->SR;         /* 先读SR寄存器，再读DR寄存器 */
        (void)ESP8266_USART_Handle.Instance->DR;
    }

    if (__HAL_UART_GET_FLAG(&ESP8266_USART_Handle, UART_FLAG_RXNE) != RESET) /* UART接收中断 */
    {
        HAL_UART_Receive(&ESP8266_USART_Handle, &tmp, 1, HAL_MAX_DELAY); /* UART接收数据 */

        if (rx_frame.sta.len < (ESP8266_RX_BUF_SIZE - 1)) /* 判断UART接收缓冲是否溢出
                                                           * 留出一位给结束符'\0'
                                                           */
        {
            rx_frame.buf[rx_frame.sta.len] = tmp; /* 将接收到的数据写入缓冲 */
            rx_frame.sta.len++;                   /* 更新接收到的数据长度 */
        } else                                    /* UART接收缓冲溢出 */
        {
            rx_frame.sta.len               = 0;   /* 覆盖之前收到的数据 */
            rx_frame.buf[rx_frame.sta.len] = tmp; /* 将接收到的数据写入缓冲 */
            rx_frame.sta.len++;                   /* 更新接收到的数据长度 */
        }
    }

    if (__HAL_UART_GET_FLAG(&ESP8266_USART_Handle, UART_FLAG_IDLE) != RESET) /* UART总线空闲中断 */
    {
        rx_frame.sta.finsh = 1; /* 标记帧接收完成 */

        __HAL_UART_CLEAR_IDLEFLAG(&ESP8266_USART_Handle); /* 清除UART总线空闲中断 */
    }
}
/***********************************ESP8266 驱动程序**************************************/
/**
 *@brief 微秒延时函数
 *@param us：微秒数
 */
static void delay_us(uint16_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (delay--) {
        ;
    }
}

/**
 *@brief 毫秒延时函数
 *@param ms：微秒数
 */
static void delay_ms(uint16_t ms)
{
    for (int i = 0; i < ms; i++) {
        delay_us(1000);
    }
}

/**
 * @brief       ESP8266硬件复位
 * @param       无
 * @retval      无
 */
static void ESP8266_Reset(void)
{
    ESP8266_RST(0);
    delay_ms(100);
    ESP8266_RST(1);
    delay_ms(500);
}

/**
 * @brief       ESP8266发送AT指令
 * @param       cmd    : 待发送的AT指令
 * @param       ack    : 等待的响应
 * @param       timeout: 等待超时时间
 * @retval      ESP8266_OK     : 函数执行成功
 *              ESP8266_TIMEOUT: 等待期望应答超时，函数执行失败
 */
static uint8_t ESP8266_AT_Cmd(char *cmd, char *ack, uint32_t timeout)
{
    uint8_t *ret = NULL;

    ESP8266_Receive_Restart();
    ESP8266_Printf("%s\r\n", cmd);

    if ((ack == NULL) || (timeout == 0)) {
        return ESP8266_OK;
    } else {
        while (timeout > 0) {
            ret = ESP8266_Receive_Frame();
            if (ret != NULL) {
                if (strstr((const char *)ret, ack) != NULL) {
                    return ESP8266_OK;
                } else {
                    ESP8266_Receive_Restart();
                }
            }
            timeout--;
            delay_ms(1);
        }
        return ESP8266_TIMEOUT;
    }
}

/***************************************ESP8266 用户函数***************************************/
/**
 * @brief       ESP8266初始化
 * @param       无
 * @retval      ATK_MW8266D_EOK  : ESP8266初始化成功，函数执行成功
 *              ATK_MW8266D_ERROR: ATK-MW8266D初始化失败，函数执行失败
 */
uint8_t ESP8266_Init(void)
{
    ESP8266_Reset();                     /* ESP8266硬件复位 */
    if (ESP8266_AT_Test() == ESP8266_OK) /* ESP8266指令测试 */
    {
        return ESP8266_OK;
    }
    return ESP8266_ERROR;
}

/**
 * @brief       ESP8266恢复出厂设置
 * @param       无
 * @retval      ESP8266_OK  : 恢复出厂设置成功
 *              ESP8266_ERROR: 恢复出厂设置失败
 */
uint8_t ESP8266_Restore(void)
{
    uint8_t ret;

    ret = ESP8266_AT_Cmd("AT+RESTORE", "ready", 3000);
    if (ret == ESP8266_OK) {
        return ESP8266_OK;
    } else {
        return ESP8266_ERROR;
    }
}

/**
 * @brief       ESP8266 AT指令测试
 * @param       无
 * @retval      ESP8266_OK  : AT指令测试成功
 *              ESP8266_ERROR: AT指令测试失败
 */
uint8_t ESP8266_AT_Test(void)
{
    uint8_t ret;
    uint8_t i;

    for (i = 0; i < 10; i++) {
        ret = ESP8266_AT_Cmd("AT", "OK", 500);
        if (ret == ESP8266_OK) {
            return ESP8266_OK;
        }
    }
    return ESP8266_ERROR;
}

/**
 * @brief       设置ESP8266工作模式
 * @param       mode: 1，Station模式
 *                    2，AP模式
 *                    3，AP+Station模式
 * @retval      ESP8266_OK      : 工作模式设置成功
 *              ESP8266_ERROR   : 工作模式设置失败
 *              ESP8266_INVAL:  mode参数错误，工作模式设置失败
 */
uint8_t ESP8266_Mode(uint8_t mode)
{
    uint8_t ret;

    switch (mode) {
        case 1: {
            ret = ESP8266_AT_Cmd("AT+CWMODE=1", "OK", 500); /* Station模式 */
            break;
        }
        case 2: {
            ret = ESP8266_AT_Cmd("AT+CWMODE=2", "OK", 500); /* AP模式 */
            break;
        }
        case 3: {
            ret = ESP8266_AT_Cmd("AT+CWMODE=3", "OK", 500); /* AP+Station模式 */
            break;
        }
        default: {
            return ESP8266_INVAL;
        }
    }

    if (ret == ESP8266_OK) {
        return ESP8266_OK;
    } else {
        return ESP8266_ERROR;
    }
}

/**
 * @brief       ATK-MW8266D软件复位
 * @param       无
 * @retval      ESP8266_OK      : 软件复位成功
 *              ESP8266_ERROR   : 软件复位失败
 */
uint8_t ESP8266_Software_RST(void)
{
    uint8_t ret;

    ret = ESP8266_AT_Cmd("AT+RST", "OK", 500);
    if (ret == ESP8266_OK) {
        return ESP8266_OK;
    } else {
        return ESP8266_ERROR;
    }
}

/**
 * @brief       ESP8266设置回显模式
 * @param       cfg: 0，关闭回显
 *                   1，打开回显
 * @retval      ESP8266_OK  : 设置回显模式成功
 *              ESP8266_ERROR: 设置回显模式失败
 */
uint8_t ESP8266_ATE_Config(uint8_t cfg)
{
    uint8_t ret;

    switch (cfg) {
        case 0: {
            ret = ESP8266_AT_Cmd("ATE0", "OK", 500); /* 关闭回显 */
            break;
        }
        case 1: {
            ret = ESP8266_AT_Cmd("ATE1", "OK", 500); /* 打开回显 */
            break;
        }
        default: {
            return ESP8266_INVAL;
        }
    }

    if (ret == ESP8266_OK) {
        return ESP8266_OK;
    } else {
        return ESP8266_ERROR;
    }
}

/**
 * @brief       ESP8266连接WIFI
 * @param       ssid: WIFI名称
 *              pwd : WIFI密码
 * @retval      ESP8266_OK      : WIFI连接成功
 *              ESP8266_ERROR   : WIFI连接失败
 */
uint8_t ESP8266_JOIN_AP(char *ssid, char *pwd)
{
    uint8_t ret;
    char cmd[64];

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
    ret = ESP8266_AT_Cmd(cmd, "WIFI GOT IP", 10000);
    if (ret == ESP8266_OK) {
        return ESP8266_OK;
    } else {
        return ESP8266_ERROR;
    }
}

/**
 * @brief       ESP8266获取IP地址
 * @param       buf: IP地址，需要16字节内存空间
 * @retval      ESP8266_OK      : 获取IP地址成功
 *              ESP8266_ERROR   : 获取IP地址失败
 */
uint8_t ESP8266_GET_IP(char *buf)
{
    uint8_t ret;
    char *p_start;
    char *p_end;

    ret = ESP8266_AT_Cmd("AT+CIFSR", "OK", 500);
    if (ret != ESP8266_OK) {
        return ESP8266_ERROR;
    }

    p_start = strstr((const char *)ESP8266_Receive_Frame(), "\"");
    p_end   = strstr(p_start + 1, "\"");
    *p_end  = '\0';
    sprintf(buf, "%s", p_start + 1);

    return ESP8266_OK;
}

/**
 * @brief       ESP8266连接TCP服务器
 * @param       server_ip  : TCP服务器IP地址
 *              server_port: TCP服务器端口号
 * @retval      ESP8266_OK      : 连接TCP服务器成功
 *              ESP8266_ERROR   : 连接TCP服务器失败
 */
uint8_t ESP8266_Connect_TCP_Server(char *server_ip, char *server_port)
{
    uint8_t ret;
    char cmd[64];

    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", server_ip, server_port);
    ret = ESP8266_AT_Cmd(cmd, "CONNECT", 5000);
    if (ret == ESP8266_OK) {
        return ESP8266_OK;
    } else {
        return ESP8266_ERROR;
    }
}

/**
 * @brief       ESP8266进入透传
 * @param       无
 * @retval      ESP8266_OK      : 进入透传成功
 *              ESP8266_ERROR   : 进入透传失败
 */
uint8_t ESP8266_Enter_Unvarnished(void)
{
    uint8_t ret;

    ret = ESP8266_AT_Cmd("AT+CIPMODE=1", "OK", 500);
    ret += ESP8266_AT_Cmd("AT+CIPSEND", ">", 500);
    if (ret == ESP8266_OK) {
        return ESP8266_OK;
    } else {
        return ESP8266_ERROR;
    }
}

/**
 * @brief       ATK-MW8266D退出透传
 * @param       无
 * @retval      无
 */
void ESP8266_Quit_Unvarnished(void)
{
    ESP8266_Printf("+++");
}

/**
 * @brief       ATK-MW8266D连接原子云服务器
 * @param       id : 原子云设备编号
 *              pwd: 原子云设备密码
 * @retval      ATK_MW8266D_EOK  : 连接原子云服务器成功
 *              ATK_MW8266D_ERROR: 连接原子云服务器失败
 */
uint8_t ESP8266_Connect_TCP_ATK(char *id, char *pwd)
{
    uint8_t ret;
    char cmd[64];

    sprintf(cmd, "AT+ATKCLDSTA=\"%s\",\"%s\"", id, pwd);
    ret = ESP8266_AT_Cmd(cmd, "CLOUD CONNECTED", 10000);
    if (ret == ESP8266_OK) {
        return ESP8266_OK;
    } else {
        return ESP8266_ERROR;
    }
}

/**
 * @brief       ATK-MW8266D断开原子云服务器连接
 * @param       无
 * @retval      ATK_MW8266D_EOK  : 断开原子云服务器连接成功
 *              ATK_MW8266D_ERROR: 断开原子云服务器连接失败
 */
uint8_t ESP8266_Disconnect_TCP_ATK(void)
{
    uint8_t ret;

    ret = ESP8266_AT_Cmd("AT+ATKCLDCLS", "CLOUD DISCONNECT", 500);
    if (ret == ESP8266_OK) {
        return ESP8266_OK;
    } else {
        return ESP8266_ERROR;
    }
}
/*********************************ESP8266 用户函数****************************************/
/**
 * @brief       显示IP地址
 * @param       无
 * @retval      无
 */
static void ESP8266_Show_IP(char *buf)
{
    printf("IP: %s\r\n", buf);
}

/**
 * @brief       功能测试
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 * @retval      无
 */
static void ESP8266_AP_Test(uint8_t is_unvarnished)
{
    uint8_t ret;

    if (is_unvarnished == 0) {
        /* 进行AT指令测试 */
        ret = ESP8266_AT_Test();
        if (ret == 0) {
            printf("AT test success!\r\n");
        } else {
            printf("AT test failed!\r\n");
        }
    } else {
        /* 通过透传，发送信息至TCP Server */
        ESP8266_Printf("This ATK-MW8266D TCP Connect Test.\r\n");
    }
}

/**
 * @brief       切换透传模式
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 * @retval      无
 */
static void ESP8266_AP_Change(uint8_t *is_unvarnished)
{
    uint8_t ret;

    if (*is_unvarnished == 0) {
        /* 进入透传 */
        ret = ESP8266_Enter_Unvarnished();
        if (ret == 0) {
            *is_unvarnished = 1;
            printf("Enter unvarnished!\r\n");
        } else {
            printf("Enter unvarnished failed!\r\n");
        }
    } else {
        /* 退出透传 */
        ESP8266_Quit_Unvarnished();
        *is_unvarnished = 0;
        printf("Exit unvarnished!\r\n");
    }
}

/**
 * @brief       进入透传时，将接收自TCP Server的数据发送到串口调试助手
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 * @retval      无
 */
static void ESP8266_Get_Data(uint8_t is_unvarnished)
{
    uint8_t *buf;

    if (is_unvarnished == 1) {
        /* 接收来自ATK-MW8266D UART的一帧数据 */
        buf = ESP8266_Receive_Frame();
        if (buf != NULL) {
            printf("%s", buf);
            /* 重开开始接收来自ATK-MW8266D UART的数据 */
            ESP8266_Receive_Restart();
        }
    }
}

/**
 * @brief       用户函数
 * @param       无
 * @retval      无
 */
void ESP8266_Run(void)
{
    uint8_t ret;
    char ip_buf[16];
    uint8_t is_unvarnished = 0;

    /* 初始化ATK-MW8266D */
    ret = ESP8266_Init();
    if (ret == 0) {
        printf("Init success! ret = %d\r\n", ret);
    } else {
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
        }
    }
    printf("Joining to AP... ret = %d\r\n", ret);
    ret += ESP8266_Restore(); /* 恢复出厂设置 */
    if (ret == 0) {
        printf("Restore success! ret = %d\r\n", ret);
    } else {
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
        }
    }
    ret += ESP8266_AT_Test(); /* AT测试 */
    if (ret == 0) {
        printf("AT success! ret = %d\r\n", ret);
    } else {
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
        }
    }
    ret += ESP8266_Mode(1); /* Station模式 */
    if (ret == 0) {
        printf("Mode set success! ret = %d\r\n", ret);
    } else {
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
        }
    }
    ret += ESP8266_Software_RST(); /* 软件复位 */
    if (ret == 0) {
        printf("RST success! ret = %d\r\n", ret);
    } else {
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
        }
    }
    ret += ESP8266_ATE_Config(0); /* 关闭回显功能 */
    if (ret == 0) {
        printf("ate config success! ret = %d\r\n", ret);
    } else {
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
        }
    }
    ret += ESP8266_JOIN_AP(WIFI_SSID, WIFI_PWD); /* 连接WIFI */
    if (ret == 0) {
        printf("WIFI Connect success! ret = %d\r\n", ret);
    } else {
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
        }
    }
    ret += ESP8266_GET_IP(ip_buf); /* 获取IP地址 */
    if (ret == 0) {
        printf("IP success! ret = %d\r\n", ret);
    } else {
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
        }
    }
    ESP8266_Show_IP(ip_buf);

    /* 连接TCP服务器 */
    ret += ESP8266_Connect_TCP_Server(TCP_SERVER_IP, TCP_SERVER_PORT);
    if (ret == 0) {
        printf("TCP success! ret = %d\r\n", ret);
    } else {
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
        }
    }
    printf("TCP success!\r\n");

    /* 重新开始接收新的一帧数据 */
    ESP8266_Receive_Restart();
    ESP8266_Enter_Unvarnished();
    is_unvarnished = 1;
    printf("Unvarnished!\r\n");

    while (1) {
        /* 发送透传接收自TCP Server的数据到串口调试助手 */
        ESP8266_Get_Data(is_unvarnished);
        delay_ms(10);
    }
}