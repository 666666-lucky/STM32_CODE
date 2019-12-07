#include "iwdg.h"

void Iwdg_Init(u8 pre,u16 reload)		//独立看门狗复位时间计算公式：t=(4*2^pre*rlr)/40
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);		//取消寄存器写保护
	IWDG_SetPrescaler(pre);		//设置分频系数
	IWDG_SetReload(reload);		//重装载值设置
	IWDG_ReloadCounter();			//重装载
	IWDG_Enable();						//使能
}

