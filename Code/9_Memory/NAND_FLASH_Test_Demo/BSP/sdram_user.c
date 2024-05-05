#include "sdram_user.h"
#include "tim.h"

/**
 * @brief 微秒延时函数
 */
static void delay_us(uint32_t us)
{
    uint32_t cnt = 0;
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    __HAL_TIM_SET_AUTORELOAD(&htim2, us);
    HAL_TIM_Base_Start(&htim2);
    while (cnt != us) {
        cnt = __HAL_TIM_GET_COUNTER(&htim2);
    }
    HAL_TIM_Base_Stop(&htim2);
}

/**
 * @brief   SDRAM 发送指令函数
 */
static uint8_t SDRAM_SendCommand(uint32_t CommandMode, uint32_t Bank, uint32_t RefreshNum, uint32_t RegVal)
{
    uint32_t CommandTarget;
    FMC_SDRAM_CommandTypeDef Command;

    if (Bank == 1)
        CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    else if (Bank == 2)
        CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;

    Command.CommandMode            = CommandMode;
    Command.CommandTarget          = CommandTarget;
    Command.AutoRefreshNumber      = RefreshNum;
    Command.ModeRegisterDefinition = RegVal;
    if (HAL_SDRAM_SendCommand(&hsdram1, &Command, 0x1000) == HAL_OK)
        return 1;
    else
        return 0;
}

/**
 * @brief   SDRAM 初始化函数
 */
void SDRAM_Init(void)
{
    uint32_t temp;

    SDRAM_SendCommand(FMC_SDRAM_CMD_CLK_ENABLE, 1, 1, 0);       // 使能时钟信号
    delay_us(500);                                              // 至少延时200us
    SDRAM_SendCommand(FMC_SDRAM_CMD_PALL, 1, 1, 0);             // 发送全部预充电命令
    SDRAM_SendCommand(FMC_SDRAM_CMD_AUTOREFRESH_MODE, 1, 8, 0); // 设置自动刷新次数

    temp = SDRAM_MODEREG_BURST_LENGTH_1 |                   // 设置突发长度
           SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |            // 设置突发类型
           SDRAM_MODEREG_CAS_LATENCY_3 |                    // 设置CAS值
           SDRAM_MODEREG_OPERATING_MODE_STANDARD |          // 设置操作模式
           SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;            // 设置突发写模式：单点访问
    SDRAM_SendCommand(FMC_SDRAM_CMD_LOAD_MODE, 1, 1, temp); // 装载模式寄存器的值

    HAL_SDRAM_ProgramRefreshRate(&hsdram1, 683); // 设置刷新速率
}

/**
 * @brief   在指定地址(WriteAddr+Bank5_SDRAM_ADDR)开始,连续写入n个字节
 * @param   pBuffer     字节指针
 * @param   WriteAddr   要写入的地址
 * @param   n           要写入的字节数
 */
void SDRAM_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t n)
{
    for (; n != 0; n--) {
        *(volatile uint8_t *)(Bank5_SDRAM_ADDR + WriteAddr) = *pBuffer;
        WriteAddr++;
        pBuffer++;
    }
}

/**
 * @brief   在指定地址(WriteAddr+Bank5_SDRAM_ADDR)开始,连续读出n个字节
 * @param   pBuffer     字节指针
 * @param   WriteAddr   要读出的地址
 * @param   n           要读出的字节数
 */
void SDRAM_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t n)
{
    for (; n != 0; n--) {
        *pBuffer++ = *(volatile uint8_t *)(Bank5_SDRAM_ADDR + ReadAddr);
        ReadAddr++;
    }
}