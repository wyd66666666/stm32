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
#include "PI_Cale.h"
#include "Drive parameters.h"

extern    PIDREG_T       pi_spd; //�ٶ�PID
extern    PIDREG_T       pi_ICurr;//����PID

void PID_init(void) //PID������ʼ��
{
  pi_spd.Kp = Speed_Kp;    //�ٶ�KP
  pi_spd.Ki = Speed_Ki;    //�ٶ�KI 
	pi_spd.Kc = Speed_Kc;    //�ٶ�KD
  pi_spd.OutMax =10000;    //������ 10000
  pi_spd.OutMin =1000;     //��С���1000
	pi_spd.Ref = 0;         //�ٶȲο� 0

  pi_ICurr.Kp = Current_Kp;  //����KP  
  pi_ICurr.Ki = Current_Ki;  //����KI
	pi_ICurr.Kc = Current_Kc;  //����KD
  pi_ICurr.OutMax =10000;    //������ 10000
  pi_ICurr.OutMin =1000;     //��С���1000
	pi_ICurr.Ref = 0;	         //�ٶȲο� 0
	
 }


