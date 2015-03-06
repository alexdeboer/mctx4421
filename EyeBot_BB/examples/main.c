#include "eyebot.h"

int mainbis(int argv, char *argc[])
{
	printf("Version: %s\n", OSVersion());
	printf("Machine speed: %i\n", OSMachineSpeed());
	printf("Machine type: %i\n", OSMachineType());
	printf("Machine name: %s\n", OSMachineName());
	printf("Machine ID: %i\n", OSMachineID());
	info_cpu_t infoCPU;
	OSInfoCPU(&infoCPU);
	printf("CPU Arch: %s\n", infoCPU.arch);
	printf("CPU BogoMIPS: %s\n", infoCPU.bogomips);
	printf("CPU Mhz: %s\n", infoCPU.mhz);
	printf("CPU Name: %s\n", infoCPU.name);
	info_mem_t infoMem;
	OSInfoMem(&infoMem);
	printf("Total memory: %s\n", infoMem.total);
	printf("Free memory: %s\n", infoMem.free);
	info_proc_t infoProc;
	OSInfoProc(&infoProc);
	printf("Process number: %s\n", infoProc.num);
	info_misc_t infoMisc;
	OSInfoMisc(&infoMisc);
	printf("Uptime: %s\n", infoMisc.uptime);
	printf("Vbat: %s\n", infoMisc.vbatt);
	printf("Vbat: %i\n", infoMisc.vbatt_8);
	/*LCDInit();
	KEYInit();
	while (gLCDEnabled)
	{
		if (LCDNeedRefresh())
		{
			LCDArea(0, 0, 100, 100, LCD_GREEN);
			LCDArea(100, 100, 200, 200, LCD_BLACK);
			LCDArea(100, 0, 200, 100, LCD_PURPLE);
			LCDSetPixel(50, 150, LCD_RED);
			rgb_t test;
			test = LCDGetPixel(50, 150);
			printf("%d %d %d\n", test.red, test.green, test.blue);
			LCDInvertPixel(50, 150);
			test = LCDGetPixel(50, 150);
			printf("%d %d %d\n", test.red, test.green, test.blue);
			LCDLine(0, 0, 200, 200, LCD_MAGENTA);
			LCDLineInvert(50, 0, 200, 200);
			LCDLine(0, 50, 200, 200, LCD_AQUA);
			LCDFrame(20, 20, 60, 60, LCD_CYAN);
			LCDTextColor(LCD_BLACK, LCD_WHITE, LCD_BGCOL_TRANSPARENT);
			LCDPrintf("test %02X %i", 10, 56);
			LCDPrintf("data\ntest");
			LCDPrintf("abs\n");
			LCDSetPrintf(4, 4, "sd %02X %i", 13, 56);
			LCDPutChar('a');
			LCDSetChar(5, 0, 'b');
			LCDPutChar('c');
			LCDTextColor(LCD_BLUE, LCD_GRAY, LCD_BGCOL_NOTRANSPARENT);
			LCDPutString("string\ntes\n\n\n");
			LCDSetString(6, 1, "sds");
			LCDPutHex(0x453);
			LCDPutInt(10);
			LCDPutIntS(10, 5);
			LCDPutIntS(123456, 5);
			LCDPutFloat(0.23);
			LCDPutFloatS(4.234234, 5, 2);
			LCDSetPos(8, 0);
			LCDPrintf("Test");
			LCDFrame(25, 25, 55, 55, LCD_CYAN);
			LCDTextBar(9, 10, 50, 78, LCD_BLUE);
			int r, c;
			LCDGetPos(&r, &c);
			printf("%i %i\n", r, c);
			LCDPrintf("ADS");
			//LCDAreaInvert(50, 50, 150, 150);
			LCDRefresh();
			OSWait(10);
			LCDClear();
			LCDMenu("Test0", "Test1", "Test2", "Test3");
			LCDMenuI(0, "Data", LCD_GREEN, LCD_RED, NULL);
			menuitem_t item[2];
			strcpy(item[0].label, "test selected");
			item[0].fgcol = LCD_RED;
			item[0].bgcol = LCD_AQUA;
			strcpy(item[1].label, "tes");
			item[1].fgcol = LCD_RED;
			item[1].bgcol = LCD_AQUA;
			listmenu_t list;
			strcpy(list.title, "Test");
			list.fgcol = LCD_GRAY;
			list.bgcol = LCD_BLUE;
			list.pitems = item;
			list.size = 2;
			list.index = 0;
			list.start = 0;
			LCDList(&list);
			LCDRefresh();
			OSWait(10);
			LCDClear();
			LCDSetList(&list);
			menurect_t* menuRect = LCDListBox(1);
			LCDArea(menuRect->x, menuRect->y, menuRect->x+menuRect->width, menuRect->y+menuRect->height, LCD_RED);
			menuitem_t* menuItem = LCDListActiveItem();
			LCDPrintf("%s", menuItem->label);
			LCDRefresh();
//			KEYWait(KEY1);
//			printf("Key1\n");
		}
		KEYWait(KEY1 | KEY2);
		printf("Test\n");
		keycode_t readKey = KEYGet();
		printf("%i\n", readKey);
		coord_pair_t coords = KEYGetXY();
		printf("%i %i\n", coords.x, coords.y);
		touch_map_t* ptouch_map;
		KEYGetTM(&ptouch_map);
		printf("%i\n", ptouch_map->mode);
		m6key_box_t region;
		region.tl.x = 0;
		region.tl.y = 0;
		region.br.x = 100;
		region.br.y = 100;
		KEYSetRegion(0, &region);
		m6key_box_t regionbis;
		KEYGetRegion(0, &regionbis);
		printf("%i\n", regionbis.br.x);
	}
	LCDRelease();
	exit(0);*/
	SPIHandle spiHandle = SPIInit(0);
	/*QUADHandle quadHandle0 = QUADInit("ENCODER0");
	QUADHandle quadHandle1 = QUADInit("ENCODER1");
	PSDHandle psdHandle0 = PSDInit("PSD0");
	PSDHandle psdHandle1 = PSDInit("PSD1");
	PSDHandle psdHandle2 = PSDInit("PSD2");
	PSDHandle psdHandle3 = PSDInit("PSD3");
	PSDHandle psdHandle4 = PSDInit("PSD4");
	PSDHandle psdHandle5 = PSDInit("PSD5");
	OSLatchBankSetup(IOBANK0, IN);
	OSLatchBankSetup(IOBANK1, IN);
	OSLatchSetup(6, 1);
	QUADReset(quadHandle0);
	while (true)
	{
		OSLatchWrite(5, 1);
		OSLatchWrite(14, 1);
		int res = OSLatchRead(6);
//		OSLatchWrite(6, 1);
		long result1 = QUADRead(quadHandle0);
		long result2 = QUADRead(quadHandle1);
		int result3 = PSDGetRaw(psdHandle0);
		int result4 = PSDGetRaw(psdHandle1);
		int result5 = PSDGetRaw(psdHandle2);
		int result6 = PSDGetRaw(psdHandle3);
		int result7 = PSDGetRaw(psdHandle4);
		int result8 = PSDGetRaw(psdHandle5);
		printf("%i / ", res);
		printf("%li %li / ", result1, result2);
		printf("%i / ", result3);
		printf("%i / ", result4);
		printf("%i / ", result5);
		printf("%i / ", result6);
		printf("%i / ", result7);
		printf("%i\n", result8);
		OSLatchWrite(5, 0);
		OSLatchWrite(14, 0);
		usleep(500000);
	}*/
	VWHandle vwHandle0 = VWInit("DIFF", 0);
	VWDriveStraight(vwHandle0, 1, 0.1);
	VWDriveWait(vwHandle0);
	VWDriveStraight(vwHandle0, 1, 0);
	VWDriveWait(vwHandle0);
	/*MOTORHandle motorHandle0 = MOTORInit("MOTOR0");
	MOTORHandle motorHandle1 = MOTORInit("MOTOR1");
	MOTORHandle motorHandle2 = MOTORInit("MOTOR2");
	MOTORHandle motorHandle3 = MOTORInit("MOTOR3");

	SERVOHandle servoHandle0 = SERVOInit("SERVO0");
	SERVOHandle servoHandle1 = SERVOInit("SERVO1");
	SERVOHandle servoHandle2 = SERVOInit("SERVO2");
	SERVOHandle servoHandle3 = SERVOInit("SERVO3");

	MOTORDrive(motorHandle0, 25);
	MOTORDrive(motorHandle1, 100);
	OSWait(300);
	MOTORDrive(motorHandle0, -50);
	MOTORDrive(motorHandle1, -50);
	OSWait(300);
	MOTORDrive(motorHandle0, 25);
	OSWait(300);
	MOTORDrive(motorHandle0, 50);
	OSWait(300);
	MOTORDrive(motorHandle0, 0);
	OSWait(300);
	MOTORDrive(motorHandle0, -100);
	//unsigned long result = QUADRead(quadHandle0);
	//printf("%lu\n", result);

	SERVOSet(servoHandle0, 0);
	SERVOSet(servoHandle1, 0);
	SERVOSet(servoHandle2, 0);
	SERVOSet(servoHandle3, 0);
	OSWait(100);
	SERVOSet(servoHandle3, 360);
	SERVOSet(servoHandle2, 360);
	SERVOSet(servoHandle1, 360);
	SERVOSet(servoHandle0, 360);
	OSWait(100);

	SERVORelease(servoHandle0);
	SERVORelease(servoHandle1);
	SERVORelease(servoHandle2);
	SERVORelease(servoHandle3);

	MOTORRelease(motorHandle0);
	MOTORRelease(motorHandle1);*/

	SPIRelease(spiHandle);
	return 0;
}
