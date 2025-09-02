
#ifndef __OLED_H
#define __OLED_H			  	 

#include "main.h"

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����


//****************��λ*********************

#define OLED_RST_L  HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin, GPIO_PIN_RESET)
#define OLED_RST_H  HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin, GPIO_PIN_SET)

//****************����/����***************

#define OLED_DC_L  HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin, GPIO_PIN_RESET)
#define OLED_DC_H  HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin, GPIO_PIN_SET)

//****************Ƭѡ*********************

#define OLED_CS_L  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin, GPIO_PIN_RESET)
#define OLED_CS_H  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin, GPIO_PIN_SET)


#define     OLED_RES_GPIO_Port   GPIOA
#define     OLED_RES_Pin         GPIO_PIN_8

#define     OLED_DC_GPIO_Port    GPIOA
#define     OLED_DC_Pin          GPIO_PIN_15

#define     OLED_CS_GPIO_Port    GPIOA
#define     OLED_CS_Pin          GPIO_PIN_4



//OLEDģʽ����
//0:4�ߴ���ģʽ
//1:����8080ģʽ

#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED�˿ڶ���----------------  					   
//#define uint8_t unsigned char
//#define uint16_t unsigned int
//#define uint32_t unsigned long


//void delay_ms(uint16_t ms);

//OLED�����ú���
void  OLED_WR_Byte(uint8_t dat,uint8_t cmd);
void  OLED_Display_On(void);
void  OLED_Display_Off(void);
void  OLED_Init(void);
void  OLED_Clear(void);
void  OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void  OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void  OLED_Set_Pos(unsigned char x, unsigned char y);
void  OLED_GPIO_Init(void);


#endif  
