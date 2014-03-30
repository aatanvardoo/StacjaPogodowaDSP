/*
 * RTC.c
 *
 *  Created on: 26-03-2014
 *      Author: Adi
 */

#include "inc/hw_types.h"

#include "driverlib/hibernate.h"
#include "inc/hw_hibernate.h"
#include "driverlib/rom.h"
#include "RTC.h"
#include "myTypes.h"

static u32 gRtcStart = 0;
static u32 gRtcInterr = 0;

void HibernateHandler(void)
{
    unsigned long ulStatus;
    //
    // Get the interrupt status, and clear any pending interrupts.
    //
    ulStatus = HibernateIntStatus(1);
    HibernateIntClear(ulStatus);
    //
    // Process the RTC match 0 interrupt.
    //
    if(ulStatus & HIBERNATE_INT_RTC_MATCH_0)
    {
        //UARTSend("30sec", 5);
        gRtcInterr = ROM_HibernateRTCGet();
    }
}

void InitRTC(void)
{
    /* Enabling peripheral */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);

    /* Turning on external crystal 32.768kHz */
    HWREG(HIB_CTL) |= HIB_CTL_CLK32EN | HIB_CTL_OSCDRV;

    ROM_SysCtlDelay (1500 * (ROM_SysCtlClockGet() / 3 / 1000));

    /* Set initial RTC value in sec */
    ROM_HibernateRTCSet(0);
    //HibernateRTCzMatchSet(0, 0xFFFFFFFF);
    //HibernateRTCSSMatchSet(0, 0);
    /* Set clock divider to 32767 */

    /* Enabling interrupt */
    ROM_HibernateIntEnable(HIBERNATE_INT_RTC_MATCH_0);

    ROM_HibernateIntClear(HIBERNATE_INT_PIN_WAKE | HIBERNATE_INT_LOW_BAT |
    HIBERNATE_INT_RTC_MATCH_0 |
    HIBERNATE_INT_RTC_MATCH_1);
    ROM_HibernateRTCTrimSet(0x7FFF);

    /* Linking interrupt handler */
    HibernateIntRegister(HibernateHandler);
    /* Enabling RTC counter */
    ROM_HibernateRTCEnable();

    /* Getting initial RTC counter value (should be 0)*/
    gRtcStart = ROM_HibernateRTCGet();

    /* Interrupt will be in 30s */
    HibernateRTCMatch0Set(gRtcStart+30);
}
