
#include "lcd.h"
#include "lcd_font.h"
#include "main.h"
#include "tim.h"
/*********************************** 延时函数段 **************************************************/
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

/********************************** SDRAM 初始化段 ***********************************************/
#include "fmc.h"
// SDRAM 宏定义
#define BANK5_SDRAM_ADDR ((uint32_t)(0XC0000000)) /* SDRAM开始地址 */

/* SDRAM配置参数 */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

/**
 * @brief       向SDRAM发送命令
 * @param       bankx       0,向BANK5上面的SDRAM发送指令;1,向BANK6上面的SDRAM发送指令
 * @param       cmd         指令(0,正常模式/1,时钟配置使能/2,预充电所有存储区/3,自动刷新/4,加载模式寄存器/5,自刷新/6,掉电)
 * @param       refresh     自刷新次数(cmd=3时有效)
 * @param       regval      模式寄存器值
 * @retval      返回值       0,正常; 1,失败.
 */
static uint8_t SDRAM_Send_Cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval)
{
    uint32_t target_bank = 0;
    FMC_SDRAM_CommandTypeDef command;

    if (bankx == 0) {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
    } else if (bankx == 1) {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
    }

    command.CommandMode            = cmd;         /* 命令 */
    command.CommandTarget          = target_bank; /* 目标SDRAM存储区域 */
    command.AutoRefreshNumber      = refresh;     /* 自刷新次数 */
    command.ModeRegisterDefinition = regval;      /* 要写入模式寄存器的值 */

    if (HAL_SDRAM_SendCommand(&hsdram1, &command, 0X1000) == HAL_OK) /* 向SDRAM发送命令 */
    {
        return 0;
    }

    else
        return 1;
}

/**
 * @brief       在指定地址(writeaddr + Bank5_SDRAM_ADDR)开始,连续写入n个字节
 * @param       pbuf        : 字节指针
 * @param       writeaddr   : 要写入的地址
 * @param       n           : 要写入的字节数
 * @retval      无
 */
static void SDRAM_Write_Buffer(uint8_t *pbuf, uint32_t writeaddr, uint32_t n)
{
    for (; n != 0; n--) {
        *(volatile uint8_t *)(BANK5_SDRAM_ADDR + writeaddr) = *pbuf;
        writeaddr++;
        pbuf++;
    }
}

/**
 * @brief       在指定地址(readaddr + Bank5_SDRAM_ADDR)开始,连续读取n个字节
 * @param       pbuf        : 字节指针
 * @param       readaddr    : 要读取的地址
 * @param       n           : 要读取的字节数
 * @retval      无
 */
static void SDRAM_Read_Buffer(uint8_t *pbuf, uint32_t readaddr, uint32_t n)
{
    for (; n != 0; n--) {
        *pbuf++ = *(volatile uint8_t *)(BANK5_SDRAM_ADDR + readaddr);
        readaddr++;
    }
}

/**
 * @brief       发送SDRAM初始化序列
 * @param       无
 * @retval      无
 */
static void SDRAM_Initialization_Sequence(void)
{
    uint32_t temp = 0;

    /* SDRAM控制器初始化完成以后还需要按照如下顺序初始化SDRAM */
    SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);       /* 时钟配置使能 */
    delay_us(500);                                           /* 至少延时500us */
    SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_PALL, 1, 0);             /* 对所有存储区预充电 */
    SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0); /* 设置自刷新次数 */

    /* 配置模式寄存器,SDRAM的bit0~bit2为指定突发访问的长度，
     * bit3为指定突发访问的类型，bit4~bit6为CAS值，bit7和bit8为运行模式
     * bit9为指定的写突发模式，bit10和bit11位保留位 */
    temp = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1 |      /* 设置突发长度:1(可以是1/2/4/8) */
           SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |         /* 设置突发类型:连续(可以是连续/交错) */
           SDRAM_MODEREG_CAS_LATENCY_2 |                 /* 设置CAS值:2(可以是2/3) */
           SDRAM_MODEREG_OPERATING_MODE_STANDARD |       /* 设置操作模式:0,标准模式 */
           SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;         /* 设置突发写模式:1,单点访问 */
    SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp); /* 设置SDRAM的模式寄存器 */

    /* 刷新频率计数器(以SDCLK频率计数),计算方法:
     * COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
     * 我们使用的SDRAM刷新周期为64ms,SDCLK=192/2=96Mhz,行数为8192(2^13).
     * 所以,COUNT=64*1000*96/8192-20=730 */
    HAL_SDRAM_ProgramRefreshRate(&hsdram1, 730); /* 设置刷新频率 */
}
/********************************** LTDC 初始化段 ***********************************************/
#include "ltdc.h"
#define LCD_LTDC_LAYER_FB_ADDR ((uint32_t)0xC0000000)
/********************************* LCD  显示驱动段 **********************************************/

