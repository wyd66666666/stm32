#include "hx710.h"

long HX710_Buffer = 0;
long Weight_Maopi = 0,Weight_Shiwu = 0;

unsigned long count;
//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi()
{
  HX710_Buffer = HX710_Read();
  Weight_Maopi = HX710_Buffer/100;    
} 

//****************************************************
//称重
//****************************************************
unsigned int Get_Weight()
{
  HX710_Buffer = HX710_Read();
  HX710_Buffer = HX710_Buffer/100;

  Weight_Shiwu = HX710_Buffer;
  Weight_Shiwu = Weight_Shiwu - Weight_Maopi;       //获取实物的AD采样数值。
  
  Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/7.35f);   
    //计算实物的实际重量
    //因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的4.30这个除数。
    //当发现测试出来的重量偏大时，增加该数值。
    //如果测试出来的重量偏小时，减小改数值。
    //该数值一般在7.16左右。因传感器不同而定。
    //+0.05是为了四舍五入百分位

  return Weight_Shiwu;
}
//HX710_DT PA11
//HX710_SCK PA12
//****************************************************
//读取HX711
//****************************************************
unsigned long HX710_Read(void)  //增益128
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


