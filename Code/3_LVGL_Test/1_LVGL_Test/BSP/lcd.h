#ifndef __LCD_H
#define __LCD_H

#include "stm32f4xx.h"
/************************用户定义段******************************/
#define LCD_USING_DMA2D 1 /* 定义是否使用DMA2D */

#define LCD_USING_TOUCH 1 /* 定义是否使用RGB LCD模块触摸 */

#define LCD_LVGL        1 /* 定义是否启用LVGL */

#define LCD_FONT_12     1 /* 定义RGB LCD模块启用的字体 */
#define LCD_FONT_16     1 /* 定义RGB LCD模块启用的字体 */
#define LCD_FONT_24     1 /* 定义RGB LCD模块启用的字体 */
#define LCD_FONT_32     1 /* 定义RGB LCD模块启用的字体 */

#ifndef LCD_USING_DMA2D /* 默认启用DMA2D */
#define LCD_USING_DMA2D 1
#endif

#ifndef LCD_USING_TOUCH /* 默认启用触摸 */
#define LCD_USING_TOUCH 1
#endif

#if ((LCD_FONT_12 == 0) && (LCD_FONT_16 == 0) && (LCD_FONT_24 == 0) && (LCD_FONT_32 == 0)) /* 默认启用12号字体 */
#undef LCD_FONT_12
#define LCD_FONT_12 1
#endif

#if (LCD_USING_TOUCH != 0) /* 包含头文件 */
#include "lcd_touch.h"
#endif
/************************宏定义段******************************/
/* 常用颜色定义 */
#define LCD_WHITE   0xFFFF
#define LCD_BLACK   0x0000
#define LCD_BLUE    0x001F
#define LCD_BRED    0xF81F
#define LCD_GRED    0xFFE0
#define LCD_GBLUE   0x07FF
#define LCD_RED     0xF800
#define LCD_MAGENTA 0xF81F
#define LCD_GREEN   0x07E0
#define LCD_CYAN    0x7FFF
#define LCD_YELLOW  0xFFE0
#define LCD_BROWN   0xBC40
#define LCD_BRRED   0xFC07
#define LCD_GRAY    0x8430

/************ 结构体段 ****************/
/* RGB LCD模块PID枚举 */
enum {
    LCD_PID_4342 = 0x4342, /* ATK-MD0430R-480272 */
    LCD_PID_7084 = 0x7084, /* ATK-MD0700R-800480 */
    LCD_PID_7016 = 0x7016, /* ATK-MD0700R-1024600 */
    LCD_PID_7018 = 0x7018, /* ATK-MD0700R-1280800 */
    LCD_PID_4384 = 0x4384, /* ATK-MD0430R-800480 */
    LCD_PID_1018 = 0x1018, /* ATK-MD1010R-1280800 */
};

/* RGB LCD模块LCD旋转方向枚举 */
typedef enum {
    LCD_LCD_DISP_DIR_0 = 0x00, /* LCD顺时针旋转0°显示内容 */
    LCD_LCD_DISP_DIR_90,       /* LCD顺时针旋转90°显示内容 */
    LCD_LCD_DISP_DIR_180,      /* LCD顺时针旋转180°显示内容 */
    LCD_LCD_DISP_DIR_270,      /* LCD顺时针旋转270°显示内容 */
} LCD_lcd_disp_dir_t;

/* RGB LCD模块LCD显示字体枚举 */
typedef enum {
#if (LCD_FONT_12 != 0)
    LCD_LCD_FONT_12, /* 12号字体 */
#endif
#if (LCD_FONT_16 != 0)
    LCD_LCD_FONT_16, /* 16号字体 */
#endif
#if (LCD_FONT_24 != 0)
    LCD_LCD_FONT_24, /* 24号字体 */
#endif
#if (LCD_FONT_32 != 0)
    LCD_LCD_FONT_32, /* 32号字体 */
#endif
} LCD_lcd_font_t;

/* RGB LCD模块时序结构体 */
typedef struct
{
    uint32_t clock_freq;
    uint8_t pixel_clock_polarity;
    uint16_t hactive;
    uint16_t hback_porch;
    uint16_t hfront_porch;
    uint16_t hsync_len;
    uint16_t vactive;
    uint16_t vback_porch;
    uint16_t vfront_porch;
    uint16_t vsync_len;
} LCD_timint;

/* RGB LCD模块LCD显示数字模式枚举 */
typedef enum {
    LCD_NUM_SHOW_NOZERO = 0x00, /* 数字高位0不显示 */
    LCD_NUM_SHOW_ZERO,          /* 数字高位0显示 */
} LCD_num_mode_t;

/************************用户函数段******************************/

uint8_t LCD_Init(void);                                                                                                          /* RGB LCD模块初始化 */
uint16_t LCD_Get_PID(void);                                                                                                      /* 获取RGB LCD模块PID */
uint16_t LCD_Get_Width(void);                                                                                                    /* 获取RGB LCD模块LCD宽度 */
uint16_t LCD_Get_Height(void);                                                                                                   /* 获取RGB LCD模块LCD高度 */
void LCD_Backlight_On(void);                                                                                                     /* 开启RGB LCD模块LCD背光 */
void LCD_Backlight_Off(void);                                                                                                    /* 关闭RGB LCD模块LCD背光 */
void LCD_Display_On(void);                                                                                                       /* 开启RGB LCD模块LCD显示 */
void LCD_Display_Off(void);                                                                                                      /* 关闭RGB LCD模块LCD显示 */
uint8_t LCD_Set_Disp_Dir(LCD_lcd_disp_dir_t disp_dir);                                                                           /* 设置RGB LCD模块LCD显示方向 */
LCD_lcd_disp_dir_t LCD_Get_Disp_Dir(void);                                                                                       /* 获取RGB LCD模块LCD扫描方向 */
void LCD_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);                                               /* RGB LCD模块LCD区域填充 */
void LCD_Color_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t *color);                                        /* RGB LCD模块LCD区域填充 */
void LCD_Clear(uint16_t color);                                                                                                  /* RGB LCD模块LCD清屏 */
void LCD_Draw_Point(uint16_t x, uint16_t y, uint16_t color);                                                                     /* RGB LCD模块LCD画点 */
uint16_t LCD_Read_Point(uint16_t x, uint16_t y);                                                                                 /* RGB LCD模块LCD读点 */
void LCD_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);                                          /* RGB LCD模块LCD画线段 */
void LCD_Draw_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);                                          /* RGB LCD模块LCD画矩形框 */
void LCD_Draw_Circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                                                        /* RGB LCD模块LCD画圆形框 */
void LCD_Show_Char(uint16_t x, uint16_t y, char ch, LCD_lcd_font_t font, uint16_t color);                                        /* RGB LCD模块LCD显示1个字符 */
void LCD_Show_String(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, LCD_lcd_font_t font, uint16_t color);   /* RGB LCD模块LCD显示字符串 */
void LCD_Show_Xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, LCD_num_mode_t mode, LCD_lcd_font_t font, uint16_t color); /* RGB LCD模块LCD显示数字，可控制显示高位0 */
void LCD_Show_Num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, LCD_lcd_font_t font, uint16_t color);                       /* RGB LCD模块LCD显示数字，不显示高位0 */
#endif