/************ 宏定义段 ****************/
/* 引脚定义 */
#define LCD_BL_GPIO_PORT GPIOB
#define LCD_BL_GPIO_PIN  GPIO_PIN_5
#define LCD_M0_GPIO_PORT GPIOG
#define LCD_M0_GPIO_PIN  GPIO_PIN_6
#define LCD_M1_GPIO_PORT GPIOI
#define LCD_M1_GPIO_PIN  GPIO_PIN_2
#define LCD_M2_GPIO_PORT GPIOI
#define LCD_M2_GPIO_PIN  GPIO_PIN_7

/* IO操作 */
#define LCD_BL(x)                                                                                                                                      \
    do {                                                                                                                                               \
        x ? HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_RESET); \
    } while (0)
#define LCD_READ_M0() HAL_GPIO_ReadPin(LCD_M0_GPIO_PORT, LCD_M0_GPIO_PIN)
#define LCD_READ_M1() HAL_GPIO_ReadPin(LCD_M1_GPIO_PORT, LCD_M1_GPIO_PIN)
#define LCD_READ_M2() HAL_GPIO_ReadPin(LCD_M2_GPIO_PORT, LCD_M2_GPIO_PIN)

/* 错误代码 */
#define LCD_EOK    0 /* 没有错误 */
#define LCD_ERROR  1 /* 错误 */
#define LCD_EINVAL 2 /* 非法参数 */

/************ 驱动段 ****************/
#define LCD_LCD_RAW_WIDTH  LCD_sta.param->width
#define LCD_LCD_RAW_HEIGHT LCD_sta.param->height
#define LCD_LCD_WIDTH      LCD_sta.width
#define LCD_LCD_HEIGHT     LCD_sta.height
#define LCD_FB             LCD_sta.fb

/* RGB LCD模块参数结构体 */
typedef struct
{
    uint8_t id;
    uint16_t pid;
    uint16_t width;
    uint16_t height;
    LCD_timint timing;
    LCD_touch_type_t touch_type;
} LCD_param_t;

/* RGB LCD模块参数匹配表 */
static const LCD_param_t LCD_param[] = {
    {0, LCD_PID_4342, 480, 272, {9000000, 1, 480, 40, 5, 1, 272, 8, 8, 1}, LCD_TOUCH_TYPE_GTXX},          // ATK-MD0430R-480272
    {1, LCD_PID_7084, 800, 480, {33000000, 1, 800, 46, 210, 1, 480, 23, 22, 1}, LCD_TOUCH_TYPE_FTXX},     // ATK-MD0700R-800480
    {2, LCD_PID_7016, 1024, 600, {45000000, 1, 1024, 140, 160, 20, 600, 20, 12, 3}, LCD_TOUCH_TYPE_GTXX}, // ATK-MD0700R-1024600
    {3, LCD_PID_7018, 1280, 800, {0, 1, 1280, 0, 0, 0, 800, 0, 0, 0}, LCD_TOUCH_TYPE_GTXX},               // ATK-MD0700R-1280800
    {4, LCD_PID_4384, 800, 480, {33000000, 1, 800, 88, 40, 48, 480, 32, 13, 3}, LCD_TOUCH_TYPE_GTXX},     // ATK-MD0430R-800480
    {5, LCD_PID_1018, 1280, 800, {45000000, 0, 1280, 140, 10, 10, 800, 10, 10, 3}, LCD_TOUCH_TYPE_GTXX},  // ATK-MD1010R-1280800
};

/* RGB LCD模块状态数据结构体 */
static struct
{
    const LCD_param_t *param;
    uint16_t width;
    uint16_t height;
    LCD_lcd_disp_dir_t disp_dir;
    uint16_t *fb;
} LCD_sta = {0};

/**
 * @brief       RGB LCD模块ID获取
 * @param       无
 * @retval      RGB LCD模块ID
 */
