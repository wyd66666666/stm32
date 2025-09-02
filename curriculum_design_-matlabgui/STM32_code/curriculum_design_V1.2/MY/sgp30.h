#ifndef __HX711_H
#define __HX711_H

#include "main.h"

//#define  SGP30_SCL   PBout(0)
//#define  SGP30_SDA   PBout(1)

#define SGP30_SCL_HIGH HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)
#define SGP30_SCL_LOW HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)

#define SGP30_SDA_HIGH 	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)
#define SGP30_SDA_LOW    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)

// USART GPIO ���ź궨��
#define  SGP30_SCL_GPIO_CLK        RCC_APB2Periph_GPIOB
#define  SGP30_SCL_GPIO_PORT       GPIOB
#define  SGP30_SCL_GPIO_PIN        GPIO_PIN_0

#define  SGP30_SDA_GPIO_SDA        RCC_APB2Periph_GPIOB
#define  SGP30_SDA_GPIO_PORT       GPIOB
#define  SGP30_SDA_GPIO_PIN        GPIO_PIN_1

//#define  SGP30_SDA_READ()           GPIO_ReadInputDataBit(SGP30_SDA_GPIO_PORT, SGP30_SDA_GPIO_PIN)
#define SGP30_SDA_READ	 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)

#define SGP30_read  0xb1  //SGP30�Ķ���ַ
#define SGP30_write 0xb0  //SGP30��д��ַ


void SGP30_IIC_Start(void);				//����IIC��ʼ�ź�
void SGP30_IIC_Stop(void);	  			//����IICֹͣ�ź�
void SGP30_IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint16_t SGP30_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t SGP30_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void SGP30_IIC_Ack(void);					//IIC����ACK�ź�
void SGP30_IIC_NAck(void);				//IIC������ACK�ź�
void SGP30_IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t SGP30_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	
void SGP30_Init(void);				  
void SGP30_Write(uint8_t a, uint8_t b);
uint32_t SGP30_Read(void);



#endif

