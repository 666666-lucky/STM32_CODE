#include "time.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"


extern TimerHandle_t AutoReloadTimer_Handle;			//周期定时器句柄


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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM4,ENABLE); //使能定时器
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==1)
		{
			printf("TIM4 RUNNING........\r\n");
		}
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}

void Time5_Init_ms(u16 ms)   //1<ms<5000
{	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//定时时间设置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);//使能TIM5时钟
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period =ms*12;
	TIM_TimeBaseInitStructure.TIM_Prescaler =6000-1;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);
	
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);//清除中断标志	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);//开启相应中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM5,ENABLE); //使能定时器
}

void TIM5_IRQHandler(void)
{
	static u8 i=0;
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==1)
		{
			i++;
			xTimerStartFromISR(AutoReloadTimer_Handle,NULL);
			printf("周期定时器开启\r\n");
			if(i%5==0)
			{
				xTimerStopFromISR(AutoReloadTimer_Handle,NULL);
				printf("周期定时器关闭\r\n");				
			}
		}
		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
}

