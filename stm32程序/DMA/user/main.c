#include "system.h"
#include "led.h"
#include "usart.h"
#include "dma.h"
#include "key.h"

#define send_buf_len 5000 
u8 send_buf[send_buf_len];

void send_data(u8 *p)
{
	u16 i;
	for(i=0;i<send_buf_len;i++)
		{
			*p='2';
			p++;
		}
}

int main()
{
	u8 i=0;
	LED_Init();
	Usart1_Init(9600);
	SysTick_Init(72);	
	Key_Init();
	Dmax_Init(DMA1_Channel4,(u32)&USART1->DR,(u32)send_buf,send_buf_len);
	send_data(send_buf);
	printf("系统启动");	
	
	while(1)
		{
			i++;
			if(i%20==0)
				led0=!led0;
			if(KEY_Scan(0)==KEY_UP)
			{
				USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
				Dmax_Enable(DMA1_Channel4,send_buf_len);
				while(1)
					{
						if(DMA_GetFlagStatus(DMA1_FLAG_TC4)!=0)//判断通道4传输完成
						{
							DMA_ClearFlag(DMA1_FLAG_TC4);
							break;
						}
						led1=!led1;
						delay_ms(300);	
					}
			}			
			delay_ms(10);
		}
	
}

