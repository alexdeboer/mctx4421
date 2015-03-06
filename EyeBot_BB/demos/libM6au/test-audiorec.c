/**************************************************************************************************
| File:         test-audiorec.c
| Author/Date:  SSTeoh / 14/7/08
| Description:  Test program for audio recording
***************************************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "../libM6lcd/libM6lcd.h"
#include "../libM6key/libM6key.h"
#include "../libM6time/libM6time.h"
#include "../libM6au/libM6au.h"
#include "../libM6au/libM6au-types.h"
#include "../libM6au/libM6au-intern.h"

#define RECFILENAME "/tmp/aurecfile.wav"
#define FILESIZE128K (1<<17)
#define FILESIZE256K (1<<18)
#define FILESIZE512K (1<<19)
#define FILESIZE1024K (1<<20)

#define RECFILESIZE FILESIZE1024K

#define BARMIN 0
#define BARMAX (RECFILESIZE*8*95)/(44100*16)

fbinfo_t fb;
unsigned int width;
unsigned int height;

int Playaufile(void);
int Recaufile(void);
long getfilelength(char *file);
long updatefilelength(char *file);

int Recaufile(void)
{
  	char *file = RECFILENAME;
  	long len = RECFILESIZE;
  	long freq = 44100;
	int p;
	int oscntdiff=0;
	int oscnt_start;
  	int elapsed=0;

    LCDTextBar (7, 0, fb.cursor.xmax, 0, M6LCD_GREEN);
  	LCDSetString (5, 20, "recording...");
	LCDSetPrintf(5,45,"%2d:%02d",0,0);
	oscnt_start = OSGetCount();
  	AURecordFile (file, len, freq);

  	while (!AUCheckRecord()) {
		if (KEYRead() == M6KEY2) {
			AUStopPlayRec();
  			while (!AUCheckRecord()) ;
			updatefilelength(file);
			break;
		}
		oscntdiff = OSGetCount()-oscnt_start;
		if (elapsed != oscntdiff/100)  {
			elapsed = oscntdiff/100;
			LCDSetPrintf(5,45,"%2d:%02d",elapsed/60,elapsed%60);
		}
    	if (oscntdiff > BARMAX)
      		oscntdiff = BARMAX;
		p = 100 * (oscntdiff - BARMIN) / (BARMAX - BARMIN);
    	LCDTextBar (7, 0, fb.cursor.xmax, p, M6LCD_GREEN);
		OSWait(10);
	}
  	LCDSetString (5, 20, "recording...end");
	OSWait(100);
	LCDClear();
  	return 0;
}


int Playaufile(void)
{
  	char *file = RECFILENAME;
	int p;
	int oscntdiff=0;
  	int elapsed=0;
	int oscnt_start;
	long filelength;
	int barmax;

	filelength = getfilelength(file);
	barmax = (int)((filelength*8*95)/(44100*16));
	//printf("filel=%ld\nBarmx=%d\n", filelength, barmax);
	oscnt_start = OSGetCount();
  	if (AUPlayFile (file)) {
  		LCDSetString (5, 16, "No recorded file to play");
		OSWait(100);
  		LCDSetString (5, 16, "                                 ");
		return -1;
	}
	LCDSetString (5, 20, "playing...");
    LCDTextBar (7, 0, fb.cursor.xmax, 0, M6LCD_GREEN);
	LCDSetPrintf(5,45,"%2d:%02d",0,0);

  	while (!AUCheckSample()) {
		if (KEYRead() == M6KEY2) {
			AUStopPlayRec();
			break;
		}
		oscntdiff = OSGetCount()-oscnt_start;
		if (elapsed != oscntdiff/100)    {
			elapsed = oscntdiff/100;
			LCDSetPrintf(5,45,"%2d:%02d",elapsed/60,elapsed%60);
		}
    	if (oscntdiff > barmax)
      		oscntdiff = barmax;
		p = 100 * (oscntdiff - BARMIN) / (barmax - BARMIN);
    	LCDTextBar (7, 0, fb.cursor.xmax, p, M6LCD_GREEN);
		OSWait(10);
	}
	OSWait(100);
	LCDClear();

  	return 0;
}



int main()
{
	int done=0;
	int test;
	keycode_t keycode;
  	LCDGetFBInfo(&fb);
  	width = fb.cursor.xmax + 1;
  	height = fb.cursor.ymax + 1;

	if((test=LCDInit(M6LCD_BIGFONT))<0)
	{
		printf("Cannot initialize M6LCD library! [%d]\n",test);
		return -1;
	}
	if((test=KEYInit())<0)
	{
		printf("Cannot initialize M6KEY library! [%d]\n",test);
		return -1;
	}

	LCDMenu(" PLAY"," STOP","  REC"," EXIT");
	LCDSetMode(M6LCD_SHOWMENU);

	while (!done)
	{
		LCDSetPrintf(0,0,"Test sound recording");
		switch (keycode = KEYGet())
		{
		  	case M6KEY1:	//Play
				Playaufile();
		  		break;

		  	case M6KEY2:	//stop
		  		break;

		  	case M6KEY3:	//rec
				Recaufile();
		  		break;

		  	case M6KEY4:
				done=1;
      			LCDSetString (5, 20, "Exit...");
		  		break;
		}
	}
	//LCDSetMode(M6LCD_HIDEMENU);
	LCDClear();
	LCDRelease();
	KEYRelease();
	return 0;
}


long getfilelength(char *file)
{
  	int file_descriptor;          /* of the sample file */
  	off_t file_length;
  	char *file_name = file;       /* of the sample file */

  	if ((file_descriptor = open (file_name, O_RDONLY)) == -1) {
		printf("unable to open file\n");
      	return -1;
    }
  	file_length = lseek (file_descriptor, 0, SEEK_END);
	close(file_descriptor);
	return file_length;
}

long updatefilelength(char *file)
{
  	int file_descriptor;
  	off_t file_length;
  	char *file_name = file;
  	header_t params;
  	const int header_length = 12 + 24 + 8;
  	char header[header_length];

  	if ((file_descriptor = open (file_name, O_RDWR)) == -1) {
		printf("Unable to open rec file\n");
      	return -1;
    }
  	file_length = lseek (file_descriptor, 0, SEEK_END);

 	lseek (file_descriptor, 0, SEEK_SET);
  	if ((read (file_descriptor, header, header_length)) <= 0) {
		printf("Save rec file error\n");
		close(file_descriptor);
      	return -1;
	}
  	extract_header_params (&params, header);
	//print_header_params (&params);

  	params.data_size = file_length-36 -8;
  	params.riff_size = file_length-8;
  	create_header (header, &params);

 	lseek (file_descriptor, 0, SEEK_SET);
	if ((write (file_descriptor, header, header_length))==-1) {
		printf("Save rec file error\n");
		close(file_descriptor);
      	return -1;
	}

	close(file_descriptor);

	return 0;
}