static uint8_t LCD_Get_ID(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    uint8_t id;
    gpio_init_struct.Pin   = LCD_M0_GPIO_PIN;
    gpio_init_struct.Mode  = GPIO_MODE_INPUT;
    gpio_init_struct.Pull  = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(LCD_M0_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_M1_GPIO_PIN;
    HAL_GPIO_Init(LCD_M1_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_M2_GPIO_PIN;
    HAL_GPIO_Init(LCD_M2_GPIO_PORT, &gpio_init_struct);

    id = LCD_READ_M0();
    id |= LCD_READ_M1() << 1;
    id |= LCD_READ_M2() << 2;

    return id;
}

/**
 * @brief       RGB LCD模块参数初始化
 * @param       通过RGB LCD的ID确定RGB LCD的尺寸和时序
 * @retval      LCD_EOK   : RGB LCD模块参数初始化成功
 *              LCD_EINVAL: 输入ID无效
 */
static uint8_t LCD_Setup_Param_By_ID(uint8_t id)
{
    uint8_t index;

    for (index = 0; index < (sizeof(LCD_param) / sizeof(LCD_param[0])); index++) {
        if (id == LCD_param[index].id) {
            LCD_sta.param = &LCD_param[index];
            return LCD_EOK;
        }
    }

    return LCD_EINVAL;
}

/**
 * @brief       RGB LCD模块硬件初始化
 * @param       无
 * @retval      LCD_EOK  : RGB LCD模块初始化成功
 *              LCD_ERROR: RGB LCD模块初始化失败
 */
static void LCD_HardWare_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    gpio_init_struct.Pin   = LCD_BL_GPIO_PIN;
    gpio_init_struct.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull  = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_BL_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       平方函数，x^y
 * @param       x: 底数
 *              y: 指数
 * @retval      x^y
 */
static uint32_t LCD_pow(uint8_t x, uint8_t y)
{
    uint8_t loop;
    uint32_t res = 1;

    for (loop = 0; loop < y; loop++) {
        res *= x;
    }

    return res;
}

/**
 * @brief       根据RGB LCD模块显示方向换算坐标值
 * @param       x: X坐标值
 *              y: Y坐标值
 * @retval      无
 */
static inline void LCD_Pos_Transform(uint16_t *x, uint16_t *y)
{
    uint16_t x_target = 0;
    uint16_t y_target = 0;

    switch (LCD_sta.disp_dir) {
        case LCD_LCD_DISP_DIR_0: {
            x_target = *x;
            y_target = *y;
            break;
        }
        case LCD_LCD_DISP_DIR_90: {
            x_target = LCD_LCD_RAW_WIDTH - *y - 1;
            y_target = *x;
            break;
        }
        case LCD_LCD_DISP_DIR_180: {
            x_target = LCD_LCD_RAW_WIDTH - *x - 1;
            y_target = LCD_LCD_RAW_HEIGHT - *y - 1;
            break;
        }
        case LCD_LCD_DISP_DIR_270: {
            x_target = *y;
            y_target = LCD_LCD_RAW_HEIGHT - *x - 1;
            break;
        }
    }

    *x = x_target;
    *y = y_target;
}

#if (LCD_USING_DMA2D != 0)
/**
 * @brief       DMA2D初始化
 * @param       无
 * @retval      无
 */
static void LCD_DMA2D_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN; /* 使能DMA2D时钟 */
    DMA2D->CR &= ~DMA2D_CR_START;        /* 停止DMA2D */
    DMA2D->CR &= ~DMA2D_CR_MODE_Msk;     /* 寄存器到存储器模式 */
    DMA2D->CR |= DMA2D_R2M;
    DMA2D->OPFCCR &= ~DMA2D_OPFCCR_CM_Msk; /* RGB565模式 */
    DMA2D->OPFCCR |= DMA2D_OUTPUT_RGB565;
}

/**
 * @brief       DMA2D LCD区域填充
 * @param       xs   : 区域起始X坐标
 *              ys   : 区域起始Y坐标
 *              xe   : 区域终止X坐标
 *              ye   : 区域终止Y坐标
 *              color: 区域填充颜色
 * @retval      无
 */
static inline void LCD_DMA2D_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
    LCD_Pos_Transform(&xs, &ys);
    LCD_Pos_Transform(&xe, &ye);

    if (xs > xe) {
        xs = xs ^ xe;
        xe = xs ^ xe;
        xs = xs ^ xe;
    }
    if (ys > ye) {
        ys = ys ^ ye;
        ye = ys ^ ye;
        ys = ys ^ ye;
    }

    DMA2D->CR &= ~(DMA2D_CR_START); /* 停止DMA2D */
    DMA2D->CR     = DMA2D_R2M;      /* 寄存器到存储器模式 */
    DMA2D->OPFCCR = CM_RGB565;
    DMA2D->OOR    = LCD_LCD_RAW_WIDTH - (xe - xs + 1);              /* 行偏移 */
    DMA2D->OMAR   = (uint32_t)&LCD_FB[ys * LCD_LCD_RAW_WIDTH + xs]; /* 存储器地址 */
    DMA2D->NLR &= ~DMA2D_NLR_PL_Msk;                                /* 每行的像素数 */
    DMA2D->NLR |= ((xe - xs + 1) << DMA2D_NLR_PL_Pos);
    DMA2D->NLR &= ~DMA2D_NLR_NL_Msk; /* 总的行数 */
    DMA2D->NLR |= ((ye - ys + 1) << DMA2D_NLR_NL_Pos);
    DMA2D->OCOLR = color;        /* 输出颜色 */
    DMA2D->CR |= DMA2D_CR_START; /* 开启DMA2D */
    while ((DMA2D->ISR & DMA2D_ISR_TCIF) != DMA2D_ISR_TCIF)
        ;                            /* 等待DMA2D传输完成 */
    DMA2D->IFCR |= DMA2D_IFCR_CTCIF; /* 清除传输完成标志 */
}

