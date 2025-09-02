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

#ifndef _VvvfControl_H
#define _VvvfControl_H

#include "stm32f4xx.h"

typedef struct {
	uint16_t      Freq_T_Ref;      //开环根据固定霍尔状态启动频率
	uint16_t      Voilage_Ref;     //开环给定参考电压
	uint16_t      VvvF_huanxCount; //换相计数
	uint16_t      VvvF_Count;      //VF计算
	uint16_t      VvvF_state;      //VF换相状态
	uint16_t      VvvF_num[6];     //VF霍尔换相顺序
	uint16_t      OldVF_State;     //历史VF换相状态 
	uint16_t      initial_state;   //VF初始状态定位
	uint16_t      Freq_T_Ref_Count;//换相计数
	uint16_t      Count;           //换相计数
	  }VvvF_start;

#define  VvvF_start_DEFAULTS   {0,0,0,0,0,0,0,0,0,0} // 初始化参数

void Delay_MS(u32 nCount_temp);
void Delay_US(u32 nCount_temp);
void Anwerfen(void);
void Anwerfen_SW(void);
void Change_Voltage(void);
#endif
