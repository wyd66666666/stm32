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

extern    ADCSamp      ADCSampPare; //AD采集
extern    State        StateContr;  //控制状态
extern    PIDREG_T     pi_spd;     //速度PID
extern    PIDREG_T     pi_ICurr;   //电流PID 
void Key_Gpio(void)  //按键初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = RUN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//电平由高到低对应上拉
	GPIO_Init(RUN_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = STOP_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//电平由高到低对应上拉
	GPIO_Init(STOP_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = UP_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//电平由高到低对应上拉
	GPIO_Init(UP_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DOWN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//电平由高到低对应上拉
	GPIO_Init(DOWN_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DIR_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//电平由高到低对应上拉
	GPIO_Init(DIR_GPIO_PORT, &GPIO_InitStructure);
}
		
void Key_Scanning(void) //按键处理   普通输入（没有用外部中断）
{
	static uint16_t Count = 0;
	
	if(RUN_STATUS == 0&& (Count == 0)) //运行
	{
		Start_Motor(); //启动   启用pwm
		StateContr.drive_car = 1; //使能控制  电机状态 1 运行  0停止
		//StateContr.Run_mode=0;
		if(	StateContr.Control_Mode == 1) //开环
		{
		StateContr.Aim_Speed = 20;  //开环给值
		StateContr.Aim_Duty = 10000 * StateContr.Aim_Speed / 100;//算占空比
		}
		if(StateContr.Control_Mode == 2 || StateContr.Control_Mode == 3) //速度 电流环
		{
		pi_spd.Ref = 300;  //参考速度
		}
		Count = 100;   //按键滤波
	}
	
	if(STOP_STATUS == 0&& (Count == 0)) //停止按键
	{
		StateContr.Duty = 0;    //占空比0
		TIM_SetCompare1(TIM1, 0);
		TIM_SetCompare2(TIM1, 0);
		TIM_SetCompare1(TIM1, 0);
		Stop_Motor(); //停止电机
		StateContr.Aim_Speed = 0; //参数设为0
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
	
		pi_spd.Err = 0;  //PID参数初始化
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

		Count = 200; //按键滤波
	}
	
	if(UP_STATUS == 0&& (Count == 0)) //按键加
	{
		if(	StateContr.Control_Mode == 2 || StateContr.Control_Mode == 3) //速度电流环
		{
		pi_spd.Ref = pi_spd.Ref + 10; //参考速度加
		}
		if(	StateContr.Control_Mode == 1 ) //开环
		{
		StateContr.Aim_Speed = StateContr.Aim_Speed + 1; //目标值
		StateContr.Aim_Duty = 10000 * StateContr.Aim_Speed / 100;//算成占空比
		}
		if(StateContr.Aim_Speed > 50) //占空比最大值
		{
		StateContr.Aim_Speed = 50;
		}
		if(pi_spd.Ref > 2100)//速度最大值
		{
		pi_spd.Ref = 2100;
		}
		
		Count = 200;  //按键滤波
	}
	
	if(DOWN_STATUS == 0&& (Count == 0)) //按键减
	{
		if(	StateContr.Control_Mode == 2 || StateContr.Control_Mode == 3)//速度 电流环
		{
		pi_spd.Ref = pi_spd.Ref - 10;//速度减
		}
		if(	StateContr.Control_Mode == 1 )//开环
		{
		StateContr.Aim_Speed = StateContr.Aim_Speed - 1;//占空比值
		StateContr.Aim_Duty = 10000 * StateContr.Aim_Speed / 100;//换算占空比
		}
		if(StateContr.Aim_Speed < 10)//占空比最小值
		{
		StateContr.Aim_Speed = 10;
		}
		if(pi_spd.Ref < 280) //速度最小值
		{
		pi_spd.Ref = 280;
		}
		
		Count = 200; //按键滤波
	}
	
	if(DIR_STATUS == 0&& (Count == 0))//换向暂时没用 
	{
		if(StateContr.Run_mode)
		{
			StateContr.Run_mode=0;
		}
		else
	   StateContr.Run_mode=1;
		
		Count = 200;
	}
	
	if (Count > 0)  //滤波减计数  相当于延时，一次只能按下一个键
	{
		
		Count--;
	}	
}

void OLED_Display(void) //OLED显示
{
	//第一行
//	OLED_ShowCHinese(32,0,46,1);//联
//	OLED_ShowCHinese(48,0,47,1);//控 
//	OLED_ShowCHinese(64,0,48,1);//智
//	OLED_ShowCHinese(80,0,49,1);//能 
  //第二行
	if(StateContr.Control_Mode == 2||StateContr.Control_Mode == 3) //速度电流环
	{	
	OLED_ShowCHinese(0,2,12,1); //目
	OLED_ShowCHinese(16,2,13,1); //标
	OLED_ShowCHinese(32,2,14,1);//速
	OLED_ShowCHinese(48,2,15,1);//度 
	
	OLED_ShowString(80,2," ",1);
	OLED_ShowNum(90,2,pi_spd.Ref,4,16);// 参考速度
	}

	//第三行
	if(StateContr.Control_Mode == 2||StateContr.Control_Mode == 3)
	{
	OLED_ShowCHinese(0,4,16,1);   //实
	OLED_ShowCHinese(16,4,17,1); //际
	OLED_ShowCHinese(32,4,14,1);//速
	OLED_ShowCHinese(48,4,15,1);//度 
	
	OLED_ShowNum(90,4,pi_spd.Fdb,4,16);//反馈速度值
	OLED_ShowString(80,4," ",1);
	}
	
	//第四行
	if(	StateContr.Control_Mode == 1) //开环
	{
	OLED_ShowCHinese(0,3,67,1);   //占
	OLED_ShowCHinese(16,3,68,1); //空
	OLED_ShowCHinese(32,3,69,1);//比
	OLED_ShowNum(90,3,StateContr.Aim_Speed,4,16); //值
	OLED_ShowString(80,3," ",1);	
		
	OLED_ShowCHinese(8,6,60,1);   //无
	OLED_ShowCHinese(24,6,61,1); //感
	OLED_ShowCHinese(40,6,62,1);//方
	OLED_ShowCHinese(56,6,63,1); //波
	OLED_ShowCHinese(72,6,64,1);//双
	OLED_ShowCHinese(88,6,65,1);//开
	OLED_ShowCHinese(104,6,66,1);//环
	}
	if(	StateContr.Control_Mode == 2) //速度环
	{	
	OLED_ShowCHinese(8,6,60,1);   //无
	OLED_ShowCHinese(24,6,61,1); //感
	OLED_ShowCHinese(40,6,62,1);//方
	OLED_ShowCHinese(56,6,63,1); //波
	OLED_ShowCHinese(72,6,70,1);//单
	OLED_ShowCHinese(88,6,71,1);//闭
	OLED_ShowCHinese(104,6,72,1);//环
	}
	if(	StateContr.Control_Mode == 3)//速度电流环
	{
	OLED_ShowCHinese(8,0,60,1);   //无
	OLED_ShowCHinese(24,0,61,1); //感
	OLED_ShowCHinese(40,0,62,1);//方
	OLED_ShowCHinese(56,0,63,1); //波
	OLED_ShowCHinese(72,0,73,1);//双
	OLED_ShowCHinese(88,0,71,1);//闭
	OLED_ShowCHinese(104,0,72,1);//环
	}
	OLED_ShowString(0,6,"WYD",1);
}