/**
 * @brief       DMA2D LCD区域填充
 * @param       xs   : 区域起始X坐标
 *              ys   : 区域起始Y坐标
 *              xe   : 区域终止X坐标
 *              ye   : 区域终止Y坐标
 *              color: 区域填充颜色
 * @retval      无
 */
static inline void LCD_DMA2D_Color_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t *color)
{
    LCD_Pos_Transform(&xs, &ys);
    LCD_Pos_Transform(&xe, &ye);

    if (xs > xe) {
        xs = xs ^ xe;
        xe = xs ^ xe;
        xs = xs ^ xe;
    }
    if (ys > ye) {
        ys = ys ^ ye;
        ye = ys ^ ye;
        ys = ys ^ ye;
    }

    DMA2D->CR &= ~(DMA2D_CR_START); /* 停止DMA2D */
    DMA2D->CR      = DMA2D_M2M;     /* 寄存器到存储器模式 */
    DMA2D->FGPFCCR = CM_RGB565;
    DMA2D->FGOR    = 0;                                 /* 前景层行偏移为0 */
    DMA2D->OOR     = LCD_LCD_RAW_WIDTH - (xe - xs + 1); /* 行偏移 */

    DMA2D->OMAR = (uint32_t)&LCD_FB[ys * LCD_LCD_RAW_WIDTH + xs]; /* 存储器地址 */
    DMA2D->NLR &= ~DMA2D_NLR_PL_Msk;                              /* 每行的像素数 */
    DMA2D->NLR |= ((xe - xs + 1) << DMA2D_NLR_PL_Pos);
    DMA2D->NLR &= ~DMA2D_NLR_NL_Msk; /* 总的行数 */
    DMA2D->NLR |= ((ye - ys + 1) << DMA2D_NLR_NL_Pos);
    DMA2D->FGMAR = (uint32_t)color; /* 输出颜色 */
    DMA2D->CR |= DMA2D_CR_START;    /* 开启DMA2D */
    while ((DMA2D->ISR & DMA2D_ISR_TCIF) != DMA2D_ISR_TCIF)
        ;                            /* 等待DMA2D传输完成 */
    DMA2D->IFCR |= DMA2D_IFCR_CTCIF; /* 清除传输完成标志 */
}
#endif

/**
 * @brief       获取RGB LCD模块PID
 * @param       无
 * @retval      0     : RGB LCD模块未初始化
 *              其他值: RGB LCD模块PID
 */
uint16_t LCD_Get_PID(void)
{
    return LCD_sta.param->pid;
}

/**
 * @brief       获取RGB LCD模块LCD宽度
 * @param       无
 * @retval      0     : RGB LCD模块未初始化
 *              其他值: RGB LCD模块LCD宽度
 */
uint16_t LCD_Get_Width(void)
{
    return LCD_sta.width;
}

/**
 * @brief       获取RGB LCD模块LCD高度
 * @param       无
 * @retval      0     : RGB LCD模块未初始化
 *              其他值: RGB LCD模块LCD高度
 */
uint16_t LCD_Get_Height(void)
{
    return LCD_sta.height;
}

/**
 * @brief       开启RGB LCD模块LCD背光
 * @param       无
 * @retval      无
 */
void LCD_Backlight_On(void)
{
    LCD_BL(1);
}

/**
 * @brief       关闭RGB LCD模块LCD背光
 * @param       无
 * @retval      无
 */
void LCD_Backlight_Off(void)
{
    LCD_BL(0);
}

/**
 * @brief       开启RGB LCD模块LCD显示
 * @param       无
 * @retval      无
 */
void LCD_Display_On(void)
{
    __HAL_LTDC_ENABLE(&hltdc);
}

