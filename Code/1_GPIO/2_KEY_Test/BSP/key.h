#ifndef __KEY_H
#define	__KEY_H

#include "stm32f1xx.h"

#define KEY_Port	GPIOA
#define KEY_Pin   GPIO_PIN_5

extern uint8_t key_flag;

void Key_Scan(void);

#endif
