/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "tim.h"
#include "adc.h"
#include "esp8266_mqtt.h"   //MQTT协议
#include <GUI.h>       //字库
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void HAL_TIM_back(TIM_HandleTypeDef *htim);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern int cppmGetTimestamp(uint16_t *time1,uint16_t *time2);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int qiya=0;          //气压值
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId MPU6050TaskHandle;
osThreadId TIMtaskHandle;
osThreadId DISPLAYHandle;
osThreadId MQTTHandle;
osMessageQId capture1QueueHandle;
osMessageQId capture2QueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void mpu6050Task(void const * argument);
void timtask(void const * argument);
void display(void const * argument);
void mqtt(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of capture1Queue */
  osMessageQDef(capture1Queue, 16, uint16_t);
  capture1QueueHandle = osMessageCreate(osMessageQ(capture1Queue), NULL);

  /* definition and creation of capture2Queue */
  osMessageQDef(capture2Queue, 16, uint16_t);
  capture2QueueHandle = osMessageCreate(osMessageQ(capture2Queue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of MPU6050Task */
  osThreadDef(MPU6050Task, mpu6050Task, osPriorityHigh, 0, 128);
  MPU6050TaskHandle = osThreadCreate(osThread(MPU6050Task), NULL);

  /* definition and creation of TIMtask */
  osThreadDef(TIMtask, timtask, osPriorityAboveNormal, 0, 128);
  TIMtaskHandle = osThreadCreate(osThread(TIMtask), NULL);

  /* definition and creation of DISPLAY */
  osThreadDef(DISPLAY, display, osPriorityNormal, 0, 128);
  DISPLAYHandle = osThreadCreate(osThread(DISPLAY), NULL);

  /* definition and creation of MQTT */
  osThreadDef(MQTT, mqtt, osPriorityNormal, 0, 128);
  MQTTHandle = osThreadCreate(osThread(MQTT), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_mpu6050Task */
int t=0;
extern short aacx,aacy,aacz;			//加速度传感器原始数据
extern short gyrox,gyroy,gyroz;		//陀螺仪原始数据
extern short temp;						//温度
extern float pitch,roll,yaw; 			//欧拉角
/**
* @brief Function implementing the MPU6050Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_mpu6050Task */
void mpu6050Task(void const * argument)
{
  /* USER CODE BEGIN mpu6050Task */
  /* Infinite loop */
  for(;;)
  {
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{
			temp=MPU_Get_Temperature();	//得到温度值
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
				if((t%10)==0)
				{ 		
					//temp值
//					if(temp<0)
//					{
//						temp=-temp;		//转为正数
//						printf(" Temp:  -%d.%dC\r\n",temp/100,temp%10);
//					}else
//						printf(" Temp:  %d.%dC\r\n",temp/100,temp%10);
//						
//					//pitch值
//					temp=pitch*10;			
//					if(temp<0)
//					{
//						temp=-temp;		//转为正数
//						printf(" Pitch: -%d.%dC\r\n",temp/10,temp%10);
//					}else
//						printf(" Pitch:  %d.%dC\r\n",temp/10,temp%10);
					
//					//roll值
//					temp=roll*10;		
//					if(temp<0)
//					{
//						temp=-temp;		//转为正数
//						printf(" Roll:  -%d.%dC\r\n",temp/10,temp%10);
//					}else
//						printf(" Roll:  %d.%dC\r\n",temp/10,temp%10);
					
					//yaw值
					temp=yaw*10;
					if(temp<0)
					{
						temp=-temp;		//转为正数
						printf(" Yaw:  -%d.%dC\r\n",temp/10,temp%10);
					}else
						printf(" Yaw:   %d.%dC\r\n",temp/10,temp%10);
				
//					printf("\r\n");
					t=0;
				}
      t++;
			}
    osDelay(1);
  }
  /* USER CODE END mpu6050Task */
}

/* USER CODE BEGIN Header_timtask */
uint16_t rcData1,rcData2;
/**
* @brief Function implementing the TIMtask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_timtask */
void timtask(void const * argument)
{
  /* USER CODE BEGIN timtask */
  /* Infinite loop */
		uint16_t can1,can2;
  for(;;)
  {
    if(cppmGetTimestamp(&can1,&can2) == pdTRUE)
		{
			rcData1=can1;
			printf("rcData1: %d ",rcData1);
			rcData2=can2;
			printf("rcData2: %d\r\n",rcData2);
	  }
    osDelay(1);
  }
  /* USER CODE END timtask */
}

/* USER CODE BEGIN Header_display */
/**
* @brief Function implementing the DISPLAY thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_display */
void display(void const * argument)
{
  /* USER CODE BEGIN display */
  /* Infinite loop */
  for(;;)
  {
		Show_Str(0, 2, "Atm:  ", 16, 0);
	  OLED_ShowNum(76,2,qiya,4,16);
    osDelay(100);
  }
  /* USER CODE END display */
}

/* USER CODE BEGIN Header_mqtt */
int i=0,j=0;
int m=0,n=0;
uint16_t adcBuf[2]={0,0};    //ad的值
float adcvcc[2];
char mqtt_message[300];	//MQTT的上报消息缓存

extern unsigned int Get_Weight(void);

#define	MQTT_PUBLISH_TOPIC "/sys/a1Y76siqtl8/ship/thing/event/property/post"
#define MQTT_EVENT_YOPIC1 "/sys/a1Y76siqtl8/ship/thing/event/HighwaterEvent/post"
#define MQTT_EVENT_YOPIC2 "/sys/a1Y76siqtl8/ship/thing/event/Highmuddy/post"
/**
* @brief Function implementing the MQTT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_mqtt */
void mqtt(void const * argument)
{
  /* USER CODE BEGIN mqtt */
  /* Infinite loop */
  for(;;)
  {
		for(i=0;i<2;i++)
  {
		HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1,0xffff);
    adcBuf[i]=HAL_ADC_GetValue(&hadc1);
		adcvcc[i]=adcBuf[i]*3.3f/4096;
    printf("------ch:%d--%.2f-------\r\n",i,adcvcc[i]);
   }
	 HAL_ADC_Stop(&hadc1);
	 qiya=Get_Weight();
	 printf("------qiya %d  \r\n",qiya);
	 taskENTER_CRITICAL();
	 if(adcvcc[0]<2&&m==0)
	 {
	    sprintf(mqtt_message,"{\"method\":\"thing.event.HighwaterEvent.post\",\"id\":\"0\",\"params\":{\"Waterlevel\":%d},\"version\":\"1.0.0\"}",1);
      MQTT_PublishData(MQTT_EVENT_YOPIC1,mqtt_message,0);
		  m=1;
	 }
	 else if(adcvcc[0]>=2)
	 {
	    m=0;
	 }
	 if(adcvcc[1]<2&&n==0)
	 {
	    sprintf(mqtt_message,"{\"method\":\"thing.event.Highmuddy.post\",\"id\":\"0\",\"params\":{\"muddy\":%d},\"version\":\"1.0.0\"}",1);
      MQTT_PublishData(MQTT_EVENT_YOPIC2,mqtt_message,0);
		  n=1;
	 }
	else if(adcvcc[1]>=2)
	 {
	    n=0;
	 }
	 if(j%10==0)
	 {
	   sprintf(mqtt_message,"{\"method\":\"thing.event.property.post\",\"id\":\"0\",\"params\":{\"Atmosphere\":%d},\"version\":\"1.0.0\"}",qiya);
    	MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
	 }
	 j++;
	 taskEXIT_CRITICAL();
    osDelay(100);
  }
  /* USER CODE END mqtt */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
