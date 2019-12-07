#include "system.h"
#include "led.h"
#include "key.h"
#include "input.h"
#include "usart.h"

int main()
{
	u8 i=0;
	u32 indata=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	LED_Init();
	Usart1_Init(9600);
	SysTick_Init(72);
	TIM5_CH1_Input_Init(0xffff,72);
	printf("系统启动");

	
	while(1)
		{

			if(TIM5_CH1_CAPTURE_STA&0x80) //成功捕获
				{
					indata=TIM5_CH1_CAPTURE_STA&0x3f;
					indata*=0xffff; //溢出次数乘以一次的计数次数时间 us
					indata+=TIM5_CH1_CAPTURE_VAL;//加上高电平捕获的时间
					printf("高电平持续时间：%d us\r\n",indata); //总的高电平时间
					TIM5_CH1_CAPTURE_STA=0; //开始下一次捕获
				}
		
			i++;
			if(i%20==0)
			{
				led1=!led1;
			}
			delay_ms(10);
		}
	
}

