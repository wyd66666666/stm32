#include "pid.h"

PID pid;

void PIDParament_Init()  //
{
    pid.choose_model = MODEL_PID;
    pid.T=330;                //�������ڣ���ʱ��ʹ��1ms������Сִ��PID������Ϊ330ms
    
    pid.set =0;            //�û��趨ֵ
    pid.Kp=0.5;                //����ϵ��
    pid.Ti=40;                //΢��ϵ������
    pid.Td=10;                //����ʱ�䳣��
    pid.OUT0=0;                //һ��ά�ֵ����

    pid.pwmcycle = 330;    //PWM������
}

    
void pid_calc()  //pid??
{
  float dk1;float dk2;
  float t1,t2,t3;
    
//    if(pid.Tdata < (pid.T))  //��С��������δ��
//     {
//            return ;
//     }
//    pid.Tdata = 0;
    
    pid.En=pid.set-pid.curr;  //�������
    dk1=pid.En-pid.En_1;   //����ƫ�����ϴ�ƫ��֮��
    dk2=pid.En-2*pid.En_1+pid.En_2;
    
  t1=pid.Kp*dk1;                            //����
    
    t2=(pid.Kp*pid.T)/pid.Ti;      //����
    t2=t2*pid.En;
    
    t3=(pid.Kp*pid.Td)/pid.T;        //΢��
    t3=t3*dk2;
    
    switch(pid.choose_model)
     {
         case MODEL_P:     pid.Dout= t1;                    printf("ʹ��P����\r\n") ;
             break;
         
         case MODEL_PI:  pid.Dout= t1+t2;                printf("ʹ��PI����\r\n") ;
             break;
                 
         case MODEL_PID: pid.Dout= t1+t2+t3;        printf("ʹ��PID����\r\n") ;
             break;
     } 
          
    pid.currpwm+=pid.Dout;  //����Ӧ�������PWM
    printf("PID��õ�OUT:\t%d\r\n",(int)pid.currpwm) ;
     
    /*�ж���������Ƿ���Ͽ���Ҫ��*/
    if(pid.currpwm>pid.pwmcycle)            //�����ֵȡֵ���϶�����0-pid.pwmcycle֮�䣬��Ȼ�Ļ�PWM��ô���
    {
      pid.currpwm=pid.pwmcycle;
    }
    if(pid.currpwm<0)
    {
     pid.currpwm=0;
    }
    
    printf("ʵ�����ʹ�õ�OUT:\t%d\r\n",(int)pid.currpwm) ;
    pid.En_2=pid.En_1;
    pid.En_1=pid.En;
    
    
    //Turn_Angle(pid.currpwm);                //���PWM
    
    

}


