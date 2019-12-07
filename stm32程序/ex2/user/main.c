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
void delay(u32 i)
{
	while(i--);	
}
void RCC_HSE_Config(u32 div,u32 pllm)
{
	RCC_DeInit ();
	RCC_HSEConfig(RCC_HSE_ON);
	if(RCC_WaitForHSEStartUp()==SUCCESS)
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PLLConfig(div,pllm);
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while(RCC_GetSYSCLKSource()!=0x08);
	}
}

int main()
{
	RCC_HSE_Config(RCC_PLLSource_HSE_Div2,RCC_PLLMul_9);
	LED_Init();
	
	while(1)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		delay(600000);
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		delay(600000);
	}
	
}

