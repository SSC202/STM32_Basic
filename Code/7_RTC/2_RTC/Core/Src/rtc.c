#include "rtc.h"
#include "gpio.h"
#include "usart.h"
#include "delay.h"


RTC_HandleTypeDef hrtc; /* RTC控制句柄 */
_calendar_obj calendar;         /* 时间结构体 */

/**
 * @brief       RTC写入后备区域SRAM
 * @param       bkrx : 后备区寄存器编号,范围:0~41
                        对应 RTC_BKP_DR1~RTC_BKP_DR42
 * @param       data : 要写入的数据,16位长度
 * @retval      无
 */
void RTC_Write_BKR(uint32_t bkrx, uint16_t data)
{
    HAL_PWR_EnableBkUpAccess(); /* 取消备份区写保护 */
    HAL_RTCEx_BKUPWrite(&hrtc, bkrx + 1, data);
}

/**
 * @brief       RTC读取后备区域SRAM
 * @param       bkrx : 后备区寄存器编号,范围:0~41
                对应 RTC_BKP_DR1~RTC_BKP_DR42
 * @retval      读取到的值
 */
uint16_t RTC_Read_BKR(uint32_t bkrx)
{
    uint32_t temp = 0;
    temp = HAL_RTCEx_BKUPRead(&hrtc, bkrx + 1);
    return (uint16_t)temp; /* 返回读取到的值 */
}

/**
 * @brief       RTC初始化
 *   @note
 *              默认尝试使用LSE,当LSE启动失败后,切换为LSI.
 *              通过BKP寄存器0的值,可以判断RTC使用的是LSE/LSI:
 *              当BKP0==0X5050时,使用的是LSE
 *              当BKP0==0X5051时,使用的是LSI
 *              注意:切换LSI/LSE将导致时间/日期丢失,切换后需重新设置.
 *
 * @param       无
 * @retval      0,成功
 *              1,进入初始化模式失败
 */
uint8_t MX_RTC_Init(void)
{
    /* 检查是不是第一次配置时钟 */
    uint16_t bkpflag = 0;

    __HAL_RCC_PWR_CLK_ENABLE(); /* 使能PWR电源时钟 */
    __HAL_RCC_BKP_CLK_ENABLE(); /* 使能BKP备份时钟 */
    HAL_PWR_EnableBkUpAccess(); /* 取消备份区写保护 */
    
    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = 32767;     /* 时钟周期设置,理论值：32767, 这里也可以用 RTC_AUTO_1_SECOND */
    hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
    if (HAL_RTC_Init(&hrtc) != HAL_OK)  /* 初始化RTC */
    {
        return 1;
    }
    
    bkpflag = RTC_Read_BKR(0);  /* 读取BKP0的值 */
    if ((bkpflag != 0X5050) && (bkpflag != 0x5051))         /* 之前未初始化过，重新配置 */
    {
        RTC_SetTime(2023, 2, 27, 0, 0, 0);              /* 设置时间 */
    }

    __HAL_RTC_ALARM_ENABLE_IT(&hrtc, RTC_IT_SEC);   /* 允许秒中断 */
    __HAL_RTC_ALARM_ENABLE_IT(&hrtc, RTC_IT_ALRA);  /* 允许闹钟中断 */
    
    HAL_NVIC_SetPriority(RTC_IRQn, 0x2, 0);                 /* 设置RTC中断 */
    HAL_NVIC_EnableIRQ(RTC_IRQn);                           /* 使能中断 */
    
    RTC_GetTime(); /* 更新时间 */
    
    return 0;
}

/**
 * @brief       RTC初始化
 *   @note
 *              RTC底层驱动，时钟配置,此函数会被HAL_RTC_Init()调用
 * @param       hrtc:RTC句柄
 * @retval      无
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    uint16_t retry = 200;
    
    __HAL_RCC_RTC_ENABLE();     /* RTC时钟使能 */

    RCC_OscInitTypeDef rcc_oscinitstruct;
    RCC_PeriphCLKInitTypeDef rcc_periphclkinitstruct;
    
    /* 使用寄存器的方式去检测LSE是否可以正常工作 */
    RCC->BDCR |= 1 << 0;    /* 开启外部低速振荡器LSE */
    
    while (retry && ((RCC->BDCR & 0X02) == 0))  /* 等待LSE准备好 */
    {
        retry--;
        delay_ms(5);
    }

    if (retry == 0)     /* LSE起振失败 使用LSI */
    {
        rcc_oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;  /* 选择要配置的振荡器 */
        rcc_oscinitstruct.LSIState = RCC_LSI_ON;                    /* LSI状态：开启 */
        rcc_oscinitstruct.PLL.PLLState = RCC_PLL_NONE;              /* PLL无配置 */
        HAL_RCC_OscConfig(&rcc_oscinitstruct);                      /* 配置设置的rcc_oscinitstruct */

        rcc_periphclkinitstruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;   /* 选择要配置的外设 RTC */
        rcc_periphclkinitstruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;   /* RTC时钟源选择 LSI */
        HAL_RCCEx_PeriphCLKConfig(&rcc_periphclkinitstruct);                /* 配置设置的rcc_periphClkInitStruct */
        RTC_Write_BKR(0, 0X5051);
    }
    else
    {
        rcc_oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_LSE ; /* 选择要配置的振荡器 */
        rcc_oscinitstruct.LSEState = RCC_LSE_ON;                    /* LSE状态：开启 */
        rcc_oscinitstruct.PLL.PLLState = RCC_PLL_NONE;              /* PLL不配置 */
        HAL_RCC_OscConfig(&rcc_oscinitstruct);                      /* 配置设置的rcc_oscinitstruct */

        rcc_periphclkinitstruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;   /* 选择要配置外设 RTC */
        rcc_periphclkinitstruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;   /* RTC时钟源选择LSE */
        HAL_RCCEx_PeriphCLKConfig(&rcc_periphclkinitstruct);                /* 配置设置的rcc_periphclkinitstruct */
        RTC_Write_BKR(0, 0X5055);
    }
}

