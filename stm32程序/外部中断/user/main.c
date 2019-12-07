#include "system.h"
#include "led.h"
#include "exti.h"
#include "key.h"

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	My_EXTI_Init();
	Key_Init();
	LED_Init();
	SysTick_Init(72);
	while(1)
	{
//		led0=~led0;
////		led1=1;
//		delay_ms(200);
	}
	
}

