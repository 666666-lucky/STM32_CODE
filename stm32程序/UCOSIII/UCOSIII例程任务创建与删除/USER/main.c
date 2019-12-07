#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "includes.h"
#include "tftlcd.h"
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
#define START_STK_SIZE 		128				//�����ջ��С	
OS_TCB StartTaskTCB;						//������ƿ�
CPU_STK START_TASK_STK[START_STK_SIZE];		//�����ջ	
void start_task(void *p_arg);				//������

#define LCD1_TASK_PRIO		4				//�������ȼ�
#define LCD1_STK_SIZE 		128				//�����ջ��С	
OS_TCB  LCD1TaskTCB;						//������ƿ�
CPU_STK LCD1_TASK_STK[LCD1_STK_SIZE];		//�����ջ	
void LCD1_task(void *p_arg);				//������

#define LCD2_TASK_PRIO		5				//�������ȼ�
#define LCD2_STK_SIZE 		128				//�����ջ��С	
OS_TCB  LCD2TaskTCB;						//������ƿ�
CPU_STK LCD2_TASK_STK[LCD2_STK_SIZE];		//�����ջ	
void LCD2_task(void *p_arg);				//������

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
	TFTLCD_Init();		//LCD��ʼ��
	FRONT_COLOR = RED;
	
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
	while(1);
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
	//����LCD2����
	OSTaskCreate((OS_TCB 	* )&LCD2TaskTCB,		
				 (CPU_CHAR	* )"LCD2 task", 		
                 (OS_TASK_PTR )LCD2_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LCD2_TASK_PRIO,     	
                 (CPU_STK   * )&LCD2_TASK_STK[0],	
                 (CPU_STK_SIZE)LCD2_STK_SIZE/10,	
                 (CPU_STK_SIZE)LCD2_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	 		 
	OS_CRITICAL_EXIT();	//�����ٽ���
	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);	
//	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����
	

}

//led0������
void LCD1_task(void *p_arg)
{
	OS_ERR err;
	u8 task1_num=0;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	FRONT_COLOR=BLACK;
	
	OS_CRITICAL_ENTER();
	LCD_DrawRectangle(5,110,115,314);				//��һ������
	LCD_DrawLine(5,130,115,130);					//��һ����
	BACK_COLOR=BLUE;
	LCD_ShowString(6,111,110,16,16,"Task1 Run:");
	OS_CRITICAL_EXIT();
	
	while(1)
	{
		task1_num++;
		LED0= ~LED0;
		printf("����1�Ѿ�ִ�У�%d��\r\n",task1_num);
		if(task1_num==5) 
		{
			OSTaskSuspend((OS_TCB*)&LCD2TaskTCB,&err);	//����1ִ��5�˺�ɾ��������2
			printf("����1��������2!\r\n");
		}
		if(task1_num==10)
		{
			OSTaskResume((OS_TCB*)&LCD2TaskTCB,&err);	//����1ִ��5�˺�ɾ��������2
			printf("����1�ָ�����2!\r\n");
		}
		LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //�������
		LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//��ʾ����ִ�д���
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
	}
}

//led1������
void LCD2_task(void *p_arg)
{
	OS_ERR err;
	u8 task2_num=0;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	FRONT_COLOR=BLACK;
	
	OS_CRITICAL_ENTER();
	LCD_DrawRectangle(125,110,234,314);
	LCD_DrawLine(125,130,234,130);
	BACK_COLOR=BLUE;
	LCD_ShowString(126,111,110,16,16,"Task2 Run:");
	OS_CRITICAL_EXIT();
	
	while(1)
	{
		task2_num++;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
		LED1=~LED1;
		printf("����2�Ѿ�ִ�У�%d��\r\n",task2_num);
		LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //��ʾ����ִ�д���
		LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //�������
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
		OSTimeDly(200,OS_OPT_TIME_DLY,&err);
//		delay_ms(1000);
	}
}

