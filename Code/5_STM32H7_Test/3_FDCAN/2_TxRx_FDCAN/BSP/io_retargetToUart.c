#include "io_retargetToUart.h"
#include "string.h"

// 串口号配置
// 注：仅 GCC 编译器支持 stderr 和 stdout 指定不同串口，ARMCC 编译器的 stderr 和 stdout 都使用 stdout_huart 输出。
static UART_HandleTypeDef *stdout_huart = &huart1;
static UART_HandleTypeDef *stderr_huart = &huart1;
static UART_HandleTypeDef *stdin_huart  = &huart1;

#ifdef IORETARGET_USE_RTOS
#include "cmsis_os.h"
#endif

#if (defined IORETARGET_STDIN_BUFFER_SIZE) && (IORETARGET_STDIN_BUFFER_SIZE > 0)
static IO_BufferQueue_t stdinBufferQueue = {
    .start      = 0,
    .end        = 0,
    .total_size = IORETARGET_STDIN_BUFFER_SIZE};

void IORetarget_Uart_Receive_IT()
{
    HAL_UART_Receive_IT(stdin_huart, (uint8_t *)&stdinBufferQueue.buffer[stdinBufferQueue.end], 1);
}

void IORetarget_Uart_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == stdin_huart) {
        stdinBufferQueue.end++;
        if (stdinBufferQueue.end >= stdinBufferQueue.total_size) stdinBufferQueue.end = 0;
        while (HAL_UART_Receive_IT(stdin_huart, (uint8_t *)&stdinBufferQueue.buffer[stdinBufferQueue.end], 1) == HAL_BUSY) {
            // 接收大量数据时，有概率发生 Overrun 错误，串口一直处于忙状态
            // 以下代码可以清除该状态
            __HAL_UART_CLEAR_OREFLAG(huart);
            huart->RxState = HAL_UART_STATE_READY;
            __HAL_UNLOCK(huart);
        }
    }
}

static int IORetarget_ReadStr(IO_BufferQueue_t *bufferQueue, char *pBuffer, int size)
{
    while (bufferQueue->start == bufferQueue->end) {
#ifdef IORETARGET_USE_RTOS
        osThreadYield();
#endif // IORETARGET_USE_RTOS
    }

    for (int i = 0; i < size; i++) {
        if (bufferQueue->start != bufferQueue->end) {
            *pBuffer = bufferQueue->buffer[bufferQueue->start++];
            pBuffer++;
            if (bufferQueue->start >= stdinBufferQueue.total_size) {
                stdinBufferQueue.start = 0;
            }
        } else {
            return i;
        }
    }
    return size;
}

#endif // (defined IORETARGET_STDIN_BUFFER_SIZE) && (IORETARGET_STDIN_BUFFER_SIZE > 0)

static void IORetarget_WriteStr(UART_HandleTypeDef *huart, const uint8_t *str, uint16_t size)
{
    while (HAL_UART_Transmit(huart, str, size, HAL_MAX_DELAY) == HAL_BUSY) {
#ifdef IORETARGET_USE_RTOS
        osThreadYield();
#endif // IORETARGET_USE_RTOS
    }
}

/**
 * @brief 从串口外设读取 1 个字符
 *
 * @param huart
 * @return char
 */
char IORetarget_ReadChar(UART_HandleTypeDef *huart)
{
    while (!__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE)) {
#ifdef IORETARGET_USE_RTOS
        osThreadYield();
#endif // IORETARGET_USE_RTOS
    }
    return (huart->Instance->RDR & (uint8_t)0x00FF);
}

// 判断是哪个编译器。GCC 会定义 __GNUC__，ARMCCv5 会定义 __ARMCC_VERSION，ARMCCv6 会定义 __GNUC__ 和 __ARMCC_VERSION
#if (defined __GNUC__) && (!defined __ARMCC_VERSION)

// GCC 编译器

#include <errno.h>
#include <sys/unistd.h>

/**
 * @brief Writes data to a file.
 *
 * @param fd File descriptor of file into which data is written.
 * @param pBuffer Data to be written.
 * @param size Number of bytes.
 * @return 成功则返回写入的字节数，否则返回 -1
 */
