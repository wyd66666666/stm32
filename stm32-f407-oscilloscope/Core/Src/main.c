/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "fsmc.h"
#include "arm_math.h"                  //添加头文件
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)	  ) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )


#define  FFT_LENGTH        1024        //FFT长度，默认是1024点FFT
#define  SAMPLE_FREQ       1024        //采样频率


uint8_t testdatatosend[50];
void TestSendData(uint8_t *dataToSend , uint8_t length);
void Test_Send_User(uint16_t data1, uint16_t data2, uint16_t data3,uint16_t data4,uint16_t data5,uint16_t data6,uint16_t data7,uint16_t data8,uint16_t data9,uint16_t data10)	;

void ENDSend(uint8_t k);

void adc_line(void);
void adc_line_clean(void);

float Get_vpp(u16 *buf);	   //获取峰峰值
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t aRxBuffer; //接收中断缓冲

//usart1发送和接收数组
uint8_t usart1_txbuf[256];
uint8_t usart1_rxbuf[512];
uint8_t usart1_rxone[1];
uint8_t usart1_rxcounter;

uint16_t adcBuf[2]={0,0};    //ad的值
float adcvcc[2];
int i;

int j;

uint16_t data[40]={500,500,500,500};

__IO uint16_t ADC_ConvertedValue[1024];
__IO float ADC_Volt;

float ADC_Value[1024] ;

u32 max_data=2048; //触发电平
vu32 temp=0; //频率输出
float vpp=0;
int freq=0;
int timeshow=500;
u8  Vpp_buff[20] = {0};//sprintf数据输出
u8  freq_buff[20] = {0};//sprintf数据输出
u8  time_buff[20] = {0};//sprintf数据输出
int time_flag=0;
int once=0;

int mode1=1;     //0 stop   1 run
int mode2=1;     //0 down    1 up
int mode3=115;   //触发电平
int mode4=2;    //0 auto   1once  2pt
int mode5=0;   //垂直位移
int mode6=0;    //0 DC 1 AC 2 GND 

float timebase=32;
int time_i=6;     //时基计数

extern int key0,key1,key2;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  u8 x=0;
	u8 lcd_id[12];				//存放LCD ID字符串
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_FSMC_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1); 
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
	
	 TFTLCD_Init(); 
	LCD_Clear(BLACK);
	Lcd_DrawNetwork();
	
		
	BACK_COLOR=BLACK;
	LCD_ShowString_COLOR(0,226,120,12,12,"Time:",WHITE);
	LCD_ShowString_COLOR(80,226,120,12,12,"V= 1v",WHITE);
	LCD_ShowString_COLOR(120,226,120,12,12,"Freq:",WHITE);
	LCD_ShowString(200,226,120,12,12,"Vpp:");
	
	BACK_COLOR=GRAYBLUE;
	LCD_ShowString_COLOR(295,15,200,12,12," run",GREEN);
	LCD_ShowString_COLOR(295,55,200,12,12,"rise",BLACK);
	LCD_ShowString_COLOR(295,95,200,12,12,"trig",BLACK);
	LCD_ShowString_COLOR(295,135,200,12,12," nor",BLACK);
	LCD_ShowString_COLOR(295,175,200,12,12,"move",BLACK);
	LCD_ShowString_COLOR(295,215,200,12,12,"DC",BLACK);
	 BACK_COLOR=GREEN;
	 LCD_ShowString_COLOR(289,mode3-5,200,12,12,"<",BLACK);
	 LCD_DrawLine_color(0,mode3,288,mode3,GREEN);
	 
	 LCD_DrawLine_color(0,mode5+120,288,mode5+120,LIGHTBLUE);
		 
	LCD_DrawLine_color(295,30,320,30,GREEN);
  //printf("ok\r\n");
	//HAL_TIM_Base_Start(&htim2);//打开定时器
//	HAL_TIM_Base_Start_IT(&htim2);//打开定时器中断

	  HAL_TIM_OC_Start_DMA(&htim3,TIM_CHANNEL_1,(uint32_t*)data,4);
		HAL_TIM_Base_Start(&htim2);
  	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_ConvertedValue, 1024);
	
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组。
	
	
	
  int i,j=0;	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		//匿名上位机+HMI串口屏
