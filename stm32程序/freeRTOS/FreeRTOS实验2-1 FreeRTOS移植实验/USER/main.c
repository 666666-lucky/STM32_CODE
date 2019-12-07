#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
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

//�������ȼ�
#define TIM4_TASK_PRIO		2
//�����ջ��С	
#define TIM4_STK_SIZE 		50  
//������
TaskHandle_t TIM4Task_Handler;
//������
void TIM4_task(void *pvParameters);

//�������ȼ�
#define TIM5_TASK_PRIO		3
//�����ջ��С	
#define TIM5_STK_SIZE 		50  
//������
TaskHandle_t TIM5Task_Handler;
//������
void TIM5_task(void *pvParameters);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(9600);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
	Time4_Init_ms(1000);
	Time5_Init_ms(1000);
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
    //����TIM4����
    xTaskCreate((TaskFunction_t )TIM4_task,     	
                (const char*    )"TIM4_task",   	
                (uint16_t       )TIM4_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TIM4_TASK_PRIO,	
                (TaskHandle_t*  )&TIM4Task_Handler);   
    //����LED1����
    xTaskCreate((TaskFunction_t )TIM5_task,     
                (const char*    )"TIM5_task",   
                (uint16_t       )TIM5_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TIM5_TASK_PRIO,
                (TaskHandle_t*  )&TIM5Task_Handler);         
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LED0������ 
void TIM4_task(void *pvParameters)
{
	u8 led0num=0;
    while(1)
    {
        led0num++;
		if(led0num==5)
		{
			printf("�ر��ж�.............\r\n");
			portDISABLE_INTERRUPTS();				//�ر��ж�
			delay_xms(5000);						//��ʱ5s
			printf("���ж�.............\r\n");	//���ж�
			portENABLE_INTERRUPTS();

		}
//		printf("TASK1 Running %d\r\n",led0num);
		LED0=~LED0;
        vTaskDelay(500);		
    }
}   

//LED1������
void TIM5_task(void *pvParameters)
{
	u8 led2num=0;
    while(1)
    {
        led2num++;
//		printf("TASK2 Running %d\r\n",led2num);
		LED1=0;
        vTaskDelay(200);
        LED1=1;
        vTaskDelay(800);
		
    }
}
