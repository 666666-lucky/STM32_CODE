#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "includes.h"
#include "tftlcd.h"
#include "key.h"
/************************************************
 ALIENTEKս��STM32������UCOSʵ��
 ��4-1 UCOSIII UCOSIII��ֲ
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK


#define START_TASK_PRIO		3				//�������ȼ�
#define START_STK_SIZE 		64				//�����ջ��С	
OS_TCB StartTaskTCB;						//������ƿ�
CPU_STK START_TASK_STK[START_STK_SIZE];		//�����ջ	
void start_task(void *p_arg);				//������

#define LCD1_TASK_PRIO		4				//�������ȼ�
#define LCD1_STK_SIZE 		64				//�����ջ��С	
OS_TCB  LCD1TaskTCB;						//������ƿ�
CPU_STK LCD1_TASK_STK[LCD1_STK_SIZE];		//�����ջ	
void LCD1_task(void *p_arg);				//������

#define TASK2_TASK_PRIO		5				//�������ȼ�
#define TASK2_STK_SIZE 		64				//�����ջ��С	
OS_TCB  TASK2TaskTCB;						//������ƿ�
CPU_STK TASK2_TASK_STK[LCD1_STK_SIZE];		//�����ջ	
void TASK2_task(void *p_arg);				//������

//=============�����ʱ��===================//
//OS_TMR 	tmr1;		//��ʱ��1
//OS_TMR	tmr2;		//��ʱ��2
//void tmr1_callback(void *p_tmr, void *p_arg); 	//��ʱ��1�ص�����
//void tmr2_callback(void *p_tmr, void *p_arg);	//��ʱ��2�ص�����

//=============�ź���===================//
OS_SEM SYNC_SEM;						//ͬ���ź�


int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };						
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();		//�����ٽ����������
	delay_init();       //��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
	uart_init(115200);    //���ڲ���������
	LED_Init();         //LED��ʼ��
	KEY_Init();
	TFTLCD_Init();		//LCD��ʼ��
	
	FRONT_COLOR = BLACK;	
	LCD_DrawRectangle(5,110,234,314);	
	LCD_DrawLine(5,130,234,130);
	FRONT_COLOR = RED;
	LCD_ShowString(30,111,200,16,16,"SYNC_SEM Value:  0");
	FRONT_COLOR = BLUE;
	
	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
}
//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����LCD1����
	OSTaskCreate((OS_TCB 	* )&LCD1TaskTCB,		
				 (CPU_CHAR	* )"LCD1 task", 		
                 (OS_TASK_PTR )LCD1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LCD1_TASK_PRIO,     
                 (CPU_STK   * )&LCD1_TASK_STK[0],	
                 (CPU_STK_SIZE)LCD1_STK_SIZE/10,	
                 (CPU_STK_SIZE)LCD1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	//�����ź�����������
	OSTaskCreate((OS_TCB 	* )&TASK2TaskTCB,		
				 (CPU_CHAR	* )"TASK2 task", 		
                 (OS_TASK_PTR )TASK2_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK2_TASK_PRIO,     
                 (CPU_STK   * )&TASK2_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK2_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK2_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	//�����ź���			 
	OSSemCreate ((OS_SEM*		)&SYNC_SEM,
                 (CPU_CHAR*		)"SYNC_SEM",
                 ( OS_SEM_CTR 	)0,
                 (OS_ERR* 		)&err);
				 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);
				 
}

//led0������
void LCD1_task(void *p_arg)
{
	u8 key,num;
	OS_ERR err;
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case WKUP_PRES:     //��key_up���µĻ��򿪶�ʱ��1				
				OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//�����ź���
				LCD_ShowxNum(150,111,SYNC_SEM.Ctr,3,16,0);	//��ʾ�ź���ֵ
			break;
			
			default:
				
			break;
		}
		num++;
		if(num==50)
		{
			num=0;
			LED0=~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);
	}
}

void TASK2_task(void *p_arg)
{
	OS_ERR err;
	u8 num=0;
	while(1)
	{		
		OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		num++;
		LCD_ShowxNum(150,111,SYNC_SEM.Ctr,3,16,0);			//��ʾ�ź���ֵ
		LCD_Fill(6,131,233,313,lcd_discolor[num%14]);		//ˢ��
		LED1 = ~LED1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);   //��ʱ1s
	}
}



