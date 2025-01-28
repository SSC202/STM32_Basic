#include "oled.h"
#include "oled_font.h"

/******************驱动方式选择**********************/
/* 如果使能硬件IIC */
#ifdef OLED_HardWare_IIC
#include "i2c.h"
#define OLED_ADDRESS  0x78 // SSD1306 从机地址

#define OLED_COM_ADDR 0x00 // SSD1306 命令指令
#define OLED_DAT_ADDR 0x40 // SSD1306 数据指令

#endif
/* 如果使能软件IIC */
#ifdef OLED_SoftWare_IIC

#endif
/* 如果使能硬件SPI */
#ifdef OLED_HardWare_SPI
#include "spi.h"
#endif
/* 如果使能软件SPI */
#ifdef OLED_SoftWare_SPI

#endif
/*******************基础驱动函数**********************/
/* 如果使能硬件IIC */
#ifdef OLED_HardWare_IIC
/**
 * @brief 向SSD1306写入一个字节
 * @param dat:要写入的数据/命令
 * @param flag:数据/命令标志 0表示命令;1表示数据;
 * @retval None
 */
static void OLED_Write_Byte(uint8_t data, uint8_t flag)
{
    if (flag == 0) {
        uint8_t TxData[2] = {OLED_COM_ADDR, data};
        // 先传地址使SSD1306判别为数据还是命令，再传入数据（命令）
        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, (uint8_t *)TxData, 2, 10);
    } else {
        uint8_t TxData[2] = {OLED_DAT_ADDR, data};
        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, (uint8_t *)TxData, 2, 10);
    }
}

/**
 * @brief 向SSD1306写入一个命令
 * @param cmd:要写入的命令
 * @retval None
 */
static void OLED_Write_Cmd(uint8_t cmd)
{
    OLED_Write_Byte(cmd, 0);
}

/**
 * @brief 向SSD1306写入一个数据
 * @param dat:要写入的数据
 * @retval None
 */
static void OLED_Write_Data(uint8_t dat)
{
    OLED_Write_Byte(dat, 1);
}
#endif
/*******************OLED 驱动函数*********************/

/**
 * @brief OLED点设置
 * @param x:点的横坐标
 * @param y:点的纵坐标
 * @retval None
 */
static void OLED_Set_Point(uint8_t x, uint8_t y)
{
    OLED_Write_Cmd(0xB0 + y);
    OLED_Write_Cmd(((x & 0xF0) >> 4) | 0x10);
    OLED_Write_Cmd(x & 0x0f);
}

/**
 * @brief OLED开启显示
 * @param None
 * @retval None
 */
static void OLED_Display_On(void)
{
    OLED_Write_Cmd(0x8D); // 设置DC引脚（电荷泵）
    OLED_Write_Cmd(0x14); // 使能DC引脚
    OLED_Write_Cmd(0XAF); // 开始显示
}

/**
 * @brief OLED关闭显示
 * @param None
 * @retval None
 */
static void OLED_Display_Off(void)
{
    OLED_Write_Cmd(0x8D); // 设置DC引脚
    OLED_Write_Cmd(0x10); // 失能DC引脚
    OLED_Write_Cmd(0XAF); // 关闭显示
}

/**
 * @brief OLED清屏函数
 * @param None
 * @retval None
 */
void OLED_Clear(void)
{
    unsigned char i, n;
    for (i = 0; i < 8; i++) {
        OLED_Write_Cmd(0xb0 + i); // 设置页地址（0~7）
        OLED_Write_Cmd(0x02);     // 设置显示位置—列低地址
        OLED_Write_Cmd(0x10);     // 设置显示位置—列高地址
        for (n = 0; n < Max_Column; n++) {
            OLED_Write_Data(0);
        }
    }
}

/**
 * @brief 在指定位置显示一个字符
 * @param x:0~127
 * @param y:0~63
 * @param chr:显示的字符
 * @param Is_Reverse:1,反白显示  0,正常显示
 * @param charSize:选择字体 16/6
 * @retval None
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t charSize, uint8_t Is_Reverse)
{
    uint8_t c = 0, i = 0;
    c = chr - ' '; // 得到偏移后的值
    if (x > Max_Column - 1) {
        x = 0;
        y = y + 2;
    }
    if (charSize == 16) {
        OLED_Set_Point(x, y);
        for (i = 0; i < 8; i++)
            OLED_Write_Data(Is_Reverse == 0 ? F8x16[c * 16 + i] : ~F8x16[c * 16 + i]);
        OLED_Set_Point(x, y + 1);
        for (i = 0; i < 8; i++)
            OLED_Write_Data(Is_Reverse == 0 ? F8x16[c * 16 + i + 8] : F8x16[c * 16 + i + 8]);
    } else if (charSize == 6) {
        OLED_Set_Point(x, y + 1);
        for (i = 0; i < 6; i++)
            OLED_Write_Data(Is_Reverse == 0 ? F6x8[c][i] : ~F6x8[c][i]);
    }
}

/**
 * @brief m^n函数
 * @param None
 * @retval None
 */
