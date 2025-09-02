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
#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include "stm32f4xx.h"
 
typedef struct {
	    uint8_t    Control_Mode;   // ����ģʽ  
	    uint8_t    TripFlagDMC;    // ���� ������־
	    uint8_t    drive_car;      // ��ʼ�������״̬
	    uint8_t    olddrive_car;   // ��ʷ��ʼ�������״̬
	    uint8_t    clear_PWMtripz; // ���������־
	    uint8_t    Run_mode;       // ����ת����״̬
	    uint16_t   Qiehuan_count;  // �л�״̬�ļ���ֵ
	    uint8_t    Start_order;    // ����PWM���������
	    uint16_t   Duty;           // �л�״̬�ļ���ֵ
	    uint16_t   Speed_Count;    // �ٶȻ���ʱ
	    uint16_t   Current_Count;  // ��������ʱ	
			uint16_t   Aim_Speed;
			uint32_t   Aim_Duty;
	   	 }State; 

#define  State_DEFAULTS  {0,0,0,0,0,0,0,1,0,0,0,0,0} // ��ʼ������

void Key_Scanning(void);
void Key_Gpio(void);
void OLED_Display(void);
#endif
