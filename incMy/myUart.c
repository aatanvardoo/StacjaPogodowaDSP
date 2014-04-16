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

const u16 SET_TIME_MSG     = 0xACAD;
const u16 EOF_SET_TIME_MSG = 0xADAC;
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
void UARTIntHandler(void)
{
    unsigned long ulStatus;
    u8 byte;
    u32 time = 0;
    //
    // Get the interrupt status.
    //
    ulStatus = ROM_UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ulStatus);
    //
    // Loop while there are characters in the receive FIFO.
    //
    while(ROM_UARTCharsAvail(UART0_BASE))
    {

        byte = ROM_UARTCharGetNonBlocking(UART0_BASE);

        GetMsgId(&byte, &gMsgId);
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
        ROM_UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
    }
}
