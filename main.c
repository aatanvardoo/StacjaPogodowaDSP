
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "string.h"
#include "myTypes.h"
#include "RTC.h"
#include <stdint.h>
#include "myUart.h"
#include "myTimers.h"
#include "measurements.h"
//*****************************************************************************


const u16 gMeasTabSize = sizeof(ramka)*40;
unsigned char gMeasTab[gMeasTabSize];


int main(void)
{
    volatile u8 frameSize = 0;
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ramka jeden[1] = {0,0,0,0,0,0,'\n'};
    ramka * const uno = jeden;
    uno->msgid = 0xAEAF;
    uno->temperatura = -20;
    uno->wilgotnosc = 12;
    uno->windDir = 1;
    uno->windSpeed = 9;
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable the GPIO pins for the LED (PF2).  
    //
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    //1wire
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    //DHTStart();
    //
    // Enable the peripherals used by this example.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    InitRTC();
    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();
   // Timer32ForDelayConfig();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    frameSize = sizeof(ramka);
    memset(gMeasTab,0x00,frameSize);
    memcpy(gMeasTab,uno,frameSize);

    while(1)
    {
        SysCtlDelay(SysCtlClockGet() / (1 * 3));

        DoAM2302Measurements(uno);
        uno->timestamp = ROM_HibernateRTCGet();
        memcpy(gMeasTab,uno,frameSize);
        UARTSend(gMeasTab, frameSize);
    }
}
