
#include "new_ultrasonic.h"


#define SCL_0 SONIC_I2C_PORT->BRR=SONIC_SCL
#define SCL_1 SONIC_I2C_PORT->BSRR=SONIC_SCL
#define SDA_0 SONIC_I2C_PORT->BRR=SONIC_SDA
#define SDA_1 SONIC_I2C_PORT->BSRR=SONIC_SDA
#define SDA_STATE ((SONIC_I2C_PORT->IDR) & SONIC_SDA)
#define SCL_STATE ((SONIC_I2C_PORT->IDR) & SONIC_SCL)
#define NOP _delay_us(25)

#define BUS_BUSY 1
#define READY 0
#define ACK 1
#define NACK 0

void sonic_i2c_init_m(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
  GPIO_InitStructure.GPIO_Pin = SONIC_SDA;
  GPIO_Init(SONIC_I2C_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SONIC_SCL;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Init(SONIC_I2C_PORT, &GPIO_InitStructure);
  
  //SDA_1;
  //SCL_1;  
  
}

void START(void)
{ 
 SDA_1; 
 NOP;
   
 SCL_1; 
 NOP;    

 SDA_0;
 NOP;
  
 SCL_0;  
 NOP; 

}

void STOP(void)
{
 SDA_0; 
 NOP;
   
 SCL_1; 
 NOP;    

 SDA_1;
 NOP;
   
}

void I2C_Ack(void)
{	
	SCL_0;
	NOP;
	SDA_0;
	NOP;
	SCL_1;
	NOP;
	SCL_0;
	NOP;
}

void SendNACK(void)
{
 SCL_0;
 NOP;
 SDA_1;
 SCL_1;
 NOP;
 SCL_0; 
 NOP; 
 //////DebugPrint("SendNACK\n");    
}

void ReceiveAck(void)
{
  SCL_1;
  while (SDA_STATE);//(SDA_IN == 0x01 /*sda==1*/));
  SCL_0;
  NOP;
}

void WriteByte(u8 Data)
{
 u8 i;

 for(i=0;i<8;i++)
 {  
	  SCL_0;
	  NOP;
	  if(Data&0x80) SDA_1;
	  else SDA_0;

	  NOP;

	  SCL_1;
	  NOP;
	  Data<<=1;
 }

// SDA_1; 
 //NOP;
 
 SCL_0;
  NOP;   
}

u8 ReceiveByte(void)
{
 u8 i,data=0,temp;
 SCL_0; 
 NOP;
 SDA_1;
 
 for(i=0;i<8;i++)
 {
  NOP;  
  SCL_1;
  NOP; 
  //data<<=1;
  if(SDA_STATE) 
  {
   temp=1;
  }  
	else temp=0;
	data=(data<<1)|temp;
  SCL_0;
        
 }
 NOP;
 
 return data;
}

u8 read_reg(u8 reg)
{
  u8 Data  = 0;
  START();
  WriteByte(SONIC_ADDR & 0xfe);
  ReceiveAck();
  WriteByte(reg);
  ReceiveAck();
  START();
  WriteByte(SONIC_ADDR | 0x01);
  ReceiveAck();
  Data  = ReceiveByte();
  SendNACK();
 
  STOP();
  
  return(Data);
}

u32 sonic_detect_m(u8 range)
{
	u32 range0=0,range1=0;
	
	//start
	START();
	//Slave addr,write
	WriteByte(SONIC_ADDR & 0xfe);
	//wait for ACK
	ReceiveAck();

	//Reg addr
	WriteByte(Cmd_Reg);
	//wait for ACK
	ReceiveAck();
	
	//Cmd
	WriteByte(range);
	//NACK
	//SendNACK();
	ReceiveAck();

	//STOP
	STOP();

	_delay_ms(1);
//	while(SCL_STATE);

	while(!SCL_STATE);

	//start
	START();

	//Slave addr,read
	WriteByte(SONIC_ADDR | 0x01);
	//wait for ACK
	ReceiveAck();

	//read 1st
	range0=ReceiveByte();
	//send ACK
	I2C_Ack();

	//read 2nd
	range1=ReceiveByte();
	//send ACK
	SendNACK();
	//STOP
	STOP();
	
	return (range0<<8)+range1;
	
	
	
}
