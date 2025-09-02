//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究  
//STM32电机控制开发板   
//联控智能
//网址: https://shop59411198.taobao.com/
//修改日期:2019/4/6
//版本：V2.0
//技术QQ: 2465727011
//技术QQ群：438404033
//############################################################
#include "ADC_int.h"
#include "Hall.h"
#include "Control stage parameters.h"
#include "State_Machine.h"
#include "Hallless.h"
#include "Drive parameters.h"

uint32_t  ADC_ConvertedValue[5]={0,0,0,0,0};//初始化AD采样值

extern volatile uint16_t ADC_DualConvertedValueTab[5];    //调用时重新取值
#define ADC1_DR_Address    (uint32_t)ADC1_BASE +0X4C//0x4001204C 
extern    State       StateContr;    //控制状态
extern    ADCSamp     ADCSampPare;   //AD采集

extern    Hallless   Hallless_Three;

void Adc_Configuration(void)          //ADC配置      
{
	ADC_InitTypeDef       ADC_InitStructure;    
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA |  RCC_AHB1Periph_GPIOB |RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   //使能相关功能

	GPIO_InitStructure.GPIO_Pin = PHASE_U_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(PHASE_U_GPIO_PORT, &GPIO_InitStructure);   //配置U相模拟量接口

	GPIO_InitStructure.GPIO_Pin = PHASE_V_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(PHASE_V_GPIO_PORT, &GPIO_InitStructure);//配置V相模拟量接口

	GPIO_InitStructure.GPIO_Pin = PHASE_W_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(PHASE_W_GPIO_PORT, &GPIO_InitStructure);//配置W相模拟量接口

	GPIO_InitStructure.GPIO_Pin = BusVolt_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(BusVolt_GPIO_PORT, &GPIO_InitStructure);//配置母线模拟量接口

	GPIO_InitStructure.GPIO_Pin = POT_GPIO_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(POT_GPIO_PORT, &GPIO_InitStructure);   //配置调速模拟量接口
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;    //通道
	DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) ADC1_DR_Address;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC_DualConvertedValueTab;  //ADC采样存在这里
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//数据外设方向带存储器
	DMA_InitStructure.DMA_BufferSize = 5;//5路通道
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//DMA外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //存储器地址向上
    
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据半地址数据
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //存储器数据半地址数据
    
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环转换模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//优先级高
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//禁止FIFO         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;//FIFO值
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//单次传输

	DMA_Init(DMA2_Stream0, &DMA_InitStructure);//初始化
	DMA_Cmd(DMA2_Stream0, ENABLE);//DMA使能
     
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//	ADC独立转换模式
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;  //2分频
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//DMA先禁止访问
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//ADC转换间隔5
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位转换精度
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;  //扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//不用外部触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 5;  //5通道转换
  ADC_Init(ADC1, &ADC_InitStructure);    //ADC初始化
  ADC_DMACmd(ADC1, ENABLE);              //使能DMA
  ADC_RegularChannelConfig(ADC1, PHASE_U_CURR_CHANNEL, 1, ADC_SampleTime_15Cycles); //U相采集
  ADC_RegularChannelConfig(ADC1, PHASE_V_CURR_CHANNEL, 2, ADC_SampleTime_15Cycles); //V相采集
  ADC_RegularChannelConfig(ADC1, PHASE_W_CURR_CHANNEL, 3, ADC_SampleTime_15Cycles); //W相采集 
	ADC_RegularChannelConfig(ADC1, BusVolt_CHANNEL, 4, ADC_SampleTime_15Cycles); //母线电压采集
	ADC_RegularChannelConfig(ADC1, POT_CHANNEL, 5, ADC_SampleTime_15Cycles);     //调速器电压采集
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);//控制DMA传输一组数据后发不发送DMA请求
  ADC_Cmd(ADC1, ENABLE);   //AD使能
  ADC_SoftwareStartConv(ADC1);//软件转换开始
}

void Offset_CurrentReading(void)   //偏移值获取
{
	static uint16_t ADC_PhaseU_Curr[64];
	static uint16_t ADC_PhaseV_Curr[64];
	static uint16_t ADC_PhaseW_Curr[64];
	 
   
  /* ADC Channel used for current reading are read  in order to get zero currents ADC values*/
  //64次采样求平均值，电流传感器初始校准   	
	
 
 //对于相电流和母线电流的电阻法测量电流,需要上电读取初始偏执电压
	if(StateContr.drive_car == 0) 
	{
		uint32_t sum_U=0;
		uint32_t sum_V=0;
		uint32_t sum_W=0;
		uint8_t i;
		for(i=0; i < 64; i++)  //采集64次 取平均值
		{
			ADC_PhaseU_Curr[i] = ADC_DualConvertedValueTab[0];
	    ADC_PhaseV_Curr[i] = ADC_DualConvertedValueTab[1];  
	    ADC_PhaseW_Curr[i] = ADC_DualConvertedValueTab[2];
			sum_U += ADC_PhaseU_Curr[i];
			sum_V += ADC_PhaseV_Curr[i];
			sum_W += ADC_PhaseW_Curr[i];
		}
	     ADCSampPare.OffsetPhaseU_Curr = sum_U /64;
	     ADCSampPare.OffsetPhaseV_Curr = sum_V /64;
	     ADCSampPare.OffsetPhaseW_Curr = sum_W /64;
	}
}

void HALLLESS_ADC_Sample(void) // 放在PWM中断进来后，采样时间为1us左右,(7cycles)  保证在PWM中断后进来采样后为在PWM中间采集相电流
{    
  ADCSampPare.PhaseU_Curr  =    (ADC_DualConvertedValueTab[0]-ADCSampPare.OffsetPhaseU_Curr);	//U相电流
	ADCSampPare.PhaseV_Curr  =    (ADC_DualConvertedValueTab[1]-ADCSampPare.OffsetPhaseV_Curr);//V相电流
	ADCSampPare.PhaseW_Curr  =    (ADC_DualConvertedValueTab[2]-ADCSampPare.OffsetPhaseW_Curr);  //W相电流
  ADCSampPare.BUS_Voltage  =     ADC_DualConvertedValueTab[3];	//母线电压 
	if(Hallless_Three.Hallless_State == 0x05)//根据霍尔状态计算母线电流值
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseV_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x01)//根据霍尔状态计算母线电流值
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x03)//根据霍尔状态计算母线电流值
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x02)//根据霍尔状态计算母线电流值
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseV_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x06)//根据霍尔状态计算母线电流值
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseV_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x04)//根据霍尔状态计算母线电流值
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseW_Curr + ADCSampPare.PhaseV_Curr;
	}
}
