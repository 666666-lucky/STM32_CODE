#include "system.h"
#include "led.h"
#include "usart.h"
#include "dac.h"
#include "adc.h"

int main()
{
	u8 i=0;
	u16 vol=0;
	u16 dacval=0;
	float dac_vol=0;
	LED_Init();
	Usart1_Init(9600);
	SysTick_Init(72);
	DAC1_Init();
	ADCx_Init();
	
	printf("系统启动");	
	while(1)
		{
			i++;
			if(i%20==0)
				led0=!led0;
			if(i%100==0)
				{
					vol+=200;
					if(vol>4094)
						vol=0;
					DAC_SetChannel1Data(DAC_Align_12b_R,vol);
					i=0;
					dacval=DAC_GetDataOutputValue(DAC_Channel_1);
					dac_vol=(float)dacval*(3.3/4096);
					printf("输出DAC电压值为：%.2fV\r\n",dac_vol);
					
					dacval=Get_ADC_Vaule(ADC_Channel_1,10);
					dac_vol=dacval*(3.3/4096);
					printf("输入电压值为：%.2fV\r\n",dac_vol);
				}
				
				delay_ms(10);
		}
	
}

