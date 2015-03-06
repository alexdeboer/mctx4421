#include <stdio.h>
#include <eyebot.h>

int run1()
{
	int exit = 0;
	int ret = 0;
	keycode_t key = 0x0;
	LCDClear();
	LCDMenu("Get Image","","","EXIT");
	PICAM cam = CAMInitPiCam("");
	//CAMHandle cam = CAMInit("");
	BYTE image[320*240*3];		//change to a fixed array of values
	LCDRefresh();
	while (exit == 0)
	{
		LCDRefresh();
		key = KEYGet();
		if (key == KEY1)
		{
			fprintf(stderr,"Get image\n");
			//CAMGetFrameRGBRPi(cam,image);
			CAMGetFrameRGBRPi(NULL,image);
			fprintf(stderr,"Done\n");
			if (image == NULL)
			{
				fprintf(stderr,"ERROR: image is NULL\n");
				ret = -1;
				break;
			}
			fprintf(stderr,"Print image\n");
			LCDPutImageRGB(0,0,240,180,image);
			fprintf(stderr,"Done\n");
		}
		if (key == KEY2)
		{
			LCDClear();
			LCDRefresh();
			fprintf(stderr,"Refresh screen\n");
		}
		if (key == KEY4)
		{
			exit = 1;
		}
	}
	CAMReleasePiCam(cam);
	//CAMRelease(cam);
	return ret;
}

int run2()
{
	PICAM cam = raspiCamCvCreateCameraCapture(0);
	int exit = 0;
	keycode_t key = 0x0;
	LCDClear();
	LCDMenu("","","","EXIT");
	BYTE image[320*240*3];
	int compression[2] = {CV_IMWRITE_JPEG_QUALITY,80};
	IplImage *image2;
	LCDRefresh();
	while (exit == 0)
	{
		LCDRefresh2();
		image2 = raspiCamCvQueryFrame(cam);
		cvCvtColor(image2,image2,CV_BGR2RGB);
		cvShowImage("Image",image2);
		cvWaitKey(1);
		cvSaveImage("temp.jpeg",image2,compression);
		LCDPutImageRGB(0,0,240,180,image2->imageData);
		key = KEYRead();
		if (key == KEY2)
		{
			LCDClear();
			LCDRefresh();
		}
		if (key == KEY4)
		{
			exit = 1;
		}
		//LCDMenu("","","","EXIT");
	}
	raspiCamCvReleaseCapture(&cam);
	fprintf(stderr,"Exiting run 2\n");
	return 0;
}

int main(int argc, char**argv)
{
	int ret = 0;
	fprintf(stderr,"Camera demo start\n");
	if (LCDInit()<0)
	{
		fprintf(stderr,"ERROR: failed to initialise the LCD\n");
		exit(1);
	}
	if (KEYInit()<0)
	{
		fprintf(stderr,"ERROR: failed to initialise the keys\n");
		exit(1);
	}
	ret = run2();
	LCDRelease();
	KEYRelease();
	fprintf(stderr,"Camera demo end\n");
	return ret;
}
