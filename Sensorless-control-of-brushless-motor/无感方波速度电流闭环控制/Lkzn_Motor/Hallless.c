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
#include "Hallless.h"
#include "State_Machine.h"
#include "Tim1_PWM.h"
#include "Control stage parameters.h"
#include "Drive parameters.h"
#include "PMSM motor parameters.h"
#include "PI_Cale.h"
#ifdef HALLLESS //�޸�ģʽ

extern    State      StateContr;
extern    Hallless   Hallless_Three;

void ThreeHalllessPara_init(void)  //�޸в�����ʼ��
{
	Hallless_Three.Poles = MOTOR_POLES;  //���������
	Hallless_Three.speed_coeff = 16000 * 60 / Hallless_Three.Poles / 2;//�ٶ�ϵ�� 16K�����ܵ������ٶ�
}

unsigned char UEMF_Edge( unsigned char val) //���綯�Ƶ�ƽ����
{
	static  unsigned char oldval=0;
	
	if(oldval != val)  //���ε�ƽ��һ��
	{
		oldval = val;
		if(val == 0) return 0;   //���ص͵�ƽ
		else return 1;			     //���ظߵ�ƽ
	}
	return 2;          //���ε�ƽһ��
}

unsigned char VEMF_Edge( unsigned char val)//���綯�Ƶ�ƽ����
{
	static  unsigned char oldval=0;
	
	if(oldval != val)//���ε�ƽ��һ��
	{
		oldval = val;
		if(val == 0) return 0; //���ص͵�ƽ
		else return 1;		//���ظߵ�ƽ
	}
	return 2;  //���ε�ƽһ��
}

unsigned char WEMF_Edge( unsigned char val)//���綯�Ƶ�ƽ����
{
	static  unsigned char oldval=0;
	
	if(oldval != val)//���ε�ƽ��һ��
	{
		oldval = val;
		if(val == 0) return 0;  //���ص͵�ƽ
		else return 1;		//���ظߵ�ƽ
	}
	return 2;  //���ε�ƽһ��
}

