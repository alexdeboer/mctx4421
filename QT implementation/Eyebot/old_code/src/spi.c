/*
 * spi.c
 *
 *  Created on: 10 Jun 2013
 *      Author: Remi KEAT
 */

/**
 * \file    spi.c
 * \brief   Defines fonctions for sending and receiving SPI messages
 * \author  Remi KEAT
 */

/*
 * FT232H SPI Test Program
 * - Created for the Eyebot M8 Board
 *
 * By John Hodge (20518201)
 * - Eyebot M8 by Andrew Adamson
 * Uses the linux version of ftd2xx.h from http://www.ftdichip.com/Drivers/D2XX.htm
 * You may need to run 'sudo rmmod ftdi_sio usbserial' before running this to remove any drivers that linux has automatically loaded
 * The above driver command is not necessary with the angstrom distribution currently on the beagleboard
 */

#include "spi.h"


/**
 * \brief   Check if an error happened if so print error message
 * \param   SPIHandle handle
 * \return  int retVal : non-null value if an error happened
 */
int checkError(SPIHandle handle)
{
	int result;
	if (handle != NULL)
	{
		char* string = ErrorString(handle);
		char* string2 = "all fine";
		result = strncmp(string, string2, 100);
		if (DEBUG != 0)
		{
			printf("%s\n", string);
			if(result != 0)
			{
				printf("Error\n");
			}
		}
		return result;
	}
	return -1;
}

/**
 * \brief   Initialize the SPI device
 * \param   int deviceNumber
 * \return  SPIHandle spiHandle
 */
SPIHandle SPIInit(int deviceNumber)
{
	SPIHandle spiHandle;
	int tries = 0;
	int error = 0;
	do
	{
		if (DEBUG != 0)
		{
			printf("Opening device...\n");
		}
		//spiHandle = MPSSE(SPI0, 50000, MSB);
		spiHandle = MPSSE(SPI0, TWO_MHZ/40, MSB);
		if (spiHandle == NULL)
		{
			sleep(1);
		}
		tries++;
		error = checkError(spiHandle);
	} while (error != 0 && tries < NUMBER_TRY);
	if (error == 0 && tries < NUMBER_TRY)
	{
		gDeviceHandle = spiHandle;
		fprintf(stderr,"Expansion board connection successful!\n");
		fprintf(stderr,"Device description: %s\nDevice vid: %d\nDevice pid: %d\n",spiHandle->description,spiHandle->vid,spiHandle->pid);
		return spiHandle;
	}
	fprintf(stderr,"Expansion board connection unsuccessful!\n");
	return NULL;
}

/**
 * \brief   Release the SPI device
 * \param   SPIHandle spiHandle
 * \return  int retVal : always 0
 */
int SPIRelease(SPIHandle spiHandle)
{
	printf("Closing device...\n");
	if (spiHandle != NULL && DEBUG != 0)
	{
		Close(spiHandle);
	}
	return 0;
}

/**
 * \brief   Send a SPI message
 * \param   SPIHandle spiHandle
 * \param   size_t length
 * \param   const uint8_t data[]
 * \return  int retVal : always 0
 */
int SPISend(SPIHandle spiHandle, size_t length, const uint8_t data[])
{
	int i;
	int error = 0;
	do
	{
		if (DEBUG != 0)
		{
			printf("Writing : ");
			for (i = 0; i < length; i++)
			{
				printf("%02X ", data[i]);
			}
			printf("\n");
		}
		if (spiHandle != NULL)
		{
			Start(spiHandle);
			Write(spiHandle, data, length);
			Stop(spiHandle);
			error = checkError(spiHandle);
		}
		if (error != 0)
		{
			sleep(1);
		}
	} while (error != 0);
	usleep(500);
	return 0;
}

/**
 * \brief   Read a SPI message
 * \param   SPIHandle spiHandle
 * \param   size_t length
 * \param   uint8_t* data[]
 * \return  int retVal : always 0
 */
int SPIRead(SPIHandle spiHandle, size_t length, uint8_t* data[])
{
	int i;
	int error = 0;
	do
	{
		if (spiHandle != NULL)
		{
			Start(spiHandle);
			Write(spiHandle, "\x00", 1);
			*data = Read(spiHandle, length);
			Stop(spiHandle);
		}
		else
		{
			*data = malloc(length);
			for (i = 0; i < length; i++)
			{
				(*data)[i] = 0x00;
			}
		}
		if (DEBUG != 0)
		{
			printf("Reading : ");
			for (i = 0; i < length; i++)
			{
				printf("%02X ", (*data)[i]);
			}
			printf("\n");
		}
		if (spiHandle != NULL)
		{
			error = checkError(spiHandle);
		}
		if (error != 0)
		{
			sleep(1);
		}
	} while (error != 0);
	usleep(500);
	return 0;
}

/**
 * \brief   Read a SPI message on the default SPI device
 * \param   size_t length
 * \param   uint8_t* data[]
 * \return  int retVal : always 0
 */
int SPIReadDefault(size_t length, uint8_t* data[])
{
	return SPIRead(gDeviceHandle, length, data);
}

/**
 * \brief   Send a SPI message on the default SPI device
 * \param   size_t length
 * \param   const uint8_t data[]
 * \return  int retVal : always 0
 */
int SPISendDefault(size_t length, const uint8_t data[])
{
	return SPISend(gDeviceHandle, length, data);
}
