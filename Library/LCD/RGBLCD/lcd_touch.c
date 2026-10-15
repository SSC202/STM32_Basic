#include "lcd_touch.h"
#include <string.h>
#include "tim.h"
#if (LCD_USING_TOUCH != 0)
/************************IIC 通讯协议******************************/
/********宏定义段*********/

/* 引脚定义 */
#define LCD_TOUCH_IIC_SCL_GPIO_PORT GPIOH
#define LCD_TOUCH_IIC_SCL_GPIO_PIN  GPIO_PIN_6
#define LCD_TOUCH_IIC_SDA_GPIO_PORT GPIOI
#define LCD_TOUCH_IIC_SDA_GPIO_PIN  GPIO_PIN_3

/* IO操作 */
#define LCD_TOUCH_IIC_SCL(x)                                                                                                                                                                       \
    do {                                                                                                                                                                                           \
        x ? HAL_GPIO_WritePin(LCD_TOUCH_IIC_SCL_GPIO_PORT, LCD_TOUCH_IIC_SCL_GPIO_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(LCD_TOUCH_IIC_SCL_GPIO_PORT, LCD_TOUCH_IIC_SCL_GPIO_PIN, GPIO_PIN_RESET); \
    } while (0)

#define LCD_TOUCH_IIC_SDA(x)                                                                                                                                                                       \
    do {                                                                                                                                                                                           \
        x ? HAL_GPIO_WritePin(LCD_TOUCH_IIC_SDA_GPIO_PORT, LCD_TOUCH_IIC_SDA_GPIO_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(LCD_TOUCH_IIC_SDA_GPIO_PORT, LCD_TOUCH_IIC_SDA_GPIO_PIN, GPIO_PIN_RESET); \
    } while (0)

#define LCD_TOUCH_IIC_READ_SDA() HAL_GPIO_ReadPin(LCD_TOUCH_IIC_SDA_GPIO_PORT, LCD_TOUCH_IIC_SDA_GPIO_PIN)

/* 控制IIC通讯是读操作还是写操作 */
#define LCD_TOUCH_IIC_WRITE 0
#define LCD_TOUCH_IIC_READ  1

/* 错误代码 */
#define LCD_TOUCH_IIC_EOK   0 /* 没有错误 */
#define LCD_TOUCH_IIC_ERROR 1 /* 错误 */
/********函数定义*********/
/**
 * @brief 微秒延时函数
 */
static void delay_us(uint32_t us)
{
    uint32_t cnt = 0;
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    __HAL_TIM_SET_AUTORELOAD(&htim6, us);
    HAL_TIM_Base_Start(&htim6);
    while (cnt != us) {
        cnt = __HAL_TIM_GET_COUNTER(&htim6);
    }
    HAL_TIM_Base_Stop(&htim6);
}

/**
 * @brief 微秒延时函数
 */
static void delay_ms(uint32_t ms)
{
    while (ms) {
        delay_us(1000);
        ms--;
    }
}

/**
 * @brief       IIC接口延时函数，用于控制IIC读写速度
 * @param       无
 * @retval      无
 */
static inline void IIC_Delay(void)
{
    delay_us(2);
}

/**
 * @brief       产生IIC起始信号
 * @param       无
 * @retval      无
 */
static void IIC_Start(void)
{
    LCD_TOUCH_IIC_SDA(1);
    LCD_TOUCH_IIC_SCL(1);
    IIC_Delay();
    LCD_TOUCH_IIC_SDA(0);
    IIC_Delay();
    LCD_TOUCH_IIC_SCL(0);
    IIC_Delay();
}

/**
 * @brief       产生IIC停止信号
 * @param       无
 * @retval      无
 */
static void IIC_Stop(void)
{
    LCD_TOUCH_IIC_SDA(0);
    IIC_Delay();
    LCD_TOUCH_IIC_SCL(1);
    IIC_Delay();
    LCD_TOUCH_IIC_SDA(1);
    IIC_Delay();
}

/**
 * @brief       等待IIC应答信号
 * @param       无
 * @retval      0: 应答信号接收成功
 *              1: 应答信号接收失败
 */
static uint8_t IIC_Wait_Ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack     = 0;

    LCD_TOUCH_IIC_SDA(1);
    IIC_Delay();
    LCD_TOUCH_IIC_SCL(1);
    IIC_Delay();

    while (LCD_TOUCH_IIC_READ_SDA()) {
        waittime++;

        if (waittime > 250) {
            IIC_Stop();
            rack = 1;
            break;
        }
    }

    LCD_TOUCH_IIC_SCL(0);
    IIC_Delay();

    return rack;
}

/**
 * @brief       产生ACK应答信号
 * @param       无
 * @retval      无
 */
static void IIC_Ack(void)
{
    LCD_TOUCH_IIC_SDA(0);
    IIC_Delay();
    LCD_TOUCH_IIC_SCL(1);
    IIC_Delay();
    LCD_TOUCH_IIC_SCL(0);
    IIC_Delay();
    LCD_TOUCH_IIC_SDA(1);
    IIC_Delay();
}

/**
 * @brief       不产生ACK应答信号
 * @param       无
 * @retval      无
 */
static void IIC_Nack(void)
{
    LCD_TOUCH_IIC_SDA(1);
    IIC_Delay();
    LCD_TOUCH_IIC_SCL(1);
    IIC_Delay();
    LCD_TOUCH_IIC_SCL(0);
    IIC_Delay();
}

/**
 * @brief       IIC发送一个字节
 * @param       dat: 要发送的数据
 * @retval      无
 */
static void IIC_Send_Byte(uint8_t dat)
{
    uint8_t t;

    for (t = 0; t < 8; t++) {
        LCD_TOUCH_IIC_SDA((dat & 0x80) >> 7);
        IIC_Delay();
        LCD_TOUCH_IIC_SCL(1);
        IIC_Delay();
        LCD_TOUCH_IIC_SCL(0);
        dat <<= 1;
    }
    LCD_TOUCH_IIC_SDA(1);
}

/**
 * @brief       IIC接收一个字节
 * @param       ack: ack=1时，发送ack; ack=0时，发送nack
 * @retval      接收到的数据
 */
static uint8_t IIC_Recv_Byte(uint8_t ack)
{
    uint8_t i;
    uint8_t dat = 0;

    for (i = 0; i < 8; i++) {
        dat <<= 1;
        LCD_TOUCH_IIC_SCL(1);
        IIC_Delay();

        if (LCD_TOUCH_IIC_READ_SDA()) {
            dat++;
        }

        LCD_TOUCH_IIC_SCL(0);
        IIC_Delay();
    }

    if (ack == 0) {
        IIC_Nack();
    } else {
        IIC_Ack();
    }

    return dat;
}

/**
 * @brief       初始化IIC接口
 * @param       无
 * @retval      无
 */
static void IIC_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    /* 初始化SCL引脚 */
    gpio_init_struct.Pin   = LCD_TOUCH_IIC_SCL_GPIO_PIN; /* SCL引脚 */
    gpio_init_struct.Mode  = GPIO_MODE_OUTPUT_PP;        /* 推挽输出 */
    gpio_init_struct.Pull  = GPIO_PULLUP;                /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;       /* 高速 */
    HAL_GPIO_Init(LCD_TOUCH_IIC_SCL_GPIO_PORT, &gpio_init_struct);

    /* 初始化SDA引脚 */
    gpio_init_struct.Pin  = LCD_TOUCH_IIC_SDA_GPIO_PIN; /* SDA引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;        /* 开漏输出 */
    HAL_GPIO_Init(LCD_TOUCH_IIC_SDA_GPIO_PORT, &gpio_init_struct);

    IIC_Stop();
}

