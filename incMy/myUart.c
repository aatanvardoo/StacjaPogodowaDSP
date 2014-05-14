/*
 * myUart.c
 *
 *  Created on: 29-03-2014
 *      Author: Adi
 */
#include "inc/hw_types.h"
#include "myTypes.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/hibernate.h"

static u8 gByteCnt = 0;
static u16 gMsgId = 0;
/* We dont send more than 20 bytes so fixed table is ok */
static u8 gTabRcv[20];

static tBoolean gTimeRcv = false;
tBoolean gGataSending = false;
volatile unsigned long gUartBase = UART0_BASE;
#define SET_TIME_MSG      0xACAD
#define EOF_SET_TIME_MSG  0xADAC
#define GET_DATA_MSG      0xABAE
#define EOF_GET_DATA_MSG  0xAEAB
//*****************************************************************************
//
// Returns message id from UART stream.
//
//*****************************************************************************
inline void GetMsgId(u8* const data, u16* const msgId)
{
    *msgId = (*msgId)<<8;
    *msgId |= 0x00FF & (*data);
}

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
unsigned long ulStatus, ulStatusWireless;
void UARTIntHandler(void)
{

    u8 byte;
    u32 time = 0;
    //
    // Get the interrupt status.
    //
    ulStatus = ROM_UARTIntStatus(UART0_BASE, true);
    ulStatusWireless = ROM_UARTIntStatus(UART1_BASE, true);

    if(ulStatus != 0)
    {
        gUartBase = UART0_BASE;
        //
        // Clear the asserted interrupts.
        //
        ROM_UARTIntClear(gUartBase, ulStatus);
    }
    else if(ulStatusWireless != 0)
    {
        gUartBase = UART1_BASE;
        //
        // Clear the asserted interrupts.
        //
        ROM_UARTIntClear(gUartBase, ulStatusWireless);
    }

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(ROM_UARTCharsAvail(gUartBase))
    {

        byte = ROM_UARTCharGetNonBlocking(gUartBase);

        GetMsgId(&byte, &gMsgId);

        switch(gMsgId)
        {

        case SET_TIME_MSG:
            gTimeRcv = true;
            break;

        case EOF_SET_TIME_MSG:
            gByteCnt = 0;
            time = gTabRcv[4]<<24 | gTabRcv[3]<<16 | gTabRcv[2]<<8 | gTabRcv[1];
            gTimeRcv = false;
            //when the time is set. Make first measurements after 10s
            ROM_HibernateRTCSet(time);
            HibernateRTCMatch0Set( ROM_HibernateRTCGet() + 10 );
            break;
        case GET_DATA_MSG:
            gGataSending = true;
            break;
        }

        /*
        if(gMsgId == SET_TIME_MSG)
        {
            gTimeRcv = true;
        }
        else if(gMsgId == EOF_SET_TIME_MSG)
        {
            gByteCnt = 0;
            time = gTabRcv[4]<<24 | gTabRcv[3]<<16 | gTabRcv[2]<<8 | gTabRcv[1];
            gTimeRcv = false;
            //when the time is set. Make first measurements after 10s
            ROM_HibernateRTCSet(time);
            HibernateRTCMatch0Set( ROM_HibernateRTCGet() + 10 );
        }
*/
        if(gTimeRcv == true)
        {
            gTabRcv[gByteCnt] = byte;
            gByteCnt++;

        }
    }
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPutNonBlocking(gUartBase, *pucBuffer++);
    }
}

void
UARTWirelessSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPutNonBlocking(UART1_BASE, *pucBuffer++);
    }
}
