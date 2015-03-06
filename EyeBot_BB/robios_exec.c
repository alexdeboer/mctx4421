/** 20080717 - azman@ee.uwa.edu.au */
/** based on codes by Thomas Sommer */
/** heavily modified by Stuart Howard */

#include <dirent.h> /* readdir()? */
#include <unistd.h> /* access()? */
#include <stdlib.h> /* malloc() */
#include <stdio.h> /* sprintf() */
#include <stdarg.h> /* va_list */
#include <wait.h> /* wait() */
#include <string.h> /* strcmp()  */
#include <signal.h> /* sigaction */
#include <sys/stat.h> /* stat() */
#include <sys/types.h> /* pid_t */

//#include "../libM6lcd/libM6lcd.h"
//#include "../libM6key/libM6key.h"
#include <eyebot.h>
#include "header.h"

#define MAX_FILELIST 128

#define MSG_POS_Y 2
#define MSG_POS_X 2

static char usermsg[MAX_MSGLEN];

static int process_suspend = 0, first_run = 0;
extern int activate_escape(int escape); /* hidden function exclusive for m6main */

/* ------------------------------------------------------------------------- */
int show_msg(const char *format, ...)
{
	char formatted_text[MAX_MSGLEN+MAX_FULLPATH];
	va_list var_arg_list;
	lcdmode_t tlcdmode = LCDGetMode(); //save lcd mode! 

	//reformat message buffer 
	va_start(var_arg_list, format);
	vsprintf(formatted_text, format, var_arg_list);
	va_end(var_arg_list);

	//copy to actual message buffer 
	strcpy_n(usermsg,formatted_text,MAX_MSGLEN);

	//do that thingy! 
	LCDClear();
	LCDSetMode(LCD_HIDEMENU);
	LCDSetPrintf(MSG_POS_Y,MSG_POS_X,usermsg);
	LCDRefresh();
	sleep(MSG_WAITTIME);

	//restore lcd mode, but not what was on it!
	LCDResetMode(tlcdmode);
	LCDClear();

	return 0;
}
/* ------------------------------------------------------------------------- */
int parent_root_pos(char* fullpath)
{
	int index = 0, last_root = -1;
	while(fullpath[index]) /* get to end of line! */
	{
		if(fullpath[index]=='/') last_root = index;
		index++;
	}
	return last_root;
}
/* ------------------------------------------------------------------------- */
//------M7 menu------

void setupSoftwareMenu(listmenu_t *listtemp)
{
	DIR *dir;
	struct dirent *ent;
	fbinfo_t fb;
	LCDGetFBInfo(&fb);
	//Set up the list
	listmenuInit(listtemp,"Software:");
	/*
	 * populate the demo list menu
	 * first open the directory where the demos are located
	 */
	if ((dir = opendir("/home/pi/EyeBot_BB/usr/Software")) != NULL)
	{
		//Next while there are still demo programs to read add them to the item list
		while ((ent = readdir(dir)) != NULL)
		{
			//Exclude two entries that are invalid
			if (strcmp("..",ent->d_name)==0||strcmp(".",ent->d_name)==0)
			{
				continue;
			}
			listmenuAddItem(listtemp,ent->d_name,NULL);
		}
	}
	else
	{
		fprintf(stdout,"ERROR: unable to read software directory\n");
	}
	
	LCDList(listtemp);
}

int show_exec(void)
{
	keycode = 0x0;
	int row = 0;
	int col = 0;
	int exit = 0;
	int val= 1;
	// check first run
	if(!first_run)
	{
		first_run = 1;
		// initialize proc list!
		proclist.count = 0;
		proclist.pfirst = 0x0;
		proclist.plast = 0x0;
	}
	listmenu_t listmenu, *listtemp;
	//Get the current listmenu so that it can be restored later
	listtemp = LCDGetList();
	setupSoftwareMenu(&listmenu);
	LCDClear();
	KEYSetTM(KEYTM_CLASSIC);
	LCDTextColor(LCD_WHITE, LCD_BLACK, LCD_BGCOL_TRANSPARENT);
	LCDSetMode(LCD_NOAUTOREFRESH|LCD_SHOWMENU);
	LCDRefresh();
	LCDMenu("Run","","","BACK");
	while (exit==0)
	{
		LCDRefresh3();
		if (listmenuSize(listmenu) == 0)
		{
			show_msg("Software folder is empty\n");
			break;
		}
		switch(keycode)
		{
			case KEY1:
			{
				char *run = malloc(MAX_FILECHAR);
				strcpy(run,listmenu.pitems[listmenu.index].label);
				sprintf(run,"/usr/Eyebot/Software/%s",listmenu.pitems[listmenu.index].label);
				runProcess(run);
				free(run);
				processMenu();
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				break;
			}
			case KEY2:
			case KEY3:
			{
				keycode = 0x0;
				break;
			}
			case KEY4:
			{
				exit = 1;
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				break;
			}
			case KEY_LISTUP:
			{
				LCDListScrollUp();
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				break;
			}
			case KEY_LISTDN:
			{
				LCDListScrollDown();
				keycode = 0x0;
				LCDClear();
				LCDRefresh();
				break;
			}
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
				LCDClear();
				LCDRefresh();
			}
		}
		usleep(LOOPSLEEPTIME);
	}
	LCDClear();
	LCDSetList(listtemp);
	return 0;
}