/****************************LCD 触摸屏驱动*********************************/
/********宏定义段*********/
/* 引脚定义 */
#define LCD_TOUCH_PEN_GPIO_PORT GPIOH
#define LCD_TOUCH_PEN_GPIO_PIN  GPIO_PIN_7
#define LCD_TOUCH_TCS_GPIO_PORT GPIOI
#define LCD_TOUCH_TCS_GPIO_PIN  GPIO_PIN_8

/* IO操作 */
#define LCD_TOUCH_READ_PEN() HAL_GPIO_ReadPin(LCD_TOUCH_PEN_GPIO_PORT, LCD_TOUCH_PEN_GPIO_PIN)
#define LCD_TOUCH_PEN(x)                                                                                                                                                           \
    do {                                                                                                                                                                           \
        x ? HAL_GPIO_WritePin(LCD_TOUCH_PEN_GPIO_PORT, LCD_TOUCH_PEN_GPIO_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(LCD_TOUCH_PEN_GPIO_PORT, LCD_TOUCH_PEN_GPIO_PIN, GPIO_PIN_RESET); \
    } while (0)
#define LCD_TOUCH_TCS(x)                                                                                                                                                           \
    do {                                                                                                                                                                           \
        x ? HAL_GPIO_WritePin(LCD_TOUCH_TCS_GPIO_PORT, LCD_TOUCH_TCS_GPIO_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(LCD_TOUCH_TCS_GPIO_PORT, LCD_TOUCH_TCS_GPIO_PIN, GPIO_PIN_RESET); \
    } while (0)

/* 错误代码 */
#define LCD_TOUCH_EOK   0 /* 没有错误 */
#define LCD_TOUCH_ERROR 1 /* 错误 */

/* RGB LCD模块触摸的PID */
#define LCD_TOUCH_PID  "911"
#define LCD_TOUCH_PID1 "9147"
#define LCD_TOUCH_PID2 "9271"
#define LCD_TOUCH_PID3 "1158"

/* RGB LCD模块最大触摸点数量 */
#define LCD_TOUCH_TP_MAX 5

/* RGB LCD模块触摸部分寄存器定义 */
#define LCD_TOUCH_REG_CTRL   0x8040 /* 控制寄存器 */
#define LCD_TOUCH_REG_PID    0x8140 /* PID寄存器 */
#define LCD_TOUCH_REG_TPINFO 0x814E /* 触摸状态寄存器 */
#define LCD_TOUCH_REG_TP1    0x8150 /* 触摸点1数据寄存器 */
#define LCD_TOUCH_REG_TP2    0x8158 /* 触摸点2数据寄存器 */
#define LCD_TOUCH_REG_TP3    0x8160 /* 触摸点3数据寄存器 */
#define LCD_TOUCH_REG_TP4    0x8168 /* 触摸点4数据寄存器 */
#define LCD_TOUCH_REG_TP5    0x8170 /* 触摸点5数据寄存器 */

/* 触摸状态寄存器掩码 */
#define LCD_TOUCH_TPINFO_MASK_STA 0x80
#define LCD_TOUCH_TPINFO_MASK_CNT 0x0F

/* RGB LCD模块触摸点数据寄存器 */
static const uint16_t LCD_touch_tp_reg[LCD_TOUCH_TP_MAX] = {
    LCD_TOUCH_REG_TP1,
    LCD_TOUCH_REG_TP2,
    LCD_TOUCH_REG_TP3,
    LCD_TOUCH_REG_TP4,
    LCD_TOUCH_REG_TP5,
};

/* RGB LCD模块触摸状态结构体 */
static struct
{
    LCD_touch_iic_addr_t iic_addr;
} LCD_touch_sta;

/********驱动函数*********/

/**
 * @brief       RGB LCD模块触摸硬件初始化
 * @param       无
 * @retval      无
 */
static void LCD_Touch_HardWare_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    /* 初始化PEN引脚 */
    gpio_init_struct.Pin   = LCD_TOUCH_PEN_GPIO_PIN;
    gpio_init_struct.Mode  = GPIO_MODE_INPUT;
    gpio_init_struct.Pull  = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_TOUCH_PEN_GPIO_PORT, &gpio_init_struct);

    /* 初始化TCS引脚 */
    gpio_init_struct.Pin   = LCD_TOUCH_TCS_GPIO_PIN;
    gpio_init_struct.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull  = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_TOUCH_TCS_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       RGB LCD模块触摸硬件复位
 * @param       addr: 复位后使用的IIC通讯地址
 * @retval      无
 */
static void LCD_Touch_HardWare_Reset(LCD_touch_iic_addr_t addr)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    /* 配置PEN引脚为输出 */
    gpio_init_struct.Pin   = LCD_TOUCH_PEN_GPIO_PIN;
    gpio_init_struct.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull  = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_TOUCH_PEN_GPIO_PORT, &gpio_init_struct);

    switch (addr) {
        case LCD_TOUCH_IIC_ADDR_14: {
            LCD_TOUCH_TCS(0);
            LCD_TOUCH_PEN(0);
            LCD_TOUCH_PEN(1);
            delay_ms(1);
            LCD_TOUCH_TCS(1);
            delay_ms(10);
            break;
        }
        case LCD_TOUCH_IIC_ADDR_5D: {
            LCD_TOUCH_TCS(0);
            LCD_TOUCH_PEN(0);
            delay_ms(1);
            LCD_TOUCH_TCS(1);
            delay_ms(10);
            break;
        }
        default: {
            break;
        }
    }

    /* 重新配置PEN引脚为输入 */
    gpio_init_struct.Pin   = LCD_TOUCH_PEN_GPIO_PIN;
    gpio_init_struct.Mode  = GPIO_MODE_INPUT;
    gpio_init_struct.Pull  = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_TOUCH_PEN_GPIO_PORT, &gpio_init_struct);

    LCD_touch_sta.iic_addr = addr;
    delay_ms(100);
}

