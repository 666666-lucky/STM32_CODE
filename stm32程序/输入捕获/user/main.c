#include "system.h"
#include "led.h"
#include "key.h"
#include "input.h"
#include "usart.h"

int main()
{
	u8 i=0;
	u32 indata=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	Usart1_Init(9600);
	SysTick_Init(72);
	TIM5_CH1_Input_Init(0xffff,72);
	printf("ϵͳ����");

	
	while(1)
		{

			if(TIM5_CH1_CAPTURE_STA&0x80) //�ɹ�����
				{
					indata=TIM5_CH1_CAPTURE_STA&0x3f;
					indata*=0xffff; //�����������һ�εļ�������ʱ�� us
					indata+=TIM5_CH1_CAPTURE_VAL;//���ϸߵ�ƽ�����ʱ��
					printf("�ߵ�ƽ����ʱ�䣺%d us\r\n",indata); //�ܵĸߵ�ƽʱ��
					TIM5_CH1_CAPTURE_STA=0; //��ʼ��һ�β���
				}
		
			i++;
			if(i%20==0)
			{
				led1=!led1;
			}
			delay_ms(10);
		}
	
}

