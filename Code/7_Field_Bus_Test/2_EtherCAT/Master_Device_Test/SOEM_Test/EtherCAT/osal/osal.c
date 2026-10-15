/*
 * Licensed under the GNU General Public License version 2 with exceptions. See
 * LICENSE file in the project root for full license information
 */

#include <osal.h>
#include <time.h>
#include "tim.h"

#define  timercmp(a, b, CMP)                                \
  (((a)->tv_sec == (b)->tv_sec) ?                           \
   ((a)->tv_usec CMP (b)->tv_usec) :                        \
   ((a)->tv_sec CMP (b)->tv_sec))
#define  timeradd(a, b, result)                             \
  do {                                                      \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;           \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;        \
    if ((result)->tv_usec >= 1000000)                       \
    {                                                       \
       ++(result)->tv_sec;                                  \
       (result)->tv_usec -= 1000000;                        \
    }                                                       \
  } while (0)
#define  timersub(a, b, result)                             \
  do {                                                      \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;           \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;        \
    if ((result)->tv_usec < 0) {                            \
      --(result)->tv_sec;                                   \
      (result)->tv_usec += 1000000;                         \
    }                                                       \
  } while (0)

#define USECS_PER_SEC   1000000
// #define USECS_PER_TICK  (USECS_PER_SEC / CFG_TICKS_PER_SECOND)

int osal_usleep(uint32 usec)
{
   osal_timert qtime;
   osal_timer_start(&qtime, usec);

   while(!osal_timer_is_expired(&qtime));
   return 1;
}

int gettimeofday(timeval *tv)
{
    uint32_t sec = GetSec();
    uint32_t us = GetUSec();
    
    if(sec != GetSec())
    {
        sec = GetSec();
        us = GetUSec();
    }
    tv->tv_usec = us;
    tv->tv_sec = sec;

    return 0;
}

ec_timet osal_current_time (void)
{
    ec_timet return_value;
    
    uint32_t sec = GetSec();
    uint32_t us = GetUSec();
    
    if(sec != GetSec())
    {
        sec = GetSec();
        us = GetUSec();
    }
    
    return_value.usec = us;
    return_value.sec = sec;
    
    return return_value;
}

void osal_timer_start (osal_timert * self, uint32 timeout_usec)
{
   ec_timet ec_t_now;
    
   timeval start_time;
   timeval timeout;
   timeval stop_time;

   ec_t_now = osal_current_time();
   
   start_time.tv_sec = ec_t_now.sec;
   start_time.tv_usec = ec_t_now.usec;
   
   timeout.tv_sec = timeout_usec / USECS_PER_SEC;
   timeout.tv_usec = timeout_usec % USECS_PER_SEC;
   timeradd(&start_time, &timeout, &stop_time);

   self->stop_time.sec = stop_time.tv_sec;
   self->stop_time.usec = stop_time.tv_usec;
    
}

boolean osal_timer_is_expired (osal_timert * self)
{
   ec_timet ec_t_now;
    
   timeval current_time;
   timeval stop_time;
   int is_not_yet_expired;

   ec_t_now = osal_current_time();
   current_time.tv_sec = ec_t_now.sec;
   current_time.tv_usec = ec_t_now.usec;
   
   stop_time.tv_sec = self->stop_time.sec;
   stop_time.tv_usec = self->stop_time.usec;
   is_not_yet_expired = timercmp(&current_time, &stop_time, <);

   return is_not_yet_expired == FALSE;
}
