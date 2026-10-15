#include <stdio.h>
#include <stdlib.h>
#include "oled.h"
#include "oled_font.h"

#define SYS_SUPPORT_OS 0

#if SYS_SUPPORT_OS
#include "FreeRTOS.h"
#include "task.h"
#endif

/**
 * @brief 向SSD1306写入一个字节
 * @param dat	要写入的数据/命令
 * @param flag	数据/命令标志 0表示命令;1表示数据;
 * @retval None
 */
void OLED_Write_Byte(unsigned char dat, unsigned char flag)
{
	unsigned char i;
	if (flag)
		OLED_DC(1);
	else
		OLED_DC(0);

	for (i = 0; i < 8; i++)
	{
		OLED_SCL(0);
		if (dat & 0x80)
			OLED_SDA(1);
		else
			OLED_SDA(0);
		OLED_SCL(1);
		dat <<= 1;
	}
	OLED_DC(1);
}

/**
 * @brief 向SSD1306写入一个命令
 * @param cmd:要写入的命令
 * @retval None
 */
void OLED_Write_Cmd(uint8_t cmd)
{
	OLED_Write_Byte(cmd, 0);
}

/**
 * @brief 向SSD1306写入一个数据
 * @param dat:要写入的数据
 * @retval None
 */
void OLED_Write_Data(uint8_t dat)
{
	OLED_Write_Byte(dat, 1);
}

/**
 * @brief OLED点设置
 * @param x:点的横坐标
 * @param y:点的纵坐标
 * @retval None
 */
void OLED_Set_Point(uint8_t x, uint8_t y)
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
void OLED_Display_On(void)
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
void OLED_Display_Off(void)
{
	OLED_Write_Cmd(0x8D); // 设置DC引脚
	OLED_Write_Cmd(0x10); // 失能DC引脚
	OLED_Write_Cmd(0XAE); // 关闭显示
}

/**
 * @brief OLED清屏函数
 * @param None
 * @retval None
 */
void OLED_Clear(void)
{
	unsigned char i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_Write_Cmd(0xb0 + i); // 设置页地址（0~7）
		OLED_Write_Cmd(0x02);	  // 设置显示位置—列低地址
		OLED_Write_Cmd(0x10);	  // 设置显示位置—列高地址
		for (n = 0; n < Max_Column; n++)
		{
			OLED_Write_Data(0);
		}
	}
}

/**
 * @brief 在指定位置显示一个字符
 * @param x:0~127
 * @param y:0~63
 * @param Is_Reverse:1,反白显示  0,正常显示
 * @param charSize:选择字体 16/6
 * @retval None
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t charSize, uint8_t Is_Reverse)
{
	uint8_t c = 0, i = 0;
	c = chr - ' '; // 得到偏移后的值
	if (x > Max_Column - 1)
	{
		x = 0;
		y = y + 2;
	}
	if (charSize == 16)
	{
		OLED_Set_Point(x, y);
		for (i = 0; i < 8; i++)
			OLED_Write_Data(Is_Reverse == 0 ? F8X16[c * 16 + i] : ~F8X16[c * 16 + i]);
		OLED_Set_Point(x, y + 1);
		for (i = 0; i < 8; i++)
			OLED_Write_Data(Is_Reverse == 0 ? F8X16[c * 16 + i + 8] : F8X16[c * 16 + i + 8]);
	}
	else if (charSize == 6)
	{
		OLED_Set_Point(x, y + 1);
		for (i = 0; i < 6; i++)
			OLED_Write_Data(Is_Reverse == 0 ? F6X8[c][i] : ~F6X8[c][i]);
	}
}

/**
 * @brief m^n函数
 * @param None
 * @retval None
 */
uint32_t OLED_Pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--)
		result *= m;
	return result;
}

/**
 * @brief 显示2个数字
 * @param x,y:起点坐标
 * @param len :数字的位数，即显示几位有效数字
 * @param Size:字体大小
 * @param mode:模式	0,填充模式;1,叠加模式
 * @param num:数值(0~4294967295);
 * @retval None
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t Size, uint8_t Is_Reverse)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / OLED_Pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + 8 * t, y, ' ', Size, Is_Reverse);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + 8 * t, y, temp + '0', Size, Is_Reverse);
	}
}

/**
 * @brief   显示一个字符号串
 * @param   x : uint8_t 起点列坐标
 * @param   y : uint8_t 起点行坐标
 * @param   chr : uint8_t *字符串指针
 * @retval  无
 */
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t charSize, uint8_t Is_Reverse)
{
	unsigned char j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], charSize, Is_Reverse);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

// 显示汉字
void OLED_ShowCHinese(unsigned char x, unsigned char y, unsigned char no)
{
	unsigned char t, adder = 0;
	OLED_Set_Point(x, y);
	for (t = 0; t < 16; t++)
	{
		OLED_Write_Data(Hzk[2 * no][t]);
		adder += 1;
	}
	OLED_Set_Point(x, y + 1);
	for (t = 0; t < 16; t++)
	{
		OLED_Write_Data(Hzk[2 * no + 1][t]);
		adder += 1;
	}
}

/**
 * @brief   显示一个字符号串
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
	for (y = y0; y < y1; y++)
	{
		OLED_Set_Point(x0, y);
		for (x = x0; x < x1; x++)
		{
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
	OLED_CS(0);
	OLED_DC(1);
	OLED_SCL(1);
	OLED_SDA(1);

	OLED_RST(1);
#if SYS_SUPPORT_OS
	vTaskDelay(100);
#else
	HAL_Delay(100);
#endif
	OLED_RST(1);
#if SYS_SUPPORT_OS
	vTaskDelay(100);
#else
	HAL_Delay(100);
#endif
	OLED_RST(1);

	OLED_Write_Cmd(0xAE); //--turn off oled panel
	OLED_Write_Cmd(0x02); //---set low column address
	OLED_Write_Cmd(0x10); //---set high column address
	OLED_Write_Cmd(0x40); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_Write_Cmd(0x81); //--set contrast control register
	OLED_Write_Cmd(0xCF); // Set SEG Output Current Brightness
	OLED_Write_Cmd(0xA1); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_Write_Cmd(0xC8); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_Write_Cmd(0xA6); //--set normal display
	OLED_Write_Cmd(0xA8); //--set multiplex ratio(1 to 64)
	OLED_Write_Cmd(0x3f); //--1/64 duty
	OLED_Write_Cmd(0xD3); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_Write_Cmd(0x00); //-not offset
	OLED_Write_Cmd(0xd5); //--set display clock divide ratio/oscillator frequency
	OLED_Write_Cmd(0x80); //--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_Write_Cmd(0xD9); //--set pre-charge period
	OLED_Write_Cmd(0xF1); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_Write_Cmd(0xDA); //--set com pins hardware configuration
	OLED_Write_Cmd(0x12);
	OLED_Write_Cmd(0xDB); //--set vcomh
	OLED_Write_Cmd(0x40); // Set VCOM Deselect Level
	OLED_Write_Cmd(0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_Write_Cmd(0x02); //
	OLED_Write_Cmd(0x8D); //--set Charge Pump enable/disable
	OLED_Write_Cmd(0x14); //--set(0x10) disable
	OLED_Write_Cmd(0xA4); // Disable Entire Display On (0xa4/0xa5)
	OLED_Write_Cmd(0xA6); // Disable Inverse Display On (0xa6/a7)
	OLED_Write_Cmd(0xAF); //--turn on oled panel

	OLED_Write_Cmd(0xAF); /* display ON */
	OLED_Clear();
	OLED_Set_Point(0, 0);
}
