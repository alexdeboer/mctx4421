/** 20080917 - azman@ee.uwa.edu.au */

#include <unistd.h> /* access()? */
#include <stdlib.h> /* malloc() */
#include <stdio.h> /* sprintf() */
#include <stdarg.h> /* va_list */
#include <string.h> /* memset()  */

//#include "../libM6lcd/libM6lcd.h"
//#include "../libM6key/libM6key.h"
//#include "../libM6hdt/libM6hdt.h"
//#include "../libM6irtv/libM6irtv.h"
//#include "../libM6bot/libM6bot.h"
//#include "../libM6dio/libM6dio.h"
//#include "../libM6time/libM6time.h"
//#include "../libM6adc/libM6adc.h"
//#include "../libM6misc/types.h"
#include <eyebot.h>
#include "header.h"

//#define MAX_WAITTIME 250000 /* usec */
//#define MAX_WAITKILL 100000 /* usec */
#define MSG_WAITTIME 2 /* sec */

#define MSG_POS_Y 2
#define MSG_POS_X 2

typedef struct _HDTDevALL_t
{
	HDTTable_t* ptabmotor;
	HDTTable_t* ptabpsd;
	HDTDevMOTOR_t* pmotor;
	HDTDevENCODER_t* pencoder;
	HDTDevPSD_t* ppsd;
	HDTDevIRTV_t* pirtv;
	HDTDevSERVO_t* pservo;
	HDTDevADC_t* padc;
	int countMOTOR;
	int countENCODER;
	int countPSD;
	int countIRTV;
	int countSERVO;
	int countADC;
}
HDTDevALL_t;

static fbinfo_t fb;

char* IRTVTypeString(int type)
{
	switch(type)
	{
	case 0: return "RAW_CODE";
	case 1: return "SPACE_CODE";
	case 2: return "PULSE_CODE";
	case 3: return "MANCHESTER_CODE";
	}
	return "";
}

char* IRTVModeString(int mode)
{
	switch(mode)
	{
	case 0: return "DEFAULT_MODE";
	case 1: return "SLOPPY_MODE";
	case 2: return "REPCODE_MODE";
	}
	return "";
}

