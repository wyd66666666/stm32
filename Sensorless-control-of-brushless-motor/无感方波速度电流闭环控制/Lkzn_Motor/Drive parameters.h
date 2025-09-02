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
#ifndef __DRIVE_PARAMETERS_H
#define __DRIVE_PARAMETERS_H

#define TEST_MANUELL                    0

//�޻�����ʽ
#define  HALLLESS

//1:������2���ٶȻ���3�ٶȻ�����������
#define  LOOP                           3 

//1����ת��2����ת
#define  INVERSION                      1

//PWMƵ�ʣ���λKHZ;
#define  PWM_FREQ                      ((u16) 16) 

//����Ŀ��ת�أ������������� 
#define  Torque                         0xFBB 

//Hallless �˲�����
#define  FILTER_LONG                    0xFF 

//�ٶ�PID
#define  Speed_Kp                       0.1 
#define  Speed_Ki                       0.01 
#define  Speed_Kc                       0.01 

//����PID
#define  Current_Kp                     2 
#define  Current_Ki                     0.2
#define  Current_Kc                     0.01 

//H_PWM_L_ON����ģʽ       �������ģʽ
#define  H_PWM_L_ON

#endif
