/*
 * measurements.c
 *
 *  Created on: 01-04-2014
 *      Author: Adi
 */
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "myTypes.h"
#include "myTimers.h"
#include "myI2C.h"

u8 DHTData[5];

static void DHTStart(void)
{
    volatile u8 iVal = 0;
    u32 loopCnt = 0;
   // Here we send the 'start' sequence to the DHT sensor! We pull the DHT_IO pin low for 25mS, and
   // then high for 30mS. The DHT sensor then responds by pulling the DHT_IO pin low, and then pulling it
   // back high.

   GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);//Output_low(DHT_IO);
   GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x00);
   delay_ms(9);//SysTimeSleep(25, ESleepType_Milisec);

   GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x10);//Output_high(DHT_IO);
   delay_us(10);//SysTimeSleep(30, ESleepType_USec);
   iVal =0;
   // Here we wait while the DHT_IO pin remains low.....
   GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
   do
   {
       iVal = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)>>4;

       loopCnt++;

       if(loopCnt>10000)
           break;

   }while(!iVal);

   loopCnt = 0;
   do
   {
       iVal = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)>>4;
       loopCnt++;

       if(loopCnt>10000)
           break;

   }while(iVal);
}


static u8 DHT_ReadData(void)
{

   // This subroutine reads a byte of data (8 bits) from the DHT sensor...
   u8 i;
   u8 value = 0;
   u8 val = 0;
   u16 loopCnt = 0;

   for(i = 0; i < 8 ; i++)
   {
       do
       {
           val = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)>>4;

           loopCnt++;

           if(loopCnt>10000)
               break;

       }while (!val);
       delay_us(10);//SysTimeSleep(10, ESleepType_USec);
       loopCnt = 0;
       val = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)>>4;
      if (val)
      {
          value = value |(1<<(7 - i));

         do
         {
             val = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)>>4;
             loopCnt++;

             if(loopCnt>10000)
                 break;

         }while (val);
      }
   }

   return value;
}

void DoAM2302Measurements(ramka* const meas)
{

    u8 i;
    u16 CRC;
    DHTStart();
    for (i = 0; i < 5 ; i++)
    {
       DHTData[i] = DHT_ReadData();
    }
    // Here we calculate the CRC by adding the 1st four bytes, and looking at only the lower 8 bits.
    // This value should match the 5th byte sent by the sensor which is the CRC byte!
    CRC = DHTData[0] + DHTData[1] + DHTData[2] + DHTData[3];
    CRC = CRC & 0xFF;

    if (CRC != DHTData[4])
    {
        meas->wilgotnosc = 0xDEAD;
        meas->temperatura = 0xDEAD;
    }
    else
    {
        meas->wilgotnosc = DHTData[0]<<8 | DHTData[1];
        meas->temperatura = (DHTData[2]&0x7F)<<8 | DHTData[3];
    }
}

void DoMPLMeasurements(ramka* const meas)
{
    char readData[10];
    I2CRegWrite(I2C1_MASTER_BASE, SLAVE_ADDRESS,
             0x12, 0x00);
    delay_ms(9);
    I2CReadData(I2C1_MASTER_BASE, SLAVE_ADDRESS,
            0x00, readData, 5);
    delay_ms(9);
    meas->pressure = (readData[2]<<8 | readData[3])>>6;
}

void DoMeasurements(void)
{
   // DoAM2302Measurements(&gRtRamka);
    //DoMPLMeasurements(&gRtRamka);
   // memcpy(&gMeasTab[gTabIdxCnt],&gRtRamka,gSizeOfRamka);
/*
    gTabIdxCnt+=gSizeOfRamka;
    if(gTabIdxCnt >= gMeasTabSize)
        gTabIdxCnt = 0;
        */
}
