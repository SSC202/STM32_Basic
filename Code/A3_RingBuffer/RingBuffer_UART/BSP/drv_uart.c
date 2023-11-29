#include "usart.h"
#include "dma.h"
#include "stdarg.h"
#include "drv_uart.h"

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
static uint8_t usart1_rx_buff[USART1_RX_BUFFER_SIZE];
static uint8_t usart1_tx_buff[USART1_TX_BUFFER_SIZE];
static uint8_t usart1_tx_fifo_buff[USART1_TX_FIFO_SIZE];

usart_manage_obj_t usart1_manage_obj = {0};

static void usart_rec_to_buff(usart_manage_obj_t *m_obj, interrput_type int_type);
static void usart_transmit_hook(usart_manage_obj_t *m_obj);

/********************** Custom API **********************************************/
void debug_raw_printf(char *fmt, ...)
{
    va_list arg;
    uint8_t buff[USART1_PRINTF_BUFF_SIZE];
    uint8_t printf_len;

    va_start(arg, fmt);
    printf_len = vsnprintf((char *)buff, USART1_PRINTF_BUFF_SIZE, fmt, arg);
    va_end(arg);

    if (printf_len > USART1_PRINTF_BUFF_SIZE) {
        printf_len = USART1_PRINTF_BUFF_SIZE;
    }

    HAL_UART_Transmit(&huart1, (uint8_t *)buff, printf_len, 0xFFFF);
}

/********************** Custom API **********************************************/

/**
 * @brief   USART1 空闲中断回调函数
 */
void usart1_idle_callback(void)
{
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        usart_rec_to_buff(&usart1_manage_obj, INTERRUPT_TYPE_UART);
    }
}

/**
 * @brief   串口1初始化函数
 */
