#include "key.h"

void Key_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_3|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
}
/*******************************************************************************
* 函 数 名         : KEY_Scan
* 函数功能		   : 按键扫描检测
* 输    入         : mode=0:单次按下按键
					 mode=1：连续按下按键
* 输    出         : 0：未有按键按下
					 KEY_UP：K_UP键按下
					 KEY_DOWN：K_DOWN键按下
					 KEY_LEFT：K_LEFT键按下
					 KEY_RIGHT：K_RIGHT键按下
*******************************************************************************/
u8 KEY_Scan(u8 mode)
{
	static u8 key=1;
	if(key==1&&(key_UP==1||key0==0||key1==0||key2==0)) //任意一个按键按下
	{
		delay_ms(10);  //消抖
		key=0;
		if(key_UP==1)
		{
			return KEY_UP; 
		}
		else if(key1==0)
		{
			return KEY_DOWN; 
		}
		else if(key2==0)
		{
			return KEY_LEFT; 
		}
		else
		{
			return KEY_RIGHT;
		}
	}
	else if(key_UP==0&&key0==1&&key1==1&&key2==1)    //无按键按下
	{
		key=1;
	}
	if(mode==1) //连续按键按下
	{
		key=1;
	}
	return 0;
}
