#include "system.h"
#include "led.h"
#include "usart.h"
#include "adc.h"

int main()
{
	u8 i=0;
	int vol=0;
	
	LED_Init();
	Usart1_Init(9600);
	SysTick_Init(72);
	ADCTemp_Init();
	printf("系统启动");	
	while(1)
		{
			i++;
			if(i%20==0)
				led0=!led0;
			if(i%50==0)
				{
					
					vol=Get_Temperture();
					if(vol>0)
						printf("内部温度 %.2f \r\n",(float)vol/100);
					else
					{
						vol=-vol;
						printf("内部温度-%.2f \r\n",(float)vol/100);
					}
					i=0;
				}
				delay_ms(10);
		}
	
}

