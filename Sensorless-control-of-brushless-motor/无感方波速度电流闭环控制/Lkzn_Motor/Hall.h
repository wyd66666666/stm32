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
#ifndef _Hall_H
#define _Hall_H

#include "stm32f4xx.h"

unsigned char UHALL_Edge( unsigned char val);
void Hall_SW(void);
void ThreeHallPara_init(void);

typedef struct {
	    uint8_t       HallUVW[3]; // ��ȡ���������Ķ�Ӧ״̬
	    uint8_t       Hall_State; //��ǰ����״̬
	    uint8_t       OldHall_State; // ��ʷ����״̬
			int32_t       step_angle_error;  //�����Ƕ����
	    int32_t       step_angle ;  //�����Ƕ� 
			int32_t       step_angleFitter ; //�����Ƕ� �˲�ֵ
	    uint16_t      Speed_count;   //�ٶȼ���ֵ
	    uint16_t      Speed_countFitter; //�ٶȼ����˲�ֵ
      uint16_t      Speed_count_old;  //�ٶȼ�����ʷֵ
			uint32_t      speed_coeff;    //�ٶ�ϵ��
	    uint8_t       Poles;    //���������
	    uint8_t       Move_State;       //�����ת״̬
      uint16_t      Speed_RPM;     //�����ת�ٶ� 
      uint16_t      Speed_RPMF;	   //�����ת�˲��ٶ�
			uint16_t      Filter_Edge;	
			uint16_t      Filter_Count;
			uint16_t    	Filter_CountF;
			uint16_t      Queue_Status[3];
			uint16_t      Filter_Math;
			uint16_t      QFilter_Status[3];
	   } Hall;
		 
#define  Hall_DEFAULTS {0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0} // ������ʼ��		 

#endif
