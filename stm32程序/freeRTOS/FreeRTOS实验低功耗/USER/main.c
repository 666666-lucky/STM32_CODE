#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
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


#define Times_TASK_PRIO		2			//任务优先级	
#define Times_STK_SIZE 		50  		//任务堆栈大小
TaskHandle_t TimesTask_Handler;			//任务句柄
void Times_task(void *pvParameters);		//任务函数

TimerHandle_t 	AutoReloadTimer_Handle;			//周期定时器句柄
TimerHandle_t	OneShotTimer_Handle;			//单次定时器句柄



//void PreSleepProcessing(uint32_t xExpectedIdleTime)
//{
//	
//}
//void PostSleepProcessing(uint32_t xExpectedIdleTime)
//{

//}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	  
	uart_init(9600);					//初始化串口
	LED_Init();		  					//初始化LED
//	Time5_Init_ms(3000);
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
	
 
    //创建软件定时器任务
    xTaskCreate((TaskFunction_t )Times_task,            //任务函数
                (const char*    )"Times_task",          //任务名称
                (uint16_t       )Times_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )Times_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&TimesTask_Handler);   //任务句柄      
     
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//软件定时器任务函数 
void Times_task(void *pvParameters)
{
	u8 led0num=0;
    while(1)
    {
		led0num++;
		if(led0num%3==0)
		printf("开启重复定时器\r\n");
		LED0=~LED0;
        vTaskDelay(500);		
    }
}   

	


