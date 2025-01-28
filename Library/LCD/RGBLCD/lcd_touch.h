#ifndef __LCD_TOUCH_H
#define __LCD_TOUCH_H

#include "lcd.h"
/************************结构体段******************************/
/* 触摸点坐标数据结构 */
typedef struct
{
    uint16_t x;    /* 触摸点X坐标 */
    uint16_t y;    /* 触摸点Y坐标 */
    uint16_t size; /* 触摸点大小 */
} LCD_touch_point_t;

/* RGB LCD触摸IIC通讯地址枚举 */
typedef enum {
    LCD_TOUCH_IIC_ADDR_14 = 0x14, /* 0x14 */
    LCD_TOUCH_IIC_ADDR_5D = 0x5D, /* 0x5D */
    LCD_TOUCH_IIC_ADDR_38 = 0x38, /* 0x38 */
} LCD_touch_iic_addr_t;

/* RGB LCD触摸IC类型枚举 */
typedef enum {
    LCD_TOUCH_TYPE_GTXX = 0x00, /* GT9111、GT9147、GT9271 */
    LCD_TOUCH_TYPE_FTXX,        /* FT5206 */
} LCD_touch_type_t;
/************************用户函数段******************************/
uint8_t LCD_Touch_Init(LCD_touch_type_t type);
uint8_t LCD_Touch_Scan(LCD_touch_point_t *point, uint8_t cnt);

#endif