//		for(j=0;j<20;j++)
//		{printf("add 2,0,255");
//		ENDSend(0xff);
//		Test_Send_User(1,2,3,4,5,6,7,8,9,10);
//    HAL_Delay(50);
//		}
//		for(j=0;j<20;j++)
//		{printf("add 2,0,100");
//		ENDSend(0xff);
//		Test_Send_User(1,2,3,4,5,6,7,8,9,10);
//    HAL_Delay(50);
//		}
	
//	//adc测试代码
//	for(i=0;i<2;i++)
//  {
//		HAL_ADC_Start(&hadc1);    
//    HAL_ADC_PollForConversion(&hadc1,0xffff);
//    adcBuf[i]=HAL_ADC_GetValue(&hadc1);
//		adcvcc[i]=adcBuf[i]*3.3f/4096;
//    printf("------ch:%d--%.2f-------\r\n",i,adcvcc[i]);
//   }
//	 HAL_ADC_Stop(&hadc1);
//	 HAL_Delay(1);
     vpp=Get_vpp(ADC_ConvertedValue);
     BACK_COLOR=BLACK;
     sprintf((char*)Vpp_buff,"Vpp:%0.3fV",vpp);
     LCD_ShowString_COLOR(200,226,120,12,12,Vpp_buff,WHITE);
		 
		 sprintf((char*)freq_buff,"Freq:%6dHz",freq);
     LCD_ShowString_COLOR(120,226,120,12,12,freq_buff,WHITE);
		 
		 sprintf((char*)time_buff,"Time:%5dus/div",timeshow);
		 LCD_ShowString_COLOR(0,226,120,12,12,time_buff,WHITE);
		 
		 
     if(key0%6==1&&key1==1)
		 {
			 if(mode1==1)
			 {
					BACK_COLOR=GRAYBLUE;
					LCD_ShowString_COLOR(295,15,200,12,12,"stop",RED);
					mode1=0;
			 }
			 else
			 {
					BACK_COLOR=GRAYBLUE;
					LCD_ShowString_COLOR(295,15,200,12,12," run",GREEN);
				  Lcd_DrawNetwork();
					mode1=1;
			 }
			 key1=0;
		 }
		   

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void TestSendData(uint8_t *dataToSend , uint8_t length)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&testdatatosend, length, 0xffff);
}


void Test_Send_User(uint16_t data1, uint16_t data2, uint16_t data3,uint16_t data4,uint16_t data5,uint16_t data6,uint16_t data7,uint16_t data8,uint16_t data9,uint16_t data10)	
{
	uint8_t _cnt=0;
	
	testdatatosend[_cnt++]=0xAA;
	testdatatosend[_cnt++]=0x05;
	testdatatosend[_cnt++]=0xAF;
	testdatatosend[_cnt++]=0xF1;
	testdatatosend[_cnt++]=0;
 
	testdatatosend[_cnt++]=BYTE1(data1);
	testdatatosend[_cnt++]=BYTE0(data1);
	
	testdatatosend[_cnt++]=BYTE1(data2);
	testdatatosend[_cnt++]=BYTE0(data2);
	
	testdatatosend[_cnt++]=BYTE1(data3);
	testdatatosend[_cnt++]=BYTE0(data3);
	
	testdatatosend[_cnt++]=BYTE1(data4);
	testdatatosend[_cnt++]=BYTE0(data4);
	
	testdatatosend[_cnt++]=BYTE1(data5);
	testdatatosend[_cnt++]=BYTE0(data5);
	
	testdatatosend[_cnt++]=BYTE1(data6);
	testdatatosend[_cnt++]=BYTE0(data6);
	
	testdatatosend[_cnt++]=BYTE1(data7);
	testdatatosend[_cnt++]=BYTE0(data7);
	
	testdatatosend[_cnt++]=BYTE1(data8);
	testdatatosend[_cnt++]=BYTE0(data8);
	
	testdatatosend[_cnt++]=BYTE1(data9);
	testdatatosend[_cnt++]=BYTE0(data9);
	
	testdatatosend[_cnt++]=BYTE1(data10);
	testdatatosend[_cnt++]=BYTE0(data10);
	
	
 
	testdatatosend[4] = _cnt-5;
	
	uint8_t sum = 0;	
	for(uint8_t i=0;i<_cnt;i++)
		sum += testdatatosend[i];
	
	testdatatosend[_cnt++]=sum;	
 
	TestSendData(testdatatosend, _cnt);	
}

