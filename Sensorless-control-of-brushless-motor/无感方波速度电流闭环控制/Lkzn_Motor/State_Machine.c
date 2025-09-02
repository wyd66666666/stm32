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
#include "State_Machine.h"
#include "OLED.h"
#include "PI_Cale.h"
#include "Drive parameters.h"
#include "ADC_int.h"
#include "Tim1_PWM.h"
#include "Control stage parameters.h"

#include "Hallless.h"
#include "VvvfControl.h"
 
extern    Hallless     Hallless_Three;
extern    VvvF_start   VvvF_startPare;

extern    ADCSamp      ADCSampPare; //AD�ɼ�
extern    State        StateContr;  //����״̬
extern    PIDREG_T     pi_spd;     //�ٶ�PID
extern    PIDREG_T     pi_ICurr;   //����PID 
void Key_Gpio(void)  //������ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = RUN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//��ƽ�ɸߵ��Ͷ�Ӧ����
	GPIO_Init(RUN_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = STOP_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//��ƽ�ɸߵ��Ͷ�Ӧ����
	GPIO_Init(STOP_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = UP_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//��ƽ�ɸߵ��Ͷ�Ӧ����
	GPIO_Init(UP_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DOWN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//��ƽ�ɸߵ��Ͷ�Ӧ����
	GPIO_Init(DOWN_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DIR_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//��ƽ�ɸߵ��Ͷ�Ӧ����
	GPIO_Init(DIR_GPIO_PORT, &GPIO_InitStructure);
}
		
void Key_Scanning(void) //��������   ��ͨ���루û�����ⲿ�жϣ�
{
	static uint16_t Count = 0;
	
	if(RUN_STATUS == 0&& (Count == 0)) //����
	{
		Start_Motor(); //����   ����pwm
		StateContr.drive_car = 1; //ʹ�ܿ���  ���״̬ 1 ����  0ֹͣ
		//StateContr.Run_mode=0;
		if(	StateContr.Control_Mode == 1) //����
		{
		StateContr.Aim_Speed = 20;  //������ֵ
		StateContr.Aim_Duty = 10000 * StateContr.Aim_Speed / 100;//��ռ�ձ�
		}
		if(StateContr.Control_Mode == 2 || StateContr.Control_Mode == 3) //�ٶ� ������
		{
		pi_spd.Ref = 300;  //�ο��ٶ�
		}
		Count = 100;   //�����˲�
	}
	
	if(STOP_STATUS == 0&& (Count == 0)) //ֹͣ����
	{
		StateContr.Duty = 0;    //ռ�ձ�0
		TIM_SetCompare1(TIM1, 0);
		TIM_SetCompare2(TIM1, 0);
		TIM_SetCompare1(TIM1, 0);
		Stop_Motor(); //ֹͣ���
		StateContr.Aim_Speed = 0; //������Ϊ0
		StateContr.Aim_Duty = 0;
		StateContr.drive_car = 0;
		StateContr.Start_order = 1; 
		StateContr.Current_Count = 0;
		StateContr.Speed_Count = 0;
		
		VvvF_startPare.Count = 0;
		Hallless_Three.Filter_Count = 0;
		Hallless_Three.Filter_CountF = 0;
		Hallless_Three.Speed_RPM = 0;
		Hallless_Three.Speed_RPMF = 0;
	
		pi_spd.Err = 0;  //PID������ʼ��
		pi_spd.Fdb = 0;
		pi_spd.Out =0;
		pi_spd.OutF =0;
		pi_spd.OutPreSat =0;
		pi_spd.Ref = 0;
		pi_spd.SatErr = 0;
		pi_spd.Ud = 0;
		pi_spd.Ui = 0;
		pi_spd.Ui_1 = 0;
		pi_spd.Up = 0;
		pi_spd.Up1 = 0;
		
		pi_ICurr.Err = 0;
		pi_ICurr.Fdb = 0;
		pi_ICurr.Out =0;
		pi_ICurr.OutF =0;
		pi_ICurr.OutPreSat =0;
		pi_ICurr.Ref = 0;
		pi_ICurr.SatErr = 0;
		pi_ICurr.Ud = 0;
		pi_ICurr.Ui = 0;
		pi_ICurr.Ui_1 = 0;
		pi_ICurr.Up = 0;
		pi_ICurr.Up1 = 0;

		Count = 200; //�����˲�
	}
	
	if(UP_STATUS == 0&& (Count == 0)) //������
	{
		if(	StateContr.Control_Mode == 2 || StateContr.Control_Mode == 3) //�ٶȵ�����
		{
		pi_spd.Ref = pi_spd.Ref + 10; //�ο��ٶȼ�
		}
		if(	StateContr.Control_Mode == 1 ) //����
		{
		StateContr.Aim_Speed = StateContr.Aim_Speed + 1; //Ŀ��ֵ
		StateContr.Aim_Duty = 10000 * StateContr.Aim_Speed / 100;//���ռ�ձ�
		}
		if(StateContr.Aim_Speed > 50) //ռ�ձ����ֵ
		{
		StateContr.Aim_Speed = 50;
		}
		if(pi_spd.Ref > 2100)//�ٶ����ֵ
		{
		pi_spd.Ref = 2100;
		}
		
		Count = 200;  //�����˲�
	}
	
	if(DOWN_STATUS == 0&& (Count == 0)) //������
	{
		if(	StateContr.Control_Mode == 2 || StateContr.Control_Mode == 3)//�ٶ� ������
		{
		pi_spd.Ref = pi_spd.Ref - 10;//�ٶȼ�
		}
		if(	StateContr.Control_Mode == 1 )//����
		{
		StateContr.Aim_Speed = StateContr.Aim_Speed - 1;//ռ�ձ�ֵ
		StateContr.Aim_Duty = 10000 * StateContr.Aim_Speed / 100;//����ռ�ձ�
		}
		if(StateContr.Aim_Speed < 10)//ռ�ձ���Сֵ
		{
		StateContr.Aim_Speed = 10;
		}
		if(pi_spd.Ref < 280) //�ٶ���Сֵ
		{
		pi_spd.Ref = 280;
		}
		
		Count = 200; //�����˲�
	}
	
	if(DIR_STATUS == 0&& (Count == 0))//������ʱû�� 
	{
		if(StateContr.Run_mode)
		{
			StateContr.Run_mode=0;
		}
		else
	   StateContr.Run_mode=1;
		
		Count = 200;
	}
	
	if (Count > 0)  //�˲�������  �൱����ʱ��һ��ֻ�ܰ���һ����
	{
		
		Count--;
	}	
}

void OLED_Display(void) //OLED��ʾ
{
	//��һ��
//	OLED_ShowCHinese(32,0,46,1);//��
//	OLED_ShowCHinese(48,0,47,1);//�� 
//	OLED_ShowCHinese(64,0,48,1);//��
//	OLED_ShowCHinese(80,0,49,1);//�� 
  //�ڶ���
	if(StateContr.Control_Mode == 2||StateContr.Control_Mode == 3) //�ٶȵ�����
	{	
	OLED_ShowCHinese(0,2,12,1); //Ŀ
	OLED_ShowCHinese(16,2,13,1); //��
	OLED_ShowCHinese(32,2,14,1);//��
	OLED_ShowCHinese(48,2,15,1);//�� 
	
	OLED_ShowString(80,2," ",1);
	OLED_ShowNum(90,2,pi_spd.Ref,4,16);// �ο��ٶ�
	}

	//������
	if(StateContr.Control_Mode == 2||StateContr.Control_Mode == 3)
	{
	OLED_ShowCHinese(0,4,16,1);   //ʵ
	OLED_ShowCHinese(16,4,17,1); //��
	OLED_ShowCHinese(32,4,14,1);//��
	OLED_ShowCHinese(48,4,15,1);//�� 
	
	OLED_ShowNum(90,4,pi_spd.Fdb,4,16);//�����ٶ�ֵ
	OLED_ShowString(80,4," ",1);
	}
	
	//������
	if(	StateContr.Control_Mode == 1) //����
	{
	OLED_ShowCHinese(0,3,67,1);   //ռ
	OLED_ShowCHinese(16,3,68,1); //��
	OLED_ShowCHinese(32,3,69,1);//��
	OLED_ShowNum(90,3,StateContr.Aim_Speed,4,16); //ֵ
	OLED_ShowString(80,3," ",1);	
		
	OLED_ShowCHinese(8,6,60,1);   //��
	OLED_ShowCHinese(24,6,61,1); //��
	OLED_ShowCHinese(40,6,62,1);//��
	OLED_ShowCHinese(56,6,63,1); //��
	OLED_ShowCHinese(72,6,64,1);//˫
	OLED_ShowCHinese(88,6,65,1);//��
	OLED_ShowCHinese(104,6,66,1);//��
	}
	if(	StateContr.Control_Mode == 2) //�ٶȻ�
	{	
	OLED_ShowCHinese(8,6,60,1);   //��
	OLED_ShowCHinese(24,6,61,1); //��
	OLED_ShowCHinese(40,6,62,1);//��
	OLED_ShowCHinese(56,6,63,1); //��
	OLED_ShowCHinese(72,6,70,1);//��
	OLED_ShowCHinese(88,6,71,1);//��
	OLED_ShowCHinese(104,6,72,1);//��
	}
	if(	StateContr.Control_Mode == 3)//�ٶȵ�����
	{
	OLED_ShowCHinese(8,0,60,1);   //��
	OLED_ShowCHinese(24,0,61,1); //��
	OLED_ShowCHinese(40,0,62,1);//��
	OLED_ShowCHinese(56,0,63,1); //��
	OLED_ShowCHinese(72,0,73,1);//˫
	OLED_ShowCHinese(88,0,71,1);//��
	OLED_ShowCHinese(104,0,72,1);//��
	}
	OLED_ShowString(0,6,"WYD",1);
}



