/*
 * myTimers.h
 *
 *  Created on: 16-04-2014
 *      Author: Adi
 */

#ifndef MYTIMERS_H_
#define MYTIMERS_H_
#include "inc/hw_types.h"
#include "myTypes.h"
typedef enum
{
    ESleepType_USec,
    ESleepType_Milisec,
    ESleepType_Seconds
}ESleepType;

void delay_ms(u32 delay);
void delay_us(u32 delay);
void SysTimeSleep(u32 delay, ESleepType sleepType);
//initializes timer for SysTimeSleep
void TimerForDelayConfig(void);
void Timer32ForDelayConfig(void);

#endif /* MYTIMERS_H_ */
