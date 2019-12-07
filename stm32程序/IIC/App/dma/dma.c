#include "dma.h"


//通道选择，外设地址，存储器（内存）地址，传输量
void Dmax_Init(DMA_Channel_TypeDef* DMAy_Channelx,u32 per,u32 mar,u16 ndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitStructure.DMA_BufferSize=ndtr;							//发送数量<=65536
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;    //数据传输方向存储器到外设
	DMA_InitStructure.DMA_MemoryBaseAddr=mar;						//存储内存地址
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;					//不开启存储器到存储器模式
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//开启内存地址增加
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//DMA为单次传输
	DMA_InitStructure.DMA_PeripheralBaseAddr=per;//外设地址
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;//外设数据长度
	DMA_InitStructure.DMA_MemoryDataSize=DMA_PeripheralDataSize_Byte;//内存数据长度
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; //不开启外设地址增加
	DMA_InitStructure.DMA_Priority=DMA_Priority_Low;//此通道优先级为低
	DMA_Init(DMAy_Channelx,&DMA_InitStructure);//初始化DMA
}

//开启一次DMA数据传送
void Dmax_Enable(DMA_Channel_TypeDef* DMAy_Channelx,u16 ndtr)
{
	DMA_Cmd(DMAy_Channelx,DISABLE);
	DMA_SetCurrDataCounter(DMAy_Channelx,ndtr);
	DMA_Cmd(DMAy_Channelx,ENABLE);
}


