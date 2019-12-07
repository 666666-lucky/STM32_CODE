#include "adc.h"

void ADCx_Init() //ADC1_CH1_��ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);//ʹ��GPIOA��ADC1ʱ��
	
	//��ʼ��GPIOA1
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//��ʼ��ADC1
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//�ر�����ת��
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//�Ҷ���	
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;// �����ڶ���ģʽ
	ADC_InitStructure.ADC_NbrOfChannel=1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//��ɨ��ģʽ	
	ADC_Init(ADC1,&ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ʱ�ӷ�Ƶϵ��
	ADC_Cmd(ADC1,ENABLE);//����ADC1ͨ��
	
	ADC_ResetCalibration(ADC1);//����ָ����ADC��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡADC����У׼�Ĵ�����״̬
	
	ADC_StartCalibration(ADC1);//��ʼָ��ADC��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));//��ȡָ��ADC��У׼����
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ�ܻ���ʧ��ָ����ADC�����ת����������
}

u16 Get_ADC_Vaule(u8 CHx,u8 time)
{
	u32 average_value=0;
	u8 cishu=0;
	ADC_RegularChannelConfig(ADC1, CHx, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��,239.5������,��߲���ʱ�������߾�ȷ��			    
	
	for(;cishu<time;cishu++)	
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ��ָ����ADC1�����ת����������	
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
			average_value+=ADC_GetConversionValue(ADC1);
			delay_ms(5);
		}
	return 	average_value/time;
}

void ADCTemp_Init()
{
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//ʹ��ADC1ʱ��
	
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//�ر�����ת��
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//�Ҷ���	
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;// �����ڶ���ģʽ
	ADC_InitStructure.ADC_NbrOfChannel=1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//��ɨ��ģʽ	
	ADC_Init(ADC1,&ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ʱ�ӷ�Ƶϵ��
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_Cmd(ADC1,ENABLE);//����ADC1ͨ��
	
	ADC_ResetCalibration(ADC1);//����ָ����ADC��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡADC����У׼�Ĵ�����״̬
	
	ADC_StartCalibration(ADC1);//��ʼָ��ADC��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));//��ȡָ��ADC��У׼����
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ�ܻ���ʧ��ָ����ADC�����ת����������
}
u16 Get_ADC_Temp_Vaule(u8 CHx,u8 time)
{
	u32 average_value=0;
	u8 cishu=0;
	ADC_RegularChannelConfig(ADC1, CHx, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��,239.5������,��߲���ʱ�������߾�ȷ��			    
	
	for(;cishu<time;cishu++)	
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ��ָ����ADC1�����ת����������	
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
			average_value+=ADC_GetConversionValue(ADC1);
			delay_ms(5);
		}
	return 	average_value/time;
}

int Get_Temperture()
{
	u16 value=0;
	double Temperture=0;
	int Temp=0;
	value=Get_ADC_Temp_Vaule(ADC_Channel_16,10);
	Temperture=(1.43-value*(3.3/4096))/0.0043+25;
	Temp=Temperture*100;
	return Temp;
}
