/*
 * psd_sensors.c
 *
 *  Created on: 4 Jul 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file    psd_sensors.c
 * \brief   Defines functions to use the PSD sensors
 * \author  Remi KEAT
 */

#include "psd_sensors.h"

/**
 * \brief   Initialize single PSD with given semantics. Up to 8 PSDs can be initialized
 * \param   DeviceSemantics semantics : unique definition for desired PSD
 * \return  PSDHandle psdHandle : unique handle for all further operations
 */
PSDHandle PSDInit(DeviceSemantics semantics)
{
	int psdNum;
	//TODO: Complete init
	HDTDevPSD_t* ppsd = (HDTDevPSD_t*)HDTLoadPSD(HDT_FILE, semantics);
	if (ppsd)
	{
		ppsd->ptable = (HDTTable_t*)HDTLoadTable(HDT_FILE, (HDTDevice_t*)ppsd);
		if (!ppsd->ptable)
		{
			free(ppsd);
			ppsd = NULL;
		}
	}
	return (PSDHandle)ppsd;
}

/**
 * \brief   Delivers raw-data measured by the selected PSD
 * \param   PSDHandle psd : Handle of the psd to read
 * \return  int readVal : actual raw-data (not converted)
 */
int PSDGetRaw(PSDHandle psd)
{
	HDTDevPSD_t* ppsd = (HDTDevPSD_t*)psd;
	uint8_t data[] = {SPIPSDGetCmd, (ppsd->regaddr)<<4, 0x00, 0x00};
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
 * \brief   Delivers actual timestamp or distance measured by the selected PSD.
 * If the raw reading is out of range for the given sensor,
 * PSD_OUT_OF_RANGE (=9999) is returned.
 * \param   PSDHandle psd : the number of the psd to read
 * \return  int retVal : actual distance in mm (converted through internal table)
 */
int PSDGet(PSDHandle psd)
{
	//TODO: Implement conversion
	return PSDGetRaw(psd);
}

/**
 * \brief  Stops measurings and releases a PSD
 * \param  PSDHandle psd
 * \return int retVal : always 0
 */
int PSDRelease(PSDHandle psd)
{
	HDTClearPSD((HDTDevPSD_t*)psd);
	return 0;
}
