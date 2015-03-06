/*
 * globals.h
 *
 *  Created on: 21 Jun 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file    globals.h
 * \brief   Header file for global variables
 * \author  Remi KEAT
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

//#include "ftd2xx.h"
#include "types.h"

extern struct mpsse_context* gDeviceHandle;
extern LCDHandle* gLCDHandle;
extern bool gLCDEnabled;
extern int gCurPosX, gCurPosY;
extern int gMousePosX, gMousePosY, gMouseButton;
extern touch_map_t* gTouchMap;

#endif /* GLOBALS_H_ */
