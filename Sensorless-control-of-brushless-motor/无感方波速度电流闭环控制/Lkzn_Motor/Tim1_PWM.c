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
#include "Tim1_PWM.h"
#include "State_Machine.h"
#include "Drive parameters.h"
#include "Control stage parameters.h"

extern    State      StateContr;

void TIM1_Gpio(void)  //TIM1引脚定义
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	 
	GPIO_InitStructure.GPIO_Pin = PHASE_UH_GPIO_PIN| PHASE_VH_GPIO_PIN | PHASE_WH_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(PHASE_UH_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(PHASE_UH_GPIO_PORT, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(PHASE_VH_GPIO_PORT, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(PHASE_WH_GPIO_PORT, GPIO_PinSource10, GPIO_AF_TIM1);

	GPIO_InitStructure.GPIO_Pin =  PHASE_UL_GPIO_PIN | PHASE_VL_GPIO_PIN | PHASE_WL_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PHASE_UL_GPIO_PORT, &GPIO_InitStructure);
}

void  Tim1_PWM_Init(void)
{ 
  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef       TIM_OCInitStructure;
  TIM_BDTRInitTypeDef     TIM_BDTRInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM1_Gpio();
 
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;            //  增计数模式
  TIM_TimeBaseStructure.TIM_Period = 168000 / PWM_FREQ - 1;     //  MOS开关频率 168000/ 10500 = 16k   
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;                  
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;      
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;    //PWM模式1 
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //正输出使能
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;//负输出使能
  TIM_OCInitStructure.TIM_Pulse = 0;                                     //PWM占空比
  
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ;             //上桥臂
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High ;           //下桥臂
	
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);                         
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);
  
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);//配置OC1 自动加载   
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);//配置OC2 自动加载
  TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);//配置OC3 自动加载
  
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
         
  TIM_BDTRInitStructure.TIM_DeadTime = 0x6A; 															//死区1.8us
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;        
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);	
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);  
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);                            //清中断标志位
  TIM_ITConfig(TIM1,TIM_IT_Update ,ENABLE);                              //打开中断 
 
  TIM_Cmd(TIM1, ENABLE);                       //使能TIM1
	TIM_CtrlPWMOutputs(TIM1, ENABLE);       //TIM1 PWM输出使能

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //配置 TIM1 溢出中断
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
 
void Stop_Motor(void)  //停止电机
{
	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
	TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);

	TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
	TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);

	TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
	TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
}

void Start_Motor(void) //启动电机
{
	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
	TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);

	TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
	TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);

	TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
	TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);

}

void MOS_Q41PWM(void)      
{   
	TIM1->CCR2 = 0;         
	TIM1->CCR1 = StateContr.Duty;					  
	TIM1->CCR3 = 0;
	GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_15); 
	GPIO_SetBits(GPIOB, GPIO_Pin_14); 	
}
 
void  MOS_Q16PWM(void)
{    
	TIM1->CCR2 = 0;            
	TIM1->CCR1 = StateContr.Duty;			//赋值占空比		  
	TIM1->CCR3 = 0;
	GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14); //设置引脚电平 
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
	
}
 
void MOS_Q63PWM(void)
{    	
	TIM1->CCR1=0;       
	TIM1->CCR2 = StateContr.Duty;					  
	TIM1->CCR3=0;
	GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14); 
	GPIO_SetBits(GPIOB, GPIO_Pin_15); 
}
 
void MOS_Q32PWM(void) //
{    

	TIM1->CCR1 = 0;        
	TIM1->CCR2 = StateContr.Duty;					  
	TIM1->CCR3 = 0;
	GPIO_ResetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_15); 
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
}


void MOS_Q25PWM(void)
{
	TIM1->CCR2 = 0;
	TIM1->CCR3 = StateContr.Duty;					  
	TIM1->CCR1 = 0;
	GPIO_ResetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_15); 
	GPIO_SetBits(GPIOB, GPIO_Pin_13); 
}
 
void MOS_Q54PWM(void)
{  
	TIM1->CCR2 = 0; 
	TIM1->CCR3 = StateContr.Duty;					  
	TIM1->CCR1 = 0;
	GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_15); 
	GPIO_SetBits(GPIOB, GPIO_Pin_14);   
}
