#include "wkup.h"


//进入待机模式
void Enter_Standby_Mode()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//使能电源外设时钟
	PWR_ClearFlag(PWR_FLAG_WU);	//清除唤醒标志
	PWR_WakeUpPinCmd(ENABLE); //使能唤醒源
	PWR_EnterSTANDBYMode();//进入待机模式
}
