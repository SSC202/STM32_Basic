#include "key.h"

uint8_t key_flag = 0;

void Key_Scan(void)
{
	if(HAL_GPIO_ReadPin(KEY_Port,KEY_Pin) == GPIO_PIN_RESET)
	{
		HAL_Delay(20);
		if(HAL_GPIO_ReadPin(KEY_Port,KEY_Pin) == GPIO_PIN_RESET)
		{
			if(key_flag == 0)
			{
				key_flag = 1;
			}
			else
			{
				key_flag = 0;
			}
		}
	}
}
