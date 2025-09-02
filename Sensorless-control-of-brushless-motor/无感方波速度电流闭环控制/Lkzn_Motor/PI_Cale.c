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
#include "PI_Cale.h"
#include "Drive parameters.h"

extern    PIDREG_T       pi_spd; //速度PID
extern    PIDREG_T       pi_ICurr;//电流PID

void PID_init(void) //PID参数初始化
{
  pi_spd.Kp = Speed_Kp;    //速度KP
  pi_spd.Ki = Speed_Ki;    //速度KI 
	pi_spd.Kc = Speed_Kc;    //速度KD
  pi_spd.OutMax =10000;    //最大输出 10000
  pi_spd.OutMin =1000;     //最小输出1000
	pi_spd.Ref = 0;         //速度参考 0

  pi_ICurr.Kp = Current_Kp;  //电流KP  
  pi_ICurr.Ki = Current_Ki;  //电流KI
	pi_ICurr.Kc = Current_Kc;  //电流KD
  pi_ICurr.OutMax =10000;    //最大输出 10000
  pi_ICurr.OutMin =1000;     //最小输出1000
	pi_ICurr.Ref = 0;	         //速度参考 0
	
 }


