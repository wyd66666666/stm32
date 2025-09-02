#include "esp8266_at.h"

//usart2发送和接收数组
uint8_t usart2_txbuf[256];
uint8_t usart2_rxbuf[512];
uint8_t usart2_rxone[1];
uint8_t usart2_rxcounter;

uint8_t Uart2_Rx_Cnt ;

//串口1发送一个字节
static void USART2_SendOneByte(uint8_t val)
{
	((UART_HandleTypeDef *)&huart2)->Instance->DR = ((uint16_t)val & (uint16_t)0x01FF);
	while((((UART_HandleTypeDef *)&huart2)->Instance->SR&0X40)==0);//等待发送完成
}


//向ESP8266发送定长数据
void ESP8266_ATSendBuf(uint8_t* buf,uint16_t len)
{
	memset(usart2_rxbuf,0, 256);
	//OLED_Clear();
	//每次发送前将接收串口接收总数置0,为了接收
	//usart2_rxcounter = 0;	
	Uart2_Rx_Cnt = 0;
	//定长发送
	//usart2_txbuf[len] =0x13;
	HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,0xFFFF);
	//rt_kprintf(buf);
	/////////////////
//	Show_Str(0, 0, usart2_txbuf, 16, 0);
	rt_thread_delay(1000);
//	Show_Str(0, 4, usart2_rxbuf, 16, 0);
	//////////
	rt_thread_delay(2000);
	//rt_kprintf(buf);
}

//向ESP8266发送字符串
void ESP8266_ATSendString(char* str)
{
  memset(usart2_rxbuf,0, 256);
	//OLED_Clear();
	//每次发送前将接收串口接收总数置0,为了接收
	Uart2_Rx_Cnt = 0;
	//发送方法1
	rt_kprintf(str);
	strcpy(usart2_txbuf,str);
	////
	//Show_Str(0, 0, usart2_txbuf, 16, 0);
	//rt_thread_delay(1000);
	//Show_Str(0, 4, usart2_rxbuf, 16, 0);
	//rt_thread_delay(2000);
	////
}

//退出透传
void ESP8266_ExitUnvarnishedTrans(void)
{
	ESP8266_ATSendString("+++\r");rt_thread_delay(50);
	ESP8266_ATSendString("+++\r");rt_thread_delay(50);	
}

//查找字符串中是否包含另一个字符串
uint8_t FindStr(char dest[],char src[],uint16_t retry_nms)
{
	retry_nms/=10;                   //超时时间

  rt_thread_delay(retry_nms);	
  if(strstr(dest,src)!=NULL)
	{
	  return 1; 
	}
	return 0; 
}

/**
 * 功能：检查ESP8266是否正常
 * 参数：None
 * 返回值：ESP8266返回状态
 *        非0 ESP8266正常
 *        0 ESP8266有问题  
 */
