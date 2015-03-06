#include "eyebot.h"

int main(int argc, char **argv)
{
	fprintf(stderr,"Demo start\n");
	int exit = 0;
	keycode_t keycode;
	fprintf(stderr,"Initialising LCD...");
	if(LCDInit(0)<0)
	{
		printf("LCDInit Error! Exiting!\n");
		return -1;
	}
	fprintf(stderr,"Done\n");
	fprintf(stderr,"Initialising Keys...");
	if(KEYInit()<0)
	{
		printf("KEYInit Error! Exiting!\n");
		return -1;
	}
	fprintf(stderr,"Done\n");
	KEYSetTM(KEYTM_CLASSIC);
	LCDTextColor(LCD_WHITE, LCD_BLACK, LCD_BGCOL_TRANSPARENT);
	LCDSetMode(LCD_NOAUTOREFRESH|LCD_SHOWMENU);
	fprintf(stderr,"While loop start\n");
	LCDClear();
	LCDRefresh();
	while (!exit)
	{
		LCDClear();
		LCDMenu("","","","EXIT");
		LCDSetPrintf(1,1,"Test Demo\n");
		LCDRefresh();
		keycode = KEYGet();
		if (keycode == KEY4)
		{
				exit = 1;
		}
	}
	fprintf(stderr,"Exiting!\n");
	return 0;
}
