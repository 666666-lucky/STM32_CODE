#ifndef _dma_H
#define _dma_H

#include "system.h"

void Dmax_Init(DMA_Channel_TypeDef* DMAy_Channelx,u32 per,u32 mar,u16 ndtr);
void Dmax_Enable(DMA_Channel_TypeDef* DMAy_Channelx,u16 ndtr);

#endif

