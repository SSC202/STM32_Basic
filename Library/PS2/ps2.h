#ifndef _PS2_H
#define _PS2_H

#include "stm32f4xx.h"
#include "delay.h"
#include <stdio.h>

#define PS2_CS_Port                     GPIOA
#define PS2_CS_Pin                      GPIO_PIN_15

#define PS2_CLK_Port                    GPIOC
#define PS2_CLK_Pin                     GPIO_PIN_10

#define PS2_CMD_Port                    GPIOC               //CMD
#define PS2_CMD_Pin                     GPIO_PIN_12

#define PS2_DAT_Port                    GPIOC               //DAT
#define PS2_DAT_Pin                     GPIO_PIN_11

typedef struct
{
    uint8_t A_D;                                            //模拟(红灯)为1 数字(无灯)为0
    int8_t  Rocker_RX, Rocker_RY, Rocker_LX, Rocker_LY;     //摇杆值(模拟状态为实际值0-0xFF)(数字态为等效的值0,0x80,0xFF)
    //按键值0为未触发,1为触发态
    uint8_t Key_L1, Key_L2, Key_R1, Key_R2;                 //后侧大按键
    uint8_t Key_L_Right, Key_L_Left, Key_L_Up, Key_L_Down;  //左侧按键
    uint8_t Key_R_Right, Key_R_Left, Key_R_Up, Key_R_Down;  //右侧按键
    uint8_t Key_Select;                                     //选择键
    uint8_t Key_Start;                                      //开始键
    uint8_t Key_Rocker_Left, Key_Rocker_Right;              //摇杆按键
} PS2_TypeDef;
extern  PS2_TypeDef PS2_Data;
extern  uint8_t     PS2_RawData[9];

void _PS2_CS(uint8_t val);
void _PS2_CLK(uint8_t val);
void _PS2_CMD(uint8_t val);
uint8_t _PS2_DAT(void);
uint8_t _PS2_ReadWrite_Byte(uint8_t TxData);
void _PS2_Decode(void);
void _PS2_Read_Data(void);


#endif 