/**
 * @brief       RTC时钟中断
 *   @note      秒钟中断 / 闹钟中断 共用同一个中断服务函数
 *              根据RTC_CRL寄存器的 SECF 和 ALRF 位区分是哪个中断
 * @param       无
 * @retval      无
 */
void RTC_IRQHandler(void)
{
    if (__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_SEC) != RESET)     /* 秒中断 */
    {
        RTC_GetTime();     /* 更新时间 */
        __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_SEC);            /* 清除秒中断 */
        //printf("sec:%d\r\n", calendar.sec);   /* 打印秒钟 */
    }

    if (__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_ALRAF) != RESET)   /* 闹钟中断 */
    {
        __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);          /* 清除闹钟中断 */
        printf("Alarm Time:%d-%d-%d %d:%d:%d\n", calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, calendar.sec);
    }

    __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_OW);                 /* 清除溢出中断标志 */
    
    while (!__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_RTOFF));       /* 等待RTC寄存器操作完成, 即等待RTOFF == 1 */
}

/**
 * @brief       判断年份是否是闰年
 *   @note      月份天数表:
 *              月份   1  2  3  4  5  6  7  8  9  10 11 12
 *              闰年   31 29 31 30 31 30 31 31 30 31 30 31
 *              非闰年 31 28 31 30 31 30 31 31 30 31 30 31
 * @param       year : 年份
 * @retval      0, 非闰年; 1, 是闰年;
 */
static uint8_t RTC_LeapYear(uint16_t year)
{
    /* 闰年规则: 四年闰百年不闰，四百年又闰 */
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
 * @brief       设置时间, 包括年月日时分秒
 *   @note      以1970年1月1日为基准, 往后累加时间
 *              合法年份范围为: 1970 ~ 2105年
                HAL默认为年份起点为2000年
 * @param       syear : 年份
 * @param       smon  : 月份
 * @param       sday  : 日期
 * @param       hour  : 小时
 * @param       min   : 分钟
 * @param       sec   : 秒钟
 * @retval      0, 成功; 1, 失败;
 */
uint8_t RTC_SetTime(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    seccount = RTC_Sec(syear, smon, sday, hour, min, sec); /* 将年月日时分秒转换成总秒钟数 */

    __HAL_RCC_PWR_CLK_ENABLE(); /* 使能电源时钟 */
    __HAL_RCC_BKP_CLK_ENABLE(); /* 使能备份域时钟 */
    HAL_PWR_EnableBkUpAccess(); /* 取消备份域写保护 */
    /* 上面三步是必须的! */
    
    RTC->CRL |= 1 << 4;         /* 进入配置模式 */
    
    RTC->CNTL = seccount & 0xffff;
    RTC->CNTH = seccount >> 16;
    
    RTC->CRL &= ~(1 << 4);      /* 退出配置模式 */

    while (!__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_RTOFF));       /* 等待RTC寄存器操作完成, 即等待RTOFF == 1 */

    return 0;
}

/**
 * @brief       设置闹钟, 具体到年月日时分秒
 *   @note      以1970年1月1日为基准, 往后累加时间
 *              合法年份范围为: 1970 ~ 2105年
 * @param       syear : 年份
 * @param       smon  : 月份
 * @param       sday  : 日期
 * @param       hour  : 小时
 * @param       min   : 分钟
 * @param       sec   : 秒钟
 * @retval      0, 成功; 1, 失败;
 */
