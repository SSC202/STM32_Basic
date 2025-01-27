#ifndef __OLED_H
#define __OLED_H
			  	 
#include "stm32f1xx_hal.h"
 	
#define SIZE                        16
#define XLevelL		                0x00
#define XLevelH		                0x10

#define Max_Column	                128
#define Max_Row		                64
#define	OLED_BRIGHTNESS	            0xFF 

#define X_WIDTH 	                128
#define Y_WIDTH 	                64	  

#define OLED_CMD  0	
#define OLED_DATA 1	
  						  
#define OLED_RST_Port			GPIOB
#define OLED_RST_Pin		    GPIO_PIN_6
#define OLED_DC_Port			GPIOB
#define OLED_DC_Pin		        GPIO_PIN_7
#define OLED_SCL_Port			GPIOB               //DO
#define OLED_SCL_Pin		    GPIO_PIN_4
#define OLED_SDA_Port			GPIOB               //DI
#define OLED_SDA_Pin		    GPIO_PIN_5
#define OLED_CS_Port            GPIOB
#define OLED_CS_Pin             GPIO_PIN_8


#define OLED_CS(x)      do{ x == 1 ? \
                        HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, GPIO_PIN_SET) : \
                        HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, GPIO_PIN_RESET); \
                        }while(0)   

#define OLED_RST(x)     do{ x == 1 ? \
                        HAL_GPIO_WritePin(OLED_RST_Port, OLED_RST_Pin, GPIO_PIN_SET) : \
                        HAL_GPIO_WritePin(OLED_RST_Port, OLED_RST_Pin, GPIO_PIN_RESET); \
                        }while(0)   

#define OLED_DC(x)      do{ x == 1 ? \
                        HAL_GPIO_WritePin(OLED_DC_Port, OLED_DC_Pin, GPIO_PIN_SET) : \
                        HAL_GPIO_WritePin(OLED_DC_Port, OLED_DC_Pin, GPIO_PIN_RESET); \
                        }while(0)   

#define OLED_SCL(x)     do{ x == 1 ? \
                        HAL_GPIO_WritePin(OLED_SCL_Port, OLED_SCL_Pin, GPIO_PIN_SET) : \
                        HAL_GPIO_WritePin(OLED_SCL_Port, OLED_SCL_Pin, GPIO_PIN_RESET); \
                        }while(0)   

#define OLED_SDA(x)     do{ x == 1 ? \
                        HAL_GPIO_WritePin(OLED_SDA_Port, OLED_SDA_Pin, GPIO_PIN_SET) : \
                        HAL_GPIO_WritePin(OLED_SDA_Port, OLED_SDA_Pin, GPIO_PIN_RESET); \
                        }while(0)   

 		    
void OLED_Write_Byte(unsigned char dat, unsigned char flag);
void OLED_Write_Cmd(uint8_t cmd);
void OLED_Write_Data(uint8_t dat);
uint32_t OLED_Pow(uint8_t m, uint8_t n);

void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);

void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t charSize, uint8_t Is_Reverse);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t charSize, uint8_t Is_Reverse);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t Size, uint8_t Is_Reverse);
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t charSize, uint8_t Is_Reverse);	 
void OLED_Set_Point(unsigned char x, unsigned char y);
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
#endif  
	 