/* ------------------------------------------------------------------------- */
int load_devs(HDTDevice_t* pdevices, int type, int count, listmenu_t *plist)
{
	int loop;
	char *title;
	HDTDevice_t* pdev;

	/* are you sure? */
	if(count<0)
		return -1;

	/* check device type */
	switch(type)
	{
	case HDT_IDX_PSD:
		title = "PSD List";
		break;
	case HDT_IDX_SERVO:
		title = "Servo List";
		break;
	case HDT_IDX_MOTOR:
		title = "Motor List";
		break;
	case HDT_IDX_ENCODER:
		title = "Encoder List";
		break;
	case HDT_IDX_DRIVE:
		title = "Drive Module List";
		break;
	case HDT_IDX_COMPASS:
		title = "Compass List";
		break;
	case HDT_IDX_IRTV:
		title = "IRTV List";
		break;
	case HDT_IDX_CAM:
		title = "Camera List";
		break;
	case HDT_IDX_ADC:
		title = "ADC List";
		break;
	default:
		title = 0x0;
		break;
	}

	/* unsupported device type */
	if(!title)
		return -2;

	/* setup filemenu list */
	listmenuInit(plist,title);
	for(loop=0, pdev=pdevices; loop<count && pdev; loop++, pdev=pdev->pnext)
	{
		listmenuAddItem(plist,pdev->name,(void *)pdev);
	}
	LCDList(plist);

	return loop;
}
/* ------------------------------------------------------------------------- */
int show_info(HDTDevice_t* pdev, int type)
{
	HDTDevMOTOR_t* pmotor;
	HDTDevENCODER_t* pencoder;
	HDTDevPSD_t* ppsd;
	HDTDevIRTV_t* pirtv;
	HDTDevSERVO_t* pservo;
	HDTDevADC_t* padc;
	fbinfo_t fb;

	LCDClear();
	LCDSetMode(LCD_CLASSICMENU);
	KEYSetTM(KEYTM_CLASSIC);
	LCDGetFBInfo(&fb);
	LCDMenu("","",""," BACK");

	switch(type)
	{
	case HDT_IDX_PSD:
		ppsd = (HDTDevPSD_t*) pdev;
		LCDPrintf("PSD Name: \"%s\"\n",ppsd->name);
		LCDPrintf("=> RegAddr: 0x%06x\n",ppsd->regaddr);
		LCDPrintf("=> Table: \"%s\"\n",ppsd->ptable?
			ppsd->ptable->name:"NOT FOUND!");
		break;
	case HDT_IDX_SERVO:
		pservo = (HDTDevSERVO_t*) pdev;
		LCDPrintf("Servo Name: \"%s\"\n",pservo->name);
		LCDPrintf("=> RegAddr: 0x%06x\n",pservo->regaddr);
		LCDPrintf("=> Freq: %d\n", pservo->freq);
		LCDPrintf("=> PWM min: %i us\n",pservo->mintime);
		LCDPrintf("=> PWM max: %i us\n",pservo->maxtime);
		break;
	case HDT_IDX_MOTOR:
		pmotor = (HDTDevMOTOR_t*) pdev;
		LCDPrintf("Motor Name: \"%s\"\n",pmotor->name);
		LCDPrintf("=> RegAddr: 0x%06x\n",pmotor->regaddr);
		LCDPrintf("=> Freq: %d\n",pmotor->freq);
		LCDPrintf("=> Table: \"%s\"\n",pmotor->ptable?
			pmotor->ptable->name:"NOT FOUND!");
		break;
	case HDT_IDX_ENCODER:
		pencoder = (HDTDevENCODER_t*) pdev;
		LCDPrintf("Encoder Name: \"%s\"\n",pencoder->name);
		LCDPrintf("=> RegAddr: 0x%06x\n",pencoder->regaddr);
		LCDPrintf("=> Clickspm: %d\n",pencoder->clickspm);
		LCDPrintf("=> Motor: \"%s\"\n",pencoder->pmotor?
			pencoder->pmotor->name:"NOT FOUND!");
		break;
	case HDT_IDX_DRIVE:
		break;
	case HDT_IDX_COMPASS:
		break;
	case HDT_IDX_IRTV:
		pirtv = (HDTDevIRTV_t*) pdev;
		LCDPrintf("IRTV \"%s\" Found!\n",pirtv->name);
		LCDPrintf("=> Type: %s\n",IRTVTypeString(pirtv->type));
		LCDPrintf("=> Length: %d\n",pirtv->length);
		LCDPrintf("=> Toggle Mask: 0x%04x\n",pirtv->togmask);
		LCDPrintf("=> Invert Mask: 0x%04x\n",pirtv->invmask);
		LCDPrintf("=> Mode: %s\n",IRTVModeString(pirtv->mode));
		LCDPrintf("=> Buffer Size: %d\n",pirtv->buffsize);
		LCDPrintf("=> Delay: %d\n",pirtv->delay);
		break;
	case HDT_IDX_CAM:
        LCDPrintf("Raspi-Cam\n");    
		break;
	case HDT_IDX_ADC:
		padc = (HDTDevADC_t*) pdev;
		LCDPrintf("ADC \"%s\" Found!\n",padc->name);
		break;
	default:
		break;
	}
	
	LCDRefresh();
	int exit = 0;
	while (exit == 0)
	{
		switch (keycode)
		{
			case KEY4:
			{
				exit = 1;
				break;
			}
		}
		LCDRefresh3();
		usleep(LOOPSLEEPTIME);
	}
	LCDSetMode(LCD_LISTMENU);

	return 0;
}
/* ------------------------------------------------------------------------- */
int test_motor(HDTDevice_t* pdev)
{
	int row = 5, speed = 0, step = 10, dir = 0, setspeed, changed = 0;
	HDTDevMOTOR_t* pmotor = (HDTDevMOTOR_t*) pdev;
	MOTORHandle testmotor;

	LCDClear();
	LCDMenu(" DIR"," INC SPD","DEC SPD"," DONE");
	LCDPrintf("Testing Motor \"%s\"\n",pmotor->name);
	LCDPrintf("=> RegAddr: 0x%06x\n",pmotor->regaddr);
	LCDPrintf("=> Freq: %d\n",pmotor->freq);
	LCDPrintf("=> Table: \"%s\"\n",pmotor->ptable?
			pmotor->ptable->name:"NOT FOUND!");

	testmotor = MOTORInit(pmotor->name);
	if(!testmotor)
		LCDPrintf("Error! MOTORInit \"%s\"\n",pmotor->name);
	else
	{
		MOTORDrive(testmotor,speed);
		LCDSetPrintf(row,0,"Speed = %3d/100",speed);
		LCDSetPrintf(row+2,0,"Direction = %d",dir);
	}
	keycode = 0x0;
	LCDRefresh();
	int exit = 0;
	while(exit==0)
	{
		LCDRefresh2();
		switch(keycode)
		{
			case KEY1:
			{
				dir ^= 0x01;
				changed = 1;
				keycode = 0x0;
				break;
			}
			case KEY2:
			{
				if(speed<=(100-step))
					speed += step;
				changed = 1;
				keycode = 0x0;
				break;
			}
			case KEY3:
			{
				if(speed>=step)
					speed -= step;
				changed = 1;
				keycode = 0x0;
				break;
			}
			case KEY4:
			{
				exit = 1;
				break;
			}
		}
		if(changed)
		{
			setspeed = speed;
			if(dir) setspeed = -setspeed;
			MOTORDrive(testmotor,setspeed);
			LCDSetPrintf(row,0,"Speed = %3d/100",speed);
			LCDSetPrintf(row+2,0,"Direction = %d",dir);
			LCDRefresh();
			changed = 0;
		}
	}
	MOTORRelease(testmotor);

	return 0;
}
/* ------------------------------------------------------------------------- */
int test_encoder(HDTDevice_t* pdev)
{
	int row = 5, value, speed = 0, step = 10, dir = 0, setspeed, changed = 0;
	HDTDevENCODER_t* penc = (HDTDevENCODER_t*) pdev;
	char *motorname = malloc(LCD_LIST_STRLENGTH);
	QUADHandle testquad;
	MOTORHandle testmotor;

	LCDClear();
	LCDMenu(" DIR"," INC SPD","DEC SPD"," DONE");

	LCDPrintf(" Testing Encoder \"%s\"\n",penc->name);
	LCDPrintf("=> RegAddr: 0x%06x\n",penc->regaddr);
	LCDPrintf("=> Clickspm: %d\n",penc->clickspm);
			
	testquad = QUADInit(penc->name);
	if(!testquad)
		LCDPrintf("Error! QUADInit \"%s\"\n",penc->name);
	else
		motorname = (char *) QUADGetMotor(testquad);
	testmotor = MOTORInit(motorname);
	if(!testmotor)
		LCDPrintf("Error! MOTORInit \"%s\"\n",motorname);
	else
		LCDPrintf("=> Motor: \"%s\"\n",motorname);
	if(testquad&&testmotor)
	{
		QUADReset(testquad);
		MOTORDrive(testmotor,speed);
		value = QUADRead(testquad);
		LCDSetPrintf(row,0,"Decoder = %d (%04x)",value,value);
		LCDSetPrintf(row+2,0,"Speed = %3d/100",speed);
		LCDSetPrintf(row+4,0,"Direction = %1d",dir);
	}
	LCDRefresh();
	keycode = 0x0;
	int exit = 0;
	while(exit==0)
	{
		switch(keycode)
		{
			case KEY1:
			{
				dir ^= 0x01;
				changed = 1;
				keycode = 0x0;
				break;
			}
			case KEY2:
			{
				if(speed<=(100-step))
					speed += step;
				changed = 1;
				keycode = 0x0;
				break;
			}
			case KEY3:
			{
				if(speed>=step)
					speed -= step;
				changed = 1;
				keycode = 0x0;
				break;
			}
			case KEY4:
			{
				exit = 1;
				break;
			}
		}
		if(changed)
		{
			setspeed = speed;
			if(dir) setspeed = -setspeed;
			MOTORDrive(testmotor,setspeed);
			LCDSetPrintf(row+2,0,"Speed = %3d/100",speed);
			LCDSetPrintf(row+4,0,"Direction = %d",dir);
			changed = 0;
		}
		value = QUADRead(testquad);
		LCDSetPrintf(row,0,"Decoder = %d (%04x)",value,value);
		LCDRefresh3();
		//OSWait(10);
	}
	MOTORRelease(testmotor);
	QUADRelease(testquad);
	
	return 0;
}
/* ------------------------------------------------------------------------- */
int test_psd(HDTDevice_t* pdev)
{
	int row = 5, col = 2;
	HDTDevPSD_t* ppsd = (HDTDevPSD_t*) pdev;
	PSDHandle testpsd;

	LCDClear();
	LCDMenu("","",""," DONE");

	LCDPrintf("Testing PSD \"%s\"\n",ppsd->name);
	LCDPrintf("PSD Name: \"%s\"\n",ppsd->name);
	LCDPrintf("=> Table: \"%s\"\n",ppsd->ptable?
			ppsd->ptable->name:"NOT FOUND!");
	LCDPrintf("=> RegAddr: 0x%06x\n",ppsd->regaddr);
	LCDRefresh();
	testpsd = PSDInit(ppsd->name);
	if(!testpsd)
		LCDPrintf("Error! PSDInit \"%s\"\n",ppsd->name);
	while(keycode!=KEY4)
	{
		LCDSetPrintf(row,col,"Raw Value = %4i", PSDGetRaw(testpsd));
		LCDSetPrintf(row+1,col,"Table Value = %4icm", PSDGet(testpsd));
		LCDRefresh3();
		//OSWait(10);
	}
	PSDRelease(testpsd);

	return 0;
}
/* ------------------------------------------------------------------------- */
int test_servo(HDTDevice_t* pdev)
{
	int row = 5, angle = 180, step = 10, changed = 0, posPerc = 0;
	HDTDevSERVO_t* pservo = (HDTDevSERVO_t*) pdev;
	SERVOHandle testservo;
	int maxAngle = 360;

	LCDClear();
	LCDMenu("MIN"," INCREASE"," DECREASE"," DONE");
	LCDPrintf("Testing Servo \"%s\"\n",pservo->name);
	LCDPrintf("=> RegAddr: 0x%06x\n",pservo->regaddr);
	LCDPrintf("=> Freq: %d\n", pservo->freq);
	LCDPrintf("=> PWM min: %i us\n",pservo->mintime);
	LCDPrintf("=> PWM max: %i us\n",pservo->maxtime);

	testservo = SERVOInit(pservo->name);
	if(!testservo)
		LCDPrintf("Error! SERVOInit \"%s\"\n",pservo->name);
	else
	{
		SERVOSet(testservo,angle);
		posPerc = (angle*100)/maxAngle;
		LCDSetPrintf(row,0,"Servo position (Value, percent):\n%3d, %3d%%",angle,posPerc);
	}
	LCDRefresh();
	keycode = 0x0;
	int exit = 0;
	while(exit == 0)
	{
		switch(keycode)
		{
			case KEY1:
			{
				if (angle <= maxAngle && angle > 0)
				{
					angle = 0;
					changed = 1;
				}
				else
				{
					angle = maxAngle;
					changed = 1;
				}
				keycode = 0x0;
				break;
			}
			case KEY2:
			{
				angle += step;
				if(angle>maxAngle) angle = maxAngle;
				changed = 1;
				keycode = 0x0;
				break;
			}
			case KEY3:
			{
				angle -= step;
				if(angle<0) angle = 0;
				changed = 1;
				keycode = 0x0;
				break;
			}
			case KEY4:
			{
				exit = 1;
				break;
			}
		}
		if(changed)
		{
			SERVOSet(testservo,angle);
			posPerc = (angle*100)/maxAngle;
			LCDSetPrintf(row,0,"Servo position (Value, percent):\n%3d, %3d%%",angle,posPerc);
			LCDRefresh();
			changed = 0;
			if (angle <= maxAngle && angle > 0)
			{
				LCDMenu("MIN"," INCREASE"," DECREASE"," DONE");
			}
			else if (angle == 0)
			{
				LCDMenu("MAX"," INCREASE"," DECREASE"," DONE");
			}
		}
		LCDRefresh3();
	}
	SERVORelease(testservo);

	return 0;
}
/* ------------------------------------------------------------------------- */
int test_irtv(HDTDevice_t* pdev)
{
#define ROW1 4
#define ROW2 6
	int row = ROW1, col = 0, mask = 0, loop = 0, code;
	HDTDevIRTV_t* pirtv = (HDTDevIRTV_t*) pdev;

	while(loop<pirtv->length)
	{
		mask |= (0x01 << loop);
		loop++;
	}

	LCDClear();
	LCDMenu("","",""," DONE");
	LCDPrintf("IRTV \"%s\" Found!\n",pirtv->name);
	LCDPrintf("=> Type: %s\n",IRTVTypeString(pirtv->type));
	LCDPrintf("=> Length: %d\n",pirtv->length);
	LCDPrintf("=> Toggle Mask: 0x%04x\n",pirtv->togmask);
	LCDPrintf("=> Invert Mask: 0x%04x\n",pirtv->invmask);
	LCDPrintf("=> Mode: %s\n",IRTVModeString(pirtv->mode));
	LCDPrintf("=> Buffer Size: %d\n",pirtv->buffsize);
	LCDPrintf("=> Delay: %d\n",pirtv->delay);
	LCDPrintf("\nTesting IRTV \"%s\" (Press any key on remote)\n\n",pirtv->name);
	LCDPrintf("Code Mask : 0x%08x (Length = %d bit)", mask, pirtv->length);
	LCDRefresh();
	if(IRTVInit(pirtv->name))
		LCDPrintf("Error! IRTVInit \"%s\"\n",pirtv->name);
	while(KEYGet()!=KEY4)
	{
		code = IRTVGet();
		if(code)
		{
			LCDSetPrintf(row,col,"Raw Code = 0x%08x", code&mask);
			row = (row==ROW1?ROW2:ROW1);
			LCDSetPrintf(row,col,"                     ");
			LCDRefresh();
		}
	}
	IRTVRelease();

	return 0;
}
/* ------------------------------------------------------------------------- */
int test_dio(void)
{
	int i, latch_n;
	listmenu_t *lcdlist;

	lcdlist = LCDGetList();
	LCDSetList(0x0);
	LCDClear();
	LCDMenu("","",""," DONE");
	LCDPrintf("Status of digital inputs:\n");
	OSLatchInit();


	if ( (i=OSLatchBankSetup(IOBANK0, IN)) != 0 )
	{
		printf("Set bank 0 error %d\n", i);
		return 0;
	}
	if ( (i=OSLatchBankSetup(IOBANK1, IN)) != 0 )
	{
		printf("Set bank 1 error %d\n", i);
		return 0;
	}

	for (latch_n=LATCH0; latch_n<=LATCH15; latch_n++)
	{
		OSLatchSetup(latch_n, IN);
	}

	LCDSetPrintf(4, 0, "PIN: 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15");
	LCDRefresh();

	while(keycode!=KEY4)
	{
		for (latch_n=LATCH0; latch_n<=LATCH15; latch_n++)
		{
			LCDSetPrintf(5, (latch_n+1)*3+2, "%d",OSLatchRead(latch_n));
			printf("%d  ",OSLatchRead(latch_n));
			LCDRefresh2();
		}
		printf("\n");
		LCDRefresh3();
		//OSWait(10);
	}
	OSLatchCleanup();
	LCDSetList(lcdlist);
	LCDClear();
	LCDMenu(" SELECT", " RELOAD", " DIGITAL IO", " BACK");
	//LCDResetMode(lcdmode);

	return 0;
}

