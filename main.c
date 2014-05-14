
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
#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"
#include "string.h"
#include "myTypes.h"
#include "RTC.h"
#include <stdint.h>
#include "myUart.h"
#include "myTimers.h"
#include "measurements.h"

//*****************************************************************************

extern tBoolean gMakeMeas;
extern tBoolean gGataSending;
ramka gRtRamka = {0xDEAD,0,0,0,0,0,1,'\n'};
const u16 gTabSize = 100;
u16 gTabIdxCnt = 0;
const u16 gMeasTabSize = sizeof(ramka)*gTabSize;
const u16 gSizeOfRamka = sizeof(ramka);
unsigned char gMeasTab[gMeasTabSize];
const u16 gRtMeasTabSize = sizeof(ramka);

u8 gRtMeasTab[gRtMeasTabSize];
extern volatile tBoolean gTimerSem;
u32 gTempTemp;
u32 gTempPress;

int main(void)
{
    volatile u8 frameSize = 0;
    u8 i;
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ramka jeden[1] = {0,0,0,0,0,0,1,'\n'};
    ramka * const uno = jeden;
    uno->msgid = 0xAEAF;
    uno->temperatura = -20;
    uno->wilgotnosc = 12;
    uno->windDir = 1;
    uno->windSpeed = 9;

   // memset(gMeasTab,0x00,gMeasTabSize);
    memset(gRtMeasTab,0x00,gRtMeasTabSize);


    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //
    // The I2C0 peripheral must be enabled before use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    I2CMasterInitExpClk(I2C1_MASTER_BASE, SysCtlClockGet(), false);

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
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    InitRTC();
    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();
    Timer32ForDelayConfig();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    ROM_UARTConfigSetExpClk(UART1_BASE, ROM_SysCtlClockGet(), 9600,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    ROM_IntEnable(INT_UART1);
    ROM_UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

    while(1)
    {

        //SysCtlDelay(SysCtlClockGet() / (1 * 3));

      //  DoAM2302Measurements(uno);
      //  DoMPLMeasurements(uno);
      //  uno->timestamp = ROM_HibernateRTCGet();
      //  if(gMakeMeas == false)
   //    {
    //       memcpy(gRtMeasTab,uno,gRtMeasTabSize);
     //       UARTSend(gRtMeasTab, gRtMeasTabSize);

     //   }
     //   else
      //  {
        if(gMakeMeas == true)
        {
            gMakeMeas = false;
            // ID wiadomosci inne przy sci¹ganiu wszystkich pomiarów. Potrzebne
            //aby program na komputerze mug rozru¿niæ czy ma wyswietliæ pomiary czy je zapisaæ.

            uno->msgid = 0xABAF;
            memcpy(&gMeasTab[gTabIdxCnt],uno,gSizeOfRamka);

            gTabIdxCnt+=gSizeOfRamka;
            if(gTabIdxCnt >= gMeasTabSize)
                gTabIdxCnt = 0;
        }
        else if(gGataSending == true)
        {
            gGataSending = false;

            for(i=0;i<gTabSize;i++)
            {
            UARTSend(&gMeasTab[i*gRtMeasTabSize], gRtMeasTabSize);
            delay_ms(10);
            }
        }
        else if(gTimerSem == true)
        {
            gTimerSem = false;
            uno->msgid = 0xAEAF;
            DoAM2302Measurements(uno);
            DoMPLMeasurements(uno);
            uno->timestamp = ROM_HibernateRTCGet();

            memcpy(gRtMeasTab,uno,gRtMeasTabSize);
            UARTSend(gRtMeasTab, gRtMeasTabSize);
        }
     //   }

    }
}
