/*
 * irtv.h
 *
 *  Created on: 19 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		irtv.h
 * \brief   Header file for the IRTV functions
 * \author  Remi KEAT
 */

#ifndef IRTV_H_
#define IRTV_H_

#include "spi.h"

int IRTVInit(DeviceSemantics semantics);
int IRTVRead(void);
void IRTVRelease(void);

#endif /* IRTV_H_ */