uint8_t ESP8266_Check(void)
{
	uint8_t check_cnt=5;
	while(check_cnt--)
	{
		memset(usart2_rxbuf,0,sizeof(usart2_rxbuf)); 	 //清空接收缓冲
		ESP8266_ATSendString("AT\n");     		 			//发送AT握手指令	
		if(FindStr((char*)usart2_rxbuf,"OK",200) != 0)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * 功能：初始化ESP8266
 * 参数：None
 * 返回值：初始化结果，非0为初始化成功,0为失败
 */
uint8_t ESP8266_Init(void)
{
	//清空发送和接收数组
	memset(usart2_txbuf,0,sizeof(usart2_txbuf));
	memset(usart2_rxbuf,0,sizeof(usart2_rxbuf));
	
	ESP8266_ExitUnvarnishedTrans();		//退出透传
	rt_thread_delay(500);
	ESP8266_ATSendString("AT+RST\n");
	rt_thread_delay(800);
	if(ESP8266_Check()==0)              //使用AT指令检查ESP8266是否存在
	{
		return 0;
	}
	
 	memset(usart2_rxbuf,0,sizeof(usart2_rxbuf));    //清空接收缓冲
	return 1;                         //设置成功
}

/**
 * 功能：恢复出厂设置
 * 参数：None
 * 返回值：None
 * 说明:此时ESP8266中的用户设置将全部丢失回复成出厂状态
 */
void ESP8266_Restore(void)
{
	ESP8266_ExitUnvarnishedTrans();          	//退出透传
  rt_thread_delay(500);
	ESP8266_ATSendString("AT+RESTORE\n");		//恢复出厂 	
}

/**
 * 功能：连接热点
 * 参数：
 *         ssid:热点名
 *         pwd:热点密码
 * 返回值：
 *         连接结果,非0连接成功,0连接失败
 * 说明： 
 *         失败的原因有以下几种(UART通信和ESP8266正常情况下)
 *         1. WIFI名和密码不正确
 *         2. 路由器连接设备太多,未能给ESP8266分配IP
 */
uint8_t ESP8266_ConnectAP(char* ssid,char* pswd)
{
	uint8_t cnt=5;
	while(cnt--)
	{
		memset(usart2_rxbuf,0,sizeof(usart2_rxbuf));     
		ESP8266_ATSendString("AT+CWMODE_CUR=1\n");              //设置为STATION模式	
		if(FindStr((char*)usart2_rxbuf,"OK",200) != 0)
		{
			break;
		}             		
	}
	if(cnt == 0)
		return 0;

	cnt=2;
	while(cnt--)
	{                    
		memset(usart2_txbuf,0,sizeof(usart2_txbuf));//清空发送缓冲
		memset(usart2_rxbuf,0,sizeof(usart2_rxbuf));//清空接收缓冲
		sprintf((char*)usart2_txbuf,"AT+CWJAP_CUR=\"%s\",\"%s\"\n",ssid,pswd);//连接目标AP
		ESP8266_ATSendString((char*)usart2_txbuf);	
		if(FindStr((char*)usart2_rxbuf,"CONNECTED",8000)!=0)                      //连接成功且分配到IP
		{
			return 1;
		}
	}
	return 0;
}

//开启透传模式
static uint8_t ESP8266_OpenTransmission(void)
{
	//设置透传模式
	uint8_t cnt=2;
	while(cnt--)
	{
		memset(usart2_rxbuf,0,sizeof(usart2_rxbuf));    
		ESP8266_ATSendString("AT+CIPMODE=1\n");  
		if(FindStr((char*)usart2_rxbuf,"OK",200)!=0)
		{	
			return 1;
		}
	}
	return 0;
}

/**
 * 功能：使用指定协议(TCP/UDP)连接到服务器
 * 参数：
 *         mode:协议类型 "TCP","UDP"
 *         ip:目标服务器IP
 *         port:目标是服务器端口号
 * 返回值：
 *         连接结果,非0连接成功,0连接失败
 * 说明： 
 *         失败的原因有以下几种(UART通信和ESP8266正常情况下)
 *         1. 远程服务器IP和端口号有误
 *         2. 未连接AP
 *         3. 服务器端禁止添加(一般不会发生)
 */
uint8_t ESP8266_ConnectServer(char* mode,char* ip,uint16_t port)
{
	uint8_t cnt;
   
	ESP8266_ExitUnvarnishedTrans();                   //多次连接需退出透传
	rt_thread_delay(500);

	//连接服务器
	cnt=2;
	while(cnt--)
	{
		memset(usart2_txbuf,0,sizeof(usart2_txbuf));//清空发送缓冲
		memset(usart2_rxbuf,0,sizeof(usart2_rxbuf));//清空接收缓冲   
		sprintf((char*)usart2_txbuf,"AT+CIPSTART=\"%s\",\"%s\",%d\n",mode,ip,port);
		ESP8266_ATSendString((char*)usart2_txbuf);
		if(FindStr((char*)usart2_rxbuf,"OK",8000) !=0 )
		{
			break;
		}
	}
	if(cnt == 0) 
		return 0;
	
	//设置透传模式
	if(ESP8266_OpenTransmission()==0) return 0;
	
	//开启发送状态
	cnt=2;
	while(cnt--)
	{
		memset(usart2_rxbuf,0,sizeof(usart2_rxbuf)); //清空接收缓冲   
		ESP8266_ATSendString("AT+CIPSEND\n");//开始处于透传发送状态
		if(FindStr((char*)usart2_rxbuf,">",200)!=0)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * 功能：主动和服务器断开连接
 * 参数：None
 * 返回值：
 *         连接结果,非0断开成功,0断开失败
 */
uint8_t DisconnectServer(void)
{
	uint8_t cnt;
	
	ESP8266_ExitUnvarnishedTrans();	//退出透传
	rt_thread_delay(500);
	
	while(cnt--)
	{
		memset(usart2_rxbuf,0,sizeof(usart2_rxbuf)); //清空接收缓冲   
		ESP8266_ATSendString("AT+CIPCLOSE\n");//关闭链接

		if(FindStr((char*)usart2_rxbuf,"CLOSED",200)!=0)//操作成功,和服务器成功断开
		{
			break;
		}
	}
	if(cnt) return 1;
	return 0;
}

