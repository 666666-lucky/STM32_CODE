#include "exti.h"

void My_EXTI_Init()
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);		//使能外设AFIO
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);//映射相应管脚中断线路
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	
	//EXTI0中断向量配置（中断优先级）
	NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn;			//中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;			//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;					//子优先级
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line2|EXTI_Line3|EXTI_Line4;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
}

void EXTI0_IRQHandler(void)
{
		if(EXTI_GetITStatus(EXTI_Line0)==1)
		{			
				delay_ms(10);
				if(key_UP==1)
				{
					led0 =0;
				}			
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2)==1)
		{			
				delay_ms(10);
				if(key2==0)
					{
						led1=0;
					}			
		}
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3)==1)
		{			
				delay_ms(10);
				if(key1==0)
					{
						led0=1;
					}			
		}
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4)==1)
		{			
				delay_ms(10);
				if(key0==0)
					{
						led1=1;
					}			
		}
	EXTI_ClearITPendingBit(EXTI_Line4);
}