void ENDSend(uint8_t k)           //字节发送函数
{   
    uint8_t i;
    for(i=0;i<3;i++)
    {
           if(k!=0)
          {  
           HAL_UART_Transmit(&huart1,&k,1,1000);    //发送一个字节   
           HAL_Delay(10);
           while((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TXE)==RESET)){};   //等待发送结束
           HAL_Delay(10);
           } 
    }   
} 

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)	// 判断是由哪个串口触发的中断
	{
		//将接收到的数据放入接收usart1接收数组
		usart1_rxbuf[usart1_rxcounter] = usart1_rxone[0];
		usart1_rxcounter++;	//接收数量＋1
		if(usart1_rxone[0]==0x30)
		{
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
		}
		if(usart1_rxone[0]==0x31)
		{
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
		}
		//重新使能串口1接收中断
		HAL_UART_Receive_IT(&huart1,usart1_rxone,1);		
	}
}


void adc_line()
{	
	static u16 Ypos1 = 0,Ypos2 = 0;
	static vu32 min_data=0;//buf[1];
	static vu32 n=0,con_t=0,con_t1=0;
   static	u16 i = 0,y=0,t=0;
	POINT_COLOR = RED;
	  for(n = 0;n<1024;n++)
      {
         if(ADC_ConvertedValue[n] > max_data&&ADC_ConvertedValue[n+3] < max_data)
         {
            con_t=n;
            break;
         }			
      } 
      for(n = con_t+3;n<1024;n++)
      {
         if(ADC_ConvertedValue[n] > max_data&&ADC_ConvertedValue[n+3] < max_data)
         {
            con_t1=n;
            break;
         }			
      }
      temp=con_t1- con_t;
			freq=(temp*timebase)*0.5;
			
			
		 if(key0%6==2&&key1==1)
		 {
		    if(mode2==1)
			 {
					BACK_COLOR=GRAYBLUE;
					LCD_ShowString_COLOR(295,55,200,12,12,"down",BLACK);
					mode2=0;
			 }
			 else
			 {
					BACK_COLOR=GRAYBLUE;
					LCD_ShowString_COLOR(295,55,200,12,12,"rise",BLACK);
					mode2=1;
			 }
			 key1=0;
		 }
		 else if(key0%6==4&&key1==1)
		 {
		    if(mode4==0)
				{
				   BACK_COLOR=GRAYBLUE;
					 LCD_ShowString_COLOR(295,135,200,12,12,"once",BLACK);
					 mode4=1;
				}
				else if(mode4==1)
				{
				   BACK_COLOR=GRAYBLUE;
					 LCD_ShowString_COLOR(295,135,200,12,12," nor",BLACK);
					 mode4=2;
				}
				else
				{
					 BACK_COLOR=GRAYBLUE;
					 LCD_ShowString_COLOR(295,135,200,12,12,"auto",BLACK);
					 mode4=0;
				  
				}
		    key1=0;
		 }
			else if(key0%6==0)
     {   
			  if(time_flag==0)
				{
		     time_i=key1-key2+6;
			   if(time_i==1)
				 {
				    timebase=0.64;
					 TIM2->ARR = timebase - 1;
					  timeshow=10;
				 }
				 else if(time_i==2)
				 {
				    timebase=1.28;
					 TIM2->ARR = timebase - 1;
					 timeshow=20;
				 }
				 else if(time_i==3)
				 {
				    timebase=3.2;
					 TIM2->ARR = timebase - 1;
					 timeshow=50;
				 }
				 else if(time_i==4)
				 {
				    timebase=6.4;
					 TIM2->ARR = timebase - 1;
					  timeshow=100;
				 }
				 else if(time_i==5)
				 {
				    timebase=12.8;
					 TIM2->ARR = timebase - 1;
					 timeshow=200;
				 }
				 else if(time_i==6)
				 {
				    timebase=32;
					 TIM2->ARR = timebase - 1;
					 timeshow=500;
				 }
				 else if(time_i==7)
				 {
				    timebase=64;
					 TIM2->ARR = timebase - 1;
					  timeshow=1000;
				 }
				 else if(time_i==8)
				 {
				    timebase=128;
					 TIM2->ARR = timebase - 1;
					 timeshow=2000;
				 }
				 else if(time_i==9)
				 {
				    timebase=320;
					 TIM2->ARR = timebase - 1;
					 timeshow=5000;
				 }
				 else if(time_i==10)
				 {
				    timebase=640;
					 TIM2->ARR = timebase - 1;
					  timeshow=10000;
				 }
				 else if(time_i==11)
				 {
				    timebase=1280;
					 TIM2->ARR = timebase - 1;
					 timeshow=20000;
				 }
				 else if(time_i==12)
				 {
				    timebase=3200;
					 TIM2->ARR = timebase - 1;
					 timeshow=50000;
				 }
				 else if(time_i==13)
				 {
				    timebase=6400;
					 TIM2->ARR = timebase - 1;
					  timeshow=100000;
				 }
				 else if(time_i==14)
				 {
				    timebase=12800;
					 TIM2->ARR = timebase - 1;
					  timeshow=200000;
				 }
				 else if(time_i==15)
				 {
				    timebase=32000;
					 TIM2->ARR = timebase - 1;
					  timeshow=500000;
				 }
				 else if(time_i==16)
				 {
				    timebase=64000;
					 TIM2->ARR = timebase - 1;
					   timeshow=1000000;
				 }
				 else if(time_i==17)
				 {
				    timebase=128000;
					 TIM2->ARR = timebase - 1;
					  timeshow=2000000;
				 }
				 else if(time_i==18)
				 {
				    timebase=320000;
					 TIM2->ARR = timebase - 1;
					  timeshow=5000000;
				 }
				 else if(time_i==19)
				 {
				    timebase=640000;
					 TIM2->ARR = timebase - 1;
					  timeshow=10000000;
					  
				 }
				 else if(time_i==20)
				 {
				    timebase=1280000;
					 TIM2->ARR = timebase - 1;
					 timeshow=20000000;
				 }
				 else if(time_i==21)
				 {
				    timebase=3200000;
					 TIM2->ARR = timebase - 1;
					 timeshow=50000000;
				 }
				 if(time_i>21)
					 time_i=21;
				 if(time_i<i)
					 time_i=1;
				 time_flag=1;
			 }
		}
		 else if(key0%6==3)
		 {
		    if(key1==1)
				{
					adc_line_clean();
	        Lcd_DrawNetwork();
					LCD_ShowString_COLOR(289,mode3-5,200,12,12," ",BLACK);
		      LCD_DrawLine_color(0,mode3,288,mode3,BLACK);
					mode3=mode3+5;
					BACK_COLOR=GREEN;
	        LCD_ShowString_COLOR(289,mode3-5,200,12,12,"<",BLACK);
		      LCD_DrawLine_color(0,mode3,288,mode3,GREEN);
					key1=0;
				}
		    if(key2==1)
				{
					adc_line_clean();
	        Lcd_DrawNetwork();
					LCD_ShowString_COLOR(289,mode3-5,200,12,12," ",BLACK);
		      LCD_DrawLine_color(0,mode3,288,mode3,BLACK);
				  mode3=mode3-5;
					BACK_COLOR=GREEN;
	        LCD_ShowString_COLOR(289,mode3-5,200,12,12,"<",BLACK);
		      LCD_DrawLine_color(0,mode3,288,mode3,GREEN);
					key2=0;
				}
		 
		 }
		 
		 	else if(key0%6==5)
		{
		   if(key1==1)
			 {
				 adc_line_clean();
	        Lcd_DrawNetwork();
			   LCD_DrawLine_color(0,mode5+120,288,mode5+120,BLACK);
				 mode5=mode5+5;
				 LCD_DrawLine_color(0,mode5+120,288,mode5+120,LIGHTBLUE);
				 key1=0;
			 }
			 if(key2==1)
			 {
				 adc_line_clean();
	        Lcd_DrawNetwork();
			   LCD_DrawLine_color(0,mode5+120,288,mode5+120,BLACK);
				 mode5=mode5-5;
				 LCD_DrawLine_color(0,mode5+120,288,mode5+120,LIGHTBLUE);
				 key2=0;
			 }
		
		}
		 
		if(mode4==0)
		{
			once=0;
			for(y=0;y<750;)
			{
				
				for(i=0+y;i<288+y;i++)
				{
					if(t==160||ADC_Value[i]==120)
					{
						LCD_Fast_DrawPoint(t++,ADC_Value[i],BROWN);
					}
					else
						LCD_Fast_DrawPoint(t++,ADC_Value[i],BLACK);
				}
				t=0;
				i=i-1;
				y=y+10;
				for(i=0+y;i<288+y;i++)
				{
					  if(mode1==1)
						{
							ADC_Value[i] =120-((ADC_ConvertedValue[i] * 3.3 / 4095)*30+mode5);
						  LCD_Fast_DrawPoint(t++,ADC_Value[i],POINT_COLOR);
						}
				}
				t=0;	
		
			}
	  }
		if(mode4==2&&mode2==1)
		{   
			once=0;
			for(n = 160;n<1024;n++)
      {
        if((120-(ADC_ConvertedValue[n]* 3.3 / 4095)*30)>mode3&&(120-(ADC_ConvertedValue[n+1]* 3.3 / 4095)*30)<mode3)      //down
				{
					 t=0;
						adc_line_clean();
					  for(i=n-160;i<n+128;i++)
					 {
					   ADC_Value[t] =120-((ADC_ConvertedValue[i] * 3.3 / 4095)*30)+mode5;
		         LCD_Fast_DrawPoint(t++,ADC_Value[t],POINT_COLOR);
					 }
            break;
         }				 
      }
		}
		 if(mode4==2&&mode2==0)
		{   
			once=0;
			for(n = 160;n<1024;n++) 
      {
        if((120-(ADC_ConvertedValue[n]* 3.3 / 4095)*30)<mode3&&(120-(ADC_ConvertedValue[n+1]* 3.3 / 4095)*30)>mode3)      //up
				{
					 t=0;
					 adc_line_clean();
					  for(i=n-160;i<n+128;i++)
					 {
					   ADC_Value[t] =120-((ADC_ConvertedValue[i] * 3.3 / 4095)*30)+mode5;
		         LCD_Fast_DrawPoint(t++,ADC_Value[t],POINT_COLOR);
					 }
            break;
         }
       			
      }
		}
		
			if(once==0&&mode4==1&&mode2==0)
		{   
			for(n = 160;n<1024;n++)
      {
        if(ADC_ConvertedValue[n]>mode3&&ADC_ConvertedValue[n+1]<mode3)      //down
				{
					 t=0;
						adc_line_clean();
					  for(i=n-160;i<n+128;i++)
					 {
					   ADC_Value[t] =120-((ADC_ConvertedValue[i] * 3.3 / 4095)*30)+mode5;
		         LCD_Fast_DrawPoint(t++,ADC_Value[t],POINT_COLOR);
					 }
					 once=1;
            break;
         }				 
      }
		}
		 if(once==0&&mode4==1&&mode2==1)
		{   
			for(n = 160;n<1024;n++) 
      {
        if(ADC_ConvertedValue[n]<mode3&&ADC_ConvertedValue[n+1]>mode3)      //up
				{
					 t=0;
					 adc_line_clean();
					  for(i=n-160;i<n+128;i++)
					 {
					   ADC_Value[t] =120-((ADC_ConvertedValue[i] * 3.3 / 4095)*30)+mode5;
		         LCD_Fast_DrawPoint(t++,ADC_Value[t],POINT_COLOR);
					 }
					  once=1;
            break;
         }				 
      }
		}
}

void adc_line_clean()
{
   for(i=0;i<288;i++)
		{
		if(i==160||ADC_Value[i]==120)
		{
		 LCD_Fast_DrawPoint(i,ADC_Value[i],BROWN);
		}
		else
		LCD_Fast_DrawPoint(i,ADC_Value[i],BLACK);
		}
}


float Get_vpp(u16 *buf)	   //获取峰峰值
{
	u32 max_data=buf[0];
	u32 min_data=buf[0];//buf[1];
	u32 n=0;
	float Vpp=0;
	for(n = 1;n<1024;n++)
	{
		if(buf[n] > max_data)
		{
			max_data = buf[n];
		}
		if(buf[n] < min_data)
	{
			min_data = buf[n];
		}			
	} 	
	Vpp = (float)(max_data - min_data);
	Vpp = Vpp*(3.3/4096);
	return Vpp;	
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
