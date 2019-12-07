#ifndef _adc_H
#define _adc_H

#include "system.h"
void ADCx_Init(void);
u16 Get_ADC_Vaule(u8 CHx,u8 time);
void ADCTemp_Init(void);
int Get_Temperture(void);

#endif
