#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "malloc.h"
#include "includes.h"
#include "tftlcd.h"
#include "key.h"
#include "sram.h"
#include "beep.h"

//UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()



#define START_TASK_PRIO		3				//�������ȼ�
#define START_STK_SIZE 		128				//�����ջ��С	
OS_TCB StartTaskTCB;						//������ƿ�
CPU_STK START_TASK_STK[START_STK_SIZE];		//�����ջ	
void start_task(void *p_arg);				//��������

#define Main_TASK_PRIO		4				//�������ȼ�
#define Main_STK_SIZE 		128				//�����ջ��С	
OS_TCB  MainTaskTCB;						//������ƿ�
CPU_STK Main_TASK_STK[Main_STK_SIZE];		//�����ջ	
void Main_task(void *p_arg);				//��������

#define KEYPROCESS_TASK_PRIO		5		//�������ȼ�
#define KEYPROCESS_STK_SIZE 		128		//�����ջ��С	
OS_TCB  KeyProcessTaskTCB;					//������ƿ�
CPU_STK KEYPROCESS_TASK_STK[Main_STK_SIZE];	//�����ջ	
void KeyProcess_task(void *p_arg);			//��������

#define MSGDIS_TASK_PRIO		6		//�������ȼ�
#define MSGDIS_STK_SIZE 		128		//�����ջ��С	
OS_TCB  MsgdisTaskTCB;					//������ƿ�
CPU_STK MSGDIS_TASK_STK[Main_STK_SIZE];	//�����ջ	
void Msgdis_task(void *p_arg);			//��������

//=============�����ʱ��===================//
OS_TMR 	tmr1;		//��ʱ��1
u8 tmr1sta=0;		//״̬���
void tmr1_callback(void *p_tmr, void *p_arg); 	//��ʱ��1�ص�����


//==============��Ϣ����===================//
OS_Q KEY_Msg;				//������Ϣ����
OS_Q DATA_Msg;				//������Ϣ����


//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

//����������
void ucos_load_main_ui(void)
{
	FRONT_COLOR = RED;
	LCD_ShowString(10,10,200,16,16,"ALIENTEK STM32F1");	
	LCD_ShowString(10,30,200,16,16,"UCOSIII Examp 11-1");
	LCD_ShowString(10,50,200,16,16,"Message Queue");
	LCD_ShowString(10,70,220,16,16,"KEY_UP:LED1 KEY0:Refresh LCD");
	LCD_ShowString(10,90,200,16,16,"KEY1:Tmr1 KEY2:BEEP");
	
	FRONT_COLOR = BLACK;
	LCD_DrawLine(0,107,239,107);		//����
	LCD_DrawLine(119,107,119,319);		//����
	LCD_DrawRectangle(125,110,234,314);	//������
	FRONT_COLOR = RED;
	LCD_ShowString(0,130,100,16,16,"tmr1 state:");
	LCD_ShowString(0,170,120,16,16,"DATA_Msg Size:");
	LCD_ShowString(0,210,120,16,16,"DATA_Msg rema:");
	LCD_ShowString(0,250,100,16,16,"DATA_Msg:");
	FRONT_COLOR = BLUE;
	LCD_ShowString(10,150,100,16,16,"TMR1 STOP! ");
}						
//��ѯDATA_Msg��Ϣ�����е��ܶ���������ʣ���������
void check_msg_queue(u8 *p)
{
	CPU_SR_ALLOC();
	u8 msgq_remain_size;	//��Ϣ����ʣ���С
	OS_CRITICAL_ENTER();	//�����ٽ��
	msgq_remain_size = DATA_Msg.MsgQ.NbrEntriesSize-DATA_Msg.MsgQ.NbrEntries;
	p = mymalloc(SRAMIN,20);	//�����ڴ�
	sprintf((char*)p,"Total Size:%d",DATA_Msg.MsgQ.NbrEntriesSize);	//��ʾDATA_Msg��Ϣ�����ܵĴ�С
	LCD_ShowString(10,190,100,16,16,p);
	sprintf((char*)p,"Remain Size:%d",msgq_remain_size);	//��ʾDATA_Msgʣ���С
	LCD_ShowString(10,230,100,16,16,p);
	myfree(SRAMIN,p);		//�ͷ��ڴ�
	OS_CRITICAL_EXIT();		//�˳��ٽ��
}
						
						
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();		//�����ٽ����������
	
	delay_init();       //��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
	uart_init(115200);    //���ڲ���������
	LED_Init();         //LED��ʼ��
	KEY_Init();			//������ʼ��
	TFTLCD_Init();		//LCD��ʼ��
	BEEP_Init();		//��ʼ��������
	FSMC_SRAM_Init();	//��ʼ��SRAM
	my_mem_init(SRAMIN);//��ʼ���ڲ�RAM
	ucos_load_main_ui();//������UI
	

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
	
		//������Ϣ����KEY_Msg
	OSQCreate ((OS_Q*		)&KEY_Msg,	//��Ϣ����
                (CPU_CHAR*	)"KEY Msg",	//��Ϣ��������
                (OS_MSG_QTY	)1,			//��Ϣ���г��ȣ���������Ϊ1
                (OS_ERR*	)&err);		//������
	//������Ϣ����DATA_Msg
	OSQCreate ((OS_Q*		)&DATA_Msg,	
                (CPU_CHAR*	)"DATA Msg",	
                (OS_MSG_QTY	)4,	
                (OS_ERR*	)&err);	
				
	//����Main����
	OSTaskCreate((OS_TCB 	* )&MainTaskTCB,		
				 (CPU_CHAR	* )"Main task", 		
                 (OS_TASK_PTR )Main_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )Main_TASK_PRIO,     
                 (CPU_STK   * )&Main_TASK_STK[0],	
                 (CPU_STK_SIZE)Main_STK_SIZE/10,	
                 (CPU_STK_SIZE)Main_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	//����KeyProcess����
	OSTaskCreate((OS_TCB 	* )&KeyProcessTaskTCB,		
				 (CPU_CHAR	* )"KeyProcess task", 		
                 (OS_TASK_PTR )KeyProcess_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )KEYPROCESS_TASK_PRIO,     
                 (CPU_STK   * )&KEYPROCESS_TASK_STK[0],	
                 (CPU_STK_SIZE)KEYPROCESS_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEYPROCESS_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	//����Msgdis����
	OSTaskCreate((OS_TCB 	* )&MsgdisTaskTCB,		
				 (CPU_CHAR	* )"Msgdis task", 		
                 (OS_TASK_PTR )Msgdis_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MSGDIS_TASK_PRIO,     
                 (CPU_STK   * )&MSGDIS_TASK_STK[0],	
                 (CPU_STK_SIZE)MSGDIS_STK_SIZE/10,	
                 (CPU_STK_SIZE)MSGDIS_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				 
				 
	//������ʱ��1
	 OSTmrCreate((OS_TMR*				)&tmr1,			//��ʱ��������ƿ�
                 (CPU_CHAR*				)"tmr1",		//��ʱ������
                 (OS_TICK				)0,				//0ms
                 (OS_TICK				)50,			//50mS
                 (OS_OPT				)OS_OPT_TMR_PERIODIC,//����ģʽ
                 (OS_TMR_CALLBACK_PTR	)tmr1_callback,	//�ص�������
                 (void* 				)0,				//����Ϊ0
                 (OS_ERR*				)&err);		    //���صĴ�����
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);		
}

