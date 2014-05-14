/*
 * myTypes.h
 *
 *  Created on: 29-03-2014
 *      Author: Adi
 */

#ifndef MYTYPES_H_
#define MYTYPES_H_

typedef unsigned char u8;
typedef char           i8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed short i16;
typedef struct
{
    u16 msgid;
    u32 timestamp;
    i16 temperatura;
    u16 wilgotnosc;
    u8 windSpeed;
    u8 windDir;
    u16 pressure;
    const u8 endOfframe;
}ramka;

#endif /* MYTYPES_H_ */
