/*
 * camera.c
 *
 *  Created on: 3 Sep 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		camera.c
 * \brief		Defines functions for the camera
 * \author  Remi KEAT
 */

#include <stdio.h>
#include <camera.h>

RaspiCamCvCapture *cam;

/**
 * \brief		Configure & Initialize camera
 * \param		DeviceSemantics semantics : handle of the desired camera
 * \return  CAMHandle handle
 */
CAMHandle CAMInit(DeviceSemantics semantics)
{
	//TODO: Complete
	HDTDevCAM_t* pcam;
	//pcam = (HDTDevCAM_t*)HDTLoadCAM(HDT_FILE, semantics);
	return (CAMHandle)pcam;
}

PICAM CAMInitPiCam(DeviceSemantics semantics)
{
	//RaspiCamCvCapture *cam;
	
	cam = raspiCamCvCreateCameraCapture(0);
	//return cam;
	return NULL;
}

int CAMRelease(CAMHandle handle)
{
	return 0;
}

int CAMReleasePiCam(PICAM cam2)
{
	raspiCamCvReleaseCapture(&cam);
	return 0;
}

int CAMSelect(int cam)
{
	return 0;
}

int CAMWhich(void)
{
	return 0;
}

int CAMSet(CAMHandle handle, CAMSettings *camset)
{
	return 0;
}

int CAMGet(CAMHandle handle, CAMSettings *camset)
{
	return 0;
}

int CAMGetFrameGray(CAMHandle handle, BYTE *buf)
{
	return 0;
}

/**
 * \brief		Reads one full color image in RBG format, 3 bytes per pixel
 * \param		CAMHandle handle : handle of the desired camera
 * \param   BYTE* buf : pointer to image buffer of full size (use CAMGet)
 * \return  int retVal : return code\n
 * 					0 = success\n
 * 					-1 = error (camera not initialized)
 */
int CAMGetFrameRGB (CAMHandle handle, BYTE *buf)
{
	system("raspistill -o temp.jpeg -w 640 -h 480 -n -t 500");
	/*IplImage *image = cvLoadImage("temp.jpeg");
	*/
	return 0;
}

int CAMGetFrameRGBRPi(PICAM cam2, BYTE *buf)
{
	//system("raspistill -o temp.jpeg -w 320 -h 240 -n -t 100");
	fprintf(stderr,"	Get image start\n");
	int compression[2] = {CV_IMWRITE_JPEG_QUALITY,80};
	fprintf(stderr,"	Get frame from camera\n");
	IplImage *image;
	image = raspiCamCvQueryFrame(cam);
	fprintf(stderr,"	Done\n	Convert image format\n");
	cvCvtColor(image,image,CV_BGR2RGB);
	fprintf(stderr,"	Done\n	Save image to file\n");
	cvSaveImage("temp.jpeg",image,compression);
	fprintf(stderr,"	Done\nRelease image\n");
	cvReleaseImage(&image);
	fprintf(stderr,"Done\n");
	return 0;
}

int CAMGetFrameBayer(CAMHandle handle, BYTE *buf)
{
	return 0;
}