/* ------------------------------------------------------------------------- */
int test_adc(void)
{
	int adcsample,exit;
	char volt[20];
	ADCHandle adc1_handle, adc2_handle, adc3_handle, adc0_handle;
	exit = 0;

	LCDClear();
	LCDMenu("","",""," DONE");
	LCDPrintf("Reading from analog inputs:\n");
	LCDRefresh();

	adc0_handle = OSInitADC("ADC0");
	adc1_handle = OSInitADC("ADC1");
	adc2_handle = OSInitADC("ADC2");
	adc3_handle = OSInitADC("ADC3");

	while(exit == 0)
	{
		//Read and print the sample value and voltage level at each adc channel
		//ADC0
		adcsample = OSGetADC(adc0_handle);
		ConvADCSampleToVoltage(adc0_handle, volt, adcsample);
		LCDSetPrintf(2, 0, "ADC0: reading=%d    ", adcsample);
		LCDSetPrintf(2, 30, "voltage=%s v ", volt);

		//ADC1
		adcsample = OSGetADC(adc1_handle);
		ConvADCSampleToVoltage(adc1_handle, volt, adcsample);
		LCDSetPrintf(4, 0, "ADC1: reading=%d    ", adcsample);
		LCDSetPrintf(4, 30, "voltage=%s v ", volt);

		//ADC2
		adcsample = OSGetADC(adc2_handle);
		ConvADCSampleToVoltage(adc2_handle, volt, adcsample);
		LCDSetPrintf(6, 0, "ADC2: reading=%d    ", adcsample);
		LCDSetPrintf(6, 30, "voltage=%s v ", volt);

		//ADC3
		adcsample = OSGetADC(adc3_handle);
		ConvADCSampleToVoltage(adc3_handle, volt, adcsample);
		LCDSetPrintf(8, 0, "ADC3: reading=%d    ", adcsample);
		LCDSetPrintf(8, 30, "voltage=%s v ", volt);

		LCDRefresh3();
		
		if (keycode == KEY4)
		{
			exit = 1;
		}
		
		//sleep(1);
	}

	OSADCRelease(adc0_handle);
	OSADCRelease(adc1_handle);
	OSADCRelease(adc2_handle);
	OSADCRelease(adc3_handle);

	return 0;
}


