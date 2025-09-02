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
#include "cmsis_os.h"
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "hx710.h"     //测气压
#include <oled.h>      //oled
#include <GUI.h>       //字库
#include "esp8266_at.h"    //ESP8266 AT指令
#include "esp8266_mqtt.h"   //MQTT协议
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//int i=0;
short aacx,aacy,aacz;			//加速度传感器原始数据
short gyrox,gyroy,gyroz;		//陀螺仪原始数据
short temp;						//温度
float pitch,roll,yaw; 			//欧拉角

//	//陀螺仪方向设置
//static signed char orientation[9] = { 1, 0, 0,
//                                      0, 1, 0,
//                                      0, 0, 1};


#define WIFI_NAME "OPPOA3"
#define WIFI_PASSWD "12345678"

#define MQTT_BROKERADDRESS "a1Y76siqtl8.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define MQTT_CLIENTID "1234|securemode=3,signmethod=hmacsha1|"
#define MQTT_USARNAME "ship&a1Y76siqtl8"
#define MQTT_PASSWD "99720267EBEB0147B6428348B420B4ABB5B1C75B"

#define MQTT_SUBSCRIBE_TOPIC "/sys/a1Y76siqtl8/ship/thing/service/property/set"

uint8_t aRxBuffer; //接收中断缓冲
uint8_t cAlmStr[] = "数据溢出(大于256)\r\n";
uint8_t a[8];


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
int cppmGetTimestamp(uint16_t *time1,uint16_t *time2);   //返回中断遥控器值函数

void STM32_StatusReport(int i);       //MQTT上传数据
void jingbao(void);

extern void  OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2);   //显示数据
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
//  //u8 t=0;	
//int i=0;	
//	short aacx,aacy,aacz;			//加速度传感器原始数据
//	short gyrox,gyroy,gyroz;		//陀螺仪原始数据
//	short temp;						//温度
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
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
//		MPU_Init();						//初始化MPU6050
     OLED_Init();           //OLED初始化
//	while(mpu_dmp_init())    //MPU6050的DMP初始化
//	{
//		printf("MPU6050 Error\r\n");
//		HAL_Delay(200);
//	}
//	printf("MPU6050 OK\r\n");
	//mpu_dmp_get_data(&pitch,&roll,&yaw);
	Get_Maopi();              //气压传感器初始校正
	
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断（重要）
	while(!ESP8266_Init())      //ESP8266初始化
	{
	  
	}
	while(!ESP8266_ConnectAP(WIFI_NAME,WIFI_PASSWD))             //连接WiFi
	{
	}
  while(ESP8266_ConnectServer("TCP",MQTT_BROKERADDRESS,1883)==0)      //连接阿里云服务器 
	{
		
	}
	MQTT_Connect(MQTT_CLIENTID, MQTT_USARNAME, MQTT_PASSWD);              //连接设备
	while(MQTT_SubscribeTopic(MQTT_SUBSCRIBE_TOPIC,0,1) == 0)             //发送话题
	{
		
	}
	
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);
  __HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);
	
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);//启动。置1  CCER的输出使能位bit4
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);//CCER的bit0
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);
  
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
	
	printf("ok\r\n");
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 while(1)
	{ 
       
   	} 	
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
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
/* USER CODE BEGIN 4 */
//定时器中断，输入捕获
uint16_t 	Channel1HighTime, Channel2HighTime, Channel3HighTime, Channel4HighTime;
uint16_t 	Channel1Period, Channel2Period, Channel3Period, Channel4Period;
uint8_t  	Channel1Edge = 0, Channel2Edge = 0, Channel3Edge = 0, Channel4Edge = 0;
uint16_t 	Channel1Percent, Channel2Percent, Channel3Percent, Channel4Percent;
uint16_t	Channel1PercentTemp[3] = {0, 0, 0};
uint8_t 	Channel1TempCount = 0;
uint16_t 	Channel1RisingTimeLast=0, Channel1RisingTimeNow, Channel1FallingTime;
uint16_t 	Channel2RisingTimeLast=0, Channel2RisingTimeNow, Channel2FallingTime;
uint16_t 	Channel3RisingTimeLast=0, Channel3RisingTimeNow, Channel3FallingTime;
uint16_t 	Channel4RisingTimeLast=0, Channel4RisingTimeNow, Channel4FallingTime;

uint16_t 	Channel1pwm, Channel2pwm, Channel3pwm, Channel4pwm;

