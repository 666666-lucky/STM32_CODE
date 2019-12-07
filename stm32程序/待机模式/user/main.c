#include "system.h"
#include "led.h"
#include "usart.h"
#include "wkup.h"

int main()
{
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	Usart1_Init(9600);
	SysTick_Init(72);
	printf("ϵͳ����");	
	while(1)
		{
			for(i=3;i>0;i--)
				{
					printf("����ʱ %d\r\n",i);
					led0=~led0;
					delay_ms(1000);
				}
			printf("�������ģʽ\r\n");
			Enter_Standby_Mode();
		}
	
}

