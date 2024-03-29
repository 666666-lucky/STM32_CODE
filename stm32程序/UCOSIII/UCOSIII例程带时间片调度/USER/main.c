#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "includes.h"
#include "tftlcd.h"
/************************************************
 ALIENTEK战舰STM32开发板UCOS实验
 例4-1 UCOSIII UCOSIII移植
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK


#define START_TASK_PRIO		3				//任务优先级
#define START_STK_SIZE 		128				//任务堆栈大小	
OS_TCB StartTaskTCB;						//任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];		//任务堆栈	
void start_task(void *p_arg);				//任务函数

#define LCD1_TASK_PRIO		4				//任务优先级
#define LCD1_STK_SIZE 		128				//任务堆栈大小	
OS_TCB  LCD1TaskTCB;						//任务控制块
CPU_STK LCD1_TASK_STK[LCD1_STK_SIZE];		//任务堆栈	
void LCD1_task(void *p_arg);				//任务函数

#define LCD2_TASK_PRIO		4				//任务优先级
#define LCD2_STK_SIZE 		128				//任务堆栈大小	
OS_TCB  LCD2TaskTCB;						//任务控制块
CPU_STK LCD2_TASK_STK[LCD2_STK_SIZE];		//任务堆栈	
void LCD2_task(void *p_arg);				//任务函数

int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };						
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();		//进入临界区所需参数
	delay_init();       //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置
	uart_init(115200);    //串口波特率设置
	LED_Init();         //LED初始化
	TFTLCD_Init();		//LCD初始化
	FRONT_COLOR = RED;
	
	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}
//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区
	//创建LCD1任务
	OSTaskCreate((OS_TCB 	* )&LCD1TaskTCB,		
				 (CPU_CHAR	* )"LCD1 task", 		
                 (OS_TASK_PTR )LCD1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LCD1_TASK_PRIO,     
                 (CPU_STK   * )&LCD1_TASK_STK[0],	
                 (CPU_STK_SIZE)LCD1_STK_SIZE/10,	
                 (CPU_STK_SIZE)LCD1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )3,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);							 
	//创建LCD2任务
	OSTaskCreate((OS_TCB 	* )&LCD2TaskTCB,		
				 (CPU_CHAR	* )"LCD2 task", 		
                 (OS_TASK_PTR )LCD2_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LCD2_TASK_PRIO,     	
                 (CPU_STK   * )&LCD2_TASK_STK[0],	
                 (CPU_STK_SIZE)LCD2_STK_SIZE/10,	
                 (CPU_STK_SIZE)LCD2_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )1,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	 		 
	OS_CRITICAL_EXIT();	//进入临界区
	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);	
//	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务
	

}

//led0任务函数
void LCD1_task(void *p_arg)
{
	OS_ERR err;
	u8 i,task1_num=0;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	FRONT_COLOR=BLACK;
	
	OS_CRITICAL_ENTER();
	LCD_DrawRectangle(5,110,115,314);				//画一个矩形
	LCD_DrawLine(5,130,115,130);					//画一条线
	BACK_COLOR=BLUE;
	LCD_ShowString(6,111,110,16,16,"Task1 Run:");
	OS_CRITICAL_EXIT();
	
	while(1)
	{
		task1_num++;
		LED0= ~LED0;
//		printf("任务1已经执行：%d次\r\n",task1_num);
//		if(task1_num==5) 
//		{
//			OSTaskSuspend((OS_TCB*)&LCD2TaskTCB,&err);	//任务1执行5此后删除掉任务2
//			printf("任务1挂起任务2!\r\n");
//		}
//		if(task1_num==10)
//		{
//			OSTaskResume((OS_TCB*)&LCD2TaskTCB,&err);	//任务1执行5此后删除掉任务2
//			printf("任务1恢复任务2!\r\n");
//		}
		for(i=0;i<100;i++)
		{
			printf("任务1正在运行%d\r\n",i);
			if(i==10)
				OSSchedRoundRobinYield(&err);
				printf("错误代码%d",err);
		}
		LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //填充区域
		LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//显示任务执行次数
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
	}
}

//led1任务函数
void LCD2_task(void *p_arg)
{
	OS_ERR err;
	u8 i,task2_num=0;
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
		task2_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
		LED1=~LED1;
		for(i=0;i<5;i++)
		{
			printf("任务2正在运行%d\r\n",i);
		}
		LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //显示任务执行次数
		LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //填充区域
		OSTimeDly(200,OS_OPT_TIME_DLY,&err);
	}
}

