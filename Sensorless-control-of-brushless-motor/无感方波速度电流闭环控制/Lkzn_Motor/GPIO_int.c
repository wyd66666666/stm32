//############################################################
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//��Ȩ���У�����ؾ�  
//STM32������ƿ�����   
//��������
//��ַ: https://shop59411198.taobao.com/
//�޸�����:2019/4/6
//�汾��V2.0
//����QQ: 2465727011
//����QQȺ��438404033
//############################################################
#include "GPIO_int.h"
#include "Control stage parameters.h"

void Delay(u32 nCount)	 //�򵥵���ʱ����
{
	u16 t=10000;
	for(; nCount != 0; nCount--)//������ʱ�����Ǻ�0�Ƚ�
	for(;t!=0;t--);
} 

void Hall_Gpio(void)  //������������
{
  GPIO_InitTypeDef GPIO_InitStructure; 	 
  
	GPIO_InitStructure.GPIO_Pin = HALL_A_GPIO_PIN|HALL_B_GPIO_PIN|HALL_C_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  //����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ�50M
	GPIO_Init(HALL_A_GPIO_PORT, &GPIO_InitStructure); //���ų�ʼ��
}

void Hallless_Gpio(void) //�޸���������
{
  GPIO_InitTypeDef GPIO_InitStructure; 	 
  
	GPIO_InitStructure.GPIO_Pin = HALLLESS_A_GPIO_PIN|HALLLESS_B_GPIO_PIN|HALLLESS_C_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50M
	GPIO_Init(HALLLESS_A_GPIO_PORT, &GPIO_InitStructure);//���ų�ʼ��
	
	GPIO_InitStructure.GPIO_Pin = Debug_HALLLESS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50M
	GPIO_Init(Debug_HALLLESS_PORT, &GPIO_InitStructure);//���ų�ʼ��
}
