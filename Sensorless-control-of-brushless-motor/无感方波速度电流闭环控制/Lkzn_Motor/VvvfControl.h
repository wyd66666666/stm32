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

#ifndef _VvvfControl_H
#define _VvvfControl_H

#include "stm32f4xx.h"

typedef struct {
	uint16_t      Freq_T_Ref;      //�������ݹ̶�����״̬����Ƶ��
	uint16_t      Voilage_Ref;     //���������ο���ѹ
	uint16_t      VvvF_huanxCount; //�������
	uint16_t      VvvF_Count;      //VF����
	uint16_t      VvvF_state;      //VF����״̬
	uint16_t      VvvF_num[6];     //VF��������˳��
	uint16_t      OldVF_State;     //��ʷVF����״̬ 
	uint16_t      initial_state;   //VF��ʼ״̬��λ
	uint16_t      Freq_T_Ref_Count;//�������
	uint16_t      Count;           //�������
	  }VvvF_start;

#define  VvvF_start_DEFAULTS   {0,0,0,0,0,0,0,0,0,0} // ��ʼ������

void Delay_MS(u32 nCount_temp);
void Delay_US(u32 nCount_temp);
void Anwerfen(void);
void Anwerfen_SW(void);
void Change_Voltage(void);
#endif
