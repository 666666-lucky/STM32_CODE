#include "time.h"
#include "led.h"

void Time4_Init_ms(u16 ms)   //1<ms<5000
{	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//定时时间设置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//使能TIM4时钟
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period =ms*12;
	TIM_TimeBaseInitStructure.TIM_Prescaler =6000-1;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//清除中断标志	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//开启相应中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM4,ENABLE); //使能定时器
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==1)
		{
			led1=~led1;
		}
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}
