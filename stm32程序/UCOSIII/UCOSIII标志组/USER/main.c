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
#define START_STK_SIZE 		128				//任务堆栈大小	
OS_TCB StartTaskTCB;						//任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];		//任务堆栈	
void start_task(void *p_arg);				//任务函数

//#define Main_TASK_PRIO		4				//任务优先级
//#define Main_STK_SIZE 		64				//任务堆栈大小	
//OS_TCB  MainTaskTCB;						//任务控制块
//CPU_STK Main_TASK_STK[Main_STK_SIZE];		//任务堆栈	
//void Main_task(void *p_arg);				//任务函数

//任务优先级
#define MAIN_TASK_PRIO		4
//任务堆栈大小	
#define MAIN_STK_SIZE 		64
//任务控制块
OS_TCB Main_TaskTCB;
//任务堆栈	
CPU_STK MAIN_TASK_STK[MAIN_STK_SIZE];
void main_task(void *p_arg);

#define FlagsProcess_TASK_PRIO		5					//任务优先级
#define FlagsProcess_STK_SIZE 		64					//任务堆栈大小	
OS_TCB  FlagsProcessTaskTCB;							//任务控制块
CPU_STK FlagsProcess_TASK_STK[FlagsProcess_STK_SIZE];	//任务堆栈	
void FlagsProcess_task(void *p_arg);					//任务函数

//=============软件定时器===================//
OS_TMR 	tmr1;		//定时器1
OS_TMR	tmr2;		//定时器2
void tmr1_callback(void *p_tmr, void *p_arg); 	//定时器1回调函数
void tmr2_callback(void *p_tmr, void *p_arg);	//定时器2回调函数

//=============标志组===================//
#define KEY1_FLAG 		0x02
#define KEY0_FLAG 		0x01
#define KEYFLAGS_VALUE 	0x00
OS_FLAG_GRP EventFlags;



int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };	

						
//加载主界面
void ucos_load_main_ui(void)
{
	FRONT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"ALIENTEK STM32F1");	
	LCD_ShowString(30,30,200,16,16,"UCOSIII Examp 12-1");
	LCD_ShowString(30,50,200,16,16,"Event Flags");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2015/3/19");
	FRONT_COLOR = BLACK;
	LCD_DrawRectangle(5,130,234,314);	//画矩形
	FRONT_COLOR = BLUE;
	LCD_ShowString(30,110,220,16,16,"Event Flags Value:0");
}

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
	ucos_load_main_ui();
	
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
//	while(1);
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
		//创建一个事件标志组			 
	OSFlagCreate((OS_FLAG_GRP*)&EventFlags,		//指向事件标志组
                 (CPU_CHAR*	  )"Event Flags",	//名字
                 (OS_FLAGS	  )KEYFLAGS_VALUE,	//事件标志组初始值
                 (OS_ERR*  	  )&err);			//错误码
				 
	//创建主任务
	OSTaskCreate((OS_TCB*     )&Main_TaskTCB,		
				 (CPU_CHAR*   )"Main task", 		
                 (OS_TASK_PTR )main_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )MAIN_TASK_PRIO,     
                 (CPU_STK*    )&MAIN_TASK_STK[0],	
                 (CPU_STK_SIZE)MAIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)MAIN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
		printf("Err %d\r\n",err);
				 
	//创建标志处理任务
	OSTaskCreate((OS_TCB 	* )&FlagsProcessTaskTCB,		
				 (CPU_CHAR	* )"FlagsProcess task", 		
                 (OS_TASK_PTR )FlagsProcess_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )FlagsProcess_TASK_PRIO,     
                 (CPU_STK   * )&FlagsProcess_TASK_STK[0],	
                 (CPU_STK_SIZE)FlagsProcess_STK_SIZE/10,	
                 (CPU_STK_SIZE)FlagsProcess_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	
				 
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);		
}

//led0任务函数
void main_task(void *p_arg)
{
	u8 key,num;
	OS_ERR err;
	OS_FLAGS flags_num;
	while(1)
	{
		key=KEY_Scan(0);
		if(key == KEY0_PRES)
		{
			//向事件标志组EventFlags发送标志
			flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
								 (OS_FLAGS	  )KEY0_FLAG,
								 (OS_OPT	  )OS_OPT_POST_FLAG_SET,
					             (OS_ERR*	  )&err);
			LCD_ShowxNum(174,110,flags_num,1,16,0);
			printf("事件标志组EventFlags的值:%d\r\n",flags_num);
		}
		else if(key == KEY1_PRES)
		{
			//向事件标志组EventFlags发送标志
			flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
								 (OS_FLAGS	  )KEY1_FLAG,
								 (OS_OPT	  )OS_OPT_POST_FLAG_SET,
								 (OS_ERR*     )&err);
			LCD_ShowxNum(174,110,flags_num,1,16,0);
			printf("事件标志组EventFlags的值:%d\r\n",flags_num);
		}		num++;
		if(num==50)
		{
			num=0;
			LED0=~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);
	}
}

void FlagsProcess_task(void *p_arg)
{
	OS_ERR err;
	u8 num;
	while(1)
	{		
		OSFlagPend((OS_FLAG_GRP*)&EventFlags,
				   (OS_FLAGS	)KEY0_FLAG+KEY1_FLAG,
		     	   (OS_TICK     )0,
				   (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
				   (CPU_TS*     )0,
				   (OS_ERR*	    )&err);
		num++;
		LED1 = ~LED1;
		LCD_Fill(6,131,233,313,lcd_discolor[num%14]);
		printf("事件标志组EventFlags的值:%d\r\n",EventFlags.Flags);
		LCD_ShowxNum(174,110,EventFlags.Flags,1,16,0);
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
	}
}



