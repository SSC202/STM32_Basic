#include "delay_us.h"
#include "tim.h"

void delay_us(TIM_HandleTypeDef *htimx,uint16_t us)
{
    uint16_t delay = 0xffff-us-5;		//从此数进行计数

    HAL_TIM_Base_Start(htimx);  	    //开启计数器
    __HAL_TIM_SetCounter(htimx,delay);	//设置计数器
    while(delay<0xffff-5)
    {
        delay = __HAL_TIM_GetCounter(htimx); //获取当前计数值
    }
    HAL_TIM_Base_Stop(htimx);
}