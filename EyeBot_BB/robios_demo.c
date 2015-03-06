#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h> /* struct hostent */
#include <unistd.h> /* gethostname() */
#include <arpa/inet.h> /**/
#include <signal.h>

// for open()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#include "../libM6lcd/libM6lcd.h"
//#include "../libM6key/libM6key.h"
//#include "../libM6os/libM6os.h"
#include <eyebot.h>
#include "header.h"

static int process_suspend = 0;

void setupDemoMenu(listmenu_t *listtemp)
{
	DIR *dir;
	struct dirent *ent;
	fbinfo_t fb;
	LCDGetFBInfo(&fb);
	//Set up the list
	listmenuInit(listtemp,"Demos:");
	/*
	 * populate the demo list menu
	 * first open the directory where the demos are located
	 */
	if ((dir = opendir("/home/pi/EyeBot_BB/usr/Demos")) != NULL)
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
	
	LCDList(listtemp);
}

int demoPage()
{
	keycode = 0x0;
	int row = 0;
	int col = 0;
	int exit = 0;
	int val= 1;
	listmenu_t listmenu, *listtemp;
	listtemp = LCDGetList();
	setupDemoMenu(&listmenu);
	LCDClear();
	
	LCDMenu("RUN","","","BACK");
	
	LCDRefresh();	
	while (exit==0)
	{
		LCDRefresh3();
		if (listmenuSize(listmenu) == 0)
		{
			show_msg("Demo folder is empty\n");
			break;
		}
		switch(keycode)
		{
			case KEY1:
			{
				char *run = malloc(MAX_FILECHAR);
				strcpy(run,listmenu.pitems[listmenu.index].label);
				sprintf(run,"/usr/Eyebot/Demos/%s",listmenu.pitems[listmenu.index].label);
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
				break;
			}
		}
		usleep(LOOPSLEEPTIME);
	}
	LCDClear();
	LCDSetList(listtemp);
	return 0;
}
