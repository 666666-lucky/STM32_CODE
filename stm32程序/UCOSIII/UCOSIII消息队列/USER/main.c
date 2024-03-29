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

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()



#define START_TASK_PRIO		3				//任务优先级
#define START_STK_SIZE 		128				//任务堆栈大小	
OS_TCB StartTaskTCB;						//任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];		//任务堆栈	
void start_task(void *p_arg);				//任务函数名

#define Main_TASK_PRIO		4				//任务优先级
#define Main_STK_SIZE 		128				//任务堆栈大小	
OS_TCB  MainTaskTCB;						//任务控制块
CPU_STK Main_TASK_STK[Main_STK_SIZE];		//任务堆栈	
void Main_task(void *p_arg);				//任务函数名

#define KEYPROCESS_TASK_PRIO		5		//任务优先级
#define KEYPROCESS_STK_SIZE 		128		//任务堆栈大小	
OS_TCB  KeyProcessTaskTCB;					//任务控制块
CPU_STK KEYPROCESS_TASK_STK[Main_STK_SIZE];	//任务堆栈	
void KeyProcess_task(void *p_arg);			//任务函数名

#define MSGDIS_TASK_PRIO		6		//任务优先级
#define MSGDIS_STK_SIZE 		128		//任务堆栈大小	
OS_TCB  MsgdisTaskTCB;					//任务控制块
CPU_STK MSGDIS_TASK_STK[Main_STK_SIZE];	//任务堆栈	
void Msgdis_task(void *p_arg);			//任务函数名

//=============软件定时器===================//
OS_TMR 	tmr1;		//定时器1
u8 tmr1sta=0;		//状态标记
void tmr1_callback(void *p_tmr, void *p_arg); 	//定时器1回调函数


//==============消息队列===================//
OS_Q KEY_Msg;				//按键消息队列
OS_Q DATA_Msg;				//数据消息队列


//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

//加载主界面
void ucos_load_main_ui(void)
{
	FRONT_COLOR = RED;
	LCD_ShowString(10,10,200,16,16,"ALIENTEK STM32F1");	
	LCD_ShowString(10,30,200,16,16,"UCOSIII Examp 11-1");
	LCD_ShowString(10,50,200,16,16,"Message Queue");
	LCD_ShowString(10,70,220,16,16,"KEY_UP:LED1 KEY0:Refresh LCD");
	LCD_ShowString(10,90,200,16,16,"KEY1:Tmr1 KEY2:BEEP");
	
	FRONT_COLOR = BLACK;
	LCD_DrawLine(0,107,239,107);		//画线
	LCD_DrawLine(119,107,119,319);		//画线
	LCD_DrawRectangle(125,110,234,314);	//画矩形
	FRONT_COLOR = RED;
	LCD_ShowString(0,130,100,16,16,"tmr1 state:");
	LCD_ShowString(0,170,120,16,16,"DATA_Msg Size:");
	LCD_ShowString(0,210,120,16,16,"DATA_Msg rema:");
	LCD_ShowString(0,250,100,16,16,"DATA_Msg:");
	FRONT_COLOR = BLUE;
	LCD_ShowString(10,150,100,16,16,"TMR1 STOP! ");
}						
//查询DATA_Msg消息队列中的总队列数量和剩余队列数量
void check_msg_queue(u8 *p)
{
	CPU_SR_ALLOC();
	u8 msgq_remain_size;	//消息队列剩余大小
	OS_CRITICAL_ENTER();	//进入临界段
	msgq_remain_size = DATA_Msg.MsgQ.NbrEntriesSize-DATA_Msg.MsgQ.NbrEntries;
	p = mymalloc(SRAMIN,20);	//申请内存
	sprintf((char*)p,"Total Size:%d",DATA_Msg.MsgQ.NbrEntriesSize);	//显示DATA_Msg消息队列总的大小
	LCD_ShowString(10,190,100,16,16,p);
	sprintf((char*)p,"Remain Size:%d",msgq_remain_size);	//显示DATA_Msg剩余大小
	LCD_ShowString(10,230,100,16,16,p);
	myfree(SRAMIN,p);		//释放内存
	OS_CRITICAL_EXIT();		//退出临界段
}
						
						
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();		//进入临界区所需参数
	
	delay_init();       //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置
	uart_init(115200);    //串口波特率设置
	LED_Init();         //LED初始化
	KEY_Init();			//按键初始化
	TFTLCD_Init();		//LCD初始化
	BEEP_Init();		//初始化蜂鸣器
	FSMC_SRAM_Init();	//初始化SRAM
	my_mem_init(SRAMIN);//初始化内部RAM
	ucos_load_main_ui();//加载主UI
	

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
	
		//创建消息队列KEY_Msg
	OSQCreate ((OS_Q*		)&KEY_Msg,	//消息队列
                (CPU_CHAR*	)"KEY Msg",	//消息队列名称
                (OS_MSG_QTY	)1,			//消息队列长度，这里设置为1
                (OS_ERR*	)&err);		//错误码
	//创建消息队列DATA_Msg
	OSQCreate ((OS_Q*		)&DATA_Msg,	
                (CPU_CHAR*	)"DATA Msg",	
                (OS_MSG_QTY	)4,	
                (OS_ERR*	)&err);	
				
	//创建Main任务
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
	//创建KeyProcess任务
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
	//创建Msgdis任务
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
				 
	//创建定时器1
	 OSTmrCreate((OS_TMR*				)&tmr1,			//定时器任务控制块
                 (CPU_CHAR*				)"tmr1",		//定时器名字
                 (OS_TICK				)0,				//0ms
                 (OS_TICK				)50,			//50mS
                 (OS_OPT				)OS_OPT_TMR_PERIODIC,//周期模式
                 (OS_TMR_CALLBACK_PTR	)tmr1_callback,	//回调函数名
                 (void* 				)0,				//参数为0
                 (OS_ERR*				)&err);		    //返回的错误码
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);		
}