/**
 * @brief       写RGB LCD模块触摸寄存器
 * @param       reg: 待写寄存器地址
 *              buf: 待写入的数据
 *              len: 待写入数据的长度
 * @retval      LCD_TOUCH_EOK  : 写ATK-RGBLCD模块触摸寄存器成功
 *              LCD_TOUCH_ERROR: 写ATK-RGBLCD模块触摸寄存器失败
 */
static uint8_t LCD_Touch_Write_Reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t buf_index;
    uint8_t ret;

    IIC_Start();
    IIC_Send_Byte((LCD_touch_sta.iic_addr << 1) | LCD_TOUCH_IIC_WRITE);
    IIC_Wait_Ack();
    IIC_Send_Byte((uint8_t)(reg >> 8) & 0xFF);
    IIC_Wait_Ack();
    IIC_Send_Byte((uint8_t)reg & 0xFF);
    IIC_Wait_Ack();

    for (buf_index = 0; buf_index < len; buf_index++) {
        IIC_Send_Byte(buf[buf_index]);
        ret = IIC_Wait_Ack();
        if (ret != 0) {
            break;
        }
    }

    IIC_Stop();

    if (ret != 0) {
        return LCD_TOUCH_ERROR;
    }

    return LCD_TOUCH_EOK;
}

