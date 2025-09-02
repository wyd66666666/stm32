/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"

#include "esp8266_at.h"    //ESP8266 AT指令
#include "esp8266_mqtt.h"   //MQTT协议

#include "dht11.h"

#include "oled.h"

#include "lsens.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WIFI_NAME "OPPOA3"
#define WIFI_PASSWD "12345678"

#define MQTT_BROKERADDRESS "a1Y76siqtl8.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define MQTT_CLIENTID "1234|securemode=3,signmethod=hmacsha1|"
#define MQTT_USARNAME "ship&a1Y76siqtl8"
#define MQTT_PASSWD "99720267EBEB0147B6428348B420B4ABB5B1C75B"

#define MQTT_SUBSCRIBE_TOPIC "/sys/a1Y76siqtl8/ship/thing/service/property/set"

#define	MQTT_PUBLISH_TOPIC "/sys/a1Y76siqtl8/ship/thing/event/property/post"
#define MQTT_EVENT_YOPIC1 "/sys/a1Y76siqtl8/ship/thing/event/HighwaterEvent/post"
#define MQTT_EVENT_YOPIC2 "/sys/a1Y76siqtl8/ship/thing/event/Highmuddy/post"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//串口1
uint8_t RxBuffer1; //接收中断缓冲
uint8_t data_buff[1];
//串口2
uint8_t aRxBuffer; //接收中断缓冲
uint8_t cAlmStr[] = "数据溢出(大于256)\r\n";
uint8_t a[8];

char mqtt_message[300];	//MQTT的上报消息缓存

char lsens[10];
char temp[20];
char humi[20];
char win[30];
char light[10];

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
extern uint8_t FindStr(char* dest,char* src,uint16_t retry_nms);
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
  uint16_t temperature;
  uint16_t humidity;
	uint8_t adcx;
	
	int i=0;
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
  MX_TIM6_Init();
  MX_I2C1_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&RxBuffer1, 1);   //再开启接收中断（重要）
  HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断（重要）
	
	printf("ok\r\n");

  oled_init();
	oled_show_string(24,0,"www",2);
	oled_show_string(0,0,"www",1);

  Lsens_Init(); 						//初始化光敏传感器
  
	while(DHT11_Init()){
	printf("DHT11 Checked failed!!!\r\n");
	  HAL_Delay(500);
   }
  printf("DHT11 Checked Sucess!!!\r\n");
	 
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
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(i%3000000==0)
		{
		//HAL_Delay(30000);
		DHT11_Read_Data(&temperature,&humidity);
//		printf("temperature:%d.%d\r\n",temperature>>8,temperature&0xff);
//	  printf("humidity:%d.%d\r\n",humidity>>8,humidity&0xff);
		adcx=Lsens_Get_Val();
//		printf("lsens:%d\r\n",adcx);
		oled_full(0x00);
		sprintf(lsens,"lsens:%d",adcx);
		sprintf(temp,"temp:%d.%d",temperature>>8,temperature&0xff);
		sprintf(humi,"humi:%d.%d",humidity>>8,humidity&0xff);
		sprintf(win,"win:%d light:%d",1,0);
		
		oled_show_string(3,0,lsens,2);//显示ADC的值 
		oled_show_string(3,2,temp,2);
		oled_show_string(3,4,humi,2);
		oled_show_string(3,6,win,2);
		
		
		sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"Atmosphere\":%d},\"version\":\"1.0.0\"}",20);
    MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
		sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"Atmosphere\":%d},\"version\":\"1.0.0\"}",20);
    MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
	  HAL_GPIO_TogglePin(D1_GPIO_Port, D1_Pin);
		printf("test\r\n");
		i=1;
	 }
//	 if(strstr((char*)usart2_rxbuf,"\"LightSwitch\":1")!=NULL)
//	 {
//		   HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin,GPIO_PIN_RESET);
//  		 printf("1\r\n");
//			 memset(usart2_rxbuf,0,sizeof(usart2_rxbuf)); //清空接收缓冲
//	 }
//	if(strstr((char*)usart2_rxbuf,"\"LightSwitch\":0")!=NULL)
//	 {	
//		   HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin,GPIO_PIN_SET);
//  		 printf("2\r\n");
//			 memset(usart2_rxbuf,0,sizeof(usart2_rxbuf)); //清空接收缓冲
//	 }
	 if(strstr((char*)usart2_rxbuf,"LightOff")!=NULL)
	 {
		   HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin,GPIO_PIN_SET);
  		 printf("0\r\n");
			 memset(usart2_rxbuf,0,sizeof(usart2_rxbuf)); //清空接收缓冲
	 }
	 if(strstr((char*)usart2_rxbuf,"LightOn")!=NULL)
	 {	
		   HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin,GPIO_PIN_RESET);
  		 printf("1\r\n");
			 memset(usart2_rxbuf,0,sizeof(usart2_rxbuf)); //清空接收缓冲
	 }
	 i++;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//USART2 ES8266驱动串口接收中断处理函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
	  HAL_GPIO_TogglePin(D0_GPIO_Port, D0_Pin);
		printf("test\r\n");
		HAL_UART_Receive_IT(&huart1,data_buff,1);	
	}
	if(huart->Instance == USART2)	// 判断是由哪个串口触发的中断
	{
		//将接收到的数据放入接收usart1接收数组
		usart2_rxbuf[usart2_rxcounter] = usart2_rxone[0];
		usart2_rxcounter++;	//接收数量＋1
		//重新使能串口1接收中断

		HAL_UART_Receive_IT(&huart2,usart2_rxone,1);		
	}
}

//void STM32DHT11_StatusReport(int i)
//{
//	//获取温湿度信息
//	//uint8_t temperature=1;
//	
//	sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"Elevator\":%d},\"version\":\"1.0.0\"}",i);
//	

//	MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
//}

//处理MQTT下发的消息
int deal_MQTT_message(uint8_t* buf,uint16_t len)
{
	if(FindStr((char*) usart2_rxbuf,"nightLight",200)!=0)
	{ 
		if(FindStr((char*)usart2_rxbuf,":1",200)!=0)
		 {
		 		HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin,GPIO_PIN_RESET);
        return 1;
		 }
		 else
		 {
		 		HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin,GPIO_PIN_SET);
		 }
	 }
//   if(FindStr((char*) usart2_rxbuf,"1",200)!=0)
//	 {
//	   HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin,GPIO_PIN_RESET);
//		 return 1;
//	 }
//   if(FindStr((char*) usart2_rxbuf,"0",200)!=0)
//	 {
//	   HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin,GPIO_PIN_SET);
//		 return 2;
//	 }
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