__attribute__((used)) int _write(int fd, char *pBuffer, int size)
{
    switch (fd) {
        case STDOUT_FILENO: // 标准输出流
            IORetarget_WriteStr(stdout_huart, (uint8_t *)pBuffer, size);
            break;
        case STDERR_FILENO: // 标准错误流
            IORetarget_WriteStr(stderr_huart, (uint8_t *)pBuffer, size);
            break;
        default:
            // EBADF, which means the file descriptor is invalid or the file isn't opened for writing;
            errno = EBADF;
            return -1;
            break;
    }
    return size;
}

/**
 * @brief Reads data from a file
 *
 * @param fd File descriptor referring to the open file.
 * @param pBuffer Storage location for data.
 * @param size Maximum number of bytes to read.
 */
__attribute__((used)) int _read(int fd, char *pBuffer, int size)
{
    if (size <= 0) {
        return 0;
    }

    switch (fd) {
        case STDIN_FILENO:
#if (defined IORETARGET_STDIN_BUFFER_SIZE) && (IORETARGET_STDIN_BUFFER_SIZE > 0)
            return IORetarget_ReadStr(&stdinBufferQueue, pBuffer, size);
#else
            *pBuffer = IORetarget_ReadChar(stdin_huart);
            return 1;
#endif
            break;

        default:
            // EBADF, which means the file descriptor is invalid or the file isn't opened for writing;
            errno = EBADF;
            return -1;
            break;
    }
}

#else

// MDK 的 ARMCC 编译器

#include <stdio.h>
#include <rt_sys.h>
#include <rt_misc.h>
#include <time.h>

/**
 *
 * @brief 将字符写入流
 *
 * @param ch 要写入的字符
 * @param stream 流
 * @return 返回写入的字符，但如果出现错误，返回 EOF
 */
int fputc(int ch, FILE *stream)
{
    IORetarget_WriteStr(stdout_huart, (uint8_t *)&ch, 1);
    return ch;
}

int fgetc(FILE *stream)
{
    (void)stream;
    char ch;
#if (defined IORETARGET_STDIN_BUFFER_SIZE) && (IORETARGET_STDIN_BUFFER_SIZE > 0)
    IORetarget_ReadStr(&stdinBufferQueue, &ch, 1);
#else
    ch = IORetarget_ReadChar(stdin_huart);
#endif
    return (int)ch;
}

// ARMCC 默认启用半主机模式，重定向 printf 后需要关闭，防止卡死
// ARMCCv6 和 ARMCCv5 关闭半主机模式的代码不一样，需要判断
#if __ARMCC_VERSION >= 6000000
// ARMCCv6
__asm(".global __use_no_semihosting");
#elif __ARMCC_VERSION >= 5000000
// ARMCCv5
#pragma import(__use_no_semihosting)
#else
#error Unsupported compiler
#endif

// 关闭半主机模式后，需要自己实现一些相关系统函数

const char __stdin_name[]  = ":tt";
const char __stdout_name[] = ":tt";
const char __stderr_name[] = ":tt";

FILEHANDLE _sys_open(const char *name, int openmode)
{
    return 1;
}

int _sys_close(FILEHANDLE fh)
{
    return 0;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
    return 0;
}

int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
{
    return -1;
}

void _ttywrch(int ch)
{
}

int _sys_istty(FILEHANDLE fh)
{
    return 0;
}

int _sys_seek(FILEHANDLE fh, long pos)
{
    return -1;
}

long _sys_flen(FILEHANDLE fh)
{
    return -1;
}

void _sys_exit(int return_code)
{
    while (1)
        ;
}

clock_t clock(void)
{
    clock_t tmp;
    return tmp;
}

void _clock_init(void)
{
}

time_t time(time_t *timer)
{
    time_t tmp;
    return tmp;
}

int system(const char *string)
{
    return 0;
}

char *getenv(const char *name)
{
    return NULL;
}

void _getenv_init(void)
{
}
#endif
