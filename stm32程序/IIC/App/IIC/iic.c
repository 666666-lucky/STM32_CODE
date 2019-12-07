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


void I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	/*STM32F103C8T6оƬ��Ӳ��I2C: PB6 -- SCL; PB7 -- SDA */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//I2C���뿪©���
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_DeInit(I2C1);//ʹ��I2C1
//	I2C_DeInit(I2C2);//ʹ��I2C1
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x30;//������I2C��ַ,���д��
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
	
	I2C_GenerateSTART(I2C1, ENABLE);//����I2C1
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,��ģʽ*/

	I2C_Send7bitAddress(I2C1, 0xE8, I2C_Direction_Transmitter);//������ַ -- Ĭ��0x78
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, addr);//�Ĵ�����ַ
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1, data);//��������
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);//�ر�I2C1����
}
