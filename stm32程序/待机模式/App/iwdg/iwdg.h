#ifndef _iwdg_H
#define _iwdg_H

#include "system.h"
void Iwdg_Init(u8 pre,u16 reload);		//独立看门狗复位时间计算公式：t=(4*2^pre*rlr)/40


#endif