/**
 * @brief       关闭RGB LCD模块LCD显示
 * @param       无
 * @retval      无
 */
void LCD_Display_Off(void)
{
    __HAL_LTDC_DISABLE(&hltdc);
}

/**
 * @brief       设置RGB LCD模块LCD显示方向
 * @param       disp_dir: LCD_LCD_DISP_DIR_0  : LCD顺时针旋转0°显示内容
 *                        LCD_LCD_DISP_DIR_90 : LCD顺时针旋转90°显示内容
 *                        LCD_LCD_DISP_DIR_180: LCD顺时针旋转180°显示内容
 *                        LCD_LCD_DISP_DIR_270: LCD顺时针旋转270°显示内容
 * @retval      LCD_EOK   : 设置RGB LCD模块LCD显示方向成功
 *              LCD_EINVAL: 传入参数错误
 */
uint8_t LCD_Set_Disp_Dir(LCD_lcd_disp_dir_t disp_dir)
{
    switch (disp_dir) {
        case LCD_LCD_DISP_DIR_0:
        case LCD_LCD_DISP_DIR_180: {
            LCD_sta.width  = LCD_sta.param->width;
            LCD_sta.height = LCD_sta.param->height;
            break;
        }
        case LCD_LCD_DISP_DIR_90:
        case LCD_LCD_DISP_DIR_270: {
            LCD_sta.width  = LCD_sta.param->height;
            LCD_sta.height = LCD_sta.param->width;
            break;
        }
        default: {
            return LCD_EINVAL;
        }
    }

    LCD_sta.disp_dir = disp_dir;

    return LCD_EOK;
}

/**
 * @brief       获取RGB LCD模块LCD扫描方向
 * @param       无
 * @retval      RGB LCD模块LCD扫描方向
 */
LCD_lcd_disp_dir_t LCD_Get_Disp_Dir(void)
{
    return LCD_sta.disp_dir;
}

/**
 * @brief       RGB LCD模块LCD区域填充
 * @param       xs   : 区域起始X坐标
 *              ys   : 区域起始Y坐标
 *              xe   : 区域终止X坐标
 *              ye   : 区域终止Y坐标
 *              color: 区域填充颜色
 * @retval      无
 */
void LCD_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
    if (xe >= LCD_LCD_WIDTH) {
        xe = LCD_LCD_WIDTH - 1;
    }
    if (ye >= LCD_LCD_HEIGHT) {
        ye = LCD_LCD_HEIGHT - 1;
    }

#if (LCD_USING_DMA2D != 0)
    LCD_DMA2D_Fill(xs, ys, xe, ye, color);
#else
    uint16_t x_index;
    uint16_t y_index;

    for (y_index = ys; y_index < ye + 1; y_index++) {
        for (x_index = xs; x_index < xe + 1; x_index++) {
            LCD_Pos_Transform(&x_index, &y_index);
            LCD_FB[y_index * LCD_LCD_RAW_WIDTH + x_index] = color;
        }
    }
#endif
}

/**
 * @brief       RGB LCD模块LCD区域填充
 * @param       xs   : 区域起始X坐标
 *              ys   : 区域起始Y坐标
 *              xe   : 区域终止X坐标
 *              ye   : 区域终止Y坐标
 *              color: 区域填充颜色
 * @retval      无
 */
void LCD_Color_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t *color)
{
    if (xe >= LCD_LCD_WIDTH) {
        xe = LCD_LCD_WIDTH - 1;
    }
    if (ye >= LCD_LCD_HEIGHT) {
        ye = LCD_LCD_HEIGHT - 1;
    }

#if (LCD_USING_DMA2D != 0)
    LCD_DMA2D_Color_Fill(xs, ys, xe, ye, color);
#else
    uint16_t x_index;
    uint16_t y_index;
    uint16_t width  = ex - sx + 1; // 得到填充的宽度
    uint16_t height = ey - sy + 1; // 高度

    for (y_index = 0; y_index < height; y_index++) {
        LCD_Pos_Transform(&x_index, &(y_index + i));
        for (x_index = 0; x_index < width; x_index++) {

            LCD_FB[y_index * LCD_LCD_RAW_WIDTH + x_index] = color[y_index * width + x_index];
        }
    }
#endif
}

/**
 * @brief       RGB LCD模块LCD清屏
 * @param       color   清屏颜色
 * @retval      无
 */
void LCD_Clear(uint16_t color)
{
    LCD_Fill(0, 0, LCD_LCD_WIDTH - 1, LCD_LCD_HEIGHT - 1, color);
}

