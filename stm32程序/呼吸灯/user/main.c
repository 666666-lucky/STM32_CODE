#include "system.h"
#include "led.h"
#include "pwm.h"

int main()
{
	u16 i=0;
	u8 k=0;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	LED_Init();
	SysTick_Init(72);	
	TIM3_CH2_PWM_Init(500,72-1);
	
	while(1)
		{
			if(k==0)
				{
					i++;
					if(i==499)
						k=1;
				}
			else
				{
					i--;
					if(i==0)
						k=0;
				}
			TIM_SetCompare2(TIM3,i);  //i值最大可以取499，因为ARR最大值是499.
			delay_ms(10);
		}
	
}

