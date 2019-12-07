#include "system.h"
#include "led.h"
#include "usart.h"

int main()
{
	u8 i=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	LED_Init();
	SysTick_Init(72);	
//	Usart2_Init(115200);
	Usart1_Init(9600);
	while(1)
		{
			led0=~led0;			

			printf("系统运行\r\n");

			delay_ms(500);
		}
	
}

