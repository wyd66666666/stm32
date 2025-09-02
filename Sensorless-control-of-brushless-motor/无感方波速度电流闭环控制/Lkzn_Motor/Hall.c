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

void ThreeHallPara_init(void)  //HALL参数初始化
{
   Hall_Three.Poles = MOTOR_POLES;    //电机极对数
	 Hall_Three.speed_coeff = 16000 * 60 / Hall_Three.Poles / 2;  //速度系数 这个是16K时电机最大转速
}

unsigned char UHALL_Edge( unsigned char val) //HALL 引脚电平判断
{
	static  unsigned char oldval=0;
	
	if(oldval != val)
	{
		oldval = val;
		if(val == 0) return 0;  //低电平
		else return 1;			    //高电平
	}
	return 2;  //前后一致
}

void Hall_SW(void)  //霍尔换向函数
{
	if (HALL_U_STATUS==1)  //霍尔状态
		{		
		Hall_Three.HallUVW[0]=1;
		}
	else
		{
		Hall_Three.HallUVW[0]=0; 
		}
	if (HALL_V_STATUS==1) //霍尔状态
		{			 
		Hall_Three.HallUVW[1]=1;
		}
	else
		{
		Hall_Three.HallUVW[1]=0; 
		}
	if (HALL_W_STATUS==1)  //霍尔状态
		{
		Hall_Three.HallUVW[2]=1;
		}
	else
		{
		Hall_Three.HallUVW[2]=0;  
		}
		
	Hall_Three.Filter_Edge = UHALL_Edge(Hall_Three.HallUVW[0]); //电平判断
		{
			if(Hall_Three.Filter_Edge == 1) //转了一个电气周期
			{
				FirstOrder_LPF_Cacl(Hall_Three.Filter_Count,Hall_Three.Filter_CountF,0.1379); //滤波
				Hall_Three.Speed_RPM = Hall_Three.speed_coeff/Hall_Three.Filter_CountF; //算下现在速度 
				FirstOrder_LPF_Cacl(Hall_Three.Speed_RPM,Hall_Three.Speed_RPMF,0.2379); //滤波
				Hall_Three.Filter_Count = 0;
			}
			if(Hall_Three.Filter_Edge == 0)  //霍尔状态低电平
			{
				Hall_Three.Filter_Count = 0;   //计数清零
			}
			if(Hall_Three.Filter_Edge == 2)  //在相同状态
			{
				Hall_Three.Filter_Count++;    //计数
			}
		}
			
	Hall_Three.Hall_State = Hall_Three.HallUVW[0] +(Hall_Three.HallUVW[1]<<1) +(Hall_Three.HallUVW[2]<<2);//合成霍尔状态
	if(Hall_Three.Hall_State!=Hall_Three.OldHall_State)  //前后不一样
	{
#ifdef H_PWM_L_PWM  //H PWM L PWM 模式
	switch(Hall_Three.Hall_State)  //根据状态换向
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

#ifdef H_PWM_L_ON  //H PWM L ON 模式
	switch(Hall_Three.Hall_State) //根据状态换向
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
	if(Hall_Three.OldHall_State==Hall_Three.Hall_State)  //状态一样
	{		
#ifdef H_PWM_L_PWM  //H PWM L PWM 模式
	switch(Hall_Three.Hall_State) //根据状态换向
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

#ifdef H_PWM_L_ON//H PWM L on 模式
	switch(Hall_Three.Hall_State) //根据状态换向
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

