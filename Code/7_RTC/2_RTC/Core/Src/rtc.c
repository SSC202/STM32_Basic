#include "rtc.h"
#include "gpio.h"
#include "usart.h"
#include "delay.h"


RTC_HandleTypeDef hrtc; /* RTC���ƾ�� */
_calendar_obj calendar;         /* ʱ��ṹ�� */

/**
 * @brief       RTCд�������SRAM
 * @param       bkrx : �����Ĵ������,��Χ:0~41
                        ��Ӧ RTC_BKP_DR1~RTC_BKP_DR42
 * @param       data : Ҫд�������,16λ����
 * @retval      ��
 */
void RTC_Write_BKR(uint32_t bkrx, uint16_t data)
{
    HAL_PWR_EnableBkUpAccess(); /* ȡ��������д���� */
    HAL_RTCEx_BKUPWrite(&hrtc, bkrx + 1, data);
}

/**
 * @brief       RTC��ȡ������SRAM
 * @param       bkrx : �����Ĵ������,��Χ:0~41
                ��Ӧ RTC_BKP_DR1~RTC_BKP_DR42
 * @retval      ��ȡ����ֵ
 */
uint16_t RTC_Read_BKR(uint32_t bkrx)
{
    uint32_t temp = 0;
    temp = HAL_RTCEx_BKUPRead(&hrtc, bkrx + 1);
    return (uint16_t)temp; /* ���ض�ȡ����ֵ */
}

/**
 * @brief       RTC��ʼ��
 *   @note
 *              Ĭ�ϳ���ʹ��LSE,��LSE����ʧ�ܺ�,�л�ΪLSI.
 *              ͨ��BKP�Ĵ���0��ֵ,�����ж�RTCʹ�õ���LSE/LSI:
 *              ��BKP0==0X5050ʱ,ʹ�õ���LSE
 *              ��BKP0==0X5051ʱ,ʹ�õ���LSI
 *              ע��:�л�LSI/LSE������ʱ��/���ڶ�ʧ,�л�������������.
 *
 * @param       ��
 * @retval      0,�ɹ�
 *              1,�����ʼ��ģʽʧ��
 */
uint8_t MX_RTC_Init(void)
{
    /* ����ǲ��ǵ�һ������ʱ�� */
    uint16_t bkpflag = 0;

    __HAL_RCC_PWR_CLK_ENABLE(); /* ʹ��PWR��Դʱ�� */
    __HAL_RCC_BKP_CLK_ENABLE(); /* ʹ��BKP����ʱ�� */
    HAL_PWR_EnableBkUpAccess(); /* ȡ��������д���� */
    
    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = 32767;     /* ʱ����������,����ֵ��32767, ����Ҳ������ RTC_AUTO_1_SECOND */
    hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
    if (HAL_RTC_Init(&hrtc) != HAL_OK)  /* ��ʼ��RTC */
    {
        return 1;
    }
    
    bkpflag = RTC_Read_BKR(0);  /* ��ȡBKP0��ֵ */
    if ((bkpflag != 0X5050) && (bkpflag != 0x5051))         /* ֮ǰδ��ʼ�������������� */
    {
        RTC_SetTime(2023, 2, 27, 0, 0, 0);              /* ����ʱ�� */
    }

    __HAL_RTC_ALARM_ENABLE_IT(&hrtc, RTC_IT_SEC);   /* �������ж� */
    __HAL_RTC_ALARM_ENABLE_IT(&hrtc, RTC_IT_ALRA);  /* ���������ж� */
    
    HAL_NVIC_SetPriority(RTC_IRQn, 0x2, 0);                 /* ����RTC�ж� */
    HAL_NVIC_EnableIRQ(RTC_IRQn);                           /* ʹ���ж� */
    
    RTC_GetTime(); /* ����ʱ�� */
    
    return 0;
}

/**
 * @brief       RTC��ʼ��
 *   @note
 *              RTC�ײ�������ʱ������,�˺����ᱻHAL_RTC_Init()����
 * @param       hrtc:RTC���
 * @retval      ��
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    uint16_t retry = 200;
    
    __HAL_RCC_RTC_ENABLE();     /* RTCʱ��ʹ�� */

    RCC_OscInitTypeDef rcc_oscinitstruct;
    RCC_PeriphCLKInitTypeDef rcc_periphclkinitstruct;
    
    /* ʹ�üĴ����ķ�ʽȥ���LSE�Ƿ������������ */
    RCC->BDCR |= 1 << 0;    /* �����ⲿ��������LSE */
    
    while (retry && ((RCC->BDCR & 0X02) == 0))  /* �ȴ�LSE׼���� */
    {
        retry--;
        delay_ms(5);
    }

    if (retry == 0)     /* LSE����ʧ�� ʹ��LSI */
    {
        rcc_oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;  /* ѡ��Ҫ���õ����� */
        rcc_oscinitstruct.LSIState = RCC_LSI_ON;                    /* LSI״̬������ */
        rcc_oscinitstruct.PLL.PLLState = RCC_PLL_NONE;              /* PLL������ */
        HAL_RCC_OscConfig(&rcc_oscinitstruct);                      /* �������õ�rcc_oscinitstruct */

        rcc_periphclkinitstruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;   /* ѡ��Ҫ���õ����� RTC */
        rcc_periphclkinitstruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;   /* RTCʱ��Դѡ�� LSI */
        HAL_RCCEx_PeriphCLKConfig(&rcc_periphclkinitstruct);                /* �������õ�rcc_periphClkInitStruct */
        RTC_Write_BKR(0, 0X5051);
    }
    else
    {
        rcc_oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_LSE ; /* ѡ��Ҫ���õ����� */
        rcc_oscinitstruct.LSEState = RCC_LSE_ON;                    /* LSE״̬������ */
        rcc_oscinitstruct.PLL.PLLState = RCC_PLL_NONE;              /* PLL������ */
        HAL_RCC_OscConfig(&rcc_oscinitstruct);                      /* �������õ�rcc_oscinitstruct */

        rcc_periphclkinitstruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;   /* ѡ��Ҫ�������� RTC */
        rcc_periphclkinitstruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;   /* RTCʱ��Դѡ��LSE */
        HAL_RCCEx_PeriphCLKConfig(&rcc_periphclkinitstruct);                /* �������õ�rcc_periphclkinitstruct */
        RTC_Write_BKR(0, 0X5055);
    }
}

