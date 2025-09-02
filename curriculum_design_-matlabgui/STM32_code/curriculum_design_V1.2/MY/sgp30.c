#include "sgp30.h"
#include "tim.h"

void HX711_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Pin = SGP30_SCL_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SGP30_SCL_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SGP30_SDA_GPIO_PIN;
  HAL_GPIO_Init(SGP30_SDA_GPIO_PORT, &GPIO_InitStruct);
}


void SDA_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.Pin = SGP30_SDA_GPIO_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SGP30_SDA_GPIO_PORT, &GPIO_InitStructure);
}

void SDA_IN(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.Pin = SGP30_SDA_GPIO_PIN;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(SGP30_SDA_GPIO_PORT, &GPIO_InitStructure);
}

//产生IIC起始信号
void SGP30_IIC_Start(void)
{
  SDA_OUT();
  SGP30_SDA_HIGH;
  SGP30_SCL_HIGH;
  delay_us(20);

  SGP30_SDA_LOW;	//START:when CLK is high,DATA change form high to low
  delay_us(20);
  SGP30_SCL_LOW; 	//钳住I2C总线，准备发送或接收数据
}

//产生IIC停止信号
void SGP30_IIC_Stop(void)
{
  SDA_OUT();
  SGP30_SCL_LOW;
  SGP30_SDA_LOW;	//STOP:when CLK is high DATA change form low to high
  delay_us(20);
  SGP30_SCL_HIGH;
  SGP30_SDA_HIGH;	//发送I2C总线结束信号
  delay_us(20);
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t SGP30_IIC_Wait_Ack(void)
{
  uint8_t ucErrTime = 0;
  SDA_IN();
  SGP30_SDA_HIGH;
  delay_us(10);
  SGP30_SCL_HIGH;
  delay_us(10);
  while(SGP30_SDA_READ)
  {
    ucErrTime++;
    if(ucErrTime > 250)
    {
      SGP30_IIC_Stop();
      return 1;
    }
  }
  SGP30_SCL_LOW;  		//时钟输出0
  return 0;
}

//产生ACK应答
void SGP30_IIC_Ack(void)
{
  SGP30_SCL_LOW;
  SDA_OUT();
  SGP30_SDA_LOW;
  delay_us(20);
  SGP30_SCL_HIGH;
  delay_us(20);
  SGP30_SCL_LOW;
}

//不产生ACK应答
void SGP30_IIC_NAck(void)
{
  SGP30_SCL_LOW;
  SDA_OUT();
  SGP30_SDA_HIGH;;
  delay_us(20);
  SGP30_SCL_HIGH;
  delay_us(20);
  SGP30_SCL_LOW;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void SGP30_IIC_Send_Byte(uint8_t txd)
{
  uint8_t t;
  SDA_OUT();
  SGP30_SCL_LOW; 	    	//拉低时钟开始数据传输
  for(t = 0; t < 8; t++)
  {
    if((txd & 0x80) >> 7)
      SGP30_SDA_HIGH;
    else
      SGP30_SDA_LOW;
    txd <<= 1;
    delay_us(20);
    SGP30_SCL_HIGH;
    delay_us(20);
    SGP30_SCL_LOW;
    delay_us(20);
  }
  delay_us(20);

}

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint16_t SGP30_IIC_Read_Byte(uint8_t ack)
{
  uint8_t i;
  uint16_t receive = 0;
  SDA_IN();
  for(i = 0; i < 8; i++ )
  {
    SGP30_SCL_LOW;
    delay_us(20);
    SGP30_SCL_HIGH;
    receive <<= 1;
    if(SGP30_SDA_READ)
      receive++;
    delay_us(20);
  }
  if (!ack)
    SGP30_IIC_NAck();//发送nACK
  else
    SGP30_IIC_Ack(); //发送ACK
  return receive;
}


//初始化IIC接口
void SGP30_Init(void)
{
  HX711_GPIO_Init();
  SGP30_Write(0x20, 0x03);
//	SGP30_ad_write(0x20,0x61);
//	SGP30_ad_write(0x01,0x00);
}


void SGP30_Write(uint8_t a, uint8_t b)
{
  SGP30_IIC_Start();
  SGP30_IIC_Send_Byte(SGP30_write); //发送器件地址+写指令
  SGP30_IIC_Wait_Ack();
  SGP30_IIC_Send_Byte(a);		//发送控制字节
  SGP30_IIC_Wait_Ack();
  SGP30_IIC_Send_Byte(b);
  SGP30_IIC_Wait_Ack();
  SGP30_IIC_Stop();
  HAL_Delay(100);
}

uint32_t SGP30_Read(void)
{
  uint32_t dat;
  uint8_t crc;
  SGP30_IIC_Start();
  SGP30_IIC_Send_Byte(SGP30_read); //发送器件地址+读指令
  SGP30_IIC_Wait_Ack();
  dat = SGP30_IIC_Read_Byte(1);
  dat <<= 8;
  dat += SGP30_IIC_Read_Byte(1);
  crc = SGP30_IIC_Read_Byte(1); //crc数据，舍去
  crc = crc;  //为了不让出现编译警告
  dat <<= 8;
  dat += SGP30_IIC_Read_Byte(1);
  dat <<= 8;
  dat += SGP30_IIC_Read_Byte(0);
  SGP30_IIC_Stop();
  return(dat);
}




