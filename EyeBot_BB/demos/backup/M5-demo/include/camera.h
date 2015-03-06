/*
 * camera.h
 *
 *  Created on: 3 Sep 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		camera.h
 * \brief		Header file for the camera functions
 * \author  Remi KEAT
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "globals.h"
//#include "types.h"

CAMHandle CAMInit(DeviceSemantics semantics);
int CAMGetFrameRGB (CAMHandle handle, BYTE *buf);

#endif /* CAMERA_H_ */
