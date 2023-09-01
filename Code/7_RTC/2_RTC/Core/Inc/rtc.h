#ifndef __RTC_H
#define __RTC_H

#include "sys.h"


/* ʱ��ṹ��, ������������ʱ�������Ϣ */
typedef struct
{
    uint8_t hour;       /* ʱ */
    uint8_t min;        /* �� */
    uint8_t sec;        /* �� */
    /* ������������ */
    uint16_t year;      /* �� */
    uint8_t  month;     /* �� */
    uint8_t  date;      /* �� */
    uint8_t  week;      /* �� */
} _calendar_obj;

extern _calendar_obj calendar;                      /* ʱ��ṹ�� */

/* ��̬���� */
static uint8_t RTC_LeapYear(uint16_t year);     /* �жϵ�ǰ����ǲ������� */
static long RTC_Sec(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);   /* ��������ʱ����ת���������� */

/* �ӿں��� */
uint8_t MX_RTC_Init(void);                             /* ��ʼ��RTC */
void RTC_GetTime(void);                            /* ��ȡRTCʱ����Ϣ */
uint16_t RTC_Read_BKR(uint32_t bkrx);               /* ��ȡ�󱸼Ĵ��� */
void RTC_Write_BKR(uint32_t bkrx, uint16_t data);   /* д�󱸼Ĵ��� */ 
uint8_t RTC_GetWeek(uint16_t year, uint8_t month, uint8_t day);    /* ���������ջ�ȡ���ڼ� */
uint8_t RTC_SetTime(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);   /* ����ʱ�� */
uint8_t RTC_SetAlarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);  /* ��������ʱ�� */

#endif
