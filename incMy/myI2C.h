/*
 * myI2C.h
 *
 *  Created on: 09-05-2014
 *      Author: Adi
 */

#ifndef MYI2C_H_
#define MYI2C_H_

#define SLAVE_ADDRESS 0x60

unsigned long I2CReadData(unsigned long ulI2CBase,
                          unsigned char ucSlaveAdress,
                          unsigned char ucReg,
                          char* cReadData,
                          unsigned int uiSize);

unsigned long I2CRegWrite(unsigned long ulI2CBase,
                          unsigned char ucSlaveAdress,
                          unsigned char ucReg,
                          unsigned char ucValue);

unsigned long I2CWriteData(unsigned long ulI2CBase,
                           unsigned char ucSlaveAdress,
                           unsigned char ucReg,
                           char* cSendData,
                           unsigned int uiSize);
#endif /* MYI2C_H_ */
