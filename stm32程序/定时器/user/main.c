#include "system.h"
#include "led.h"
#include "time.h"

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	SysTick_Init(72);
	Time4_Init_ms(500);
	while(1)
	{
		led0=~led0;
		delay_ms(200);
	}
	
}

