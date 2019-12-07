#ifndef _mup6050_H
#define _mup6050_H

#include "stm32f10x.h"

#define GPIO_Prot_Mup GPIOA						//GPIO端口定义
#define GPIO_Mup_SDA  GPIO_Pin_5			//SDA引脚定义
#define GPIO_Mup_SCL  GPIO_Pin_3			//SCL引脚定义

#define	Mup_physical_addr				0x68		//物理地址
#define	Mup_power1_addr					0x6b		//电源1地址
#define	Mup_power2_addr					0x6c		//电源2地址




#endif 

