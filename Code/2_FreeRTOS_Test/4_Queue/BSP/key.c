#include "key.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

uint8_t Key_Scan(void)
{
    static uint8_t key_up = 0;

    int KEY1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8);
    int KEY2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9);

    if ((key_up == 0) && (KEY1 == 0 || KEY2 == 0))  // 检测到刚按下进入，如果是按住不放不会进入
    {
        osDelay(100);                               // 去抖动
        key_up = 1;                                 // 设置标志位，表示按下

        if (KEY1 == 0)
            return 1;                               // 如果k1按下就返回1
        else if (KEY2 == 0)
            return 2;                               // 如果k2按下就返回2
    } else if (KEY1 == 1 && KEY2 == 1)
        key_up = 0;                                 // 按键松开，清标志位

    return 0;                                       // 无按键按下或松开了时就返回0
}