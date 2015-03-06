/*
 * adc.c
 *
 *  Created on: 19 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file			adc.c
 * \brief			Defines the ADC functions
 * \author    Remi KEAT
 */

#include "adc.h"

/**
 * \brief     Captures one single 10bit value from the specified adc channel
 * \param     DeviceSemantics semantics : desired ADC channel
 * \return    ADCHandle handle >0: Handler for the adc channel\n
 *            0: Initialization error
 * \details   Valid values for semantics : ADC0, ADC1, ADC2, ADC3
 */
ADCHandle OSInitADC(DeviceSemantics semantics)
{
	//TODO: Complete
	HDTDevADC_t* padc;
	padc = (HDTDevADC_t*)HDTLoadADC(HDT_FILE, semantics);
	return (ADCHandle)padc;
}

/**
 * \brief			Captures one single 10bit value from specified AD-channel.
 * The return	value is stored in the least significant bits of the 32 bit return value.
 * \param			ADCHandle handle : Handler for the adc channel
 * \return		int retVal >0: 10 bit sampled value\n
							-1: invalid channel
 */
int OSGetADC(ADCHandle adchandle)
{
	return 0;
}

/**
 * \brief			Convert the adc sample to voltage
 * \param     ADCHandle adchandle : desired AD-channel
 * \param     char* volt : pointer to string
 * \param     int sample : ADC sample
 * \details   Result is stored in char *volt.
 * Valid values: ADC0, ADC1, ADC2, ADC3
 * \return    int retVal : 0: ok\n
 *            -1: invalid channel
 */
int ConvADCSampleToVoltage(ADCHandle adchandle, char *volt, int sample)
{
	//TODO: Implement
	return 0;
}

/**
 * \brief     Release the adc channel
 * \param     ADCHandle handle
 * \return    int retVal : always 0
 */
int OSADCRelease(ADCHandle handle)
{
	//TODO: Complete
	HDTClearADC((HDTDevADC_t*)handle);
	return 0;
}