/**
 * @brief       RGB LCD模块LCD画点
 * @param       x       待画点的X坐标
 * @param       y       待画点的Y坐标
 * @param       color   待画点的颜色
 * @retval      无
 */
void LCD_Draw_Point(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_Pos_Transform(&x, &y);
    LCD_FB[y * LCD_LCD_RAW_WIDTH + x] = color;
}

/**
 * @brief       RGB LCD模块LCD读点
 * @param       x    待读点的X坐标
 * @param       y    待读点的Y坐标
 * @retval      待读点的颜色
 */
uint16_t LCD_Read_Point(uint16_t x, uint16_t y)
{
    LCD_Pos_Transform(&x, &y);
    return LCD_FB[y * LCD_LCD_RAW_WIDTH + x];
}

/**
 * @brief       RGB LCD模块LCD画线段
 * @param       x1      待画线段端点1的X坐标
 * @param       y1      待画线段端点1的Y坐标
 * @param       x2      待画线段端点2的X坐标
 * @param       y2      待画线段端点2的Y坐标
 * @param       color   待画线段的颜色
 * @retval      无
 */
void LCD_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t x_delta;
    uint16_t y_delta;
    int16_t x_sign;
    int16_t y_sign;
    int16_t error;
    int16_t error2;

    x_delta = (x1 < x2) ? (x2 - x1) : (x1 - x2);
    y_delta = (y1 < y2) ? (y2 - y1) : (y1 - y2);
    x_sign  = (x1 < x2) ? 1 : -1;
    y_sign  = (y1 < y2) ? 1 : -1;
    error   = x_delta - y_delta;

    LCD_Draw_Point(x2, y2, color);

    while ((x1 != x2) || (y1 != y2)) {
        LCD_Draw_Point(x1, y1, color);

        error2 = error << 1;
        if (error2 > -y_delta) {
            error -= y_delta;
            x1 += x_sign;
        }

        if (error2 < x_delta) {
            error += x_delta;
            y1 += y_sign;
        }
    }
}

/**
 * @brief       RGB LCD模块LCD画矩形框
 * @param       x1      待画矩形框端点1的X坐标
 * @param       y1      待画矩形框端点1的Y坐标
 * @param       x2      待画矩形框端点2的X坐标
 * @param       y2      待画矩形框端点2的Y坐标
 * @param       color   待画矩形框的颜色
 * @retval      无
 */
void LCD_Draw_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    LCD_Draw_Line(x1, y1, x2, y1, color);
    LCD_Draw_Line(x1, y2, x2, y2, color);
    LCD_Draw_Line(x1, y1, x1, y2, color);
    LCD_Draw_Line(x2, y1, x2, y2, color);
}

/**
 * @brief       RGB LCD模块LCD画圆形框
 * @param       x       待画圆形框原点的X坐标
 * @param       y       待画圆形框原点的Y坐标
 * @param       r       待画圆形框的半径
 * @param       color   待画圆形框的颜色
 * @retval      无
 */
void LCD_Draw_Circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    int32_t x_t;
    int32_t y_t;
    int32_t error;
    int32_t error2;

    x_t   = -r;
    y_t   = 0;
    error = 2 - 2 * r;

    do {
        LCD_Draw_Point(x - x_t, y + y_t, color);
        LCD_Draw_Point(x + x_t, y + y_t, color);
        LCD_Draw_Point(x + x_t, y - y_t, color);
        LCD_Draw_Point(x - x_t, y - y_t, color);

        error2 = error;
        if (error2 <= y_t) {
            y_t++;
            error = error + (y_t * 2 + 1);
            if ((-x_t == y_t) && (error2 <= x_t)) {
                error2 = 0;
            }
        }

        if (error2 > x_t) {
            x_t++;
            error = error + (x_t * 2 + 1);
        }
    } while (x_t <= 0);
}

/**
 * @brief       RGB LCD模块LCD显示1个字符
 * @param       x       待显示字符的X坐标
 * @param       y       待显示字符的Y坐标
 * @param       ch      待显示字符
 * @param       font    待显示字符的字体
 * @param       color   待显示字符的颜色
 * @retval      无
 */
