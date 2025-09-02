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
#include "Tim1_ISR_MCLoop.h"
#include "ADC_int.h"
#include "Hall.h"
#include "State_Machine.h"
#include "PI_Cale.h"
#include "VvvfControl.h"
#include "Hallless.h"
#include "Drive parameters.h"

extern    Hallless      Hallless_Three;

extern    PIDREG_T      pi_spd;
extern    PIDREG_T      pi_ICurr;
extern    State         StateContr;
extern    ADCSamp       ADCSampPare;
extern    VvvF_start    VvvF_startPare;


void TIM1_UP_TIM10_IRQHandler(void) //TIM1 UP�ж�
{
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);//���жϱ�־	  	  
 	            
	if(StateContr.drive_car == 1)
	{	
	//������
	if(StateContr.Start_order == 1)
	{
		Anwerfen();     //��Ƶ����
		StateContr.Start_order = 2;	
	}
	if(StateContr.Start_order == 2)
	{	
		HALLLESS_ADC_Sample();//ADֵ�ɼ�
		Hallless_SW();  //�޸л���

	//������+�ٶȻ�
	if(StateContr.Control_Mode == 3)
	{
		StateContr.Speed_Count++;
		StateContr.Current_Count++;
		if(StateContr.Speed_Count > 2) //�ٶȻ�
		{
			 pi_spd.Fdb = Hallless_Three.Speed_RPMF ;   
			 PID_CALC(pi_spd);
			 FirstOrder_LPF_Cacl(pi_spd.Out,pi_spd.OutF,0.08379f);
			 StateContr.Speed_Count = 0;
		}
		if(StateContr.Current_Count > 1)
		{
			 pi_ICurr.Ref = Torque;      //������                              
			 pi_ICurr.Fdb = ADCSampPare.BUS_Curr;   
			 PID_CALC(pi_ICurr);
			 FirstOrder_LPF_Cacl(pi_ICurr.Out,pi_ICurr.OutF,0.08379f);
			 StateContr.Current_Count = 0;
		}
		if(pi_ICurr.OutF>pi_spd.OutF)	 //�������ֵ�����ٶ����ֵ
		{
			 StateContr.Duty= pi_spd.OutF; //ռ�ձ����ٶ����ֵ             
			 pi_ICurr.Ui  = pi_spd.Ui;  //����UI
		} 
		else 
		{
			 StateContr.Duty= pi_ICurr.OutF;  //ռ�ձ��ǵ������ֵ        
			 pi_spd.Ui  = pi_ICurr.Ui;//����UI 
		}
	}
	}
}
}
