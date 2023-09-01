#include "pwr.h"
#include "usart.h"
#include "gpio.h"

void PWR_Key_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    PWR_WKUP_GPIO_CLK_ENABLE();                             /* WKUP引脚时钟使能 */

    gpio_init_struct.Pin = PWR_WKUP_GPIO_PIN;               /* WKUP引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING;            /* 中断,上升沿 */
    gpio_init_struct.Pull = GPIO_PULLDOWN;                  /* 下拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(PWR_WKUP_GPIO_PORT, &gpio_init_struct);   /* WKUP引脚初始化 */

    HAL_NVIC_SetPriority(PWR_WKUP_INT_IRQn, 2, 2);          /* 抢占优先级2，子优先级2 */
    HAL_NVIC_EnableIRQ(PWR_WKUP_INT_IRQn); 
}

void PWR_WKUP_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(PWR_WKUP_GPIO_PIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == PWR_WKUP_GPIO_PIN)
    {
        /* HAL_GPIO_EXTI_IRQHandler()函数已经为我们清除了中断标志位，所以我们进了回调函数可以不做任何事 */
    }
}

void PWR_PVD_Init(void)
{
    PWR_PVDTypeDef pwr_pvd_init_struct;
    
    
    /* 1 使能PWR时钟 */
    __HAL_RCC_PWR_CLK_ENABLE();
    
    /* 2 配置PVD */
    pwr_pvd_init_struct.PVDLevel = PWR_PVDLEVEL_7;
    pwr_pvd_init_struct.Mode = PWR_PVD_MODE_IT_RISING_FALLING;
    HAL_PWR_ConfigPVD(&pwr_pvd_init_struct);
    
    /* 3 使能PVD */
    HAL_PWR_EnablePVD();
    
    /* 4 设置PVD中断优先级 */
    HAL_NVIC_SetPriority(PVD_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(PVD_IRQn); 
}

void PVD_IRQHandler(void)
{
    HAL_PWR_PVD_IRQHandler();
}


void HAL_PWR_PVDCallback(void)
{
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO))  /* 电压比PLS设置的还低 */
    {
        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET); 
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET); 
    }
}
