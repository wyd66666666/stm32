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
#include "Hallless.h"
#include "State_Machine.h"
#include "Tim1_PWM.h"
#include "Control stage parameters.h"
#include "Drive parameters.h"
#include "PMSM motor parameters.h"
#include "PI_Cale.h"
#ifdef HALLLESS //无感模式

extern    State      StateContr;
extern    Hallless   Hallless_Three;

void ThreeHalllessPara_init(void)  //无感参数初始化
{
	Hallless_Three.Poles = MOTOR_POLES;  //电机极对数
	Hallless_Three.speed_coeff = 16000 * 60 / Hallless_Three.Poles / 2;//速度系数 16K可以跑到最大的速度
}

unsigned char UEMF_Edge( unsigned char val) //反电动势电平检修
{
	static  unsigned char oldval=0;
	
	if(oldval != val)  //两次电平不一样
	{
		oldval = val;
		if(val == 0) return 0;   //返回低电平
		else return 1;			     //返回高电平
	}
	return 2;          //两次电平一样
}

unsigned char VEMF_Edge( unsigned char val)//反电动势电平检修
{
	static  unsigned char oldval=0;
	
	if(oldval != val)//两次电平不一样
	{
		oldval = val;
		if(val == 0) return 0; //返回低电平
		else return 1;		//返回高电平
	}
	return 2;  //两次电平一样
}

unsigned char WEMF_Edge( unsigned char val)//反电动势电平检修
{
	static  unsigned char oldval=0;
	
	if(oldval != val)//两次电平不一样
	{
		oldval = val;
		if(val == 0) return 0;  //返回低电平
		else return 1;		//返回高电平
	}
	return 2;  //两次电平一样
}

void Hallless_SW(void)  //无感换相程序
{
	Hallless_Three.Queue_Status[0] = Hallless_Three.Queue_Status[0] << 1; //把上次电平放到高位
	Hallless_Three.Queue_Status[1] = Hallless_Three.Queue_Status[1] << 1;	//把上次电平放到高位
	Hallless_Three.Queue_Status[2] = Hallless_Three.Queue_Status[2] << 1;//把上次电平放到高位
	
	Hallless_Three.Queue_Status[0] |= HALLLESS_U_STATUS;    //采集U相反电动势电平
	Hallless_Three.Queue_Status[1] |= HALLLESS_V_STATUS;    //采集V相反电动势电平
	Hallless_Three.Queue_Status[2] |= HALLLESS_W_STATUS;    //采集W相反电动势电平
	
	Hallless_Three.Filter_Math = Hallless_Three.Queue_Status[0] & FILTER_LONG; //U相反电动势滤波
	if(Hallless_Three.Filter_Math == FILTER_LONG)//U相高电平
	{
		Hallless_Three.QFilter_Status[0] = 1;     //状态高
	}
	else if(Hallless_Three.Filter_Math == 0x00)//U相低电平
	{
		Hallless_Three.QFilter_Status[0] = 0;//状态低
	}
	else
	{
		return;  //返回
	}
	
	Hallless_Three.Filter_Math = Hallless_Three.Queue_Status[1] & FILTER_LONG;//V相反电动势滤波
	if(Hallless_Three.Filter_Math == FILTER_LONG)//V相高电平
	{
		Hallless_Three.QFilter_Status[1] = 1;//状态高
	}
	else if(Hallless_Three.Filter_Math == 0x00)//V相低电平
	{
		Hallless_Three.QFilter_Status[1] = 0;//状态低
	}
	else
	{
		return; //返回
	}
	
	Hallless_Three.Filter_Math = Hallless_Three.Queue_Status[2] & FILTER_LONG;//W相反电动势滤波
	if(Hallless_Three.Filter_Math == FILTER_LONG)//V相高电平
	{
		Hallless_Three.QFilter_Status[2] = 1;//状态高
	}
	else if(Hallless_Three.Filter_Math == 0x00)//W相低电平
	{
		Hallless_Three.QFilter_Status[2] = 0;//状态低
	}
	else
	{
		return;//返回
	}	
	Hallless_Three.Filter_Edge = UEMF_Edge(Hallless_Three.QFilter_Status[0]);//判断第一个电平状态
	if(Hallless_Three.Filter_Edge == 1) //高电平时进入算速度
	{
	//延迟30°换相，因为硬件上低通滤波器和软件延迟的原因，实际延迟角度为24°左右，
	//最优解可以通过示波器确定。通过PA4查看实际延迟角度
		FirstOrder_LPF_Cacl(Hallless_Three.Filter_Count,Hallless_Three.Filter_CountF,0.1379);//滤波
		Hallless_Three.Speed_RPM = Hallless_Three.speed_coeff/Hallless_Three.Filter_CountF;//速度计算 相当于分割最大速度
		FirstOrder_LPF_Cacl(Hallless_Three.Speed_RPM,Hallless_Three.Speed_RPMF,0.2379); //滤波
		Hallless_Three.Filter_Delay = Hallless_Three.Filter_CountF / 6;  //延时30度
		Hallless_Three.Filter_Count = 0;
	}
	if(Hallless_Three.Filter_Edge == 0) //低电平清零
	{
		Hallless_Three.Filter_Count = 0;
	}
	if(Hallless_Three.Filter_Edge == 2)//电平一样时计数
	{
		Hallless_Three.Filter_Count++;
	}
	
	Hallless_Three.Hallless_State = Hallless_Three.QFilter_Status[0] +(Hallless_Three.QFilter_Status[1]<<1) +(Hallless_Three.QFilter_Status[2]<<2);
	//得到反电动势状态
	
	
	if(Hallless_Three.Hallless_State!=Hallless_Three.OldHallless_State) //两个状态不一样时开始计数
	{
		Hallless_Three.Filter_Count_All++;//计数
	}
	//调试延迟30°函数
//	Debug_HALLLESS_OFF;
	if(Hallless_Three.Filter_Count_All >= Hallless_Three.Filter_Delay) //延时30度换相
	{
	//调试延迟30°函数
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
	Hallless_Three.OldHallless_State=Hallless_Three.Hallless_State ;//记忆上次状态
	}
}

#endif

