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

PIDREG_T     pi_spd = PIDREG_T_DEFAULTS;  //速度PID默认值
PIDREG_T     pi_ICurr = PIDREG_T_DEFAULTS;//电流PID默认值
State        StateContr=State_DEFAULTS;   //控制状态默认值
ADCSamp      ADCSampPare=ADCSamp_DEFAULTS;//ADC默认值

Hallless     Hallless_Three=Hallless_DEFAULTS;  //HALL初始化
VvvF_start   VvvF_startPare=VvvF_start_DEFAULTS; //VF初始化

extern       Hallless   Hallless_Three;

volatile uint16_t ADC_DualConvertedValueTab[5];  //电位器接口函数
int main(void)                        //主函数
{
	Delay(10000);	                      //延时
	SysTickConfig();                    //滴答时钟初始化
	StateContr.Control_Mode = LOOP;     //控制环模式  3
	PID_init();                         //PID初始化 
	if(	StateContr.Control_Mode == 1)   //开环
	{
	StateContr.Aim_Speed = 20;          //速度    
	StateContr.Aim_Duty = 10000 * StateContr.Aim_Speed / 100; //速度换算成占空比
	}
	if(StateContr.Control_Mode == 2 || StateContr.Control_Mode == 3)  //速度闭合或者速度电流双闭环
	{
		pi_spd.Ref = 300;               //默认参考值
	}
	Delay(10000);	                   //延时
	Key_Gpio();                      //按键初始化
                     
    
	ThreeHalllessPara_init();        //无感参数初始化   电机参数
	Hallless_Gpio();                 //无感引脚配置

	Delay(10000);	                   //延时
	Adc_Configuration();             //ADC配置  DMA   电流检测（电压采集）
	Delay(10000);	                   //延时
	Tim1_PWM_Init();                 //TIM1配置  PWM
	Delay(10000);	                   //延时  
	Offset_CurrentReading();         //电流偏移值 校准  对于相电流和母线电流的电阻法测量电流,需要上电读取初始偏执电压
	Delay(10000);	                    //延时
	OLED_Init();                      //oled初始化
	while(1)
	{
		OLED_Display();                 //显示
		Key_Scanning();                 //按键逻辑处理
	}
}