/**
 * @brief       RTCʱ���ж�
 *   @note      �����ж� / �����ж� ����ͬһ���жϷ�����
 *              ����RTC_CRL�Ĵ����� SECF �� ALRF λ�������ĸ��ж�
 * @param       ��
 * @retval      ��
 */
void RTC_IRQHandler(void)
{
    if (__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_SEC) != RESET)     /* ���ж� */
    {
        RTC_GetTime();     /* ����ʱ�� */
        __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_SEC);            /* ������ж� */
        //printf("sec:%d\r\n", calendar.sec);   /* ��ӡ���� */
    }

    if (__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_ALRAF) != RESET)   /* �����ж� */
    {
        __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);          /* ��������ж� */
        printf("Alarm Time:%d-%d-%d %d:%d:%d\n", calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, calendar.sec);
    }

    __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_OW);                 /* �������жϱ�־ */
    
    while (!__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_RTOFF));       /* �ȴ�RTC�Ĵ����������, ���ȴ�RTOFF == 1 */
}

/**
 * @brief       �ж�����Ƿ�������
 *   @note      �·�������:
 *              �·�   1  2  3  4  5  6  7  8  9  10 11 12
 *              ����   31 29 31 30 31 30 31 31 30 31 30 31
 *              ������ 31 28 31 30 31 30 31 31 30 31 30 31
 * @param       year : ���
 * @retval      0, ������; 1, ������;
 */
static uint8_t RTC_LeapYear(uint16_t year)
{
    /* �������: ��������겻���İ������� */
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief       ����ʱ��, ����������ʱ����
 *   @note      ��1970��1��1��Ϊ��׼, �����ۼ�ʱ��
 *              �Ϸ���ݷ�ΧΪ: 1970 ~ 2105��
                HALĬ��Ϊ������Ϊ2000��
 * @param       syear : ���
 * @param       smon  : �·�
 * @param       sday  : ����
 * @param       hour  : Сʱ
 * @param       min   : ����
 * @param       sec   : ����
 * @retval      0, �ɹ�; 1, ʧ��;
 */
uint8_t RTC_SetTime(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    seccount = RTC_Sec(syear, smon, sday, hour, min, sec); /* ��������ʱ����ת������������ */

    __HAL_RCC_PWR_CLK_ENABLE(); /* ʹ�ܵ�Դʱ�� */
    __HAL_RCC_BKP_CLK_ENABLE(); /* ʹ�ܱ�����ʱ�� */
    HAL_PWR_EnableBkUpAccess(); /* ȡ��������д���� */
    /* ���������Ǳ����! */
    
    RTC->CRL |= 1 << 4;         /* ��������ģʽ */
    
    RTC->CNTL = seccount & 0xffff;
    RTC->CNTH = seccount >> 16;
    
    RTC->CRL &= ~(1 << 4);      /* �˳�����ģʽ */

    while (!__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_RTOFF));       /* �ȴ�RTC�Ĵ����������, ���ȴ�RTOFF == 1 */

    return 0;
}

/**
 * @brief       ��������, ���嵽������ʱ����
 *   @note      ��1970��1��1��Ϊ��׼, �����ۼ�ʱ��
 *              �Ϸ���ݷ�ΧΪ: 1970 ~ 2105��
 * @param       syear : ���
 * @param       smon  : �·�
 * @param       sday  : ����
 * @param       hour  : Сʱ
 * @param       min   : ����
 * @param       sec   : ����
 * @retval      0, �ɹ�; 1, ʧ��;
 */
