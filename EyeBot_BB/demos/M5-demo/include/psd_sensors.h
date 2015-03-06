/*
 * psd_sensors.h
 *
 *  Created on: 10 Jun 2013
 *      Author: Remi KEAT
 */

/**
 * \file    psd_sensors.h
 * \brief   Header file for the PSD sensors functions
 * \author  Remi KEAT
 */

#ifndef PSD_SENSORS_H_
#define PSD_SENSORS_H_

#include "spi.h"

PSDHandle PSDInit(DeviceSemantics semantics);
int PSDGetRaw(PSDHandle psd);
int PSDGet(PSDHandle psd);
int PSDRelease(PSDHandle psd);

#endif /* PSD_SENSORS_H_ */
