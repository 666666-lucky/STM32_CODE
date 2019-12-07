#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "time.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 FreeRTOS实验2-1
 FreeRTOS移植实验-库函数版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


#define START_TASK_PRIO		1			//任务优先级	
#define START_STK_SIZE 		128  		//任务堆栈大小
TaskHandle_t StartTask_Handler;			//任务句柄
void start_task(void *pvParameters);	//任务函数

//任务优先级
#define TIM4_TASK_PRIO		2
//任务堆栈大小	
#define TIM4_STK_SIZE 		50  
//任务句柄
TaskHandle_t TIM4Task_Handler;
//任务函数
void TIM4_task(void *pvParameters);

//任务优先级
#define TIM5_TASK_PRIO		3
//任务堆栈大小	
#define TIM5_STK_SIZE 		50  
//任务句柄
TaskHandle_t TIM5Task_Handler;
//任务函数
void TIM5_task(void *pvParameters);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	  
	uart_init(9600);					//初始化串口
	LED_Init();		  					//初始化LED
	Time4_Init_ms(1000);
	Time5_Init_ms(1000);
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TIM4任务
    xTaskCreate((TaskFunction_t )TIM4_task,     	
                (const char*    )"TIM4_task",   	
                (uint16_t       )TIM4_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TIM4_TASK_PRIO,	
                (TaskHandle_t*  )&TIM4Task_Handler);   
    //创建LED1任务
    xTaskCreate((TaskFunction_t )TIM5_task,     
                (const char*    )"TIM5_task",   
                (uint16_t       )TIM5_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TIM5_TASK_PRIO,
                (TaskHandle_t*  )&TIM5Task_Handler);         
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//LED0任务函数 
void TIM4_task(void *pvParameters)
{
	u8 led0num=0;
    while(1)
    {
        led0num++;
		if(led0num==5)
		{
			printf("关闭中断.............\r\n");
			portDISABLE_INTERRUPTS();				//关闭中断
			delay_xms(5000);						//延时5s
			printf("打开中断.............\r\n");	//打开中断
			portENABLE_INTERRUPTS();

		}
//		printf("TASK1 Running %d\r\n",led0num);
		LED0=~LED0;
        vTaskDelay(500);		
    }
}   

//LED1任务函数
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
