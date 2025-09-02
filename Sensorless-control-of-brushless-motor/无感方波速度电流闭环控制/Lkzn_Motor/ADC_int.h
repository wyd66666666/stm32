//############################################################
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
#ifndef ADC_int_H
#define ADC_int_H

#include "stm32f4xx.h"

void Adc_Configuration(void);
void Offset_CurrentReading(void);
void HALL_ADC_Sample(void);
void HALLLESS_ADC_Sample(void);

typedef struct {
	     int32_t   BUS_CurrF ;     // ĸ�ߵ��� DC Bus  Current
			 int32_t   BUS_Curr ;     // ĸ�ߵ��� DC Bus  Current
	     int32_t   PhaseW_Curr ;  // W����� Phase U Current
	     int32_t   PhaseU_Curr;   // U����� Phase U Current
	     int32_t   PhaseV_Curr;   // V�����Phase V Current
	     int32_t   BUS_Voltage ;  //ĸ�ߵ�ѹDC Bus  Voltage	     
	     int32_t   RP_speed_Voltage ;   // ��λ����ѹ RP1_Voltage
	     int32_t   OffsetPhaseU_Curr;   // U�����ƫִֵ  Phase U Current
	     int32_t   OffsetPhaseV_Curr;   // V�����ƫִֵ Phase V Current
		   int32_t   OffsetPhaseW_Curr ;  // W�����ƫִֵ Phase V Current
	     int32_t   Coeff_filterK1;   // һ�׵�ͨ�˲���ϵ��1
		   int32_t   Coeff_filterK2;   // һ�׵�ͨ�˲���ϵ��2
       }ADCSamp;

#define  ADCSamp_DEFAULTS  {0,0,0,0,0,0,0,0,0,1000,24}   // ��ʼ������



#endif