//队列进行存储数据
//extern osMessageQId capture1QueueHandle;
//extern osMessageQId capture2QueueHandle;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the __HAL_TIM_IC_CaptureCallback could be implemented in the user file
   */
	//portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if(Channel1Edge == 0)
		{
			Channel1RisingTimeNow = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);//获取上升沿时间点
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);//切换捕获极性
			HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
			Channel1Edge = 1;
			if(Channel1RisingTimeLast == 0)
			{
				Channel1Period = 0;
			}
			else
			{
				if(Channel1RisingTimeNow > Channel1RisingTimeLast)
				{
					Channel1Period = Channel1RisingTimeNow - Channel1RisingTimeLast;
				}
				else
				{
					Channel1Period = Channel1RisingTimeNow + 0xffff - Channel1RisingTimeLast + 1;
				}
			}
			Channel1RisingTimeLast = Channel1RisingTimeNow;
		}
		else if(Channel1Edge == 1)
		{
			Channel1FallingTime = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);	
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
			
			if(Channel1FallingTime < Channel1RisingTimeNow)
			{
				Channel1HighTime = Channel1FallingTime + 0xffff - Channel1RisingTimeNow + 1;
			}
			else
			{
				Channel1HighTime = Channel1FallingTime - Channel1RisingTimeNow;
			}
			if(Channel1Period != 0)
			{
				Channel1Percent = (uint8_t)(((float)Channel1HighTime / Channel1Period) * 1000);
//				printf("%d",Channel1Percent);
				Channel1pwm=(19.98*Channel1Percent-979.02);
				//Channel1pwm=39.96*Channel1Percent-2957.04;
				//xQueueSendFromISR(capture1QueueHandle, &Channel1Percent, &xHigherPriorityTaskWoken);
			}
			Channel1Edge = 0;
		}
	}
	else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		if(Channel2Edge == 0)
		{
			Channel2RisingTimeNow = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2);
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
			HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
			Channel2Edge = 1;
			if(Channel2RisingTimeLast == 0)
			{
				Channel2Period = 0;
			}
			else
			{
				if(Channel2RisingTimeNow > Channel2RisingTimeLast)
				{
					Channel2Period = Channel2RisingTimeNow - Channel2RisingTimeLast;
				}
				else
				{
					Channel2Period = Channel2RisingTimeNow + 0xffff - Channel2RisingTimeLast + 1;
				}
			}
			Channel2RisingTimeLast = Channel2RisingTimeNow;
		}
		else if(Channel2Edge == 1)
		{
			Channel2FallingTime = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2);	
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
			HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
			
			if(Channel2FallingTime < Channel2RisingTimeNow)
			{
				Channel2HighTime = Channel2FallingTime + 0xffff - Channel2RisingTimeNow + 1;
			}
			else
			{
				Channel2HighTime = Channel2FallingTime - Channel2RisingTimeNow;
			}
			if(Channel2Period != 0)
			{
				Channel2Percent = (uint8_t)(((float)Channel2HighTime / Channel2Period) * 1000);
				printf("%d",Channel2Percent);
				//xQueueSendFromISR(capture2QueueHandle, &Channel2Percent, &xHigherPriorityTaskWoken);
				Channel2pwm=(39.96*Channel2Percent-2957.04);
				//printf("Channel1 = %d	 Channel1pwm = %d\r\n", Channel1Percent,Channel1pwm);
				//printf("Channel2 = %d	 Channel2pwm = %d\r\n", Channel2Percent,Channel2pwm);
        if(Channel2Percent<74)   //后退
				{
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);
	        HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);
	        HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET);
	        HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
//					    if(Channel1Percent<74)
//							{
//							__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, Channel1pwm);
//      				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 999-Channel2pwm);
//							}
//							else if(Channel1Percent==74) //后退
//							{
							__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 999-Channel2pwm);
      				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 999-Channel2pwm);
							printf("%d 后退\r\n",Channel2Percent);