void LCD_Show_Char(uint16_t x, uint16_t y, char ch, LCD_lcd_font_t font, uint16_t color)
{
    const uint8_t *ch_code;
    uint8_t ch_width;
    uint8_t ch_height;
    uint8_t ch_size;
    uint8_t ch_offset;
    uint8_t byte_index;
    uint8_t byte_code;
    uint8_t bit_index;
    uint8_t width_index  = 0;
    uint8_t height_index = 0;

    ch_offset = ch - ' ';

    switch (font) {
#if (LCD_FONT_12 != 0)
        case LCD_LCD_FONT_12: {
            ch_code   = LCD_font_1206[ch_offset];
            ch_width  = LCD_FONT_12_CHAR_WIDTH;
            ch_height = LCD_FONT_12_CHAR_HEIGHT;
            ch_size   = LCD_FONT_12_CHAR_SIZE;
            break;
        }
#endif
#if (LCD_FONT_16 != 0)
        case LCD_LCD_FONT_16: {
            ch_code   = LCD_font_1608[ch_offset];
            ch_width  = LCD_FONT_16_CHAR_WIDTH;
            ch_height = LCD_FONT_16_CHAR_HEIGHT;
            ch_size   = LCD_FONT_16_CHAR_SIZE;
            break;
        }
#endif
#if (LCD_FONT_24 != 0)
        case LCD_LCD_FONT_24: {
            ch_code   = LCD_font_2412[ch_offset];
            ch_width  = LCD_FONT_24_CHAR_WIDTH;
            ch_height = LCD_FONT_24_CHAR_HEIGHT;
            ch_size   = LCD_FONT_24_CHAR_SIZE;
            break;
        }
#endif
#if (LCD_FONT_32 != 0)
        case LCD_LCD_FONT_32: {
            ch_code   = LCD_font_3216[ch_offset];
            ch_width  = LCD_FONT_32_CHAR_WIDTH;
            ch_height = LCD_FONT_32_CHAR_HEIGHT;
            ch_size   = LCD_FONT_32_CHAR_SIZE;
            break;
        }
#endif
        default: {
            return;
        }
    }

    if ((x + ch_width > LCD_LCD_WIDTH) || (y + ch_height > LCD_LCD_HEIGHT)) {
        return;
    }

    for (byte_index = 0; byte_index < ch_size; byte_index++) {
        byte_code = ch_code[byte_index];
        for (bit_index = 0; bit_index < 8; bit_index++) {
            if ((byte_code & 0x80) != 0) {
                LCD_Draw_Point(x + width_index, y + height_index, color);
            }
            width_index++;
            if (width_index == ch_width) {
                width_index = 0;
                height_index++;
                break;
            }
            byte_code <<= 1;
        }
    }
}

/**
 * @brief       RGB LCD模块LCD显示字符串
 * @note        会自动换行和换页
 * @param       x       待显示字符串的X坐标
 * @param       y       待显示字符串的Y坐标
 * @param       width   待显示字符串的显示高度
 * @param       height  待显示字符串的显示宽度
 * @param       str     待显示字符串
 * @param       font    待显示字符串的字体
 * @param       color   待显示字符串的颜色
 * @retval      无
 */
void LCD_Show_String(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, LCD_lcd_font_t font, uint16_t color)
{
    uint8_t ch_width;
    uint8_t ch_height;
    uint16_t x_raw;
    uint16_t y_raw;
    uint16_t x_limit;
    uint16_t y_limit;

    switch (font) {
#if (LCD_FONT_12 != 0)
        case LCD_LCD_FONT_12: {
            ch_width  = LCD_FONT_12_CHAR_WIDTH;
            ch_height = LCD_FONT_12_CHAR_HEIGHT;
            break;
        }
#endif
#if (LCD_FONT_16 != 0)
        case LCD_LCD_FONT_16: {
            ch_width  = LCD_FONT_16_CHAR_WIDTH;
            ch_height = LCD_FONT_16_CHAR_HEIGHT;
            break;
        }
#endif
#if (LCD_FONT_24 != 0)
        case LCD_LCD_FONT_24: {
            ch_width  = LCD_FONT_24_CHAR_WIDTH;
            ch_height = LCD_FONT_24_CHAR_HEIGHT;
            break;
        }
#endif
#if (LCD_FONT_32 != 0)
        case LCD_LCD_FONT_32: {
            ch_width  = LCD_FONT_32_CHAR_WIDTH;
            ch_height = LCD_FONT_32_CHAR_HEIGHT;
            break;
        }
#endif
        default: {
            return;
        }
    }

    x_raw   = x;
    y_raw   = y;
    x_limit = ((x + width + 1) > LCD_LCD_WIDTH) ? LCD_LCD_WIDTH : (x + width + 1);
    y_limit = ((y + height + 1) > LCD_LCD_HEIGHT) ? LCD_LCD_HEIGHT : (y + height + 1);

    while ((*str >= ' ') && (*str <= '~')) {
        if (x + ch_width >= x_limit) {
            x = x_raw;
            y += ch_height;
        }

        if (y + ch_height >= y_limit) {
            y = x_raw;
            x = y_raw;
        }

        LCD_Show_Char(x, y, *str, font, color);

        x += ch_width;
        str++;
    }
}

