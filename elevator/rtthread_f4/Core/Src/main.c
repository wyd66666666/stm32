/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <rtthread.h>
#include <oled.h>
#include <GUI.h>
#include <mlx90614.h>
#include "esp8266_at.h"
#include "esp8266_mqtt.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
static rt_thread_t led1_thread = RT_NULL;
static rt_thread_t led2_thread = RT_NULL;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//此处根据自己的wifi作调整
#define WIFI_NAME "OPPOA3"
#define WIFI_PASSWD "12345678"

#define MQTT_BROKERADDRESS "a1RXJrGJrIB.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define MQTT_CLIENTID "1234|securemode=3,signmethod=hmacsha1|"
#define MQTT_USARNAME "test&a1RXJrGJrIB"
#define MQTT_PASSWD "B449315815976B38ACCE28CF625472708F94C43D"
#define	MQTT_PUBLISH_TOPIC "/sys/a1RXJrGJrIB/test/thing/event/property/post"
#define MQTT_SUBSCRIBE_TOPIC "/sys/a1RXJrGJrIB/test/thing/service/property/set"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t aRxBuffer; //接收中断缓冲
//uint8_t Uart2_Rx_Cnt = 0; //接收缓冲计数
uint8_t cAlmStr[] = "数据溢出(大于256)\r\n";
uint8_t a[8];
char mqtt_message[300];	//MQTT的上报消息缓存
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void STM32DHT11_StatusReport(int i);
int deal_MQTT_message(uint8_t* buf,uint16_t len);
extern uint8_t FindStr(char* dest,char* src,uint16_t retry_nms);

void User_Modification(uint8_t dat);
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
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断
	OLED_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	


  while(!ESP8266_Init()) 
	{
	  
	}
	while(!ESP8266_ConnectAP(WIFI_NAME,WIFI_PASSWD))
	{
	}
  while(ESP8266_ConnectServer("TCP",MQTT_BROKERADDRESS,1883)==0)
	{
		
	}
	while(MQTT_Connect(MQTT_CLIENTID, MQTT_USARNAME, MQTT_PASSWD)==0)
	{
		
	}
	while(MQTT_SubscribeTopic(MQTT_SUBSCRIBE_TOPIC,0,1) == 0)
	{
		
	}
	while(1)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
		rt_thread_delay(1000);
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
		//rt_thread_delay(1000);
		int i=1,j=0,s=0,m=0,n=0;
		float temp = 0;
		if(m==0)
		{
			STM32DHT11_StatusReport(i);
		  sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"human\":%d},\"version\":\"1.0.0\"}",s);
	    MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
		  Show_Str(0, 2, "电梯在   楼", 16, 0);
	    OLED_ShowNum(56,2,i,1,16);
	    Show_Str(0, 5, "目前有   人", 16, 0);
	    OLED_ShowNum(56,5,s,1,16);
			m++;
		}
	  if(Uart2_Rx_Cnt)
	  {
		  j=deal_MQTT_message(usart2_rxbuf,Uart2_Rx_Cnt);
		}
			
		if(j==2)
		{
			while(1)
			{
//			  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
//		    rt_thread_delay(1000);
//		    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
//		    rt_thread_delay(1000);
				STM32DHT11_StatusReport(i);
				//temp= MLX90614_ReadTemp(0x00,0x07);
				temp=36.6;
				sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"Temperature\":%f},\"version\":\"1.0.0\"}",temp);
	      MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
				rt_thread_delay(1000);
				s++;
				sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"human\":%d},\"version\":\"1.0.0\"}",s);
	      MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
				Show_Str(0, 2, "电梯在   楼", 16, 0);
	      OLED_ShowNum(56,2,i,1,16);
	      Show_Str(0, 5, "目前有   人", 16, 0);
	      OLED_ShowNum(56,5,s,1,16);
				j++;
				if(j==3)
				{
					for(i=1;i<=9;i++)
				  {
						Show_Str(0, 2, "电梯在   楼", 16, 0);
	          OLED_ShowNum(56,2,i,1,16);
	          Show_Str(0, 5, "目前有   人", 16, 0);
	          OLED_ShowNum(56,5,s,1,16);
					  STM32DHT11_StatusReport(i);
						 rt_thread_delay(1000);
					} 
					i=i-1;
					rt_thread_delay(1000);
					s=s-1;
	        Show_Str(0, 5, "目前有   人", 16, 0);
	        OLED_ShowNum(56,5,s,1,16);
					sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"human\":%d},\"version\":\"1.0.0\"}",s);
	        MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
					while(1)
					{
//					  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
//		        rt_thread_delay(1000);
//		        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
//		        rt_thread_delay(1000);
						n++;
						STM32DHT11_StatusReport(i);
						rt_thread_delay(1000);
						if(n%5==0)
						{
						  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
						}
					}
				}
			}
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



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */

  UNUSED(huart);
	if(Uart2_Rx_Cnt >= 511)  //溢出判断
 {
		Uart2_Rx_Cnt = 0;
		memset(usart2_rxbuf,0x00,sizeof(usart2_rxbuf));
	}
	else
	{
		usart2_rxbuf[Uart2_Rx_Cnt++] = aRxBuffer;   //接收数据转存
		if((usart2_rxbuf[Uart2_Rx_Cnt-1] == 0x0A)&&(usart2_rxbuf[Uart2_Rx_Cnt-2] == 0x0D)) //判断结束位
		{
			Uart2_Rx_Cnt = 0;
		}
	}
	
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断
	

}

void STM32DHT11_StatusReport(int i)
{
	//获取温湿度信息
	//uint8_t temperature=1;
	
	sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"Elevator\":%d},\"version\":\"1.0.0\"}",i);
	

	MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
}

//处理MQTT下发的消息
int deal_MQTT_message(uint8_t* buf,uint16_t len)
{
//	if(FindStr((char*) usart2_rxbuf,"nightLight",200)!=0)
//	{ 
//		if(FindStr((char*)usart2_rxbuf,":1",200)!=0)
//		 {
//		 		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
//        return 1;
//		 }
//		 else
//		 {
//		 		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
//		 }
//	 }
   if(FindStr((char*) usart2_rxbuf,"begin",200)!=0)
	 {
	   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
		 return 1;
	 }
   if(FindStr((char*) usart2_rxbuf,"end",200)!=0)
	 {
	   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
		 return 2;
	 }
   memset(usart2_rxbuf,0,sizeof(usart2_rxbuf)); //清空接收缓冲	 
	 return 0; 

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
