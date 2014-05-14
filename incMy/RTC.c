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
#include "measurements.h"

static u32 gRtcStart = 0;

tBoolean gMakeMeas = false;
u16 gmeasurementPeriod = 1;
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
       // DoMeasurements();
        gRtcStart = ROM_HibernateRTCGet();
        /* Interrupt will be in 1s */
        HibernateRTCMatch0Set(gRtcStart+gmeasurementPeriod);
        gMakeMeas = true;
    }
}

void InitRTC(void)
{
    /* Enabling peripheral */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);

    /* Turning on external crystal 32.768kHz */
    HWREG(HIB_CTL) |= HIB_CTL_CLK32EN | HIB_CTL_OSCDRV;

    SysCtlDelay (1500 * (SysCtlClockGet() / 3 / 1000));

    /* Set initial RTC value in sec */
    HibernateRTCSet(0);
    //HibernateRTCzMatchSet(0, 0xFFFFFFFF);
    //HibernateRTCSSMatchSet(0, 0);
    /* Set clock divider to 32767 */

    /* Enabling interrupt */
    HibernateIntEnable(HIBERNATE_INT_RTC_MATCH_0);

    HibernateIntClear(HIBERNATE_INT_PIN_WAKE | HIBERNATE_INT_LOW_BAT |
    HIBERNATE_INT_RTC_MATCH_0 |
    HIBERNATE_INT_RTC_MATCH_1);
    // do not use one from ROM!!!
    HibernateRTCTrimSet(0x7FFF);

    /* Linking interrupt handler */
    HibernateIntRegister(HibernateHandler);
    /* Enabling RTC counter */
    HibernateRTCEnable();

    /* Getting initial RTC counter value (should be 0)*/
    gRtcStart = ROM_HibernateRTCGet();

    /* Interrupt will be in 30s */
    HibernateRTCMatch0Set(gRtcStart+gmeasurementPeriod);
}
