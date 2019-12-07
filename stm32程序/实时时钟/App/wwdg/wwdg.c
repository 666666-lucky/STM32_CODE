#include "wwdg.h"
#include "led.h"

void Wwdg_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE); //使能看门狗时钟
	WWDG_SetPrescaler(WWDG_Prescaler_8);               //设置分频系数
	WWDG_SetWindowValue(0x5f);												//设置窗口值
	
	NVIC_InitStructure.NVIC_IRQChannel=WWDG_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStructure);
	
	WWDG_Enable(0x7f);			//使能窗口看门狗并初始化计数器值
	WWDG_ClearFlag();
	WWDG_EnableIT();
	
}

void WWDG_IRQHandler()
{
	WWDG_SetCounter(0x7f);
	WWDG_ClearFlag();
	led1=~led1;
}
