/*
 * myTimers.c
 *
 *  Created on: 16-04-2014
 *      Author: Adi
 */
#include "inc/hw_ints.h"
#include "myTimers.h"

//#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"

volatile tBoolean gTimerSem = false;

void delay_ms(u32 delay)
{
    SysCtlDelay((SysCtlClockGet()*delay) / 1000);
}
void delay_us(u32 delay)
{
    SysCtlDelay((SysCtlClockGet()*delay) / 1000000);
}


//accuracy +10us...
void SysTimeSleep(u32 delay, ESleepType sleepType)
{
    u32 sysClk = SysCtlClockGet();
    u32 temp = 0;
    gTimerSem = true;
    switch(sleepType)
    {
        case ESleepType_USec:
           temp =  (sysClk*delay/1000000);
           break;
        case ESleepType_Milisec:
            temp =  (sysClk/1000)*delay;
            break;
        case ESleepType_Seconds:
            temp =  (sysClk)*delay;
            break;
        default:
            temp =  (sysClk/1000)*delay;
            break;
    }
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, temp);
    TimerEnable(TIMER0_BASE, TIMER_BOTH);
    IntEnable(INT_TIMER0A);

    while(gTimerSem == true);
}


void TimerForDelayConfig(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR |
                   TIMER_CFG_B_PERIODIC);

    //1ms:
    TimerLoadSet(TIMER0_BASE, TIMER_B, SysCtlClockGet()/1000 );

    //IntMasterEnable(); //already done in startup
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    IntEnable(INT_TIMER0B);
    TimerEnable(TIMER0_BASE, TIMER_B);
}

void Timer32ForDelayConfig(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);

    //1ms:
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, SysCtlClockGet()/100000);

    //IntMasterEnable(); //already done in startup
    TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
   // IntEnable(INT_TIMER0A);
   // TimerEnable(TIMER0_BASE, TIMER_BOTH);
}

void Timer0BIntHandler(void)
{
    //
    // Clear the timer interrupt flag.
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    IntDisable(INT_TIMER0A);
    TimerDisable(TIMER0_BASE, TIMER_BOTH);
    gTimerSem = false;
}
