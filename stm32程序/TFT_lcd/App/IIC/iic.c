#include "iic.h"

void IIC_Init()
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB,&GPIO_InitStructure);//6sclʱ��7sda	
	
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

//������������
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

//����ֹͣ����
void IIC_Stop(void)
{
    SDA_OUT();  
		IIC_SDA_Out = 0;
    IIC_SCL = 1;
    delay_us(5);
    IIC_SDA_Out = 1;
     delay_us(5);
}

//����Ӧ��
void IIC_SendAck(u8 ackbit)// 0��Ӧ��1����Ӧ��
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

//�ȴ�Ӧ��
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

//ͨ��I2C���߷�������
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

//��I2C�����Ͻ�������
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

