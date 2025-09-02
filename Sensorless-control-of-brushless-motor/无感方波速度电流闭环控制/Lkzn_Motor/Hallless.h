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

#ifndef _Hallless_H
#define _Hallless_H

#include "stm32f4xx.h"

void ThreeHalllessPara_init(void);
void Hallless_SW(void);
unsigned char UEMF_Edge( unsigned char val);
unsigned char VEMF_Edge( unsigned char val);
unsigned char WEMF_Edge( unsigned char val);
typedef struct {
	    uint8_t       HalllessUVW[3];       //��ȡ�������綯�ƵĶ�Ӧ״̬
	    uint8_t       Hallless_State;       //��ǰ���綯��״̬ 
	    uint8_t       OldHallless_State;    //��ʷ���綯��״̬
			int32_t       step_angle_error;     //�����Ƕ����
	    int32_t       step_angle ;          //�����Ƕ� 
			int32_t       step_angleFitter ;    //�����Ƕ� �˲�ֵ
	    uint16_t      Speed_count;          //�ٶȼ���ֵ
	    uint16_t      Speed_countFitter;    //�ٶȼ����˲�ֵ
      uint16_t      Speed_count_old;      //�ٶȼ�����ʷֵ
			uint32_t      speed_coeff;          //�ٶ�ϵ��
	    uint8_t       Poles;                //���������
	    uint8_t       Move_State;           //�����ת״̬
      uint16_t      Speed_RPM;            //�����ת�ٶ� 
      uint16_t      Speed_RPMF;	          //�����ת�˲��ٶ�
			uint16_t      Queue_Status[3];	    //�˲�
			uint16_t      Filter_Count;	        //�˲�����
			uint16_t      Filter_CountF;	      //�˲�����
			uint16_t      Filter_Math;	        //�˲�
			uint16_t      QFilter_Status[3];	  //�˲�
			uint16_t      Filter_Edge;	        //�˲�
			uint16_t      Filter_Delay;	        //�˲�
			uint16_t      Filter_Count_All;	    //�˲�����
	   } Hallless;

#define  Hallless_DEFAULTS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}  // ��ʼ������

#endif