//led0������
void Main_task(void *p_arg)
{
	u8 key,num;
	OS_ERR err;
	u8 *p;
	while(1)
	{
		key=KEY_Scan(0);
		if(key)
		{
			OSQPost ((OS_Q*			)   &KEY_Msg,
					 (void*			)   &key,
					 (OS_MSG_SIZE	)   1,
					 (OS_OPT		)   OS_OPT_POST_FIFO,
					 (OS_ERR*		)   &err);
		}
		num++;
		if(num%10==0) check_msg_queue(p);//���DATA_Msg��Ϣ���е�����
		if(num==50)
		{
			num=0;
			LED0=~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);
	}
}

void KeyProcess_task(void *p_arg)
{
	OS_ERR err;
	u8 *key,num;
	OS_MEM_SIZE size;
	while(1)
	{
	key=OSQPend((OS_Q*			)&KEY_Msg,   			//��Ϣ����
				(OS_TICK		)0,						//�ȴ�ʱ��
				(OS_OPT			)OS_OPT_PEND_BLOCKING,	//�Ƿ�ѡ������ģʽ
				(OS_MSG_SIZE*	)&size,					//��Ϣ����
				(CPU_TS*		)0,						//ʱ���
				(OS_ERR*		)&err);
	switch(*key)
		{
			case WKUP_PRES:     //��key_up���µĻ��򿪶�ʱ��1
					LED1=~LED1;
				break;
			
			case KEY0_PRES:		//��key0���µĻ��򿪶�ʱ��2
				num++;
				LCD_Fill(126,111,233,313,lcd_discolor[num%14]);
				break;
			
			case KEY2_PRES:		//��key1���»��͹رն�ʱ��
					BEEP=~BEEP;
				break;
			
			case KEY1_PRES:		//��key1���»��͹رն�ʱ��
				tmr1sta = !tmr1sta;
				if(tmr1sta) 
				{
					OSTmrStart(&tmr1,&err);
					LCD_ShowString(10,150,100,16,16,"TMR1 START!");
				}
				else
				{
					OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //ֹͣ��ʱ��1
					LCD_ShowString(10,150,100,16,16,"TMR1 STOP! ");
				}
				break;
				
			default :
				
			break;
		}
	}
}

//��ʾ��Ϣ�����е���Ϣ
void Msgdis_task(void *p_arg)
{
	u8 *p;
	OS_MSG_SIZE size;
	OS_ERR err;
	while(1)
	{
		p=OSQPend((OS_Q*		)&DATA_Msg,   			//��Ϣ����
				 (OS_TICK		)0,						//�ȴ�ʱ��
				 (OS_OPT		)OS_OPT_PEND_BLOCKING,	//�Ƿ�ѡ������ģʽ
				 (OS_MSG_SIZE*	)&size,					//��Ϣ����
				 (CPU_TS*		)0,						//ʱ���
				 (OS_ERR*		)&err);
		LCD_ShowString(5,270,100,16,16,p);
		myfree(SRAMIN,p);	//�ͷ��ڴ�
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //��ʱ1s
	}
}

void tmr1_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	u8 *pbuf;
	static u8 msg_num;
	pbuf = mymalloc(SRAMIN,10);	//����10���ֽ�
	if(pbuf)	//�����ڴ�ɹ�
	{
		msg_num++;
		sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
		//������Ϣ
		OSQPost((OS_Q*		)&DATA_Msg,		
				(void*		)pbuf,
				(OS_MSG_SIZE)10,
				(OS_OPT		)OS_OPT_POST_FIFO,
				(OS_ERR*	)&err);
		if(err != OS_ERR_NONE)
		{
			printf("�������%d",err);
			myfree(SRAMIN,pbuf);	//�ͷ��ڴ�
			OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //ֹͣ��ʱ��1
			tmr1sta = !tmr1sta;
			LCD_ShowString(10,150,100,16,16,"TMR1 STOP! ");
		}
	}
}

