#include "AT24C02.h"
#include "usart.h"
void At24c02_Init()
{
	IIC_Init();
}

void At24c02_Write(u8 addr,u8 *Sbuffer,u8 len)
{
	u8 i;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	for(i=0;i<len;i++)
	{
		IIC_SendByte(*Sbuffer);
		IIC_WaitAck();
		if(i!=len)
			Sbuffer++;
	}
	IIC_Stop();
	delay_ms(10);
}

void At24c02_Read(u8 addr,u8 *sbuff,u8 len)
{
	u8 i;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	for(i=0;i<len;i++)
	{
		*sbuff=IIC_RecByte();	
		if(i==len-1)
			IIC_SendAck(1);
		else
		{
			IIC_SendAck(0);
			sbuff++;
		}
	}
	IIC_Stop();
	delay_ms(10);
}

void write_byte(unsigned char address,unsigned char reg,unsigned char command) 		//address+register+command
{
	IIC_Start();
	IIC_SendByte(address);
	if(IIC_WaitAck())
		printf("写错误\r\n");
	IIC_SendByte(reg);
	IIC_WaitAck();
	IIC_SendByte(command);
	IIC_WaitAck();
	IIC_Stop();		
	delay_ms(10);
}
unsigned char read_byte(unsigned char address,unsigned char reg) //address(with bit 0 set) + register
{
	unsigned char dat;
	IIC_Start();
	IIC_SendByte(address);
	
	if(IIC_WaitAck()==0)
		printf("写错误\r\n");
	IIC_SendByte(reg);
	IIC_WaitAck();
	
	IIC_Start();
	IIC_SendByte(address+1);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	
	return dat;
}

unsigned int detect(unsigned char address,unsigned char command)
{
	unsigned int distance,count;
	write_byte(address,2,command); 		//use command "0xb0" to detect the distance
	delay_ms(1); 						//安全延时,如果显示不清晰可以将延时调大一些
	//delayms(80); 						//如果是探测温度此处延时需根据表1 所列时间相应延长
	count = 1000;
	while(--count || !IIC_SCL) 				//等待探测结束，count 值调小将减小探测等待时间
	{
		; // 空语句
		//display(range); 				//显示语句，可根据需要保留或删除
	}
	// while(!SCL)display(range); //you can delete “display(range)”
	//通过查询SCL 线来智能识别探测是否结束，使用本语句可删除上条语句(count=1000;while…)以节省探测时间
	distance = read_byte(address,2);
	distance <<= 8;
	distance += read_byte(address,3);
	return distance; 						//return 16 bit distance in millimeter
}
