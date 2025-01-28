#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f1xx.h"
#include "usart.h"
/*************************用户引脚选择*********************************/

#define ESP8266_RST_PORT GPIOA
#define ESP8266_RST_PIN  GPIO_PIN_4
#define ESP8266_USART    3                          //串口号
/*************************IP 密码设置*********************************/

#define WIFI_SSID          "SSC"
#define WIFI_PWD           "15730000"
#define TCP_SERVER_IP      "192.168.137.1"
#define TCP_SERVER_PORT    "8080"
/**************************功能函数************************************/

uint8_t ESP8266_Init(void);
uint8_t ESP8266_Restore(void);
uint8_t ESP8266_AT_Test(void);
uint8_t ESP8266_Mode(uint8_t mode);
uint8_t ESP8266_Software_RST(void);
uint8_t ESP8266_ATE_Config(uint8_t cfg);
uint8_t ESP8266_JOIN_AP(char *ssid, char *pwd);
uint8_t ESP8266_GET_IP(char *buf);
uint8_t ESP8266_Connect_TCP_Server(char *server_ip, char *server_port);
uint8_t ESP8266_Enter_Unvarnished(void);
void ESP8266_Quit_Unvarnished(void);
uint8_t ESP8266_Connect_TCP_ATK(char *id, char *pwd);
uint8_t ESP8266_Disconnect_TCP_ATK(void);
/***************************用户函数************************************/

void ESP8266_Run(void);
#endif
