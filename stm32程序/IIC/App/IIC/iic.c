#include "iic.h"

void IIC_Init()
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB,&GPIO_InitStructure);//6scl时钟7sda	
	
	IIC_SCL=1;
	IIC_SDA_Out=1;
}

void SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

//总线启动条件
void IIC_Start(void)
{
		SDA_OUT();  
		IIC_SDA_Out = 1;
    IIC_SCL = 1;
    delay_us(5);
    IIC_SDA_Out = 0;
		delay_us(5);
    IIC_SCL = 0;	
}

//总线停止条件
void IIC_Stop(void)
{
    SDA_OUT();  
		IIC_SDA_Out = 0;
    IIC_SCL = 1;
    delay_us(5);
    IIC_SDA_Out = 1;
     delay_us(5);
}

//发送应答
void IIC_SendAck(u8 ackbit)// 0：应答，1：非应答
{
    SDA_OUT();
		IIC_SCL = 0;
    IIC_SDA_Out = ackbit&0x01;  					
    delay_us(5);
    IIC_SCL = 1;
    delay_us(5);
    IIC_SCL = 0; 
    IIC_SDA_Out = 1;
		delay_us(5);
}

//等待应答
u8 IIC_WaitAck(void)
{
    u8 ackbit;
		SDA_IN(); 
    IIC_SCL  = 1;
    delay_us(5);
    ackbit = IIC_SDA_In;
    IIC_SCL = 0;
    delay_us(5);
    return ackbit;
}

//通过I2C总线发送数据
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;
		SDA_OUT();
    for(i=0; i<8; i++)
    {
        IIC_SCL  = 0;
        delay_us(5);
        if(byt & 0x80) IIC_SDA_Out  = 1;
        else IIC_SDA_Out  = 0;
				delay_us(5);
        IIC_SCL = 1;
        byt <<= 1;
        delay_us(5);
    }
    IIC_SCL  = 0;  
}

//从I2C总线上接收数据
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
		SDA_IN();
    for(i=0; i<8; i++)
    {   
    	IIC_SCL = 1;
			delay_us(5);
			da <<= 1;
			if(IIC_SDA_In) da |= 1;
			IIC_SCL = 0;
			delay_us(5);
		}
    return da;    
}


void I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	/*STM32F103C8T6芯片的硬件I2C: PB6 -- SCL; PB7 -- SDA */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//I2C必须开漏输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_DeInit(I2C1);//使用I2C1
//	I2C_DeInit(I2C2);//使用I2C1
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x30;//主机的I2C地址,随便写的
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 200000;//400K

	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
//	I2C_Cmd(I2C2, ENABLE);
//	I2C_Init(I2C2, &I2C_InitStructure);
}
void I2C_WriteByte(uint8_t addr,uint8_t data)
{
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1, ENABLE);//开启I2C1
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,主模式*/

	I2C_Send7bitAddress(I2C1, 0xE8, I2C_Direction_Transmitter);//器件地址 -- 默认0x78
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, addr);//寄存器地址
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1, data);//发送数据
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);//关闭I2C1总线
}
