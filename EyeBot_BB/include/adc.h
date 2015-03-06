/*
 * adc.h
 *
 *  Created on: 19 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file			adc.h
 * \brief     Header file for the ADC functions
 * \author    Remi KEAT
 */

#ifndef ADC_H_
#define ADC_H_

#include "spi.h"

ADCHandle OSInitADC(DeviceSemantics semantics);
int OSADCRelease(ADCHandle handle);
int OSGetADC(ADCHandle adchandle);
int ConvADCSampleToVoltage(ADCHandle adchandle, char *volt, int sample);

#endif /* ADC_H_ */
