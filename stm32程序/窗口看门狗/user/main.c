#include "system.h"
#include "led.h"
#include "key.h"
#include "wwdg.h"

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	LED_Init();
	SysTick_Init(72);
	
	
	led0=0;
	delay_ms(1000);	
	Wwdg_Init();
	
	while(1)
		{

			led0=1;			
			delay_ms(500);
		}
	
}