uint8_t RTC_SetAlarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    seccount = RTC_Sec(syear, smon, sday, hour, min, sec); /* ��������ʱ����ת������������ */

    __HAL_RCC_PWR_CLK_ENABLE(); /* ʹ�ܵ�Դʱ�� */
    __HAL_RCC_BKP_CLK_ENABLE(); /* ʹ�ܱ�����ʱ�� */
    HAL_PWR_EnableBkUpAccess(); /* ȡ��������д���� */
    /* ���������Ǳ����! */
    
    RTC->CRL |= 1 << 4;         /* ��������ģʽ */
    
    RTC->ALRL = seccount & 0xffff;
    RTC->ALRH = seccount >> 16;
    
    RTC->CRL &= ~(1 << 4);      /* �˳�����ģʽ */

    while (!__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_RTOFF));       /* �ȴ�RTC�Ĵ����������, ���ȴ�RTOFF == 1 */

    return 0;
}

/**
 * @brief       �õ���ǰ��ʱ��
 *   @note      �ú�����ֱ�ӷ���ʱ��, ʱ�����ݱ�����calendar�ṹ������
 * @param       ��
 * @retval      ��
 */
void RTC_GetTime(void)
{
    static uint16_t daycnt = 0;
    uint32_t seccount = 0;
    uint32_t temp = 0;
    uint16_t temp1 = 0;
    const uint8_t month_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* ƽ����·����ڱ� */

    seccount = RTC->CNTH; /* �õ��������е�ֵ(������) */
    seccount <<= 16;
    seccount += RTC->CNTL;

    temp = seccount / 86400; /* �õ�����(��������Ӧ��) */

    if (daycnt != temp) /* ����һ���� */
    {
        daycnt = temp;
        temp1 = 1970;   /* ��1970�꿪ʼ */

        while (temp >= 365)
        {
            if (RTC_LeapYear(temp1)) /* ������ */
            {
                if (temp >= 366)
                {
                    temp -= 366;    /* ����������� */
                }
                else
                {
                    break;
                }
            }
            else
            {
                temp -= 365;        /* ƽ�� */
            }

            temp1++;
        }

        calendar.year = temp1;      /* �õ���� */
        temp1 = 0;

        while (temp >= 28)      /* ������һ���� */
        {
            if (RTC_LeapYear(calendar.year) && temp1 == 1) /* �����ǲ�������/2�·� */
            {
                if (temp >= 29)
                {
                    temp -= 29; /* ����������� */
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (temp >= month_table[temp1])
                {
                    temp -= month_table[temp1]; /* ƽ�� */
                }
                else
                {
                    break;
                }
            }

            temp1++;
        }

        calendar.month = temp1 + 1; /* �õ��·� */
        calendar.date = temp + 1;   /* �õ����� */
    }

    temp = seccount % 86400;                                                    /* �õ������� */
    calendar.hour = temp / 3600;                                                /* Сʱ */
    calendar.min = (temp % 3600) / 60;                                          /* ���� */
    calendar.sec = (temp % 3600) % 60;                                          /* ���� */
    calendar.week = RTC_GetWeek(calendar.year, calendar.month, calendar.date); /* ��ȡ���� */
}

/**
 * @brief       ��������ʱ����ת����������
 *   @note      ���빫�����ڵõ�����(��ʼʱ��Ϊ: ��Ԫ0��3��1�տ�ʼ, ����������κ�����, �����Ի�ȡ��ȷ������)
 *              ʹ�� ��ķ����ɭ���㹫ʽ ����, ԭ��˵��������:
 *              https://www.cnblogs.com/fengbohello/p/3264300.html
 * @param       syear : ���
 * @param       smon  : �·�
 * @param       sday  : ����
 * @retval      0, ������; 1 ~ 6: ����һ ~ ������
 */
uint8_t RTC_GetWeek(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t week = 0;

    if (month < 3)
    {
        month += 12;
        --year;
    }

    week = (day + 1 + 2 * month + 3 * (month + 1) / 5 + year + (year >> 2) - year / 100 + year / 400) % 7;
    return week;
}

/**
 * @brief       ��������ʱ����ת����������
 *   @note      ��1970��1��1��Ϊ��׼, 1970��1��1��, 0ʱ0��0��, ��ʾ��0����
 *              ����ʾ��2105��, ��Ϊuint32_t����ʾ136���������(����������)!
 *              ������ο�ֻlinux mktime����, ԭ��˵��������:
 *              http://www.openedv.com/thread-63389-1-1.html
 * @param       syear : ���
 * @param       smon  : �·�
 * @param       sday  : ����
 * @param       hour  : Сʱ
 * @param       min   : ����
 * @param       sec   : ����
 * @retval      ת�����������
 */
static long RTC_Sec(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t Y, M, D, X, T;
    signed char monx = smon;    /* ���·�ת���ɴ����ŵ�ֵ, ����������� */

    if (0 >= (monx -= 2))       /* 1..12 -> 11,12,1..10 */
    {
        monx += 12; /* Puts Feb last since it has leap day */
        syear -= 1;
    }

    Y = (syear - 1) * 365 + syear / 4 - syear / 100 + syear / 400; /* ��ԪԪ��1�����ڵ������� */
    M = 367 * monx / 12 - 30 + 59;
    D = sday - 1;
    X = Y + M + D - 719162;                      /* ��ȥ��ԪԪ�굽1970������� */
    T = ((X * 24 + hour) * 60 + min) * 60 + sec; /* �������� */
    return T;
}