//							}
//							else
//							{
//							__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 999-Channel2pwm);
//      				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, Channel1pwm);
//							}
				}
				else if(Channel2Percent==74)   //停止
				{
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);
	      HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);
	      HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET);
	      HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
      	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
				printf("停止\r\n");
				}
				else  
				{
					     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);
	             HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);
	             HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);
	             HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);
							if(Channel1Percent<74)  //左转
							{
							__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 999-Channel2pwm);
      				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 999-Channel1pwm);
							printf("左转 %d\r\n",Channel1pwm);
							}
							else if(Channel1Percent==74)  //前进
							{
								//mpu_dmp_get_data(&pitch,&roll,&yaw);    //获取MPU6050角度算差
								//ess=yaw;
								
							__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,999-Channel2pwm);
      				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 999-Channel2pwm);
							printf("前进 %.2f\r\n",roll);   //yaw 有偏移量积累不准 pitch 响应慢
							}
							else   //右转
							{
							__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,Channel1pwm);
      				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 999-Channel2pwm);
							printf("右转 %d\r\n",1000-Channel1pwm);
							}
				}

			}
			Channel2Edge = 0;
		}
	}
	else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		if(Channel3Edge == 0)
		{
			Channel3RisingTimeNow = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_3);
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
			HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
			Channel3Edge = 1;
			if(Channel3RisingTimeLast == 0)
			{
				Channel3Period = 0;
			}
			else
			{
				if(Channel3RisingTimeNow > Channel3RisingTimeLast)
				{
					Channel3Period = Channel3RisingTimeNow - Channel3RisingTimeLast;
				}
				else
				{
					Channel3Period = Channel3RisingTimeNow + 0xffff - Channel3RisingTimeLast + 1;
				}
			}
			Channel3RisingTimeLast = Channel3RisingTimeNow;
		}
		else if(Channel3Edge == 1)
		{
			Channel3FallingTime = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_3);	
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
			HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
			
			if(Channel3FallingTime < Channel3RisingTimeNow)
			{
				Channel3HighTime = Channel3FallingTime + 0xffff - Channel3RisingTimeNow + 1;
			}
			else
			{
				Channel3HighTime = Channel3FallingTime - Channel3RisingTimeNow;
			}
			if(Channel3Period != 0)
			{
				Channel3Percent = (uint8_t)(((float)Channel3HighTime / Channel3Period) * 1000);
				//printf("Channel3 = %d	", Channel3Percent);
			}
			Channel3Edge = 0;
		}
	}
	else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{
		if(Channel4Edge == 0)
		{
			Channel4RisingTimeNow = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_4);
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
			HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);
			Channel4Edge = 1;
			if(Channel4RisingTimeLast == 0)
			{
				Channel4Period = 0;
			}
			else
			{
				if(Channel4RisingTimeNow > Channel4RisingTimeLast)
				{
					Channel4Period = Channel4RisingTimeNow - Channel4RisingTimeLast;
				}
				else
				{
					Channel4Period = Channel4RisingTimeNow + 0xffff - Channel4RisingTimeLast + 1;
				}
			}
			Channel4RisingTimeLast = Channel4RisingTimeNow;
		}
		else if(Channel4Edge == 1)
		{
			Channel4FallingTime = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_4);	
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
			HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);
			
			if(Channel4FallingTime < Channel4RisingTimeNow)
			{
				Channel4HighTime = Channel4FallingTime + 0xffff - Channel4RisingTimeNow + 1;
			}
			else
			{
				Channel4HighTime = Channel4FallingTime - Channel4RisingTimeNow;
			}
			if(Channel4Period != 0)
			{
				Channel4Percent = (uint8_t)(((float)Channel4HighTime / Channel4Period) * 1000);
				//printf("Channel4 = %d\r\n", Channel4Percent);
			}
			Channel4Edge = 0;
		}
	}
}

//传回队列
int cppmGetTimestamp(uint16_t *time1,uint16_t *time2)
{

	//return xQueueReceive(capture1QueueHandle, time1, 20)&&xQueueReceive(capture2QueueHandle, time2, 20);
}


//USART2 ES8266驱动串口接收中断处理函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)	// 判断是由哪个串口触发的中断
	{
		//将接收到的数据放入接收usart1接收数组
		usart2_rxbuf[usart2_rxcounter] = usart2_rxone[0];
		usart2_rxcounter++;	//接收数量＋1
		
		//重新使能串口1接收中断
		HAL_UART_Receive_IT(&huart2,usart2_rxone,1);		
	}
}

////上传数据
//void STM32_StatusReport(int i)
//{
//	//获取温湿度信息
//	//uint8_t temperature=1;
//	
//	sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"Atmosphere\":%d},\"version\":\"1.0.0\"}",i);
//	

//	MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
//}

//void jingbao(void)
//{
//	sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"Atmosphere\":%d},\"version\":\"1.0.0\"}",0);
//	MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
//  sprintf(mqtt_message,"{\"method\":\"thing.event.HighwaterEvent.post\",\"id\":\"0\",\"params\":{\"Waterlevel\":%d},\"version\":\"1.0.0\"}",1);
//  MQTT_PublishData(MQTT_EVENT_YOPIC1,mqtt_message,0);
//	sprintf(mqtt_message,"{\"method\":\"thing.event.Highmuddy.post\",\"id\":\"0\",\"params\":{\"muddy\":%d},\"version\":\"1.0.0\"}",1);
//  MQTT_PublishData(MQTT_EVENT_YOPIC2,mqtt_message,0);
//}

/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
