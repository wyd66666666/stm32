#include "lsens.h"
#include "tim.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//LSENS(����������)��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved										  
//////////////////////////////////////////////////////////////////////////////////
  
//��ʼ������������
void Lsens_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOF_CLK_ENABLE();			//����GPIOFʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_8;            //PF8
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
 
	  //ADC3_Init();							//��ʼ��ADC3
}
//��ȡLight Sens��ֵ
//0~100:0,�;100,���� 
uint8_t Lsens_Get_Val(void)
{
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<LSENS_READ_TIMES;t++)
	{
		temp_val+=Get_Adc3(ADC_CHANNEL_6);	//��ȡADCֵ,ͨ��6
		HAL_Delay(5);
	}
	temp_val/=LSENS_READ_TIMES;//�õ�ƽ��ֵ 
	if(temp_val>4000)temp_val=4000;
	return (uint8_t)(100-(temp_val/40));
}












