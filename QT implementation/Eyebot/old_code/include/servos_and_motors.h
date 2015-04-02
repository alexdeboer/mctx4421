/*
 * servos_and_motors.h
 *
 *  Created on: 10 Jun 2013
 *      Author: Remi KEAT
 */

/**
 * \file    servos_and_motors.h
 * \brief   Header file for the servos and motors functions
 * \author  Remi KEAT
 */

#ifndef SERVOS_AND_MOTORS_H_
#define SERVOS_AND_MOTORS_H_

#include "spi.h"

SERVOHandle SERVOInit(DeviceSemantics semantics);
int SERVORelease (SERVOHandle handle);
int SERVOSet (SERVOHandle handle, int angle);
MOTORHandle MOTORInit(DeviceSemantics semantics);
int MOTORRelease (MOTORHandle handle);
int MOTORDrive (MOTORHandle handle, int speed);
long QUADRead(QUADHandle handle);
int QUADReset(QUADHandle handle);
int QUADRelease(QUADHandle handle);

#endif /* SERVOS_AND_MOTORS_H_ */
