#ifndef _rtc_H
#define _rtc_H

#include "system.h"
typedef struct
{
	u8 hour;
	u8 min;
	u8 sec;
}Time_typedef;

extern Time_typedef Rtc_time;

u8 RTC_Init(void);
void Get_Time(void);

#endif
