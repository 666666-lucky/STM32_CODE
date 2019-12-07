#include "system.h"
#include "led.h"
#include "usart.h"
#include "rtc.h"



int main()
{
	u8 i=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	Usart1_Init(9600);
	SysTick_Init(72);	
	
	if(RTC_Init())
	{
		printf("RTC时钟故障");	
	}
	printf("系统启动");	
	
	while(1)
		{
			i++;
			if(i%20==0)
				led0=!led0;			
			delay_ms(10);
		}
	
}

