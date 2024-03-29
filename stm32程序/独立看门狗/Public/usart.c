#include "usart.h"

int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
{
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

void Usart1_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能串口1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA时钟

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //串口输出PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //复用推挽输出	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//TX			   //串口输入PA10
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;	    //输入浮空	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate =bound;
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode =USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity =USART_Parity_No;
	USART_InitStructure.USART_StopBits =USART_StopBits_1;
	USART_InitStructure.USART_WordLength =USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);   //初始化串口1
	
	NVIC_InitStructure.NVIC_IRQChannel =USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;
	NVIC_Init(&NVIC_InitStructure);//初始化中断向量控制
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 	
	USART_ClearFlag(USART1, USART_FLAG_TC);//清除中断标志	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
}

void USART1_IRQHandler()
{
	u8 r;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==1)
	{
		r=USART_ReceiveData(USART1);
		USART_SendData(USART1,r);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=1);
		USART_ClearFlag(USART1,USART_FLAG_TC);
	}
}
