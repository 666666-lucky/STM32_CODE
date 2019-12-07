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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);//�򿪺���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//�򿪵�Դ����ʱ��
	PWR_BackupAccessCmd(ENABLE);//ʹ�ܺ󱸼Ĵ�������
	if(BKP_ReadBackupRegister(BKP_DR1)==0xa0a0)
		{
			BKP_DeInit();//��ʼ���󱸼Ĵ���
			RCC_LSEConfig(RCC_LSE_ON);//����LSEʱ��
			while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET&&i<250)//�ȴ�����ʱ�Ӿ���
			{
				i++;
				delay_ms(10);
			}
			if(i>=250)		return 1;	
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//RTCѡ�õ���ʱ��LSE
			RCC_RTCCLKCmd(ENABLE);//ʹ��RTCʱ��
			RTC_WaitForLastTask();//д�ȴ�
			RTC_WaitForSynchro();//�ȴ�RTC�Ĵ�����RTC��APBʱ��ͬ��
			
			RTC_ITConfig(RTC_IT_SEC,ENABLE);//�������ж�
			RTC_WaitForLastTask();
			
			RTC_EnterConfigMode();//���� RTC ����ģʽ
			RTC_WaitForLastTask();
			
			RTC_SetPrescaler(32767);//����Ԥ��Ƶ��ֵ
			RTC_WaitForLastTask();
			
			RTC_SetCounter(0x1013a);//���ó�ʼʱ��
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
		printf("ʵʱʱ�䣺%d:%d:%d\r\n",Rtc_time.hour,Rtc_time.min,Rtc_time.sec);		
	}
	RTC_ClearITPendingBit(RTC_IT_SEC);
//	RTC_WaitForLastTask();
}

