/*
 * latches.h
 *
 *  Created on: 5 Jul 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file    latches.h
 * \brief   Header file for the latches functions
 * \author  Remi KEAT
 */

#ifndef LATCHES_H_
#define LATCHES_H_

#include "spi.h"

//IO buffer banks
#define IOBANK0 0
#define IOBANK1 1
#define LATCH0  0
#define LATCH15 15
#define IN  0
#define OUT 1

int OSLatchSetup(int latchnum, int direction);
int OSLatchBankSetup(int banknum, int direction);
int OSLatchRead(int latchnum);
int OSLatchWrite(int latchnum, int state);
int OSLatchInit(void);
int OSLatchCleanup(void);

#endif /* LATCHES_H_ */
