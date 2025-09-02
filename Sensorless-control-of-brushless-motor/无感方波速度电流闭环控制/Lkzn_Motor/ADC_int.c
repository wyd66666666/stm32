//############################################################
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//��Ȩ���У�����ؾ�  
//STM32������ƿ�����   
//��������
//��ַ: https://shop59411198.taobao.com/
//�޸�����:2019/4/6
//�汾��V2.0
//����QQ: 2465727011
//����QQȺ��438404033
//############################################################
#include "ADC_int.h"
#include "Hall.h"
#include "Control stage parameters.h"
#include "State_Machine.h"
#include "Hallless.h"
#include "Drive parameters.h"

uint32_t  ADC_ConvertedValue[5]={0,0,0,0,0};//��ʼ��AD����ֵ

extern volatile uint16_t ADC_DualConvertedValueTab[5];    //����ʱ����ȡֵ
#define ADC1_DR_Address    (uint32_t)ADC1_BASE +0X4C//0x4001204C 
extern    State       StateContr;    //����״̬
extern    ADCSamp     ADCSampPare;   //AD�ɼ�

extern    Hallless   Hallless_Three;

void Adc_Configuration(void)          //ADC����      
{
	ADC_InitTypeDef       ADC_InitStructure;    
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA |  RCC_AHB1Periph_GPIOB |RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   //ʹ����ع���

	GPIO_InitStructure.GPIO_Pin = PHASE_U_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(PHASE_U_GPIO_PORT, &GPIO_InitStructure);   //����U��ģ�����ӿ�

	GPIO_InitStructure.GPIO_Pin = PHASE_V_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(PHASE_V_GPIO_PORT, &GPIO_InitStructure);//����V��ģ�����ӿ�

	GPIO_InitStructure.GPIO_Pin = PHASE_W_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(PHASE_W_GPIO_PORT, &GPIO_InitStructure);//����W��ģ�����ӿ�

	GPIO_InitStructure.GPIO_Pin = BusVolt_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(BusVolt_GPIO_PORT, &GPIO_InitStructure);//����ĸ��ģ�����ӿ�

	GPIO_InitStructure.GPIO_Pin = POT_GPIO_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(POT_GPIO_PORT, &GPIO_InitStructure);   //���õ���ģ�����ӿ�
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;    //ͨ��
	DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) ADC1_DR_Address;//DMA�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC_DualConvertedValueTab;  //ADC������������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//�������跽����洢��
	DMA_InitStructure.DMA_BufferSize = 5;//5·ͨ��
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//DMA�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�洢����ַ����
    
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݰ��ַ����
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //�洢�����ݰ��ַ����
    
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//ѭ��ת��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//���ȼ���
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//��ֹFIFO         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;//FIFOֵ
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//���δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//���δ���

	DMA_Init(DMA2_Stream0, &DMA_InitStructure);//��ʼ��
	DMA_Cmd(DMA2_Stream0, ENABLE);//DMAʹ��
     
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//	ADC����ת��ģʽ
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;  //2��Ƶ
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//DMA�Ƚ�ֹ����
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//ADCת�����5
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λת������
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;  //ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//�����ⲿ����
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�����Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 5;  //5ͨ��ת��
  ADC_Init(ADC1, &ADC_InitStructure);    //ADC��ʼ��
  ADC_DMACmd(ADC1, ENABLE);              //ʹ��DMA
  ADC_RegularChannelConfig(ADC1, PHASE_U_CURR_CHANNEL, 1, ADC_SampleTime_15Cycles); //U��ɼ�
  ADC_RegularChannelConfig(ADC1, PHASE_V_CURR_CHANNEL, 2, ADC_SampleTime_15Cycles); //V��ɼ�
  ADC_RegularChannelConfig(ADC1, PHASE_W_CURR_CHANNEL, 3, ADC_SampleTime_15Cycles); //W��ɼ� 
	ADC_RegularChannelConfig(ADC1, BusVolt_CHANNEL, 4, ADC_SampleTime_15Cycles); //ĸ�ߵ�ѹ�ɼ�
	ADC_RegularChannelConfig(ADC1, POT_CHANNEL, 5, ADC_SampleTime_15Cycles);     //��������ѹ�ɼ�
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);//����DMA����һ�����ݺ󷢲�����DMA����
  ADC_Cmd(ADC1, ENABLE);   //ADʹ��
  ADC_SoftwareStartConv(ADC1);//���ת����ʼ
}

void Offset_CurrentReading(void)   //ƫ��ֵ��ȡ
{
	static uint16_t ADC_PhaseU_Curr[64];
	static uint16_t ADC_PhaseV_Curr[64];
	static uint16_t ADC_PhaseW_Curr[64];
	 
   
  /* ADC Channel used for current reading are read  in order to get zero currents ADC values*/
  //64�β�����ƽ��ֵ��������������ʼУ׼   	
	
 
 //�����������ĸ�ߵ����ĵ��跨��������,��Ҫ�ϵ��ȡ��ʼƫִ��ѹ
	if(StateContr.drive_car == 0) 
	{
		uint32_t sum_U=0;
		uint32_t sum_V=0;
		uint32_t sum_W=0;
		uint8_t i;
		for(i=0; i < 64; i++)  //�ɼ�64�� ȡƽ��ֵ
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

void HALLLESS_ADC_Sample(void) // ����PWM�жϽ����󣬲���ʱ��Ϊ1us����,(7cycles)  ��֤��PWM�жϺ����������Ϊ��PWM�м�ɼ������
{    
  ADCSampPare.PhaseU_Curr  =    (ADC_DualConvertedValueTab[0]-ADCSampPare.OffsetPhaseU_Curr);	//U�����
	ADCSampPare.PhaseV_Curr  =    (ADC_DualConvertedValueTab[1]-ADCSampPare.OffsetPhaseV_Curr);//V�����
	ADCSampPare.PhaseW_Curr  =    (ADC_DualConvertedValueTab[2]-ADCSampPare.OffsetPhaseW_Curr);  //W�����
  ADCSampPare.BUS_Voltage  =     ADC_DualConvertedValueTab[3];	//ĸ�ߵ�ѹ 
	if(Hallless_Three.Hallless_State == 0x05)//���ݻ���״̬����ĸ�ߵ���ֵ
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseV_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x01)//���ݻ���״̬����ĸ�ߵ���ֵ
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x03)//���ݻ���״̬����ĸ�ߵ���ֵ
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x02)//���ݻ���״̬����ĸ�ߵ���ֵ
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseV_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x06)//���ݻ���״̬����ĸ�ߵ���ֵ
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseV_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x04)//���ݻ���״̬����ĸ�ߵ���ֵ
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseW_Curr + ADCSampPare.PhaseV_Curr;
	}
}