uint8_t RTC_SetAlarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    seccount = RTC_Sec(syear, smon, sday, hour, min, sec); /* 将年月日时分秒转换成总秒钟数 */

    __HAL_RCC_PWR_CLK_ENABLE(); /* 使能电源时钟 */
    __HAL_RCC_BKP_CLK_ENABLE(); /* 使能备份域时钟 */
    HAL_PWR_EnableBkUpAccess(); /* 取消备份域写保护 */
    /* 上面三步是必须的! */
    
    RTC->CRL |= 1 << 4;         /* 进入配置模式 */
    
    RTC->ALRL = seccount & 0xffff;
    RTC->ALRH = seccount >> 16;
    
    RTC->CRL &= ~(1 << 4);      /* 退出配置模式 */

    while (!__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_RTOFF));       /* 等待RTC寄存器操作完成, 即等待RTOFF == 1 */

    return 0;
}

/**
 * @brief       得到当前的时间
 *   @note      该函数不直接返回时间, 时间数据保存在calendar结构体里面
 * @param       无
 * @retval      无
 */
void RTC_GetTime(void)
{
    static uint16_t daycnt = 0;
    uint32_t seccount = 0;
    uint32_t temp = 0;
    uint16_t temp1 = 0;
    const uint8_t month_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* 平年的月份日期表 */

    seccount = RTC->CNTH; /* 得到计数器中的值(秒钟数) */
    seccount <<= 16;
    seccount += RTC->CNTL;

    temp = seccount / 86400; /* 得到天数(秒钟数对应的) */

    if (daycnt != temp) /* 超过一天了 */
    {
        daycnt = temp;
        temp1 = 1970;   /* 从1970年开始 */

        while (temp >= 365)
        {
            if (RTC_LeapYear(temp1)) /* 是闰年 */
            {
                if (temp >= 366)
                {
                    temp -= 366;    /* 闰年的秒钟数 */
                }
                else
                {
                    break;
                }
            }
            else
            {
                temp -= 365;        /* 平年 */
            }

            temp1++;
        }

        calendar.year = temp1;      /* 得到年份 */
        temp1 = 0;

        while (temp >= 28)      /* 超过了一个月 */
        {
            if (RTC_LeapYear(calendar.year) && temp1 == 1) /* 当年是不是闰年/2月份 */
            {
                if (temp >= 29)
                {
                    temp -= 29; /* 闰年的秒钟数 */
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
                    temp -= month_table[temp1]; /* 平年 */
                }
                else
                {
                    break;
                }
            }

            temp1++;
        }

        calendar.month = temp1 + 1; /* 得到月份 */
        calendar.date = temp + 1;   /* 得到日期 */
    }

    temp = seccount % 86400;                                                    /* 得到秒钟数 */
    calendar.hour = temp / 3600;                                                /* 小时 */
    calendar.min = (temp % 3600) / 60;                                          /* 分钟 */
    calendar.sec = (temp % 3600) % 60;                                          /* 秒钟 */
    calendar.week = RTC_GetWeek(calendar.year, calendar.month, calendar.date); /* 获取星期 */
}

/**
 * @brief       将年月日时分秒转换成秒钟数
 *   @note      输入公历日期得到星期(起始时间为: 公元0年3月1日开始, 输入往后的任何日期, 都可以获取正确的星期)
 *              使用 基姆拉尔森计算公式 计算, 原理说明见此贴:
 *              https://www.cnblogs.com/fengbohello/p/3264300.html
 * @param       syear : 年份
 * @param       smon  : 月份
 * @param       sday  : 日期
 * @retval      0, 星期天; 1 ~ 6: 星期一 ~ 星期六
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
 * @brief       将年月日时分秒转换成秒钟数
 *   @note      以1970年1月1日为基准, 1970年1月1日, 0时0分0秒, 表示第0秒钟
 *              最大表示到2105年, 因为uint32_t最大表示136年的秒钟数(不包括闰年)!
 *              本代码参考只linux mktime函数, 原理说明见此贴:
 *              http://www.openedv.com/thread-63389-1-1.html
 * @param       syear : 年份
 * @param       smon  : 月份
 * @param       sday  : 日期
 * @param       hour  : 小时
 * @param       min   : 分钟
 * @param       sec   : 秒钟
 * @retval      转换后的秒钟数
 */
static long RTC_Sec(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t Y, M, D, X, T;
    signed char monx = smon;    /* 将月份转换成带符号的值, 方便后面运算 */

    if (0 >= (monx -= 2))       /* 1..12 -> 11,12,1..10 */
    {
        monx += 12; /* Puts Feb last since it has leap day */
        syear -= 1;
    }

    Y = (syear - 1) * 365 + syear / 4 - syear / 100 + syear / 400; /* 公元元年1到现在的闰年数 */
    M = 367 * monx / 12 - 30 + 59;
    D = sday - 1;
    X = Y + M + D - 719162;                      /* 减去公元元年到1970年的天数 */
    T = ((X * 24 + hour) * 60 + min) * 60 + sec; /* 总秒钟数 */
    return T;
}
