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
//	printf("ϵͳ����\r\n");
//	delay_ms(200);
//	printf("����%s\r\n",test1);
	write_byte(KS10xx_ADDR,2,0x71); 							//�������� ����USB����
	write_byte(KS10xx_ADDR, 2,0x8a); 							//����I2Cͨ���Ƿ�ɹ�[���Ƚ�������Ȼ��ָʾ]
	delay_ms(500);
	write_byte(KS10xx_ADDR,2,0xc0); 							//��LED̽����ʾ
	while(1)
		{
			range = detect(KS10xx_ADDR,0xcc);
			printf("����%d",range);

			led0=!led0;			
			delay_ms(1000);
		}
	
}