void Hallless_SW(void)  //�޸л������
{
	Hallless_Three.Queue_Status[0] = Hallless_Three.Queue_Status[0] << 1; //���ϴε�ƽ�ŵ���λ
	Hallless_Three.Queue_Status[1] = Hallless_Three.Queue_Status[1] << 1;	//���ϴε�ƽ�ŵ���λ
	Hallless_Three.Queue_Status[2] = Hallless_Three.Queue_Status[2] << 1;//���ϴε�ƽ�ŵ���λ
	
	Hallless_Three.Queue_Status[0] |= HALLLESS_U_STATUS;    //�ɼ�U�෴�綯�Ƶ�ƽ
	Hallless_Three.Queue_Status[1] |= HALLLESS_V_STATUS;    //�ɼ�V�෴�綯�Ƶ�ƽ
	Hallless_Three.Queue_Status[2] |= HALLLESS_W_STATUS;    //�ɼ�W�෴�綯�Ƶ�ƽ
	
	Hallless_Three.Filter_Math = Hallless_Three.Queue_Status[0] & FILTER_LONG; //U�෴�綯���˲�
	if(Hallless_Three.Filter_Math == FILTER_LONG)//U��ߵ�ƽ
	{
		Hallless_Three.QFilter_Status[0] = 1;     //״̬��
	}
	else if(Hallless_Three.Filter_Math == 0x00)//U��͵�ƽ
	{
		Hallless_Three.QFilter_Status[0] = 0;//״̬��
	}
	else
	{
		return;  //����
	}
	
	Hallless_Three.Filter_Math = Hallless_Three.Queue_Status[1] & FILTER_LONG;//V�෴�綯���˲�
	if(Hallless_Three.Filter_Math == FILTER_LONG)//V��ߵ�ƽ
	{
		Hallless_Three.QFilter_Status[1] = 1;//״̬��
	}
	else if(Hallless_Three.Filter_Math == 0x00)//V��͵�ƽ
	{
		Hallless_Three.QFilter_Status[1] = 0;//״̬��
	}
	else
	{
		return; //����
	}
	
	Hallless_Three.Filter_Math = Hallless_Three.Queue_Status[2] & FILTER_LONG;//W�෴�綯���˲�
	if(Hallless_Three.Filter_Math == FILTER_LONG)//V��ߵ�ƽ
	{
		Hallless_Three.QFilter_Status[2] = 1;//״̬��
	}
	else if(Hallless_Three.Filter_Math == 0x00)//W��͵�ƽ
	{
		Hallless_Three.QFilter_Status[2] = 0;//״̬��
	}
	else
	{
		return;//����
	}	
	Hallless_Three.Filter_Edge = UEMF_Edge(Hallless_Three.QFilter_Status[0]);//�жϵ�һ����ƽ״̬
	if(Hallless_Three.Filter_Edge == 1) //�ߵ�ƽʱ�������ٶ�
	{
	//�ӳ�30�㻻�࣬��ΪӲ���ϵ�ͨ�˲���������ӳٵ�ԭ��ʵ���ӳٽǶ�Ϊ24�����ң�
	//���Ž����ͨ��ʾ����ȷ����ͨ��PA4�鿴ʵ���ӳٽǶ�
		FirstOrder_LPF_Cacl(Hallless_Three.Filter_Count,Hallless_Three.Filter_CountF,0.1379);//�˲�
		Hallless_Three.Speed_RPM = Hallless_Three.speed_coeff/Hallless_Three.Filter_CountF;//�ٶȼ��� �൱�ڷָ�����ٶ�
		FirstOrder_LPF_Cacl(Hallless_Three.Speed_RPM,Hallless_Three.Speed_RPMF,0.2379); //�˲�
		Hallless_Three.Filter_Delay = Hallless_Three.Filter_CountF / 6;  //��ʱ30��
		Hallless_Three.Filter_Count = 0;
	}
	if(Hallless_Three.Filter_Edge == 0) //�͵�ƽ����
	{
		Hallless_Three.Filter_Count = 0;
	}
	if(Hallless_Three.Filter_Edge == 2)//��ƽһ��ʱ����
	{
		Hallless_Three.Filter_Count++;
	}
	
	Hallless_Three.Hallless_State = Hallless_Three.QFilter_Status[0] +(Hallless_Three.QFilter_Status[1]<<1) +(Hallless_Three.QFilter_Status[2]<<2);
	//�õ����綯��״̬
	
	
	if(Hallless_Three.Hallless_State!=Hallless_Three.OldHallless_State) //����״̬��һ��ʱ��ʼ����
	{
		Hallless_Three.Filter_Count_All++;//����
	}
	//�����ӳ�30�㺯��
//	Debug_HALLLESS_OFF;
	if(Hallless_Three.Filter_Count_All >= Hallless_Three.Filter_Delay) //��ʱ30�Ȼ���
	{
	//�����ӳ�30�㺯��
//	Debug_HALLLESS_ON;
	Hallless_Three.Filter_Count_All = 0;
	if(Hallless_Three.Hallless_State!=Hallless_Three.OldHallless_State)
	{
#ifdef H_PWM_L_PWM
	switch(Hallless_Three.Hallless_State)
	{
		case  0x5:     
		{
		MOS_Q41PWM();
		}
		break;
		case  0x1:    
		{
		MOS_Q16PWM();
		}
		break;
		case  0x3:    
		{
		MOS_Q63PWM();
		}
		break;
		case 0x2:  
		{
		MOS_Q32PWM();
		}
		break;
		case 0x6:   
		{
		MOS_Q25PWM();
		}
		break;
		case 0x4:
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
#endif

#ifdef H_PWM_L_ON
	switch(Hallless_Three.Hallless_State)
	{
		case  0x5:    //23 
		{
		MOS_Q41PWM(); 
		}
		break;
		case  0x1:    //13
		{
		MOS_Q16PWM();
		}
		break;
		case  0x3:   //13
		{
		MOS_Q63PWM();
		}
		break;
		case 0x2:  //12
		{
		MOS_Q32PWM();
		}
		break;
		case 0x6:   //12
		{
		MOS_Q25PWM();
		}
		break;
		case 0x4: //23
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
#endif
	
#ifdef H_ON_L_PWM
	switch(Hallless_Three.Hallless_State)
	{
		case  0x5:     
		{
		MOS_Q41PWM();
		}
		break;
		case  0x1:    
		{
		MOS_Q16PWM();
		}
		break;
		case  0x3:    
		{
		MOS_Q63PWM();
		}
		break;
		case 0x2:  
		{
		MOS_Q32PWM();
		}
		break;
		case 0x6:   
		{
		MOS_Q25PWM();
		}
		break;
		case 0x4:
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
#endif
	}	
	Hallless_Three.OldHallless_State=Hallless_Three.Hallless_State ;//�����ϴ�״̬
	}
}

#endif

