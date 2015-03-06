/*
 * vomega.h
 *
 *  Created on: 3 Sep 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		vomega.h
 * \brief		Header file for the VW functions
 * \author  Remi KEAT
 */

#ifndef VOMEGA_H_
#define VOMEGA_H_

#include "spi.h"

VWHandle VWInit(DeviceSemantics semantics, int Timescale);
int VWDriveStraight(VWHandle handle, meter delta, meterPerSec v);
int VWDriveTurn(VWHandle handle, radians delta, radPerSec w);
int VWDriveWait(VWHandle handle);

#endif /* VOMEGA_H_ */
