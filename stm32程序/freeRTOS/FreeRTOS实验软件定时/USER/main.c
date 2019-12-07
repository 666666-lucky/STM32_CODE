#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "time.h"
/************************************************
 ALIENTEK ս��STM32F103������ FreeRTOSʵ��2-1
 FreeRTOS��ֲʵ��-�⺯���汾
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


#define START_TASK_PRIO		1			//�������ȼ�	
#define START_STK_SIZE 		128  		//�����ջ��С
TaskHandle_t StartTask_Handler;			//������
void start_task(void *pvParameters);	//������


#define Times_TASK_PRIO		2			//�������ȼ�	
#define Times_STK_SIZE 		50  		//�����ջ��С
TaskHandle_t TimesTask_Handler;			//������
void Times_task(void *pvParameters);		//������

TimerHandle_t 	AutoReloadTimer_Handle;			//���ڶ�ʱ�����
TimerHandle_t	OneShotTimer_Handle;			//���ζ�ʱ�����
void AutoReloadCallback();//TimerHandle_t xTimer); 	//���ڶ�ʱ���ص�����
void OneShotCallback();//TimerHandle_t xTimer);		//���ζ�ʱ���ص�����



int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(9600);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
	Time5_Init_ms(3000);
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
	
    AutoReloadTimer_Handle=xTimerCreate((const char*		)"AutoReloadTimer",
									    (TickType_t			)1000,
							            (UBaseType_t		)pdTRUE,
							            (void*				)1,
							            (TimerCallbackFunction_t)AutoReloadCallback); //���ڶ�ʱ��������1s(1000��ʱ�ӽ���)������ģʽ
	
	    //�������ζ�ʱ��
	OneShotTimer_Handle=xTimerCreate((const char*			)"OneShotTimer",
							         (TickType_t			)2000,
							         (UBaseType_t			)pdFALSE,
							         (void*					)2,
							         (TimerCallbackFunction_t)OneShotCallback); //���ζ�ʱ��������2s(2000��ʱ�ӽ���)������ģʽ					  
	xTimerStart(AutoReloadTimer_Handle,0);
    //���������ʱ������
    xTaskCreate((TaskFunction_t )Times_task,            //������
                (const char*    )"Times_task",          //��������
                (uint16_t       )Times_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )Times_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&TimesTask_Handler);   //������      
     
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//�����ʱ�������� 
void Times_task(void *pvParameters)
{
//	u8 led0num=0;
    while(1)
    {
//		xTimerStart(AutoReloadTimer_Handle,0);
//		printf("�����ظ���ʱ��\r\n");
		LED0=~LED0;
        vTaskDelay(1000);		
    }
}   

void AutoReloadCallback()//TimerHandle_t xTimer)
{
	static u8 time_num=0;
	time_num++;
	printf("���ڶ�ʱ�����д���%d\r\n",time_num);
	if(time_num%2==0)
	{
		xTimerStart(OneShotTimer_Handle,0);		//�������ζ�ʱ��
		printf("���ζ�ʱ������\r\n");
	}
	
}	

void OneShotCallback()//TimerHandle_t xTimer)	
{
	static u8 time_num=0;
	time_num++;
	printf("���ζ�ʱ�����д���%d\r\n",time_num);
}
