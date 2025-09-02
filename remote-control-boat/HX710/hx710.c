#include "hx710.h"

long HX710_Buffer = 0;
long Weight_Maopi = 0,Weight_Shiwu = 0;

unsigned long count;
//****************************************************
//��ȡëƤ����
//****************************************************
void Get_Maopi()
{
  HX710_Buffer = HX710_Read();
  Weight_Maopi = HX710_Buffer/100;    
} 

//****************************************************
//����
//****************************************************
unsigned int Get_Weight()
{
  HX710_Buffer = HX710_Read();
  HX710_Buffer = HX710_Buffer/100;

  Weight_Shiwu = HX710_Buffer;
  Weight_Shiwu = Weight_Shiwu - Weight_Maopi;       //��ȡʵ���AD������ֵ��
  
  Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/7.35f);   
    //����ʵ���ʵ������
    //��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������4.30���������
    //�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
    //������Գ���������ƫСʱ����С����ֵ��
    //����ֵһ����7.16���ҡ��򴫸�����ͬ������
    //+0.05��Ϊ����������ٷ�λ

  return Weight_Shiwu;
}
//HX710_DT PA11
//HX710_SCK PA12
//****************************************************
//��ȡHX711
//****************************************************
unsigned long HX710_Read(void)  //����128
{
  unsigned char i;
  //int Flag = 0;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
  //digitalWrite(HX710_DT, HIGH);
  delayXus(1);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
  //digitalWrite(HX710_SCK, LOW);
  delayXus(1);
    count=0; 
    //while(digitalRead(HX710_DT));
   while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11))
	 {
	 
	 }		 
    for(i=0;i<24;i++)
  { 
      //digitalWrite(HX710_SCK, HIGH); 
		 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);
    delayXus(1);
      count=count<<1; 
    //digitalWrite(HX710_SCK, LOW);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);		
    delayXus(1);
      //if(digitalRead(HX710_DT))
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11))
      count++; 
  } 
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET); 
  count ^= 0x800000;
  delayXus(1);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
  delayXus(1);
  
  return(count);
}


