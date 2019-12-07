#include "system.h"
#include "led.h"
#include "usart.h"
#include "key.h"
#include "iwdg.h"

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	LED_Init();
	SysTick_Init(72);	
	Usart1_Init(9600);
	Iwdg_Init(4,800);	
	Key_Init();
	
	printf("系统重启\r\n");
	led1=1;
	
	while(1)
		{
			if(KEY_Scan(0)==KEY_LEFT)
			{
				IWDG_ReloadCounter();
				led1=0;
				printf("喂狗\r\n");
			}
			led0=~led0;			
			delay_ms(500);
		}
	
}