static uint32_t OLED_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--) result *= m;
    return result;
}

/**
 * @brief 显示数字
 * @param x,y :起点坐标
 * @param num:数值(0~4294967295);
 * @param len :数字的位数，即显示几位有效数字
 * @param Is_Reverse:1,反白显示  0,正常显示
 * @param charSize:选择字体 16/6
 * @retval None
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t Size, uint8_t Is_Reverse)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++) {
        temp = (num / OLED_Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                OLED_ShowChar(x + 8 * t, y, ' ', Size, Is_Reverse);
                continue;
            } else
                enshow = 1;
        }
        OLED_ShowChar(x + 8 * t, y, temp + '0', Size, Is_Reverse);
    }
}

/**
 * @brief   显示一个字符串
 * @param   x : uint8_t 起点列坐标
 * @param   y : uint8_t 起点行坐标
 * @param   chr : uint8_t *字符串指针
 * @param Is_Reverse:1,反白显示  0,正常显示
 * @param charSize:选择字体 16/6
 * @retval  无
 */
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t charSize, uint8_t Is_Reverse)
{
    unsigned char j = 0;
    while (chr[j] != '\0') {
        OLED_ShowChar(x, y, chr[j], charSize, Is_Reverse);
        x += 8;
        if (x > 120) {
            x = 0;
            y += 2;
        }
        j++;
    }
}

/**
 * @brief   显示汉字
 * @param   x : uint8_t 起点列坐标
 * @param   y : uint8_t 起点行坐标
 * @param   num : 汉字标号
 * @retval  无
 */
void OLED_ShowCHinese(unsigned char x, unsigned char y, unsigned char num)
{
    unsigned char t, adder = 0;
    OLED_Set_Point(x, y);
    for (t = 0; t < 16; t++) {
        OLED_Write_Data(Hzk[2 * num][t]);
        adder += 1;
    }
    OLED_Set_Point(x, y + 1);
    for (t = 0; t < 16; t++) {
        OLED_Write_Data(Hzk[2 * num + 1][t]);
        adder += 1;
    }
}

/**
 * @brief   显示图片
 * @param   x0 : uint8_t 左上起点列坐标
 * @param   y0 : uint8_t 左上起点行坐标
 * @param   x1 : uint8_t 右下起点列坐标
 * @param   y1 : uint8_t 右下起点行坐标
 * @param   BMP[]	图片数组
 * @retval  无
 */
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;

    if (y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;
    for (y = y0; y < y1; y++) {
        OLED_Set_Point(x0, y);
        for (x = x0; x < x1; x++) {
            OLED_Write_Data(BMP[j++]);
        }
    }
}

/**
 * @brief OLED初始化函数
 * @param None
 * @retval None
 */
void OLED_Init(void)
{
    OLED_Write_Cmd(0xAE); // 设置显示关闭

    OLED_Write_Cmd(0x20); // 设置内存寻址模式
    OLED_Write_Cmd(0x10); // 设置为页寻址模式（0x00：水平寻址，0x01：垂直寻址，0x10：页寻址）
    OLED_Write_Cmd(0xB0); // 页寻址模式起始页，0到7

    OLED_Write_Cmd(0xC8); // 设置COM口输出扫描方向
    OLED_Write_Cmd(0x00); // 设置列低地址
    OLED_Write_Cmd(0x10); // 设置列高地址
    OLED_Write_Cmd(0x40); // 设置行起始地址

    OLED_Write_Cmd(0x81);            // 设置亮度
    OLED_Write_Cmd(OLED_BRIGHTNESS); // 0xFF

    OLED_Write_Cmd(0xA1); // 设置SEG/列映射
    OLED_Write_Cmd(0xA6); // 设置正常显示

    OLED_Write_Cmd(0xA8); // 设置复用率，1到64
    OLED_Write_Cmd(0x3F);

    OLED_Write_Cmd(0xA4); // 输出使用RAM中的内容（0xA5忽略）

    OLED_Write_Cmd(0xD3); // 设置显示偏移
    OLED_Write_Cmd(0x00);

    OLED_Write_Cmd(0xD5); // 设置分辨率
    OLED_Write_Cmd(0xF0);

    OLED_Write_Cmd(0xD9); // 设置充电周期
    OLED_Write_Cmd(0X22);

    OLED_Write_Cmd(0xDA); // 设置COM引脚硬件配置
    OLED_Write_Cmd(0x12);

    OLED_Write_Cmd(0xDB); // 设置Vcomh 取消选择水平
    OLED_Write_Cmd(0x20); // 设置Vcomh 为deselect模式

    OLED_Write_Cmd(0x8D);
    OLED_Write_Cmd(0x14);
    OLED_Write_Cmd(0xAF); // 设置显示开
    OLED_Clear();
    OLED_Set_Point(0, 0); // 设置起点坐标
}