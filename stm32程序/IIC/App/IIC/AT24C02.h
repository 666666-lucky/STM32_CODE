#ifndef _AT24C02_H
#define _AT24C02_H

#include "system.h"
#include "iic.h"

void At24c02_Init(void);
void At24c02_Write(u8 addr,u8 *Sbuffer,u8 len);
void At24c02_Read(u8 addr,u8 *sbuff,u8 len);

unsigned int detect(unsigned char address,unsigned char command);
unsigned char read_byte(unsigned char address,unsigned char reg); //address(with bit 0 set) + register;
unsigned int detect(unsigned char address,unsigned char command);
void write_byte(unsigned char address,unsigned char reg,unsigned char command); 		//address+register+command
unsigned char read_byte(unsigned char address,unsigned char reg); //address(with bit 0 set) + register


#endif

