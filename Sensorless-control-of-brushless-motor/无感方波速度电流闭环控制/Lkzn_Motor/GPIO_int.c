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
#include "GPIO_int.h"
#include "Control stage parameters.h"

void Delay(u32 nCount)	 //简单的延时函数
{
	u16 t=10000;
	for(; nCount != 0; nCount--)//此种延时函数是和0比较
	for(;t!=0;t--);
} 

void Hall_Gpio(void)  //霍尔引脚配置
{
  GPIO_InitTypeDef GPIO_InitStructure; 	 
  
	GPIO_InitStructure.GPIO_Pin = HALL_A_GPIO_PIN|HALL_B_GPIO_PIN|HALL_C_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  //输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度50M
	GPIO_Init(HALL_A_GPIO_PORT, &GPIO_InitStructure); //引脚初始化
}

void Hallless_Gpio(void) //无感引脚配置
{
  GPIO_InitTypeDef GPIO_InitStructure; 	 
  
	GPIO_InitStructure.GPIO_Pin = HALLLESS_A_GPIO_PIN|HALLLESS_B_GPIO_PIN|HALLLESS_C_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50M
	GPIO_Init(HALLLESS_A_GPIO_PORT, &GPIO_InitStructure);//引脚初始化
	
	GPIO_InitStructure.GPIO_Pin = Debug_HALLLESS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50M
	GPIO_Init(Debug_HALLLESS_PORT, &GPIO_InitStructure);//引脚初始化
}
