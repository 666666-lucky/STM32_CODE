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
OS_TMR 	tmr1;		//��ʱ��1
OS_TMR	tmr2;		//��ʱ��2
void tmr1_callback(void *p_tmr, void *p_arg); 	//��ʱ��1�ص�����
void tmr2_callback(void *p_tmr, void *p_arg);	//��ʱ��2�ص�����

//=============�ź���===================//
OS_SEM MY_SEM;

u8 share_resource[30];   //������Դ��

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
	FRONT_COLOR = RED;
	
	LCD_DrawLine(0,108,239,108);		//����
	LCD_DrawLine(119,108,119,319);		//����
	
	FRONT_COLOR = BLACK;
	LCD_DrawRectangle(5,110,115,314); 	//��һ������	
	LCD_DrawLine(5,130,115,130);		//����
	
	LCD_DrawRectangle(125,110,234,314); //��һ������	
	LCD_DrawLine(125,130,234,130);		//����
	BACK_COLOR = BLUE;
	LCD_ShowString(6,111,110,16,16,	 "TIMER1:000");
	LCD_ShowString(126,111,110,16,16,"TIMER2:000");
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
//	while(1);
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
				 
	//������ʱ��1
	 OSTmrCreate((OS_TMR*				)&tmr1,			//��ʱ��������ƿ�
                 (CPU_CHAR*				)"tmr1",		//��ʱ������
                 (OS_TICK				)100,			//��һ��������ʱʱ��100ms
                 (OS_TICK				)200,			//������ʱʱ��1S
                 (OS_OPT				)OS_OPT_TMR_PERIODIC,//����ģʽ
                 (OS_TMR_CALLBACK_PTR	)tmr1_callback,	//�ص�������
                 (void* 				)0,				//����Ϊ0
                 (OS_ERR*				)&err);		    //���صĴ�����
				 
 	//������ʱ��2
	 OSTmrCreate((OS_TMR*				)&tmr2,			//��ʱ��������ƿ�
                 (CPU_CHAR*				)"tmr2",		//��ʱ������
                 (OS_TICK				)200,			//��һ��������ʱʱ��100ms
                 (OS_TICK				)0,			//������ʱʱ��1S
                 (OS_OPT				)OS_OPT_TMR_ONE_SHOT,//����ģʽ
                 (OS_TMR_CALLBACK_PTR	)tmr2_callback,	//�ص�������
                 (void* 				)0,				//����Ϊ0
                 (OS_ERR*				)&err);		    //���صĴ�����
	//�����ź���			 
	OSSemCreate ((OS_SEM*		)&MY_SEM,
                 (CPU_CHAR*		)"MY_SEM",
                 ( OS_SEM_CTR 	)1,
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
				OSTmrStart(&tmr1,&err);	//������ʱ��1
				printf("������ʱ��1\r\n");
				break;
			case KEY0_PRES:		//��key0���µĻ��򿪶�ʱ��2
				OSTmrStart(&tmr2,&err);	//������ʱ��2
				printf("������ʱ��2\r\n");
				break;
			case KEY1_PRES:		//��key1���»��͹رն�ʱ��
				OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err);	//�رն�ʱ��1
				OSTmrStop(&tmr2,OS_OPT_TMR_NONE,0,&err);	//�رն�ʱ��2
				printf("�رն�ʱ��1��2\r\n");
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
	u8 task2_str[]="Second task Running!";
	while(1)
	{		
		printf("\r\n����2:\r\n");
		OSSemPend(&MY_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); 	//�����ź���
		memcpy(share_resource,task2_str,sizeof(task2_str));	//������Դ����������
		delay_ms(300);
		printf("%s\r\n",share_resource);	//�������������Դ������
		LED1 = ~LED1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);   //��ʱ1s
	}
}


void tmr1_callback(void *p_tmr, void *p_arg)
{
	static u8 tmr1_num=0;
	OS_ERR err;
	LCD_ShowxNum(62,111,tmr1_num,3,16,0x80); //��ʾ��ʱ��1��ִ�д���
	LCD_Fill(6,131,114,313,lcd_discolor[tmr1_num%14]);//�������
	tmr1_num++;		//��ʱ��1ִ�д�����1
	OSSemPost (&MY_SEM,OS_OPT_POST_1,&err);				//�����ź���

}

//��ʱ��2�Ļص�����
void tmr2_callback(void *p_tmr,void *p_arg)
{
	static u8 tmr2_num = 0;
	OS_ERR err;
	tmr2_num++;		//��ʱ��2ִ�д�����1
	LCD_ShowxNum(182,111,tmr2_num,3,16,0x80);  //��ʾ��ʱ��1ִ�д���
	LCD_Fill(126,131,233,313,lcd_discolor[tmr2_num%14]); //�������
//	LED0 = ~LED0;
	printf("��ʱ��2���н���\r\n");
	OSSemPost (&MY_SEM,OS_OPT_POST_1,&err);
}
