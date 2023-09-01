#ifndef __RTC_H
#define __RTC_H

#include "sys.h"


/* 时间结构体, 包括年月日周时分秒等信息 */
typedef struct
{
    uint8_t hour;       /* 时 */
    uint8_t min;        /* 分 */
    uint8_t sec;        /* 秒 */
    /* 公历年月日周 */
    uint16_t year;      /* 年 */
    uint8_t  month;     /* 月 */
    uint8_t  date;      /* 日 */
    uint8_t  week;      /* 周 */
} _calendar_obj;

extern _calendar_obj calendar;                      /* 时间结构体 */

/* 静态函数 */
static uint8_t RTC_LeapYear(uint16_t year);     /* 判断当前年份是不是闰年 */
static long RTC_Sec(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);   /* 将年月日时分秒转换成秒钟数 */

/* 接口函数 */
uint8_t MX_RTC_Init(void);                             /* 初始化RTC */
void RTC_GetTime(void);                            /* 获取RTC时间信息 */
uint16_t RTC_Read_BKR(uint32_t bkrx);               /* 读取后备寄存器 */
void RTC_Write_BKR(uint32_t bkrx, uint16_t data);   /* 写后备寄存器 */ 
uint8_t RTC_GetWeek(uint16_t year, uint8_t month, uint8_t day);    /* 根据年月日获取星期几 */
uint8_t RTC_SetTime(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);   /* 设置时间 */
uint8_t RTC_SetAlarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);  /* 设置闹钟时间 */

#endif
