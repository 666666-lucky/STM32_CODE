#ifndef _AT24C02_H
#define _AT24C02_H

#include "system.h"
#include "iic.h"

void At24c02_Init(void);
void At24c02_Write(u8 addr,u8 *Sbuffer,u8 len);
void At24c02_Read(u8 addr,u8 *sbuff,u8 len);



#endif

