#include "wwdg.h"
#include "led.h"

void Wwdg_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE); //ʹ�ܿ��Ź�ʱ��
	WWDG_SetPrescaler(WWDG_Prescaler_8);               //���÷�Ƶϵ��
	WWDG_SetWindowValue(0x5f);												//���ô���ֵ
	
	NVIC_InitStructure.NVIC_IRQChannel=WWDG_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStructure);
	
	WWDG_Enable(0x7f);			//ʹ�ܴ��ڿ��Ź�����ʼ��������ֵ
	WWDG_ClearFlag();
	WWDG_EnableIT();
	
}

void WWDG_IRQHandler()
{
	WWDG_SetCounter(0x7f);
	WWDG_ClearFlag();
	led1=~led1;
}
