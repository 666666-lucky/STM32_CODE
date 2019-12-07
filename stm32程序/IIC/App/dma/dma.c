#include "dma.h"


//ͨ��ѡ�������ַ���洢�����ڴ棩��ַ��������
void Dmax_Init(DMA_Channel_TypeDef* DMAy_Channelx,u32 per,u32 mar,u16 ndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitStructure.DMA_BufferSize=ndtr;							//��������<=65536
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;    //���ݴ��䷽��洢��������
	DMA_InitStructure.DMA_MemoryBaseAddr=mar;						//�洢�ڴ��ַ
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;					//�������洢�����洢��ģʽ
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//�����ڴ��ַ����
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//DMAΪ���δ���
	DMA_InitStructure.DMA_PeripheralBaseAddr=per;//�����ַ
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;//�������ݳ���
	DMA_InitStructure.DMA_MemoryDataSize=DMA_PeripheralDataSize_Byte;//�ڴ����ݳ���
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; //�����������ַ����
	DMA_InitStructure.DMA_Priority=DMA_Priority_Low;//��ͨ�����ȼ�Ϊ��
	DMA_Init(DMAy_Channelx,&DMA_InitStructure);//��ʼ��DMA
}

//����һ��DMA���ݴ���
void Dmax_Enable(DMA_Channel_TypeDef* DMAy_Channelx,u16 ndtr)
{
	DMA_Cmd(DMAy_Channelx,DISABLE);
	DMA_SetCurrDataCounter(DMAy_Channelx,ndtr);
	DMA_Cmd(DMAy_Channelx,ENABLE);
}


