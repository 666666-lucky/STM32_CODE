#include "pwm.h"

void TIM3_CH2_PWM_Init(u16 per,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef 	GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//ʹ��TIM3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//ʹ�ܸ��ù���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);//��ӳ��GPIOB_5
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB,&GPIO_InitStructure);							//��ʼ��GPIOB_5

	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period =per;
	TIM_TimeBaseInitStructure.TIM_Prescaler =psc;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ����ʱ��3

	TIM_OCInitStructure.TIM_OCMode =TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCNPolarity =TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
	TIM_OC2Init(TIM3,&TIM_OCInitStructure);//���ͨ��2��ʼ��
	
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��TIMx�� CCR1 �ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM3,ENABLE);//ʹ��Ԥװ�ؼĴ���
	
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��
}
