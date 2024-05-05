#ifndef __TFTLCD_H
#define __TFTLCD_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "fmc.h"
#include "tim.h"

/********************************************************
 * 结构体定义
 */
// LCD 参数
typedef struct
{
    uint16_t width;   // LCD 宽度
    uint16_t height;  // LCD 高度
    uint16_t id;      // LCD ID
    uint8_t dir;      // 横屏还是竖屏控制：0，竖屏；1，横屏。
    uint16_t wramcmd; // 开始写gram指令
    uint16_t setxcmd; // 设置x坐标指令
    uint16_t setycmd; // 设置y坐标指令
} LCD_Device;

// LCD地址结构体
typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
} LCD_TypeDef;
/********************************************************
 * 全局宏定义
 */
// 背光引脚定义
#define LCD_BK_GPIO_Port GPIOB
#define LCD_BK_GPIO_Pin  GPIO_PIN_5

// 地址定义
#define LCD_BASE ((uint32_t)(0x60000000 | 0x0007FFFE))
#define LCD      ((LCD_TypeDef *)LCD_BASE)

// 扫描方向定义
#define L2R_U2D      0 // 从左到右,从上到下
#define L2R_D2U      1 // 从左到右,从下到上
#define R2L_U2D      2 // 从右到左,从上到下
#define R2L_D2U      3 // 从右到左,从下到上

#define U2D_L2R      4       // 从上到下,从左到右
#define U2D_R2L      5       // 从上到下,从右到左
#define D2U_L2R      6       // 从下到上,从左到右
#define D2U_R2L      7       // 从下到上,从右到左
#define DFT_SCAN_DIR L2R_U2D // 默认的扫描方向

// 画笔颜色
#define WHITE      0xFFFF
#define BLACK      0x0000
#define BLUE       0x001F
#define BRED       0XF81F
#define GRED       0XFFE0
#define GBLUE      0X07FF
#define RED        0xF800
#define MAGENTA    0xF81F
#define GREEN      0x07E0
#define CYAN       0x7FFF
#define YELLOW     0xFFE0
#define BROWN      0XBC40 // 棕色
#define BRRED      0XFC07 // 棕红色
#define GRAY       0X8430 // 灰色

#define DARKBLUE   0X01CF // 深蓝色
#define LIGHTBLUE  0X7D7C // 浅蓝色
#define GRAYBLUE   0X5458 // 灰蓝色

#define LIGHTGREEN 0X841F // 浅绿色
// #define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY     0XC618 // 浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE 0XA651 // 浅灰蓝色(中间层颜色)
#define LBBLUE    0X2B12 // 浅棕蓝色(选择条目的反色)

// LCD分辨率设置
#define SSD_HOR_RESOLUTION 800 // LCD水平分辨率
#define SSD_VER_RESOLUTION 480 // LCD垂直分辨率
// LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH 1   // 水平脉宽
#define SSD_HOR_BACK_PORCH  46  // 水平前廊
#define SSD_HOR_FRONT_PORCH 210 // 水平后廊

#define SSD_VER_PULSE_WIDTH 1  // 垂直脉宽
#define SSD_VER_BACK_PORCH  23 // 垂直前廊
#define SSD_VER_FRONT_PORCH 22 // 垂直前廊

#define SSD_HT              (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS             (SSD_HOR_BACK_PORCH)
#define SSD_VT              (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS             (SSD_VER_BACK_PORCH)

// LCD参数
extern LCD_Device lcddev; // 管理LCD重要参数

// LCD的画笔颜色和背景色
extern uint32_t POINT_COLOR; // 默认红色
extern uint32_t BACK_COLOR;  // 背景颜色.默认为白色

/*************************************************************
 * 用户函数
 */

void LCD_Init(void);                                                                                    // 初始化
void LCD_DisplayOn(void);                                                                               // 开显示
void LCD_DisplayOff(void);                                                                              // 关显示
void LCD_Clear(uint32_t Color);                                                                         // 清屏
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);                                                       // 设置光标
void LCD_DrawPoint(uint16_t x, uint16_t y);                                                             // 画点
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint32_t color);                                        // 快速画点
uint32_t LCD_ReadPoint(uint16_t x, uint16_t y);                                                         // 读点
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r);                                              // 画圆
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);                                  // 画线
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);                             // 画矩形
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);                      // 填充单色
void LCD_Color_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);               // 填充指定颜色
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode);                     // 显示一个字符
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size);                      // 显示一个数字
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);       // 显示 数字
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p); // 显示一个字符串,12/16字体

#ifdef __cplusplus
}
#endif
#endif