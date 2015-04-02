/*
 * globals.c
 *
 *  Created on: 21 Jun 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file    globals.c
 * \brief   Defines global variables
 * \author  Remi KEAT
 */

#include "globals.h"

struct mpsse_context*	gDeviceHandle;
LCDHandle* gLCDHandle;
bool gLCDEnabled;
int gCurPosX, gCurPosY;
int gMousePosX, gMousePosY, gMouseButton;
touch_map_t* gTouchMap;