/**
 * @brief       RGB LCD模块LCD显示数字，可控制显示高位0
 * @param       x       待显示数字的X坐标
 * @param       y       待显示数字的Y坐标
 * @param       num     待显示数字
 * @param       len     待显示数字的位数
 * @param       mode    LCD_NUM_SHOW_NOZERO: 数字高位0不显示
 *                      LCD_NUM_SHOW_ZERO  : 数字高位0显示
 * @param       font    待显示数字的字体
 * @param       color   待显示数字的颜色
 * @retval      无
 */
void LCD_Show_Xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, LCD_num_mode_t mode, LCD_lcd_font_t font, uint16_t color)
{
    uint8_t ch_width;
    uint8_t len_index;
    uint8_t num_index;
    uint8_t first_nozero = 0;
    char pad;

    switch (font) {
#if (LCD_FONT_12 != 0)
        case LCD_LCD_FONT_12: {
            ch_width = LCD_FONT_12_CHAR_WIDTH;
            break;
        }
#endif
#if (LCD_FONT_16 != 0)
        case LCD_LCD_FONT_16: {
            ch_width = LCD_FONT_16_CHAR_WIDTH;
            break;
        }
#endif
#if (LCD_FONT_24 != 0)
        case LCD_LCD_FONT_24: {
            ch_width = LCD_FONT_24_CHAR_WIDTH;
            break;
        }
#endif
#if (LCD_FONT_32 != 0)
        case LCD_LCD_FONT_32: {
            ch_width = LCD_FONT_32_CHAR_WIDTH;
            break;
        }
#endif
        default: {
            return;
        }
    }

    switch (mode) {
        case LCD_NUM_SHOW_NOZERO: {
            pad = ' ';
            break;
        }
        case LCD_NUM_SHOW_ZERO: {
            pad = '0';
            break;
        }
        default: {
            return;
        }
    }

    for (len_index = 0; len_index < len; len_index++) {
        num_index = (num / LCD_pow(10, len - len_index - 1)) % 10;
        if ((first_nozero == 0) && (len_index < (len - 1))) {
            if (num_index == 0) {
                LCD_Show_Char(x + ch_width * len_index, y, pad, font, color);
                continue;
            } else {
                first_nozero = 1;
            }
        }

        LCD_Show_Char(x + ch_width * len_index, y, num_index + '0', font, color);
    }
}

/**
 * @brief       RGB LCD模块LCD显示数字，不显示高位0
 * @param       x       待显示数字的X坐标
 * @param       y       待显示数字的Y坐标
 * @param       num     待显示数字
 * @param       len     待显示数字的位数
 * @param       font    待显示数字的字体
 * @param       color   待显示数字的颜色
 * @retval      无
 */
void LCD_Show_Num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, LCD_lcd_font_t font, uint16_t color)
{
    LCD_Show_Xnum(x, y, num, len, LCD_NUM_SHOW_NOZERO, font, color);
}

/**
 * @brief       RGB LCD模块初始化
 * @param       无
 * @retval      LCD_EOK  : RGB LCD模块初始化成功
 *              LCD_ERROR: RGB LCD模块初始化失败
 */
uint8_t LCD_Init(void)
{
    SDRAM_Initialization_Sequence();
    uint8_t id;
    uint8_t ret;

    id  = LCD_Get_ID();              /* RGB LCD模块ID获取 */
    ret = LCD_Setup_Param_By_ID(id); /* RGB LCD模块参数初始化 */
    if (ret != LCD_EOK) {
        return LCD_ERROR;
    }
    LCD_HardWare_Init(); /* RGB LCD模块硬件初始化 */
    MX_LTDC_Init();      /* RGB LCD模块LTDC接口初始化 */
    LCD_sta.fb = (uint16_t *)LCD_LTDC_LAYER_FB_ADDR;
#if (LCD_USING_DMA2D != 0)
    LCD_DMA2D_Init(); /* 初始化DMA2D */
#endif
    LCD_Set_Disp_Dir(LCD_LCD_DISP_DIR_0);
    LCD_Clear(LCD_WHITE);
    LCD_Backlight_On(); /* 开启RGB LCD模块LCD背光 */
#if (LCD_USING_TOUCH != 0)
    ret = LCD_Touch_Init(LCD_sta.param->touch_type);
    if (ret != 0) {
        return LCD_ERROR;
    }
#endif
    return LCD_EOK;
}

#if (LCD_LVGL != 0)

#endif