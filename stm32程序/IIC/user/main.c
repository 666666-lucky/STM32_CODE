#include "system.h"
#include "led.h"
#include "usart.h"
#include "AT24C02.h"
#define KS10xx_ADDR 0xE8
u8 test[]="abcdefg";
u8 test1[7];
int main()
{
	u8 i=0;
	u16 range;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	Usart1_Init(9600);
	SysTick_Init(72);
	IIC_Init();	
//	At24c02_Init();
//	At24c02_Write(0,test,7);
//	
//	At24c02_Read(0,test1,7);
//	printf("系统启动\r\n");
//	delay_ms(200);
//	printf("测试%s\r\n",test1);
	write_byte(KS10xx_ADDR,2,0x71); 							//二级降噪 适用USB供电
	write_byte(KS10xx_ADDR, 2,0x8a); 							//测试I2C通信是否成功[灯先渐渐亮，然后指示]
	delay_ms(500);
	write_byte(KS10xx_ADDR,2,0xc0); 							//开LED探测显示
	while(1)
		{
			range = detect(KS10xx_ADDR,0xcc);
			printf("距离%d",range);

			led0=!led0;			
			delay_ms(1000);
		}
	
}

