#include "system.h"

void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE,ENABLE);
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_5);
	GPIO_SetBits(GPIOB,GPIO_Pin_5);	
}

int main()
{
//	RCC_HSE_Config(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
	LED_Init();
	SysTick_Init(72);
	while(1)
	{
		PBout(5)=1;
		PEout(5)=0;
		delay_ms(500);
		PBout(5)=0;
		PEout(5)=1;
		delay_ms(500);
	}
	
}