/**
 * @brief       读RGB LCD模块触摸寄存器
 * @param       reg: 待读寄存器地址
 *              buf: 读取的数据
 *              len: 待读取数据的长度
 * @retval      无
 */
static void LCD_Touch_Read_Reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t buf_index;

    IIC_Start();
    IIC_Send_Byte((LCD_touch_sta.iic_addr << 1) | LCD_TOUCH_IIC_WRITE);
    IIC_Wait_Ack();
    IIC_Send_Byte((uint8_t)(reg >> 8) & 0xFF);
    IIC_Wait_Ack();
    IIC_Send_Byte((uint8_t)reg & 0xFF);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte((LCD_touch_sta.iic_addr << 1) | LCD_TOUCH_IIC_READ);
    IIC_Wait_Ack();

    for (buf_index = 0; buf_index < len - 1; buf_index++) {
        buf[buf_index] = IIC_Recv_Byte(1);
    }

    buf[buf_index] = IIC_Recv_Byte(0);

    IIC_Stop();
}

/**
 * @brief       RGB LCD模块触摸软件复位
 * @param       无
 * @retval      无
 */
static void LCD_Touch_SoftWare_Reset(void)
{
    uint8_t dat;

    dat = 0x02;
    LCD_Touch_Write_Reg(LCD_TOUCH_REG_CTRL, &dat, 1);
    delay_ms(10);

    dat = 0x00;
    LCD_Touch_Write_Reg(LCD_TOUCH_REG_CTRL, &dat, 1);
}

/**
 * @brief       获取RGB LCD模块触摸的PID
 * @param       pid: 获取到的PID（ASCII）
 * @retval      无
 */
static void LCD_Touch_Get_PID(char *pid)
{
    LCD_Touch_Read_Reg(LCD_TOUCH_REG_PID, (uint8_t *)pid, 4);
    pid[4] = '\0';
}

