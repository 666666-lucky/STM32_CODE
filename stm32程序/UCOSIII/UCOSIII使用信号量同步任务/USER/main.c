#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "includes.h"
#include "tftlcd.h"
#include "key.h"
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
#define START_STK_SIZE 		64				//任务堆栈大小	
OS_TCB StartTaskTCB;						//任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];		//任务堆栈	
void start_task(void *p_arg);				//任务函数

#define LCD1_TASK_PRIO		4				//任务优先级
#define LCD1_STK_SIZE 		64				//任务堆栈大小	
OS_TCB  LCD1TaskTCB;						//任务控制块
CPU_STK LCD1_TASK_STK[LCD1_STK_SIZE];		//任务堆栈	
void LCD1_task(void *p_arg);				//任务函数

#define TASK2_TASK_PRIO		5				//任务优先级
#define TASK2_STK_SIZE 		64				//任务堆栈大小	
OS_TCB  TASK2TaskTCB;						//任务控制块
CPU_STK TASK2_TASK_STK[LCD1_STK_SIZE];		//任务堆栈	
void TASK2_task(void *p_arg);				//任务函数

//=============软件定时器===================//
//OS_TMR 	tmr1;		//定时器1
//OS_TMR	tmr2;		//定时器2
//void tmr1_callback(void *p_tmr, void *p_arg); 	//定时器1回调函数
//void tmr2_callback(void *p_tmr, void *p_arg);	//定时器2回调函数

//=============信号量===================//
OS_SEM SYNC_SEM;						//同步信号


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
	KEY_Init();
	TFTLCD_Init();		//LCD初始化
	
	FRONT_COLOR = BLACK;	
	LCD_DrawRectangle(5,110,234,314);	
	LCD_DrawLine(5,130,234,130);
	FRONT_COLOR = RED;
	LCD_ShowString(30,111,200,16,16,"SYNC_SEM Value:  0");
	FRONT_COLOR = BLUE;
	
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
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	//创建信号量处理任务
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
				 
	//创建信号量			 
	OSSemCreate ((OS_SEM*		)&SYNC_SEM,
                 (CPU_CHAR*		)"SYNC_SEM",
                 ( OS_SEM_CTR 	)0,
                 (OS_ERR* 		)&err);
				 
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);
				 
}

//led0任务函数
void LCD1_task(void *p_arg)
{
	u8 key,num;
	OS_ERR err;
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case WKUP_PRES:     //当key_up按下的话打开定时器1				
				OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//发送信号量
				LCD_ShowxNum(150,111,SYNC_SEM.Ctr,3,16,0);	//显示信号量值
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
		LCD_ShowxNum(150,111,SYNC_SEM.Ctr,3,16,0);			//显示信号量值
		LCD_Fill(6,131,233,313,lcd_discolor[num%14]);		//刷屏
		LED1 = ~LED1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);   //延时1s
	}
}