//led0任务函数
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
		if(num%10==0) check_msg_queue(p);//检查DATA_Msg消息队列的容量
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
	key=OSQPend((OS_Q*			)&KEY_Msg,   			//消息队列
				(OS_TICK		)0,						//等待时间
				(OS_OPT			)OS_OPT_PEND_BLOCKING,	//是否选择阻塞模式
				(OS_MSG_SIZE*	)&size,					//消息长度
				(CPU_TS*		)0,						//时间戳
				(OS_ERR*		)&err);
	switch(*key)
		{
			case WKUP_PRES:     //当key_up按下的话打开定时器1
					LED1=~LED1;
				break;
			
			case KEY0_PRES:		//当key0按下的话打开定时器2
				num++;
				LCD_Fill(126,111,233,313,lcd_discolor[num%14]);
				break;
			
			case KEY2_PRES:		//当key1按下话就关闭定时器
					BEEP=~BEEP;
				break;
			
			case KEY1_PRES:		//当key1按下话就关闭定时器
				tmr1sta = !tmr1sta;
				if(tmr1sta) 
				{
					OSTmrStart(&tmr1,&err);
					LCD_ShowString(10,150,100,16,16,"TMR1 START!");
				}
				else
				{
					OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //停止定时器1
					LCD_ShowString(10,150,100,16,16,"TMR1 STOP! ");
				}
				break;
				
			default :
				
			break;
		}
	}
}

//显示消息队列中的消息
void Msgdis_task(void *p_arg)
{
	u8 *p;
	OS_MSG_SIZE size;
	OS_ERR err;
	while(1)
	{
		p=OSQPend((OS_Q*		)&DATA_Msg,   			//消息队列
				 (OS_TICK		)0,						//等待时间
				 (OS_OPT		)OS_OPT_PEND_BLOCKING,	//是否选择阻塞模式
				 (OS_MSG_SIZE*	)&size,					//消息长度
				 (CPU_TS*		)0,						//时间戳
				 (OS_ERR*		)&err);
		LCD_ShowString(5,270,100,16,16,p);
		myfree(SRAMIN,p);	//释放内存
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //延时1s
	}
}

void tmr1_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	u8 *pbuf;
	static u8 msg_num;
	pbuf = mymalloc(SRAMIN,10);	//申请10个字节
	if(pbuf)	//申请内存成功
	{
		msg_num++;
		sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
		//发送消息
		OSQPost((OS_Q*		)&DATA_Msg,		
				(void*		)pbuf,
				(OS_MSG_SIZE)10,
				(OS_OPT		)OS_OPT_POST_FIFO,
				(OS_ERR*	)&err);
		if(err != OS_ERR_NONE)
		{
			printf("错误代码%d",err);
			myfree(SRAMIN,pbuf);	//释放内存
			OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //停止定时器1
			tmr1sta = !tmr1sta;
			LCD_ShowString(10,150,100,16,16,"TMR1 STOP! ");
		}
	}
}

