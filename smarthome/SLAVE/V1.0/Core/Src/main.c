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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t RxBuffer1; //接收中断缓冲
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
  uint8_t len;	
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
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&RxBuffer1, 1);   //再开启接收中断（重要）
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);//***定时器初始化
	
	HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,100); //相当于一个周期内（20ms）有0.5ms高脉冲
	
  //printf("ok\r\n");//插入换行
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
				
    if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			//HAL_UART_Transmit(&huart1,(uint8_t*)USART_RX_BUF,len,1000);	//发送接收到的数据
			if(strstr((char*)USART_RX_BUF,"0")!=NULL)     //关灯
			{
			  HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);
				//printf("0\r\n");
			}
			else if(strstr((char*)USART_RX_BUF,"1")!=NULL)    //开灯
			{
			  HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_SET);
				//printf("1\r\n");
			}
			 else if(strstr((char*)USART_RX_BUF,"2")!=NULL)   //开窗帘
			{
			  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,50); //相当于一个周期内（20ms）有0.5ms高脉冲
				//printf("2\r\n");
			}
			else if(strstr((char*)USART_RX_BUF,"3")!=NULL)    //关窗帘
			{
			  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,100); //相当于一个周期内（20ms）有0.5ms高脉冲
				//printf("3\r\n");
			}
			while(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TC)!=SET);		//等待发送结束
			USART_RX_STA=0;
		}
//		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);
//		HAL_Delay(5000);
//		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_SET);
//		HAL_Delay(5000);
//		  int i=1000; //循环时间间隔1s
//      
//      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,50); //相当于一个周期内（20ms）有0.5ms高脉冲
//      HAL_Delay(i);
//      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,250); //相当于一个周期内（20ms）有2.5ms高脉冲
//      HAL_Delay(i);
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
}

/* USER CODE BEGIN 4 */

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
