#include "wkup.h"


//�������ģʽ
void Enter_Standby_Mode()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//ʹ�ܵ�Դ����ʱ��
	PWR_ClearFlag(PWR_FLAG_WU);	//������ѱ�־
	PWR_WakeUpPinCmd(ENABLE); //ʹ�ܻ���Դ
	PWR_EnterSTANDBYMode();//�������ģʽ
}
