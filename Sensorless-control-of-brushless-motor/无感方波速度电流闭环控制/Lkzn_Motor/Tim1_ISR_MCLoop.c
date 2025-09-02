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


void TIM1_UP_TIM10_IRQHandler(void) //TIM1 UP中断
{
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);//清中断标志	  	  
 	            
	if(StateContr.drive_car == 1)
	{	
	//开环起动
	if(StateContr.Start_order == 1)
	{
		Anwerfen();     //变频启动
		StateContr.Start_order = 2;	
	}
	if(StateContr.Start_order == 2)
	{	
		HALLLESS_ADC_Sample();//AD值采集
		Hallless_SW();  //无感换向

	//电流环+速度环
	if(StateContr.Control_Mode == 3)
	{
		StateContr.Speed_Count++;
		StateContr.Current_Count++;
		if(StateContr.Speed_Count > 2) //速度环
		{
			 pi_spd.Fdb = Hallless_Three.Speed_RPMF ;   
			 PID_CALC(pi_spd);
			 FirstOrder_LPF_Cacl(pi_spd.Out,pi_spd.OutF,0.08379f);
			 StateContr.Speed_Count = 0;
		}
		if(StateContr.Current_Count > 1)
		{
			 pi_ICurr.Ref = Torque;      //电流环                              
			 pi_ICurr.Fdb = ADCSampPare.BUS_Curr;   
			 PID_CALC(pi_ICurr);
			 FirstOrder_LPF_Cacl(pi_ICurr.Out,pi_ICurr.OutF,0.08379f);
			 StateContr.Current_Count = 0;
		}
		if(pi_ICurr.OutF>pi_spd.OutF)	 //电流算出值大于速度输出值
		{
			 StateContr.Duty= pi_spd.OutF; //占空比是速度输出值             
			 pi_ICurr.Ui  = pi_spd.Ui;  //更新UI
		} 
		else 
		{
			 StateContr.Duty= pi_ICurr.OutF;  //占空比是电流输出值        
			 pi_spd.Ui  = pi_ICurr.Ui;//更新UI 
		}
	}
	}
}
}
