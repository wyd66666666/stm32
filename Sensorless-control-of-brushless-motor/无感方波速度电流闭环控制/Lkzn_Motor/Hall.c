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
#include "Hall.h"
#include "State_Machine.h"
#include "Tim1_PWM.h"
#include "Control stage parameters.h"
#include "Drive parameters.h"
#include "PMSM motor parameters.h"
#include "PI_Cale.h"

#ifdef HALL 

extern    Hall       Hall_Three;
extern    State      StateContr;

void ThreeHallPara_init(void)  //HALL������ʼ��
{
   Hall_Three.Poles = MOTOR_POLES;    //���������
	 Hall_Three.speed_coeff = 16000 * 60 / Hall_Three.Poles / 2;  //�ٶ�ϵ�� �����16Kʱ������ת��
}

unsigned char UHALL_Edge( unsigned char val) //HALL ���ŵ�ƽ�ж�
{
	static  unsigned char oldval=0;
	
	if(oldval != val)
	{
		oldval = val;
		if(val == 0) return 0;  //�͵�ƽ
		else return 1;			    //�ߵ�ƽ
	}
	return 2;  //ǰ��һ��
}

void Hall_SW(void)  //����������
{
	if (HALL_U_STATUS==1)  //����״̬
		{		
		Hall_Three.HallUVW[0]=1;
		}
	else
		{
		Hall_Three.HallUVW[0]=0; 
		}
	if (HALL_V_STATUS==1) //����״̬
		{			 
		Hall_Three.HallUVW[1]=1;
		}
	else
		{
		Hall_Three.HallUVW[1]=0; 
		}
	if (HALL_W_STATUS==1)  //����״̬
		{
		Hall_Three.HallUVW[2]=1;
		}
	else
		{
		Hall_Three.HallUVW[2]=0;  
		}
		
	Hall_Three.Filter_Edge = UHALL_Edge(Hall_Three.HallUVW[0]); //��ƽ�ж�
		{
			if(Hall_Three.Filter_Edge == 1) //ת��һ����������
			{
				FirstOrder_LPF_Cacl(Hall_Three.Filter_Count,Hall_Three.Filter_CountF,0.1379); //�˲�
				Hall_Three.Speed_RPM = Hall_Three.speed_coeff/Hall_Three.Filter_CountF; //���������ٶ� 
				FirstOrder_LPF_Cacl(Hall_Three.Speed_RPM,Hall_Three.Speed_RPMF,0.2379); //�˲�
				Hall_Three.Filter_Count = 0;
			}
			if(Hall_Three.Filter_Edge == 0)  //����״̬�͵�ƽ
			{
				Hall_Three.Filter_Count = 0;   //��������
			}
			if(Hall_Three.Filter_Edge == 2)  //����ͬ״̬
			{
				Hall_Three.Filter_Count++;    //����
			}
		}
			
	Hall_Three.Hall_State = Hall_Three.HallUVW[0] +(Hall_Three.HallUVW[1]<<1) +(Hall_Three.HallUVW[2]<<2);//�ϳɻ���״̬
	if(Hall_Three.Hall_State!=Hall_Three.OldHall_State)  //ǰ��һ��
	{
#ifdef H_PWM_L_PWM  //H PWM L PWM ģʽ
	switch(Hall_Three.Hall_State)  //����״̬����
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
		Hall_Three.Speed_RPM=0;
		}
		break;
	}
#endif

#ifdef H_PWM_L_ON  //H PWM L ON ģʽ
	switch(Hall_Three.Hall_State) //����״̬����
	{
		case  0x5:    //41
		{
		MOS_Q41PWM(); 
		}
		break;
		case  0x1:    //16
		{
		MOS_Q16PWM();
		}
		break;
		case  0x3:   //63
		{
		MOS_Q63PWM();
		}
		break;
		case 0x2:  //32
		{
		MOS_Q32PWM();
		}
		break;
		case 0x6:   //25
		{
		MOS_Q25PWM();
		}
		break;
		case 0x4: //54
		{
		MOS_Q54PWM();
		}
		break;
		default:
		{
		Stop_Motor();
		Hall_Three.Speed_RPM=0;
		}
		break;
	}
#endif
	
#ifdef H_ON_L_PWM
	switch(Hall_Three.Hall_State)
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
		Hall_Three.Speed_RPM=0;
		}
		break;
	}	
#endif
}	
	if(Hall_Three.OldHall_State==Hall_Three.Hall_State)  //״̬һ��
	{		
#ifdef H_PWM_L_PWM  //H PWM L PWM ģʽ
	switch(Hall_Three.Hall_State) //����״̬����
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
		Hall_Three.Speed_RPM=0;
		}
		break;
	}
#endif

#ifdef H_PWM_L_ON//H PWM L on ģʽ
	switch(Hall_Three.Hall_State) //����״̬����
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
		Hall_Three.Speed_RPM=0;
		}
		break;
	}
#endif
	
#ifdef H_ON_L_PWM
	switch(Hall_Three.Hall_State)
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
		Hall_Three.Speed_RPM=0;
		}
		break;
	}	
#endif
		}
	Hall_Three.OldHall_State=Hall_Three.Hall_State ;
}
#endif

