#ifndef _iic_H
#define _iic_H

#include "system.h"

#define  IIC_SCL PBout(6)
#define  IIC_SDA_Out PBout(7)
#define  IIC_SDA_In PBin(7)

void IIC_Init(void);
void SDA_OUT(void);
void SDA_IN(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_SendAck(u8 ackbit);// 0£ºÓ¦´ð£¬1£º·ÇÓ¦´ð
u8 IIC_WaitAck(void);
void IIC_SendByte(unsigned char byt);
unsigned char IIC_RecByte(void);


#endif

