#include "adc.h"

void ADCx_Init() //ADC1_CH1_初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);//使能GPIOA，ADC1时钟
	
	//初始化GPIOA1
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//初始化ADC1
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//关闭连续转换
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//右对齐	
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;// 工作在独立模式
	ADC_InitStructure.ADC_NbrOfChannel=1;//1个转换在规则序列中 也就是只转换规则序列1 
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//非扫描模式	
	ADC_Init(ADC1,&ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//采样时钟分频系数
	ADC_Cmd(ADC1,ENABLE);//开启ADC1通道
	
	ADC_ResetCalibration(ADC1);//重置指定的ADC的校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));//获取ADC重置校准寄存器的状态
	
	ADC_StartCalibration(ADC1);//开始指定ADC的校准状态
	while(ADC_GetCalibrationStatus(ADC1));//获取指定ADC的校准程序
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能或者失能指定的ADC的软件转换启动功能
}

u16 Get_ADC_Vaule(u8 CHx,u8 time)
{
	u32 average_value=0;
	u8 cishu=0;
	ADC_RegularChannelConfig(ADC1, CHx, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADC通道,239.5个周期,提高采样时间可以提高精确度			    
	
	for(;cishu<time;cishu++)	
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能指定的ADC1的软件转换启动功能	
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
			average_value+=ADC_GetConversionValue(ADC1);
			delay_ms(5);
		}
	return 	average_value/time;
}

void ADCTemp_Init()
{
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//使能ADC1时钟
	
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//关闭连续转换
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//右对齐	
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;// 工作在独立模式
	ADC_InitStructure.ADC_NbrOfChannel=1;//1个转换在规则序列中 也就是只转换规则序列1 
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//非扫描模式	
	ADC_Init(ADC1,&ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//采样时钟分频系数
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_Cmd(ADC1,ENABLE);//开启ADC1通道
	
	ADC_ResetCalibration(ADC1);//重置指定的ADC的校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));//获取ADC重置校准寄存器的状态
	
	ADC_StartCalibration(ADC1);//开始指定ADC的校准状态
	while(ADC_GetCalibrationStatus(ADC1));//获取指定ADC的校准程序
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能或者失能指定的ADC的软件转换启动功能
}
u16 Get_ADC_Temp_Vaule(u8 CHx,u8 time)
{
	u32 average_value=0;
	u8 cishu=0;
	ADC_RegularChannelConfig(ADC1, CHx, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADC通道,239.5个周期,提高采样时间可以提高精确度			    
	
	for(;cishu<time;cishu++)	
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能指定的ADC1的软件转换启动功能	
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
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