/**
 * @brief       RGB LCD模块触摸初始化
 * @param       无
 * @retval      LCD_TOUCH_EOK  : RGB LCD模块触摸初始化成功
 *              LCD_TOUCH_ERROR: RGB LCD模块触摸初始化失败
 */
uint8_t LCD_Touch_Init(LCD_touch_type_t type)
{
    char pid[5];

    if (type != LCD_TOUCH_TYPE_GTXX) {
        return LCD_TOUCH_ERROR;
    }

    LCD_Touch_HardWare_Init();
    LCD_Touch_HardWare_Reset(LCD_TOUCH_IIC_ADDR_14);
    IIC_Init();
    LCD_Touch_Get_PID(pid);
    if ((strcmp(pid, LCD_TOUCH_PID) != 0) && (strcmp(pid, LCD_TOUCH_PID1) != 0) && (strcmp(pid, LCD_TOUCH_PID2) != 0) && (strcmp(pid, LCD_TOUCH_PID3) != 0)) {
        return LCD_TOUCH_ERROR;
    }
    LCD_Touch_SoftWare_Reset();

    return LCD_TOUCH_EOK;
}

/**
 * @brief       RGB LCD模块触摸扫描
 * @note        连续调用间隔需大于10ms
 * @param       point: 扫描到的触摸点信息
 *              cnt  : 需要扫描的触摸点数量（1~LCD_TOUCH_TP_MAX）
 * @retval      0   : 没有扫描到触摸点
 *              其他: 实际获取到的触摸点信息数量
 */
uint8_t LCD_Touch_Scan(LCD_touch_point_t *point, uint8_t cnt)
{
    uint8_t tp_info;
    uint8_t tp_cnt;
    uint8_t point_index;
    LCD_lcd_disp_dir_t dir;
    uint8_t tpn_info[6];
    LCD_touch_point_t point_raw;

    if ((cnt == 0) || (cnt > LCD_TOUCH_TP_MAX)) {
        return 0;
    }

    for (point_index = 0; point_index < cnt; point_index++) {
        if (&point[point_index] == NULL) {
            return 0;
        }
    }

    LCD_Touch_Read_Reg(LCD_TOUCH_REG_TPINFO, &tp_info, sizeof(tp_info));
    if ((tp_info & LCD_TOUCH_TPINFO_MASK_STA) == LCD_TOUCH_TPINFO_MASK_STA) {
        tp_cnt = tp_info & LCD_TOUCH_TPINFO_MASK_CNT;
        tp_cnt = (cnt < tp_cnt) ? cnt : tp_cnt;

        for (point_index = 0; point_index < tp_cnt; point_index++) {
            LCD_Touch_Read_Reg(LCD_touch_tp_reg[point_index], tpn_info, sizeof(tpn_info));
            point_raw.x    = (uint16_t)(tpn_info[1] << 8) | tpn_info[0];
            point_raw.y    = (uint16_t)(tpn_info[3] << 8) | tpn_info[2];
            point_raw.size = (uint16_t)(tpn_info[5] << 8) | tpn_info[4];

            dir = LCD_Get_Disp_Dir();
            switch (dir) {
                case LCD_LCD_DISP_DIR_0: {
                    point[point_index].x = point_raw.x;
                    point[point_index].y = point_raw.y;
                    break;
                }
                case LCD_LCD_DISP_DIR_90: {
                    point[point_index].x = point_raw.y;
                    point[point_index].y = LCD_Get_Height() - point_raw.x;
                    break;
                }
                case LCD_LCD_DISP_DIR_180: {
                    point[point_index].x = LCD_Get_Width() - point_raw.x;
                    point[point_index].y = LCD_Get_Height() - point_raw.y;
                    break;
                }
                case LCD_LCD_DISP_DIR_270: {
                    point[point_index].x = LCD_Get_Width() - point_raw.y;
                    point[point_index].y = point_raw.x;
                    break;
                }
            }

            point[point_index].size = point_raw.size;
        }

        tp_info = 0;
        LCD_Touch_Write_Reg(LCD_TOUCH_REG_TPINFO, &tp_info, sizeof(tp_info));

        return tp_cnt;
    } else {
        return 0;
    }
}
#endif