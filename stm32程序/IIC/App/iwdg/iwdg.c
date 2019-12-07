#include "iwdg.h"

void Iwdg_Init(u8 pre,u16 reload)		//�������Ź���λʱ����㹫ʽ��t=(4*2^pre*rlr)/40
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);		//ȡ���Ĵ���д����
	IWDG_SetPrescaler(pre);		//���÷�Ƶϵ��
	IWDG_SetReload(reload);		//��װ��ֵ����
	IWDG_ReloadCounter();			//��װ��
	IWDG_Enable();						//ʹ��
}