void usart1_manage_init(void)
{
    usart1_manage_obj.rx_buffer      = usart1_rx_buff;
    usart1_manage_obj.rx_buffer_size = USART1_RX_BUFFER_SIZE;
    usart1_manage_obj.dma_h          = &hdma_usart1_rx;
    usart1_manage_obj.uart_h         = &huart1;
    usart1_manage_obj.tx_fifo_buffer = usart1_tx_fifo_buff;
    usart1_manage_obj.tx_fifo_size   = USART1_TX_FIFO_SIZE;
    usart1_manage_obj.tx_buffer_size = USART1_TX_BUFFER_SIZE;
    usart1_manage_obj.tx_buffer      = usart1_tx_buff;
    usart1_manage_obj.is_sending     = 0;

    fifo_s_init(&(usart1_manage_obj.tx_fifo), usart1_tx_fifo_buff, USART1_TX_FIFO_SIZE);

    HAL_UART_Receive_DMA(&huart1, usart1_rx_buff, USART1_RX_BUFFER_SIZE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

/**
 * @brief 串口1的重定向函数
 */
int usart1_printf(char *fmt, ...)
{
    va_list arg;
    uint8_t buff[USART1_PRINTF_BUFF_SIZE];
    uint8_t printf_len;

    va_start(arg, fmt);
    printf_len = vsnprintf((char *)buff, USART1_PRINTF_BUFF_SIZE, fmt, arg);
    va_end(arg);

    if (printf_len > USART1_PRINTF_BUFF_SIZE) {
        printf_len = USART1_PRINTF_BUFF_SIZE;
    }

    usart_transmit(&usart1_manage_obj, buff, printf_len);

    return 0;
}

/**
 * @brief   串口1的发送函数
 * @param   buff    发送数据地址
 * @param   len     发送数据长度
 */
void usart1_transmit(uint8_t *buff, uint16_t len)
{
    usart_transmit(&usart1_manage_obj, buff, len);
}

/**
 * @brief   串口1的回调函数注册函数
 * @param   fun 回调函数
 */
void usart1_rx_callback_register(usart_call_back fun)
{
    usart_rx_callback_register(&usart1_manage_obj, fun);
    return;
}

/********************** Usual API **********************************************/
/**
 * @brief  register uart callback function.
 * @param
 * @retval void
 */
void usart_rx_callback_register(usart_manage_obj_t *m_obj, usart_call_back fun)
{
    m_obj->call_back_f = fun;
    return;
}

/**
 * @brief  rx dma half complete interupt
 * @param
 * @retval void
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        usart_rec_to_buff(&usart1_manage_obj, INTERRUPT_TYPE_DMA_HALF);
    }
    return;
}

/**
 * @brief  rx dma complete interupt
 * @param
 * @retval void
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        usart_rec_to_buff(&usart1_manage_obj, INTERRUPT_TYPE_DMA_ALL);
    }
    return;
}

/**
 * @brief  tx complete interupt
 * @param
 * @retval void
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        usart_transmit_hook(&usart1_manage_obj);
    }
    return;
}

/**
 * @brief  uart fifo transmit
 * @param
 * @retval error code
 */
int32_t usart_transmit(usart_manage_obj_t *m_obj, uint8_t *buf, uint16_t len)
{
    uint16_t to_send_len;    // 存储要发送的数据长度
    uint16_t to_tx_fifo_len; // 放入发送FIFO缓冲区的数据长度

    // 如果当前不处于发送状态，则直接进入发送状态并发送
    if (m_obj->is_sending == 0) {
        // 如果数据长度小于发送缓冲区大小
        if (len < m_obj->tx_buffer_size) {
            to_send_len    = len;
            to_tx_fifo_len = 0;
        }
        // 如果数据长度小于发送缓冲区大小加上发送FIFO缓冲区大小
        else if (len < m_obj->tx_buffer_size + m_obj->tx_fifo_size) {
            to_send_len    = m_obj->tx_buffer_size;
            to_tx_fifo_len = len - m_obj->tx_buffer_size;
        }
        // 如果数据长度大于等于发送缓冲区和发送FIFO缓冲区的总和
        else {
            to_send_len    = m_obj->tx_buffer_size;
            to_tx_fifo_len = m_obj->tx_fifo_size;
        }
    }
    //  如果当前处于发送状态，暂存于发送FIFO
    else {
        if (len < m_obj->tx_fifo_size) {
            to_send_len    = 0;
            to_tx_fifo_len = len;
        } else {
            to_send_len    = 0;
            to_tx_fifo_len = m_obj->tx_fifo_size;
        }
    }
    // 复制数据到发送缓冲区并启动DMA传输
    if (to_send_len > 0) {
        memcpy(m_obj->tx_buffer, buf, to_send_len);
        m_obj->is_sending = 1;
        HAL_UART_Transmit_DMA(m_obj->uart_h, m_obj->tx_buffer, to_send_len);
    }
    // 将数据放入发送FIFO缓冲区
    if (to_tx_fifo_len > 0) {
        uint8_t len;
        len = fifo_s_puts(&(m_obj->tx_fifo), (char *)(buf) + to_send_len, to_tx_fifo_len);

        if (len != to_tx_fifo_len) {
            return -1;
        }
    }

    return 0;
}

/**
 * @brief   在USART传输完成时的钩子函数
 */
static void usart_transmit_hook(usart_manage_obj_t *m_obj)
{
    uint16_t fifo_data_num = 0;
    uint16_t send_num      = 0;

    fifo_data_num = m_obj->tx_fifo.used_num; // 获取FIFO缓冲区中的数据数量

    // 判断是否有数据要发送
    // 如果FIFO缓冲区中有数据，执行相应的操作，否则将 is_sending 置为 0，表示当前没有正在发送的数据。
    if (fifo_data_num != 0) {
        // 确定要发送的数据数量
        if (fifo_data_num < m_obj->tx_buffer_size) {
            send_num = fifo_data_num;
        } else {
            send_num = m_obj->tx_buffer_size;
        }
        // 从FIFO缓冲区获取数据，并将数据复制到发送缓冲区
        fifo_s_gets(&(m_obj->tx_fifo), (char *)(m_obj->tx_buffer), send_num);
        m_obj->is_sending = 1;
        HAL_UART_Transmit_DMA(m_obj->uart_h, m_obj->tx_buffer, send_num);
    } else {
        m_obj->is_sending = 0;
    }
    return;
}

/**
 * @brief  rx to fifo
 * @param
 * @retval void
 */
static void usart_rec_to_buff(usart_manage_obj_t *m_obj, interrput_type int_type)
{
    uint16_t read_end_ptr        = 0;
    uint16_t read_length         = 0;
    uint16_t read_success_length = 0;
    uint16_t read_start_ptr      = m_obj->read_start_index;
    uint8_t *pdata               = m_obj->rx_buffer;

    UNUSED(read_success_length);

    uint16_t buff_left = m_obj->dma_h->Instance->CNDTR; // 获取DMA缓冲区剩余数据量(DMA自带缓冲区)

    // 根据不同的中断类型，计算读取结束的位置。
    // 如果是UART中断，结束位置为接收缓冲区的末尾减去DMA缓冲区剩余数据的数量；
    // 如果是DMA半传输中断，结束位置为接收缓冲区的一半；
    // 如果是DMA全传输中断，结束位置为接收缓冲区的末尾。
    if (int_type == INTERRUPT_TYPE_UART) {
        read_end_ptr = m_obj->rx_buffer_size - buff_left;
    }

    if (int_type == INTERRUPT_TYPE_DMA_HALF) {
        read_end_ptr = m_obj->rx_buffer_size / 2;
    }

    if (int_type == INTERRUPT_TYPE_DMA_ALL) {
        read_end_ptr = m_obj->rx_buffer_size;
    }

    //  计算实际要读取的数据长度
    read_length = read_end_ptr - m_obj->read_start_index;

    // 如果注册了回调函数，则从接收缓冲区中获取数据，并调用回调函数进行处理。
    if (m_obj->call_back_f != NULL) {
        uint8_t *read_ptr   = pdata + read_start_ptr;
        read_success_length = m_obj->call_back_f(read_ptr, read_length);
    }
    // 更新读取的起始位置，采用循环队列的方式，确保索引不超过接收缓冲区的大小
    m_obj->read_start_index = (m_obj->read_start_index + read_length) % (m_obj->rx_buffer_size);

    return;
}
