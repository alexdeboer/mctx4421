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

//openCV headers
#include <opencv/cv.h>
#include <opencv/highgui.h>

//raspicam header
#include "RaspiCamCV.h"

#include "globals.h"
//#include "types.h"

#define RPICAM "rpicam"
#define USBCAM "usbcam"

typedef RaspiCamCvCapture * PICAM;

typedef struct _CAMSettings
{
	int width, height, length;	// frame info
	int connected;	// Availability
} CAMSettings;

CAMHandle CAMInit(DeviceSemantics semantics);
PICAM CAMInitPiCam(DeviceSemantics semantics);
int CAMRelease(CAMHandle handle);
int CAMReleasePiCam(PICAM cam);
int CAMSelect(int cam);
int CAMWhich(void);
int CAMSet(CAMHandle handle, CAMSettings *camset);
int CAMGet(CAMHandle handle, CAMSettings *camset);
int CAMGetFrameGray(CAMHandle handle, BYTE *buf);
int CAMGetFrameRGB (CAMHandle handle, BYTE *buf);
int CAMGetFrameRGBRPi(PICAM cam, BYTE *buf);
int CAMGetFrameBayer(CAMHandle handle, BYTE *buf);

#endif /* CAMERA_H_ */
