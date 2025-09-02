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
#include "VvvfControl.h"
#include "State_Machine.h"
#include "Tim1_PWM.h"
#include "Hallless.h"
#include "Drive parameters.h"
extern    Hallless        Hallless_Three;
extern    State           StateContr;
extern    VvvF_start      VvvF_startPare;

void Delay_US(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8;
	while(nCount--);
}

void Delay_MS(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8000;
	while(nCount--);
}

void Anwerfen(void) //VF����
{
	VvvF_startPare.Voilage_Ref =1500;//1500

	MOS_Q32PWM(); //�̶�һ��
	Delay_MS(300);
	StateContr.Duty = VvvF_startPare.Voilage_Ref;
	VvvF_startPare.Freq_T_Ref = 300;//300
	while(1)
	{
		for(VvvF_startPare.Freq_T_Ref_Count=0;VvvF_startPare.Freq_T_Ref_Count<VvvF_startPare.Freq_T_Ref; VvvF_startPare.Freq_T_Ref_Count++) 
		{
			Delay_US(2175);  	//1800
		} 
		VvvF_startPare.Freq_T_Ref-= VvvF_startPare.Freq_T_Ref/15+1;
		VvvF_startPare.Count++;			
		Change_Voltage();
		if(VvvF_startPare.Freq_T_Ref < 25) 
		{ 		
			if(TEST_MANUELL)
			{
				VvvF_startPare.Freq_T_Ref = 25;   //����ǿ�ƻ���
			}
			else
			{	
				break; 
			}				
		}	
		VvvF_startPare.VvvF_Count++;

		if(VvvF_startPare.VvvF_Count == 6)
		{
			VvvF_startPare.VvvF_Count = 0;
		}
		Anwerfen_SW();
	}
}

void Anwerfen_SW(void) 
{
	switch(VvvF_startPare.VvvF_Count)
	{
		case  0x3:    //23 
		{
		MOS_Q41PWM(); 
		}
		break;
		case  0x4:    //13
		{
		MOS_Q16PWM();
		}
		break;
		case  0x5:   //13
		{
		MOS_Q63PWM();
		}
		break;
		case 0x0:  //12
		{
		MOS_Q32PWM();
		}
		break;
		case 0x1:   //12
		{
		MOS_Q25PWM();
		}
		break;
		case 0x2: //23
		{
		MOS_Q54PWM();
		}
		break;
		default:
		{
		Stop_Motor();
		Hallless_Three.Speed_RPM=0;
		}
		break;
		

	}

}

void Change_Voltage(void)  //�ı䲻ͬʱ�� ռ�ձ� ʵ�� ��Ƶ ����
{
	 
		if(VvvF_startPare.Count == 1)
		{
			StateContr.Duty = 100;
		}
		if(VvvF_startPare.Count == 2)
		{
			StateContr.Duty = 200;
		}
		if(VvvF_startPare.Count == 3)
		{
			StateContr.Duty = 300;
		}	
		if(VvvF_startPare.Count == 4)
		{
			StateContr.Duty = 400;
		}
		if(VvvF_startPare.Count == 5)
		{
			StateContr.Duty = 400;
		}
		if(VvvF_startPare.Count == 6)
		{
			StateContr.Duty = 400;
		}
		if(VvvF_startPare.Count == 7)
		{
			StateContr.Duty = 500;
		}
		if(VvvF_startPare.Count == 8)
		{
			StateContr.Duty = 700;
		}
		if(VvvF_startPare.Count == 9)
		{
			StateContr.Duty = 800;
		}
		if(VvvF_startPare.Count == 10)
		{
			StateContr.Duty = 1000;
		}
		if(VvvF_startPare.Count == 11)
		{
			StateContr.Duty = 1400;
		}
		if(VvvF_startPare.Count == 12)
		{
			StateContr.Duty = 1400;
		}
		if(VvvF_startPare.Count == 13)
		{
			StateContr.Duty = 1400;
		}
		if(VvvF_startPare.Count == 14)
		{
			StateContr.Duty = 1450;
		}
		if(VvvF_startPare.Count == 15)
		{
			StateContr.Duty = 1500;
		}
}

