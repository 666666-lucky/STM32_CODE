#include "system.h"
static u32 fac_us;
static u16 fac_ms;


void RCC_HSE_Config(u32 div,u32 pllm)				//ϵͳʱ��Ƶ��ѡ��
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

void SysTick_Init(u8 sysclk)					//��ʱ������ʼ��
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us=sysclk/8;
	fac_ms=(u16)fac_us*1000;
}
void delay_us(u32 nus)							//us��ʱ
{
	u32 i;
	SysTick->LOAD=nus*fac_us;				 //��ʱ��ֵ����
	SysTick->VAL=0;								 	 //���������
	SysTick->CTRL|=0x01;						 //������ʱ��
	do
	{
		i=SysTick->CTRL;							
	}
	while((i&0x01)&&!(i&(1<<16)));	//����ʱ
	SysTick->CTRL&=~0x01;						//�رռ�ʱ��
	SysTick->VAL =0X00; //��ռ�����
}

void delay_ms(u16	nms)
{
	u32 i;
	SysTick->LOAD=(u32)nms*fac_ms;
	SysTick->VAL=0;
	SysTick->CTRL|=0x01;
	do
	{
		i=SysTick->CTRL;
	}
	while((i&0x01)&&!(i&(1<<16)));
	SysTick->CTRL&=~0x01;
	SysTick->VAL =0X00; //��ռ�����
}
