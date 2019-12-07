#include "input.h"

 u8 TIM5_CH1_CAPTURE_STA; //���벶���״̬
 u16 TIM5_CH1_CAPTURE_VAL;//���벶��ֵ
void TIM5_CH1_Input_Init(u16 per,u16 psc)   
{	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);//ʹ��TIM5ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��GPIOAʱ��
	//��ʼ��CPIOA_0
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//��ʱʱ������	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period =per;
	TIM_TimeBaseInitStructure.TIM_Prescaler =psc-1;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);
	//�������벶��	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;//����ͨ��
	TIM_ICInitStructure.TIM_ICFilter=0x00;//�ر��˲�
	TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//������������
	TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;//��Ƶϵ��
	TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;//ֱ��ӳ�䵽TI1
	TIM_ICInit(TIM5,&TIM_ICInitStructure);
	
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update|TIM_IT_CC1);//����жϱ�־	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//������Ӧ�ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM5,ENABLE); //ʹ�ܶ�ʱ��
}

void TIM5_IRQHandler(void)
{
	if((TIM5_CH1_CAPTURE_STA&0x80)==0)
		{
			if(TIM_GetITStatus(TIM5,TIM_IT_Update))
				{
					if(TIM5_CH1_CAPTURE_STA&0X40)//�����˸ߵ�ƽ
						{
							if((TIM5_CH1_CAPTURE_STA&0x3f)==0x3f) //�ߵ�ƽʱ��̫��
							{
								TIM5_CH1_CAPTURE_STA|=0x80; //��־һ�β���ɹ�
								TIM5_CH1_CAPTURE_VAL=0xffff;
							}
							else
							{
								TIM5_CH1_CAPTURE_STA++;
							}
						}
				}
			if(TIM_GetITStatus(TIM5,TIM_IT_CC1))
				{
					if(TIM5_CH1_CAPTURE_STA&0x40)
						{
							TIM5_CH1_CAPTURE_STA|=0x80;
							TIM5_CH1_CAPTURE_VAL=TIM_GetCapture1(TIM5);
							TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising); //���������ز���
						}
					else
						{
							TIM5_CH1_CAPTURE_STA=0;
							TIM5_CH1_CAPTURE_VAL=0;
							TIM5_CH1_CAPTURE_STA|=0x40;
							TIM_Cmd(TIM5,DISABLE);
							TIM_SetCounter(TIM5,0);
							TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling); //�����½��ز���
							TIM_Cmd(TIM5,ENABLE);
						}
				}
					
		}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update|TIM_IT_CC1);
}