/* ------------------------------------------------------------------------- */
int test_device(HDTDevice_t* pdev, int type)
{
	LCDClear();
	LCDSetMode(LCD_CLASSICMENU);
	KEYSetTM(KEYTM_CLASSIC);

	switch(type)
	{
	case HDT_IDX_PSD:
		test_psd(pdev);
		break;
	case HDT_IDX_SERVO:
		test_servo(pdev);
		break;
	case HDT_IDX_MOTOR:
		test_motor(pdev);
		break;
	case HDT_IDX_ENCODER:
		test_encoder(pdev);
		break;
	case HDT_IDX_DRIVE:
		break;
	case HDT_IDX_COMPASS:
		break;
	case HDT_IDX_IRTV:
		test_irtv(pdev);
		break;
	case HDT_IDX_CAM:
		break;
	case HDT_IDX_ADC:
		test_adc();
		break;
	default:
		type = -1;
		break;
	}

	LCDSetMode(LCD_LISTMENU);
	KEYSetTM(KEYTM_LISTMENU);
	return type;
}
/* ------------------------------------------------------------------------- */
int pick_dev(HDTDevice_t* pdevices, int type, int count)
{
	int notdone = 1, runindex = -1, retval = 0, loop;
	listmenu_t listmenu, *listtemp;
	HDTDevice_t* pdev;

	/* load items & create list menu */
	if (count == 1)
	{
		pdev = pdevices;
		test_device(pdev,type);
		return 0;
	}
	
	listtemp = LCDGetList();
	listmenu.pitems = 0x0;
	LCDClear();
	
	if(load_devs(pdevices, type, count, &listmenu)<=0)
	{
		notdone = 0;
		retval = -1;
	}
	keycode = 0x0;
	LCDMenu("Select", "", " ", " BACK");
	/* main loop */
	while(notdone)
	{
		LCDRefresh3();
		switch(keycode)
		{
			case IR_KEY_OK:
			{
				keycode = KEY1;
				break;
			}
			case KEY4:
				notdone = 0;
				retval = 0;
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				break;
			case KEY3:
				keycode = 0x0;
				break;
			case KEY1:
				if(listmenu.index<0)
					break;
				runindex = listmenu.index+listmenu.start;
				loop = 0;
				pdev = pdevices;
				while(loop<runindex&&pdev)
				{
					pdev = pdev->pnext;
					loop++;
				}
				if(loop==runindex&&pdev)
				{
					test_device(pdev, type);
				}
				notdone = 1;
				retval = 1;
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				LCDMenu("Select", "", " ", " BACK");
				break;
			case KEY_LISTUP:
				LCDListScrollUp();
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				break;
			case KEY_LISTDN:
				LCDListScrollDown();
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				break;
			case KEY_LISTTL:
				keycode = 0x0;
				break;
			case KEY_INVALID:
				/* ignore these */
				keycode = 0x0;
				break;
			case IR_KEY_UP:
			{
				IRScrollUp();
				keycode = 0x0;
				LCDRefresh();
				break;
			}
			case IR_KEY_DOWN:
			{
				IRScrollDown();
				keycode = 0x0;
				LCDRefresh();
				break;
			}
			default:
			{
				if (keycode == 0)
				{
					break;
				}
				int test = 0;
				while(keycode&&!(keycode&KEY_LIST1))
				{
					keycode >>= 1; test++;
				}
				LCDListIndex(test);
				keycode = 0x0;
			}
			LCDClear();
			LCDRefresh();
			break;
		}
		usleep(LOOPSLEEPTIME);
	}

	if(listmenu.pitems) free(listmenu.pitems);
	LCDSetList(listtemp);

	return retval;
}
/* ------------------------------------------------------------------------- */
int clean_up(HDTDevALL_t *pdevs)
{
	if(pdevs->ptabpsd)
	{
		HDTClearTable(pdevs->ptabpsd);
		pdevs->ptabpsd = 0x0;
	}
	if(pdevs->ppsd)
	{
		HDTClearPSD(pdevs->ppsd);
		pdevs->ppsd = 0x0;
		pdevs->countPSD = 0;
	}
	if(pdevs->ptabmotor)
	{
		HDTClearTable(pdevs->ptabmotor);
		pdevs->ptabmotor = 0x0;
	}
	if(pdevs->pmotor)
	{
		HDTClearMOTOR(pdevs->pmotor);
		pdevs->pmotor = 0x0;
		pdevs->countMOTOR = 0;
	}
	if(pdevs->pencoder)
	{
		HDTClearENCODER(pdevs->pencoder);
		pdevs->pencoder = 0x0;
		pdevs->countENCODER = 0;
	}
	if(pdevs->pirtv)
	{
		HDTClearIRTV(pdevs->pirtv);
		pdevs->pirtv = 0x0;
		pdevs->countIRTV = 0;
	}
	if(pdevs->pservo)
	{
		HDTClearSERVO(pdevs->pservo);
		pdevs->pservo = 0x0;
		pdevs->countSERVO = 0;
	}
	if(pdevs->padc)
	{
		HDTClearADC(pdevs->padc);
		pdevs->padc = 0x0;
		pdevs->countADC = 0;
	}

	return 0;
}
/* ------------------------------------------------------------------------- */
int load_hdt(char* hdtfile, HDTDevALL_t *pdevs, listmenu_t *plist)
{
	HDTDevMOTOR_t* pmotor;
	HDTDevENCODER_t* pencoder;
	HDTDevPSD_t* ppsd;
	HDTDevIRTV_t* pirtv;
	HDTDevSERVO_t* pservo;
	HDTDevADC_t* padc;
#define FILE_STRLEN 6
#define M6MAIN_HDTCOUNT 6
#define M6MAIN_MOTINDEX 0
#define M6MAIN_ENCINDEX 1
#define M6MAIN_PSDINDEX 2
#define M6MAIN_ITVINDEX 3
#define M6MAIN_SRVINDEX 4
#define M6MAIN_ADCINDEX 5
	char device_name[LCD_LIST_STRLENGTH];
	char *showfile;

	memset((void*) pdevs, 0x0, sizeof(HDTDevALL_t));
	/* try to load all psds */
	pdevs->ppsd = HDTLoadPSD(hdtfile, 0x0);
	if(pdevs->ppsd)
		pdevs->ptabpsd = HDTLoadTable(hdtfile,(HDTDevice_t*)pdevs->ppsd);
	/* count psd */
	pdevs->countPSD = 0;
	ppsd = pdevs->ppsd;
	while(ppsd)
	{
		pdevs->countPSD++;
		ppsd = ppsd->pnext;
	}
	/* try to load all motors */
	pdevs->pmotor = HDTLoadMOTOR(hdtfile, 0x0);
	if(pdevs->pmotor)
		pdevs->ptabmotor = HDTLoadTable(hdtfile,(HDTDevice_t*)pdevs->pmotor);
	/* count motor */
	pdevs->countMOTOR = 0;
	pmotor = pdevs->pmotor;
	while(pmotor)
	{
		pdevs->countMOTOR++;
		pmotor = pmotor->pnext;
	}
	/* try to load all encoders */
	pdevs->pencoder = HDTLoadENCODER(hdtfile, 0x0);
	if(pdevs->pencoder&&pdevs->pmotor)
		HDTLinkENC2MOT(pdevs->pencoder,pdevs->pmotor);
	/* count encoders */
	pdevs->countENCODER = 0;
	pencoder = pdevs->pencoder;
	while(pencoder)
	{
		pdevs->countENCODER++;
		pencoder = pencoder->pnext;
	}
	/* try to load all irtv */
	pdevs->pirtv = HDTLoadIRTV(hdtfile, 0x0);
	/* count irtvs */
	pdevs->countIRTV = 0;
	pirtv = pdevs->pirtv;
	while(pirtv)
	{
		pdevs->countIRTV++;
		pirtv = pirtv->pnext;
	}
	/* try to load all servo */
	pdevs->pservo = HDTLoadSERVO(hdtfile, 0x0);
	/* count servos */
	pdevs->countSERVO = 0;
	pservo = pdevs->pservo;
	while(pservo)
	{
		pdevs->countSERVO++;
		pservo = pservo->pnext;
	}
	/* try to load all adc */
	pdevs->padc = HDTLoadADC(hdtfile, 0x0);
	/* count adc */
	pdevs->countADC = 0;
	padc = pdevs->padc;
	while(padc)
	{
		pdevs->countADC++;
		padc = padc->pnext;
	}

	/* setup filemenu list */
	char *name = malloc(LCD_LIST_STRLENGTH);
	sprintf(name,"File: %s",hdtfile);
	listmenuInit(plist,name);
	
	/* MOTOR ENTRY */
	sprintf(device_name,"%d MOTOR(s)",pdevs->countMOTOR);
	listmenuAddItem(plist,device_name,pdevs->pmotor);
	/* ENCODER ENTRY */
	sprintf(device_name,"%d ENCODER(s)",pdevs->countENCODER);
	listmenuAddItem(plist,device_name,pdevs->pencoder);
	/* PSD ENTRY */
	sprintf(device_name,"%d PSD(s)",pdevs->countPSD);
	listmenuAddItem(plist,device_name,pdevs->ppsd);
	/* IRTV ENTRY */
	sprintf(device_name,"%d IRTV(s)",pdevs->countIRTV);
	listmenuAddItem(plist,device_name,pdevs->pirtv);
	/* SERVO ENTRY */
	sprintf(device_name,"%d SERVO(s)",pdevs->countSERVO);
	listmenuAddItem(plist,device_name,pdevs->pservo);
	/* ADC ENTRY */
	sprintf(device_name,"%d ADC(s)",pdevs->countADC);
	listmenuAddItem(plist,device_name,pdevs->padc);
    /* Raspicam Entry */
    sprintf(device_name,"RASPI-CAM");
    listmenuAddItem(plist,device_name,pdevs->pcam);
	
	/* assign the list! */
	LCDList(plist);

	free(name);
	return 0;
}
/* ------------------------------------------------------------------------- */
int show_hdt_int(char* hdtfile)
{
	keycode = 0x0;
	int exit = 0, runindex = -1, /*retval = 0,*/ type = -1, count = 0;
	listmenu_t listmenu, *listtemp;
	HDTDevALL_t m6hdt;
	HDTDevice_t *pdev = 0x0;

	/* prep list menu */
	listtemp = LCDGetList();
	listmenu.pitems = 0x0;

	/* load hdt */
	load_hdt(hdtfile, &m6hdt, &listmenu);
	LCDClear();
	LCDMenu(" SELECT", " RELOAD", " DIGITAL IO", " BACK");
	LCDRefresh();
	/* main loop */
	while(exit == 0)
	{
		LCDRefresh3();
		switch(keycode)
		{
			case KEY4:
			{
				exit = 1;
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				LCDMenu(" SELECT", " RELOAD", " DIGITAL IO", " BACK");
				break;
			}
			case KEY2:
			{
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				LCDMenu(" SELECT", " RELOAD", " DIGITAL IO", " BACK");
				break;
			}
			case KEY3:
			{
				test_dio();
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				LCDMenu(" SELECT", " RELOAD", " DIGITAL IO", " BACK");
				break;
			}
			case KEY1:
			{
				if(listmenu.index<0)
					break;
				runindex = listmenu.index+listmenu.start;
				switch(runindex)
				{
					case M6MAIN_MOTINDEX:
					{
						pdev = (HDTDevice_t*) m6hdt.pmotor;
						type = HDT_IDX_MOTOR;
						count = m6hdt.countMOTOR;
						break;
					}
					case M6MAIN_ENCINDEX:
					{
						pdev = (HDTDevice_t*) m6hdt.pencoder;
						type = HDT_IDX_ENCODER;
						count = m6hdt.countENCODER;
						break;
					}
					case M6MAIN_PSDINDEX:
					{
						pdev = (HDTDevice_t*) m6hdt.ppsd;
						type = HDT_IDX_PSD;
						count = m6hdt.countPSD;
						break;
					}
					case M6MAIN_ITVINDEX:
					{
						pdev = (HDTDevice_t*) m6hdt.pirtv;
						type = HDT_IDX_IRTV;
						count = m6hdt.countIRTV;
						break;
					}
					case M6MAIN_SRVINDEX:
					{
						pdev = (HDTDevice_t*) m6hdt.pservo;
						type = HDT_IDX_SERVO;
						count = m6hdt.countSERVO;
					break;
					}
					case M6MAIN_ADCINDEX:
					{
						pdev = (HDTDevice_t*) m6hdt.padc;
						type = HDT_IDX_ADC;
						count = m6hdt.countADC;
						break;
					}
				}
				if(pdev)
				{
					//while(pick_dev(pdev, type, count)>0);
					pick_dev(pdev, type, count);
				}
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				LCDMenu(" SELECT", " RELOAD", " DIGITAL IO", " BACK");
				break;
			}
			case KEY_LISTUP:
			{
				LCDListScrollUp();
				keycode = 0x0;
				LCDRefresh();
				break;
			}
			case KEY_LISTDN:
			{
				LCDListScrollDown();
				keycode = 0x0;
				LCDRefresh();
				break;
			}
			case KEY_LISTTL:
			case KEY_INVALID:
			/* ignore these */
			keycode = 0x0;
			break;
			case IR_KEY_UP:
			{
				IRScrollUp();
				keycode = 0x0;
				LCDRefresh();
				break;
			}
			case IR_KEY_DOWN:
			{
				IRScrollDown();
				keycode = 0x0;
				LCDRefresh();
				break;
			}
			case IR_KEY_OK:
			{
				keycode = KEY1;
				break;
			}
			default:
			{
				if (keycode == 0x0)
				{
					break;
				}
				int test = 0;
				while(keycode&&!(keycode&KEY_LIST1))
				{
					keycode >>= 1; test++;
				}
				LCDListIndex(test);
				keycode = 0x0;
				LCDRefresh();
			}
		}
		usleep(LOOPSLEEPTIME);
	}

	/* clean-up & restore list */
	if(listmenu.pitems)
		listmenuFreeItems(&listmenu);
	LCDSetList(listtemp);

	/* clean-up hdt memory */
	clean_up(&m6hdt);

	//return retval;
	return 0;
}
/* ------------------------------------------------------------------------- */
int show_hdt(void)
{
	keymode_t tempmode;
	lcdmode_t tlcdmode;
	listmenu_t *listtemp;

	/* save current keymode & lcdmode */
	tempmode = KEYGetTM(0x0);
	tlcdmode = LCDGetMode();
	listtemp = LCDGetList();

	LCDSetMode(LCD_LISTMENU);
	KEYSetTM(KEYTM_LISTMENU);

	LCDGetFBInfo(&fb);
	/* execute low-level function */
	show_hdt_int(HDT_FILE);

	LCDSetList(listtemp);
	LCDResetMode(tlcdmode);
	KEYSetTM(tempmode);

	return 0;
}
/* ------------------------------------------------------------------------- */
