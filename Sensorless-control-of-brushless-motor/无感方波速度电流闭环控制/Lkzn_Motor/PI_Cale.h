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
#ifndef  PI_Cale_H
#define  PI_Cale_H

#include "stm32f4xx.h"
void PID_init(void);
typedef struct 
{  
				float  Ref;   		// Input: Reference input 
				float  Fdb;   		// Input: Feedback input 
				float  Err;			  // Variable: Error 
				float  Kp;			  // Parameter: Proportional gain
				float  Up;			  // Variable: Proportional output 
				float  Ui;			  // Variable: Integral output 
				float  Ud;			  // Variable: Derivative output	
				float  OutPreSat;	// Variable: Pre-saturated output
				float  OutMax;		// Parameter: Maximum output 
				float  OutMin;		// Parameter: Minimum output
				float  Out;   		// Output: PID output 
				float  SatErr;		// Variable: Saturated difference
				float  Ki;			  // Parameter: Integral gain
				float  Kc;			  // Parameter: Integral correction gain
				float  Kd; 			  // Parameter: Derivative gain 
				float  Up1;			  // History: Previous proportional output
				float  Ui_1; 
				float  OutF; 
} PIDREG_T ,*p_PIDREG_T ;;		 

#define PIDREG_T_DEFAULTS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}  // ��ʼ������
#define   _IQmpy(A,B)         ((A) * (B))
#define FirstOrder_LPF_Cacl(Xn, Yn_1, a)\
																					Yn_1 = (1-a)*Yn_1 + a*Xn; //Xn:in;Yn:out;a:ϵ��
#define UP16LIMIT(var,max,min) {(var) = (var)>(max)?(max):(var) ;\
																					(var) = (var)<(min)?(min):(var) ;\
																					}
#define PID_CALC(v)	\
											v.Err = v.Ref - v.Fdb; \
											v.Up= _IQmpy(v.Kp,v.Err);\
											v.Ui= v.Ui + _IQmpy(v.Ki,v.Up);\
											UP16LIMIT(v.Ui,v.OutMax,v.OutMin);\
											v.Ud = v.Kd * (v.Up - v.Up1);\
											v.Out = v.Up + v.Ui + v.Ud;\
											UP16LIMIT(v.Out,v.OutMax,v.OutMin);\
											v.Up1 = v.Up;	

#endif
