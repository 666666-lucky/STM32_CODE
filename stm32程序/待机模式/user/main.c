#include "system.h"
#include "led.h"
#include "usart.h"
#include "wkup.h"

int main()
{
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	LED_Init();
	Usart1_Init(9600);
	SysTick_Init(72);
	printf("系统启动");	
	while(1)
		{
			for(i=3;i>0;i--)
				{
					printf("倒计时 %d\r\n",i);
					led0=~led0;
					delay_ms(1000);
				}
			printf("进入待机模式\r\n");
			Enter_Standby_Mode();
		}
	
}

