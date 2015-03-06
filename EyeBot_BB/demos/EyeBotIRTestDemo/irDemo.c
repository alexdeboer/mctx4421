#include <eyebot.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	fprintf(stderr,"Demo start\n");
	int exit = 0;
	keycode_t keycode2;
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
	LCDMenu("REFRESH","REFRESH","REFRESH","EXIT");
	fprintf(stderr,"Initialising the IR functions\n");
	IRTVInit("786");
	fprintf(stderr,"Done\n");
	fprintf(stderr,"While loop start\n");\
	int IRCode;
	while (!exit)
	{
		IRCode = 0;
		LCDClear();
		LCDSetPos(5,0);
		LCDPrintf("Key input disabled, IR input enabled\nPress the blue button on the remote to enable touch input\n");
		LCDPrintf("IR code: 0\n");
		LCDRefresh();
		printf("Enter IR input. Press the blue button to exit\n");
		do
		{
			IRCode = IRTVGet();
			if (IRCode != 0)
			{
				LCDClear();
				LCDSetPos(5,0);
				LCDPrintf("Key input disabled, IR input enabled\nPress the blue button on the remote to enable touch input\n");
				LCDPrintf("IR code: %d\n",IRCode);
				LCDRefresh();
			}
		} while (IRCode != IR_KEY_BLUE);
		LCDClear();
		LCDSetPos(5,0);
		LCDPrintf("Key input enabled, IR input disabled\n");
		LCDPrintf("IR code: 0\n");
		LCDRefresh();
		keycode2 = KEYGet();
		if (keycode2 == KEY4)
		{
			exit = 1;
		}
	}
	IRTVRelease();
	fprintf(stderr,"Exiting!\n");
	return 0;
}
