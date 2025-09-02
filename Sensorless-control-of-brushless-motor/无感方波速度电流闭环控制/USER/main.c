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
#include "stm32f4xx.h"
#include "GPIO_int.h"
#include "Timer.h"
#include "ADC_int.h"
#include "OLED.h"
#include "Tim1_PWM.h"
#include "PI_Cale.h"
#include "State_Machine.h"
#include "PMSM motor parameters.h"
#include "Power stage parameters.h"
#include "Control stage parameters.h"
#include "Drive parameters.h"



#include "VvvfControl.h"
#include "Hallless.h"

PIDREG_T     pi_spd = PIDREG_T_DEFAULTS;  //�ٶ�PIDĬ��ֵ
PIDREG_T     pi_ICurr = PIDREG_T_DEFAULTS;//����PIDĬ��ֵ
State        StateContr=State_DEFAULTS;   //����״̬Ĭ��ֵ
ADCSamp      ADCSampPare=ADCSamp_DEFAULTS;//ADCĬ��ֵ

Hallless     Hallless_Three=Hallless_DEFAULTS;  //HALL��ʼ��
VvvF_start   VvvF_startPare=VvvF_start_DEFAULTS; //VF��ʼ��

extern       Hallless   Hallless_Three;

volatile uint16_t ADC_DualConvertedValueTab[5];  //��λ���ӿں���
int main(void)                        //������
{
	Delay(10000);	                      //��ʱ
	SysTickConfig();                    //�δ�ʱ�ӳ�ʼ��
	StateContr.Control_Mode = LOOP;     //���ƻ�ģʽ  3
	PID_init();                         //PID��ʼ�� 
	if(	StateContr.Control_Mode == 1)   //����
	{
	StateContr.Aim_Speed = 20;          //�ٶ�    
	StateContr.Aim_Duty = 10000 * StateContr.Aim_Speed / 100; //�ٶȻ����ռ�ձ�
	}
	if(StateContr.Control_Mode == 2 || StateContr.Control_Mode == 3)  //�ٶȱպϻ����ٶȵ���˫�ջ�
	{
		pi_spd.Ref = 300;               //Ĭ�ϲο�ֵ
	}
	Delay(10000);	                   //��ʱ
	Key_Gpio();                      //������ʼ��
                     
    
	ThreeHalllessPara_init();        //�޸в�����ʼ��   �������
	Hallless_Gpio();                 //�޸���������

	Delay(10000);	                   //��ʱ
	Adc_Configuration();             //ADC����  DMA   ������⣨��ѹ�ɼ���
	Delay(10000);	                   //��ʱ
	Tim1_PWM_Init();                 //TIM1����  PWM
	Delay(10000);	                   //��ʱ  
	Offset_CurrentReading();         //����ƫ��ֵ У׼  �����������ĸ�ߵ����ĵ��跨��������,��Ҫ�ϵ��ȡ��ʼƫִ��ѹ
	Delay(10000);	                    //��ʱ
	OLED_Init();                      //oled��ʼ��
	while(1)
	{
		OLED_Display();                 //��ʾ
		Key_Scanning();                 //�����߼�����
	}
}

