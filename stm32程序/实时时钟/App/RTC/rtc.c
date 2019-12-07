#include "rtc.h"
#include "usart.h"

Time_typedef Rtc_time;

static void RCT_NVIC_Init(void)
{
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel =RTC_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;
		NVIC_Init(&NVIC_InitStructure);
}

void Get_Time()
{
	u32 timecount=0;
	timecount=RTC_GetCounter();
	Rtc_time.hour=timecount/3600;
	Rtc_time.min=timecount%3600/60;
	Rtc_time.sec=timecount%60;
}

u8 RTC_Init()
{
	u8 i=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);//打开后备域时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//打开电源管理时钟
	PWR_BackupAccessCmd(ENABLE);//使能后备寄存器访问
	if(BKP_ReadBackupRegister(BKP_DR1)==0xa0a0)
		{
			BKP_DeInit();//初始化后备寄存器
			RCC_LSEConfig(RCC_LSE_ON);//开启LSE时钟
			while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET&&i<250)//等待低速时钟就绪
			{
				i++;
				delay_ms(10);
			}
			if(i>=250)		return 1;	
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//RTC选用低速时钟LSE
			RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
			RTC_WaitForLastTask();//写等待
			RTC_WaitForSynchro();//等待RTC寄存器与RTC的APB时钟同步
			
			RTC_ITConfig(RTC_IT_SEC,ENABLE);//开启秒中断
			RTC_WaitForLastTask();
			
			RTC_EnterConfigMode();//进入 RTC 配置模式
			RTC_WaitForLastTask();
			
			RTC_SetPrescaler(32767);//设置预分频的值
			RTC_WaitForLastTask();
			
			RTC_SetCounter(0x1013a);//设置初始时间
			RTC_WaitForLastTask();
			
			RTC_ExitConfigMode();
			BKP_WriteBackupRegister(BKP_DR1,0xa0a0);
		}
	else
		{
			RTC_WaitForSynchro();
			RTC_ITConfig(RTC_IT_SEC,ENABLE);
			RTC_WaitForLastTask();
		}
	RCT_NVIC_Init();
	Get_Time();
	return 0;
}

void RTC_IRQHandler()
{
	if(RTC_GetITStatus(RTC_IT_SEC))
	{
		Get_Time();
		printf("实时时间：%d:%d:%d\r\n",Rtc_time.hour,Rtc_time.min,Rtc_time.sec);		
	}
	RTC_ClearITPendingBit(RTC_IT_SEC);
//	RTC_WaitForLastTask();
}

