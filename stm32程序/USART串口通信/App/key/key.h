#ifndef _key_H
#define _key_H

#include "system.h"


//#define key0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键0
//#define key1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键1
//#define key2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//读取按键2 
//#define key_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP)

#define key0 		PEin(4)
#define key1 		PEin(3)
#define key2 		PEin(2)
#define key_UP 	PAin(0)

void Key_Init(void);




#endif

