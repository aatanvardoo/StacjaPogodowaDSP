/*
 * myI2C.c
 *
 *  Created on: 09-05-2014
 *      Author: Adi
 */
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"
#include "driverlib/rom.h"
#include "driverlib/debug.h"
//*****************************************************************************
//
//! Reads one/multiple bytes of data from an I2C slave device.
//!
//! \param ulI2CBase is the base for the I2C module.
//! \param ucSlaveAdress is the 7-bit address of the slave to be addressed.
//! \param ucReg is the register to start reading from.
//! \param cReadData is a pointer to the array to store the data.
//! \param uiSize is the number of bytes to read from the slave.
//!
//! This function reads one/multiple bytes of data from an I2C slave device.
//! The ulI2CBase parameter is the I2C modules master base address.
//! \e ulI2CBase parameter can be one of the following values:
//!
//! - \b I2C0_MASTER_BASE
//! - \b I2C1_MASTER_BASE
//! - \b I2C2_MASTER_BASE
//! - \b I2C3_MASTER_BASE
//!
//! \return 0 if there was an error or 1 if there was not.
//
//*****************************************************************************
unsigned long
I2CReadData(unsigned long ulI2CBase, unsigned char ucSlaveAdress,
        unsigned char ucReg, char* cReadData, unsigned int uiSize)
{
    unsigned int uibytecount;       // local variable used for byte counting/state determination
    int MasterOptionCommand;        // used to assign the commands for ROM_I2CMasterControl() function

    //
    // Check the arguments.
    //
    ASSERT(I2CMasterBaseValid(ulI2CBase));

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Tell the master module what address it will place on the bus when
    // writing to the slave.
    //
    ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucSlaveAdress, 0);

    //
    // Place the command to be sent in the data register.
    //
    ROM_I2CMasterDataPut(ulI2CBase, ucReg);

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_SINGLE_SEND);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }


    //
    // Tell the master module what address it will place on the bus when
    // reading from the slave.
    //
    ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucSlaveAdress, true);

    //
    // Start with BURST with more than one byte to write
    //
    MasterOptionCommand = I2C_MASTER_CMD_BURST_RECEIVE_START;


    for(uibytecount = 0; uibytecount < uiSize; uibytecount++)
    {
        //
        // The second and intermittent byte has to be read with CONTINUE control word
        //
        if(uibytecount == 1)
            MasterOptionCommand = I2C_MASTER_CMD_BURST_RECEIVE_CONT;

        //
        // The last byte has to be send with FINISH control word
        //
        if(uibytecount == uiSize - 1)
            MasterOptionCommand = I2C_MASTER_CMD_BURST_RECEIVE_FINISH;

        //
        // Re-configure to SINGLE if there is only one byte to read
        //
        if(uiSize == 1)
            MasterOptionCommand = I2C_MASTER_CMD_SINGLE_RECEIVE;

        //
        // Initiate read of data from the slave.
        //
        ROM_I2CMasterControl(ulI2CBase, MasterOptionCommand);

        //
        // Wait until master module is done reading.
        //
        while(ROM_I2CMasterBusy(ulI2CBase))
        {
        };

        //
        // Check for errors.
        //
        if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
        {
            return 0;
        }

        //
        // Move byte from register
        //
        cReadData[uibytecount] = I2CMasterDataGet(ulI2CBase);
    }

    // send number of received bytes
    return uibytecount;
}


unsigned long
I2CRegWrite(unsigned long ulI2CBase, unsigned char ucSlaveAdress,
         unsigned char ucReg, unsigned char ucValue)
{
    //
    // Check the arguments.
    //
    ASSERT(I2CMasterBaseValid(ulI2CBase));

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Tell the master module what address it will place on the bus when
    // writing to the slave.
    //
    ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucSlaveAdress, 0);

    //
    // Place the command to be sent in the data register.
    //
    ROM_I2CMasterDataPut(ulI2CBase, ucReg);

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }

    //
    // Place the value to be sent in the data register.
    //
    ROM_I2CMasterDataPut(ulI2CBase, ucValue);

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_BURST_SEND_CONT);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_BURST_SEND_FINISH);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }

    //
    // Return 1 if there is no error.
    //
    return 1;
}

unsigned long
I2CWriteData(unsigned long ulI2CBase, unsigned char ucSlaveAdress,
        unsigned char ucReg, char* cSendData, unsigned int uiSize)
{
    unsigned int uibytecount;       // local variable used for byte counting/state determination
    int MasterOptionCommand;        // used to assign the commands for ROM_I2CMasterControl() function

    //
    // Check the arguments.
    //
    ASSERT(I2CMasterBaseValid(ulI2CBase));

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Tell the master module what address it will place on the bus when
    // writing to the slave.
    //
    ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucSlaveAdress, false);

    //
    // Place the value to be sent in the data register.
    //
    ROM_I2CMasterDataPut(ulI2CBase, ucReg);

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }

    //
    // Start with CONT for more than one byte to write
    //
    MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_CONT;


    for(uibytecount = 0; uibytecount < uiSize; uibytecount++)
    {
        //
        // The second and intermittent byte has to be send with CONTINUE control word
        //
        if(uibytecount == 1)
            MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_CONT;

        //
        // The last byte has to be send with FINISH control word
        //
        if(uibytecount == uiSize - 1)
            MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_FINISH;

        //
        // Re-configure to SINGLE if there is only one byte to write
        //
        if(uiSize == 1)
            MasterOptionCommand = I2C_MASTER_CMD_SINGLE_SEND;

        //
        // Send data byte
        //
        ROM_I2CMasterDataPut(ulI2CBase, cSendData[uibytecount]);

        //
        // Initiate send of data from the master.
        //
        ROM_I2CMasterControl(ulI2CBase, MasterOptionCommand);

        //
        // Wait until master module is done transferring.
        //
        while(ROM_I2CMasterBusy(ulI2CBase))
        {
        };

        //
        // Check for errors.
        //
        if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
        {
            return 0;
        }
    }

    //
    // Return 1 if there is no error.
    //
    return 1;
}
