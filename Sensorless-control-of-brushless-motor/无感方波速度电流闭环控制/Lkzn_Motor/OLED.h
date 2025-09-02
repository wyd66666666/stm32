//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究  
//STM32电机控制开发板   
//联控智能
//网址: https://shop59411198.taobao.com/
//修改日期:2019/4/6
//版本：V2.0
//技术QQ: 2465727011
//技术QQ群：438404033
//############################################################
#include "stm32f4xx.h"


#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    
				   
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_11)//CLK
#define OLED_SCLK_Set() GPIO_SetBits(GPIOC,GPIO_Pin_11)


#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_12)//DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOC,GPIO_Pin_12)


#define OLED_RST_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_10)//RES
#define OLED_RST_Set() GPIO_SetBits(GPIOC,GPIO_Pin_10)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_12)//DC
#define OLED_DC_Set() GPIO_SetBits(GPIOA,GPIO_Pin_12)


#define OLED_CS_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_11)//CS
#define OLED_CS_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_11)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowCHinese(u8 x,u8 y,u8 no,u8 mod);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 mod);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
