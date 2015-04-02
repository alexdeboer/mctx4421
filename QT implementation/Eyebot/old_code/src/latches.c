/*
 * latches.c
 *
 *  Created on: 5 Jul 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file    latches.c
 * \brief   Defines functions to control latches
 * \author  Remi KEAT
 */

#include "latches.h"

/**
 * \brief   Setup the given latch as input or output
 * \param   int latchnum : latch number
 * \param   int direction : signal direction
 * \details Valid values for direction :
 * - 0 = input
 * - 1 = output
 * 
 * \return  int retVal : always 0
 */
int OSLatchSetup(int latchnum, int direction)
{
	uint8_t data[] = {SPILatchSetupCmd, (latchnum<<4)+(direction&0x0F), 0x00, 0x00};
	SPISendDefault(sizeof(data), data);
	return 0;
}

/**
 * \brief   Setup the given io buffer bank as input or output.
 * \param   int banknum : bank number
 * \param   int direction : signal direction
 * \details Valid values for direction:
 * - 0 = input
 * - 1 = output
 * 
 * Note: 
 * - LATCH0..LATCH7 are connected to IOBANK0
 * - LATCH8..LATCH15 are connected to IOBANK1
 * 
 * \return  int retVal : always 0
 */
int OSLatchBankSetup(int banknum, int direction)
{
	uint8_t data[] = {SPILatchBankSetupCmd, (banknum<<4)+(direction&0x0F), 0x00, 0x00};
	SPISendDefault(sizeof(data), data);
	return 0;
}

/**
 * \brief   Read content of the selected input latch
 * \param   int latchnum : latch number to read
 * \details Return latch status :
 * - 0 = low
 * - 1 = high
 * 
 * \return  int readValue
 */
int OSLatchRead(int latchnum)
{
	uint8_t data[] = {SPILatchReadCmd, latchnum<<4, 0x00, 0x00};
	uint8_t* reading[4];
	long result = 0;
	SPISendDefault(sizeof(data), data);
	SPIReadDefault(4, reading);
	result = ((*reading)[0])<<24;
	result += ((*reading)[1])<<16;
	result += ((*reading)[2])<<8;
	result += ((*reading)[3]);
	return result;
}

/**
 * \brief   Write to the selected output latch
 * \param   int latchnum : latch number to write
 * \param   int state : state to be set to the selected out latch
 * \details Valid values for state :
 * - 0 = low
 * - 1 = high
 * 
 * \return  int retVal : always 0
 */
int OSLatchWrite(int latchnum, int state)
{
	uint8_t data[] = {SPILatchWriteCmd, latchnum<<4, 0x00, state&0xFF};
	SPISendDefault(sizeof(data), data);
	return 0;
}

/**
 * \brief		Initialize the digital IO, call this before using any digital IO functions
 * \return  int retVal
 * \details Return code:
 * - 0  = ok
 * - -1 = Initialization error
 *
 */
int OSLatchInit(void)
{
	//TODO: Implement
	return 0;
}

/**
 * \brief    Unmap the memory for digital IOs, call these when the digital IOs functions are no longer needed
 * \return   int retVal : always 0
 */
int OSLatchCleanup(void)
{
	//TODO: Implement
	return 0;
}
