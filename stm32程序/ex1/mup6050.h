#ifndef _mup6050_H
#define _mup6050_H

#include "stm32f10x.h"

#define GPIO_Prot_Mup GPIOA						//GPIO�˿ڶ���
#define GPIO_Mup_SDA  GPIO_Pin_5			//SDA���Ŷ���
#define GPIO_Mup_SCL  GPIO_Pin_3			//SCL���Ŷ���

#define	Mup_physical_addr				0x68		//�����ַ
#define	Mup_power1_addr					0x6b		//��Դ1��ַ
#define	Mup_power2_addr					0x6c		//��Դ2��ַ




#endif 

