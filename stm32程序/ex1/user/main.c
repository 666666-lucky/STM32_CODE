#include "stm32f10x.h"

void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
	
}

int main()
{
	LED_Init();
	
	while(1)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	}
	
}

