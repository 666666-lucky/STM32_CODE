#include "system.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"

int main()
{
	u8 i=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	Usart1_Init(9600);
	SysTick_Init(72);	
	TFTLCD_Init();	
	printf("系统启动\r\n");
	FRONT_COLOR=BRED;
	
//	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,12,"Hello World!");
//	LCD_ShowString(10,30,tftlcd_data.width,tftlcd_data.height,16,"Hello World!");
//	LCD_ShowString(10,50,tftlcd_data.width,tftlcd_data.height,24,"Hello World!");
//	LCD_ShowString(0,0,tftlcd_data.width,tftlcd_data.height,16,"good!");
//	printf("tftlcd_data.width=%d\r\ntftlcd_data.height=%d\r\n",tftlcd_data.width,tftlcd_data.height);
//	LCD_ShowFontHZ(10, 80,"普中科技");
//	LCD_ShowString(10,120,tftlcd_data.width,tftlcd_data.height,24,"www.prechin.cn");
	
//	LCD_Set_Window(100,150,30,50);
	LCD_DrawLine_Color(100,150,240,150,GRED);
	LCD_DrowSign(120,300,BLUE);
	LCD_DrawRectangle(60,200,120,150);
	LCD_Draw_Circle(120,160,60);
	LCD_ShowxNum(30,20,2553939,7,24,0x81);
	while(1)
		{
			i++;
			if(i%20==0)
				led0=!led0;			
			delay_ms(10);
		}
	
}

