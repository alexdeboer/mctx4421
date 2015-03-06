#include <stdio.h>
#include <eyebot.h>

int main(int argc, char**argv)
{
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
	int exit = 0;
	int ret = 0;
	keycode_t key = 0x0;
	LCDClear();
	LCDMenu("Get Image","","","EXIT");
	PICAM cam = CAMInitPiCam("");
	BYTE *image = malloc(640*480*3);
	LCDRefresh();
	while (exit == 0)
	{
		LCDRefresh();
		key = KEYGet();
		if (key == KEY1)
		{
			fprintf(stderr,"Get image\n");
			CAMGetFrameRGBRPi(cam,image);
			if (image == NULL)
			{
				fprintf(stderr,"ERROR: image is NULL\n");
				ret = -1;
				break;
			}
			fprintf(stderr,"Print image\n");
			LCDPutImageRGB(0,0,200,200,image);
			fprintf(stderr,"Done\n");
		}
		if (key == KEY4)
		{
			exit = 1;
		}
	}
	LCDRelease();
	KEYRelease();
	fprintf(stderr,"Camera demo end\n");
	return ret;
}
