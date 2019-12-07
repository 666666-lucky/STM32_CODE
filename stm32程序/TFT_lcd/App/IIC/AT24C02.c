#include "AT24C02.h"

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
