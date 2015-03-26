/** 20080917 - azman@ee.uwa.edu.au */

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
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <eyebot.h>
#include "header.h"
#include "robios_main.h"

#include <pthread.h>

#include <string.h>

#define M6MAIN_TEXTROW 1
#define M6MAIN_TEXTCOL 0
#define M6MAIN_BATTBAR_COLOR LCD_BLUE
#define M6MAIN_HOSTNAME_LEN 64

int menu = 1;

void *keyInputThread(void *data)
{
	fprintf(stderr,"Key input thread start\n");
	XEvent event;
	while (1)
	{
		if (gMouseButton == 1)
		{
			keycode = KEYRead();
		}
		usleep(LOOPSLEEPTIME);
	}
	fprintf(stderr,"Key input thread end\n");
	return NULL;
}

void *irInputThread(void *data)
{
	fprintf(stderr,"IR input thread start\n");
	int IRCode = 0;
	while (1)
	{
		if (IRTVGetStatus()==1)
		{
			IRCode = IRTVGet();
			fprintf(stderr,"IRCode: %c\n",IRCode);
			switch (IRCode)
			{
				case IR_KEY_RED:
				{
					keycode = KEY1;
					break;
				}
				case IR_KEY_GREEN:
				{
					keycode = KEY2;
					break;
				}
				case IR_KEY_YELLOW:
				{
					keycode = KEY3;
					break;
				}
				case IR_KEY_BLUE:
				{
					keycode = KEY4;
					break;
				}
				case IR_KEY_LEFT:
				{
					keycode = KEY_LISTUP;
					break;
				}
				case IR_KEY_RIGHT:
				{
					keycode = KEY_LISTDN;
					break;
				}
				case IR_KEY_UP:
				{
					keycode = IR_KEY_UP;
					break;
				}
				case IR_KEY_DOWN:
				{
					keycode = IR_KEY_DOWN;
					break;
				}
				case IR_KEY_OK:
				{
					keycode = IR_KEY_OK;
					break;
				}
			}
		}
	}
	fprintf(stderr,"IR input thread end\n");
	return NULL;
}

void *screenRefreshThread(void *data)
{
	fprintf(stderr,"Screen refresh thread start\n");
	XEvent event;
	while (1)
	{
		usleep(LOOPSLEEPTIME);
		if (XPending(gLCDHandle->d)==0)
		{
			continue;
		}
		XPeekEvent(gLCDHandle->d,&event);
		switch (event.type)
		{
			case FocusIn:
			case FocusOut:
			{
				fprintf(stderr,"Focus changed\n");
				XNextEvent(gLCDHandle->d,&event);
				screenRefresh = 1;
				fprintf(stderr,"Done\n");
				break;
			}
		}
	}
	fprintf(stderr,"Screen refresh thread end\n");
	return 0;
}

void printfile(char filename[], char title[])
{
	EyeFont eyeFontDefaultYellow;
	EyeFont eyeFontBoldWhite;
	
	strcpy(eyeFontDefaultYellow.fontName,FONTDEFAULT);
	eyeFontDefaultYellow.fontColour = LCD_YELLOW;
	strcpy(eyeFontBoldWhite.fontName,FONTBOLD);
	eyeFontBoldWhite.fontColour = LCD_WHITE;
	
	LCDPrintfFont(eyeFontBoldWhite," %s: ", title);
	int fd;
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		LCDPrintf("Error opening file");
	}
	else
	{
		char c;
		while( read(fd, &c, 1) == 1 )
		{
			if( c == '\n' )
				break;
			LCDPrintfFont(eyeFontDefaultYellow,"%c",c);
		}
		close(fd);
    }
    LCDPrintf("\n");
}

int haddr2inaddr(char *haddr, struct in_addr *inaddr, int hlen)
{
	int loop;
	/* obviously only working for ipv4! */
	for(loop=hlen-1;loop>=0;loop--)
	{
		inaddr->s_addr <<= 8;
		inaddr->s_addr |= (uint32_t) haddr[loop];
	}
	return hlen==4? 1 : 0;
}

void settings_list(listmenu_t *listmenu)
{
	listmenuInit(listmenu,"Settings:");
	char *temp = (char *)calloc(1,LCD_LIST_STRLENGTH);
	if (IRTVGetStatus()==0)
	{
		sprintf(temp,"IR code: %s",robiosParams.irCode);
		listmenuAddItem(listmenu,temp,NULL);
		listmenuAddItem(listmenu,"IR enabled: No",NULL);
	}
	else
	{
		sprintf(temp,"IR code: %s",IRTVGetControllerCode());
		listmenuAddItem(listmenu,temp,NULL);
		listmenuAddItem(listmenu,"IR enabled: Yes",NULL);
	}
	if (robiosParams.runProg == 0)
	{
		sprintf(temp,"Program Name: %s",robiosParams.progName);
		listmenuAddItem(listmenu,temp,NULL);
		listmenuAddItem(listmenu,"Startup program enabled: No",NULL);
	}
	else
	{
		sprintf(temp,"Program Name: %s",robiosParams.progName);
		listmenuAddItem(listmenu,temp,NULL);
		listmenuAddItem(listmenu,"Startup program enabled: Yes",NULL);
	}
	sprintf(temp,"IR hold delay: %d",robiosParams.irDelay);
	listmenuAddItem(listmenu,temp,NULL);
	LCDSetList(listmenu);
	free(temp);
}

void irCodeList(listmenu_t *listmenu)
{
	FILE *inFile = fopen("/home/pi/EyeBot_BB/IRConfigs/irCodes.txt","r");
	if (inFile == NULL)
	{
		fprintf(stderr,"ERROR: unable to open irCodes.txt\n");
		return;
	}
	listmenuInit(listmenu,"IR codes");
	char line[10];
	while (fgets(line,10,inFile) != NULL)
	{
		trimline(line);
		if (strlen(line) != 3)
		{
			fprintf(stderr,"ERROR: incorrect code length\nSkipping!\n");
			continue;
		}
		listmenuAddItem(listmenu,line,NULL);
	}
	fclose(inFile);
}

void changeIRCode()
{
	keycode = 0;
	int exit = 0, selectCode = 0;
	
	listmenu_t listmenu, *listtemp;
	listtemp = LCDGetList();
	irCodeList(&listmenu);
	LCDClear();
	LCDList(&listmenu);
	LCDMenu("","","","BACK");
	LCDRefresh();
	
	while (exit == 0)
	{
		switch (keycode)
		{
			case KEY1:
			{
				selectCode = 1;
				keycode = 0;
				break;
			}
			case KEY4:
			{
				exit = 1;
				break;
			}
			case KEY_LISTUP:
			{
				LCDListScrollUp();
				keycode = 0x0;
				break;
			}
			case KEY_LISTDN:
			{
				LCDListScrollDown();
				keycode = 0x0;
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
				selectCode = 1;
				keycode = 0x0;
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
				LCDRefresh2();
			}
		}
		if (selectCode == 1)
		{
			strcpy_n(robiosParams.irCode,listmenuGetItemText(listmenu),3);
			fprintf(stderr,"IR Code: %s\n",robiosParams.irCode);
			IRTVRelease();
			IRTVInit(robiosParams.irCode);
			selectCode = 0;
		}
		LCDRefresh3();
	}
	
	listmenuFreeItems(&listmenu);
	LCDList(listtemp);
}

int page_settings()
{
	keycode = 0x0;
	int row = 0;
	int col = 0;
	int exit = 0;
	int val= 1;
	int run = 0;
	listmenu_t listmenu, *listtemp;
	listtemp = LCDGetList();
	settings_list(&listmenu);
	LCDClear();
	KEYSetTM(KEYTM_CLASSIC);
	LCDTextColor(LCD_WHITE, LCD_BLACK, LCD_BGCOL_TRANSPARENT);
	LCDSetMode(LCD_NOAUTOREFRESH|LCD_SHOWMENU);
	
	char *hostname = malloc(30);
	
	gethostname(hostname,30);
	LCDRefresh();
	while (exit == 0)
	{
		LCDMenu("SHUTDOWN","REBOOT","EDIT","BACK");
		LCDRefresh3();
		switch (keycode)
		{
			case KEY1:
			{
				/*checking if the software is running on the Raspberry Pi
				 * 	if so then need to use sudo
				 */
				if (strcmp(hostname,"raspberrypi\n"))
					system("sudo halt");
				else
					system("halt");
				keycode = 0x0;
				LCDMenu("","","EDIT","BACK");
				break;
			}
			case KEY2:
			{
				if (strcmp(hostname,"raspberrypi\n"))
					system("sudo reboot");
				else
					system("reboot");
				keycode = 0x0;
				LCDMenu("","","EDIT","BACK");
				break;
			}
			case KEY3:
			{
				run = 1;
				keycode = 0x0;
				break;
			}
			case KEY4:
			{
				exit = 1;
				break;
			}
			case KEY_LISTUP:
			{
				LCDListScrollUp();
				keycode = 0x0;
				break;
			}
			case KEY_LISTDN:
			{
				LCDListScrollDown();
				keycode = 0x0;
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
				run = 1;
				keycode = 0x0;
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
				if (test==1)
				{
					LCDMenu("Shutdown","Reboot","","BACK");
					listmenuChangeItemText(&listmenu,"IR enabled: Please wait",1);
					LCDRefresh();
					if (IRTVGetStatus()==0)
					{
						IRTVInit("786");
						listmenuFreeItems(&listmenu);
						settings_list(&listmenu);
					}
					else
					{
						IRTVRelease();
						listmenuFreeItems(&listmenu);
						settings_list(&listmenu);
					}
					LCDMenu("SHUTDOWN","REBOOT","EDIT","BACK");
					LCDClear();
					LCDRefresh();
				}
			}
		}
		if (run == 1)
		{
			run = 0;
			if (listmenu.index < 0)
			{
				break;
			}
			int runindex = listmenu.index + listmenu.start;
			switch (runindex)
			{
				case 0:
				{
					changeIRCode();
					keycode = 0;
					break;
				}
				case 1:
				{
					if (IRTVGetStatus()==0)
					{
						LCDMenu("SHUTDOWN","REBOOT","","BACK");
						listmenuChangeItemText(&listmenu,"IR enabled: Please wait",1);
						LCDRefresh();
						IRTVInit("786");
						LCDClear();
						LCDRefresh();
					}
					else
					{
						LCDMenu("SHUTDOWN","REBOOT","","BACK");
						listmenuChangeItemText(&listmenu,"IR enabled: Please wait",1);
						LCDRefresh();
						IRTVRelease();
						LCDClear();
						LCDRefresh();
					}
					break;
				}
				case 2:
				{
					char *progName = keyboardInput();
					strcpy(robiosParams.progName,progName);
					free(progName);
					break;
				}
			}
			listmenuFreeItems(&listmenu);
			settings_list(&listmenu);
			LCDMenu("SHUTDOWN","REBOOT","EDIT","BACK");
		}
		usleep(LOOPSLEEPTIME);
	}
	LCDClear();
	LCDMenu(" SYSTEM"," NETWORK","SETTINGS"," BACK");
	LCDSetList(listtemp);
	return 0;
}

int page_info(void)
{
	info_cpu_t cpu;
	info_mem_t mem;
	info_misc_t misc;
	
	EyeFont eyeFontDefaultYellow;
	EyeFont eyeFontBoldWhite;
	
	strcpy(eyeFontDefaultYellow.fontName,FONTDEFAULT);
	eyeFontDefaultYellow.fontColour = LCD_YELLOW;
	strcpy(eyeFontBoldWhite.fontName,FONTBOLD);
	eyeFontBoldWhite.fontColour = LCD_WHITE;
	
	char hostname[M6MAIN_HOSTNAME_LEN];
	int count_alias, count_addr, alength, atype;
	int row, col;
	struct hostent *phost;
	struct in_addr t_addr;
	char ipAddress[400] = "";
	
	menu = 1;

	LCDMenu(""," NETWORK","SETTINGS"," BACK");

	LCDClear();
	int changed = 1;
	while(keycode != KEY4)
	{
		switch(keycode)
		{
		case KEY1:
			menu = 1;
			LCDClear();
			keycode = 0x0;
			changed = 1;
			break;
		case KEY2:
			menu = 2;
			LCDClear();
			keycode = 0x0;
			changed = 1;
			break;
		case KEY3:
			page_settings();
			keycode = 0x0;
			changed = 1;
			break;
		default:
			keycode = 0x0;
			break;
		}
		if (menu == 1)
		{
			OSInfoCPU(&cpu);
			OSInfoMem(&mem);
			OSInfoMisc(&misc);
			/*
			 * note: using int instead of double so that LCDPrintf prints the value correctly
			 */
			int memTotal = atoi(mem.total)/1024;
			int memFree = atoi(mem.free)/1024;
			LCDSetPos(0,0);
			LCDPrintfFont(eyeFontBoldWhite,"System Page\n\n");
			LCDPrintfFont(eyeFontBoldWhite,"Processor: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%s\n", cpu.name);
			LCDPrintfFont(eyeFontBoldWhite,"    Speed: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%sMHz\n", cpu.mhz);
			LCDPrintfFont(eyeFontBoldWhite,"     Arch: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%s\n", cpu.arch);
			LCDPrintfFont(eyeFontBoldWhite," BogoMIPS: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%s\n", cpu.bogomips);
			LCDPrintfFont(eyeFontBoldWhite,"\n");
			LCDPrintfFont(eyeFontBoldWhite,"Processes: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%s\n", mem.procnum);
			LCDPrintfFont(eyeFontBoldWhite,"Total RAM: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%dMB\n", memTotal);
			LCDPrintfFont(eyeFontBoldWhite," Free RAM: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%dMB\n", memFree);
			LCDPrintfFont(eyeFontBoldWhite,"\n");
			LCDPrintfFont(eyeFontBoldWhite,"   Uptime: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%ss\n", misc.uptime);
			LCDPrintfFont(eyeFontBoldWhite,"  Battery: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%ss\n", misc.vbatt);
			if (changed == 1)
			{
				LCDMenu(""," NETWORK","SETTINGS"," BACK");
				changed = 0;
			}
			LCDRefresh2();
		}
		else if (menu == 2)
		{        	
			count_alias = 0;
			count_addr = 0;
			alength = 0;
			atype = 0;
			gethostname(hostname,M6MAIN_HOSTNAME_LEN);
			phost = gethostbyname(hostname);
			if(phost)
			{
				while(phost->h_aliases[count_alias])
					count_alias++;
				while(phost->h_addr_list[count_addr])
					count_addr++;
				alength = phost->h_length;
				atype = phost->h_addrtype;
				haddr2inaddr(phost->h_addr, &t_addr, alength);
			} 
			LCDSetPos(0,0);
			LCDPrintfFont(eyeFontBoldWhite,"Network Page\n\n");
			LCDGetPos(&row,&col);
			LCDSetPos(row,col);
			LCDPrintfFont(eyeFontBoldWhite," Hostname: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%s\n", hostname);
			LCDPrintfFont(eyeFontBoldWhite,"Addr-Type: ");
			LCDPrintfFont(eyeFontDefaultYellow,"%s\n", atype==AF_INET ?
				"AF_INET" : atype==AF_INET6 ?
				"AF_INET6" : "Unsupported");
			LCDPrintfFont(eyeFontBoldWhite,"  IP-Addr: ");
			ipAddress[0] = '\0';
			sprintf(ipAddress,"%s",execute("ifconfig eth0"));
			if (ipAddress[68] == 'i')
			{
				LCDPrintfFont(eyeFontDefaultYellow,"%s\n", execute("ifconfig eth0 | sed -rn 's/.*r:([^ ]+) .*/\\1/p' | awk '{printf \"%s \",$0} END {print \"\"}'"));
			}
			else
			{
				LCDPrintfFont(eyeFontDefaultYellow,"10.0.0.1\n");
			}
        		
			printfile("/sys/class/net/eth0/address", "MAC-Addr");

            
            LCDPrintfFont(eyeFontBoldWhite, " Wifi SSID: ");
            
            char temp[20]="";
            char serial[20]="";
            temp[0]='\0';
            serial[0]='\0';
            sprintf(temp,"%s" , execute("cat /proc/cpuinfo |grep Serial|cut -d' ' -f2"));
            int i =0;
            while(temp[i]=='0') {
                i++;
                continue;
            }
            int j=0;
            for (i;i<(int)strlen(temp); i++) {
                serial[j] = temp[i];
                j++;
            }
            
            strcpy(temp,"Pi_");
            strcat(temp,serial);
            LCDPrintfFont(eyeFontDefaultYellow, "%s\n", temp);
            
            LCDPrintfFont(eyeFontBoldWhite, " Wifi IP: ");
            LCDPrintfFont(eyeFontDefaultYellow, "10.1.1.1\n");
            
            
			if (changed == 1)
			{
				LCDMenu(" SYSTEM","","SETTINGS"," BACK");
				changed = 0;
			}
            
			LCDRefresh2();
		}
	}
	return 0;
}

int page_main(void)
{
	fbinfo_t fb;
	info_misc_t misc;
	int p, row, col;
	const int vbatt_min_8 = 3 << 8;
	const int vbatt_max_8 = 12 << 8;
	char hostname[M6MAIN_HOSTNAME_LEN];
	char ipAddress[400] = "";
    char temp[20]="";
    char serial[20]="";
    char cmd[50]="";
	XEvent event;
	
	EyeFont eyeFontDefaultYellow;
	EyeFont eyeFontBoldWhite;
	
	strcpy(eyeFontDefaultYellow.fontName,FONTDEFAULT);
	eyeFontDefaultYellow.fontColour = LCD_YELLOW;
	strcpy(eyeFontBoldWhite.fontName,FONTBOLD);
	eyeFontBoldWhite.fontColour = LCD_WHITE;
	
	pthread_create(&threadKeyInput,NULL,keyInputThread,NULL);
	pthread_create(&threadIRInput,NULL,irInputThread,NULL);
	pthread_create(&threadScreenRefresh,NULL,screenRefreshThread,NULL);
	
	KEYSetTM(KEYTM_CLASSIC);
	LCDTextColor(LCD_WHITE, LCD_BLACK, LCD_BGCOL_TRANSPARENT);
	LCDSetMode(LCD_NOAUTOREFRESH|LCD_SHOWMENU);
	LCDGetFBInfo(&fb);

	LCDClear();
	keycode = 0x0;
    
    temp[0]='\0';
    serial[0]='\0';
    sprintf(temp,"%s" , execute("cat /proc/cpuinfo |grep Serial|cut -d' ' -f2"));
    int i =0;
    while(temp[i]=='0') {
        i++;
        continue;
    }
    int j=0;
    for (i;i<(int)strlen(temp); i++) {
        serial[j] = temp[i];
        j++;
    }
    
    //Setting the SSID
    strcpy(temp,"Pi_");
    strcat(temp,serial);
    
    cmd[0]='\0';
    sprintf(cmd,"sed -i '3s/.*/ssid=%s/' /etc/hostapd/hostapd.conf",temp);
    system(cmd);
    
    system("sudo service hostapd restart");
    system("sudo service isc-dhcp-server restart");
    
	
	LCDMenu("INFO","HARDWARE"," SOFTWARE","DEMO");
	do
	{
		row = M6MAIN_TEXTROW;
		col = M6MAIN_TEXTCOL;
		if (screenRefresh)
		{
			LCDClear();
			screenRefresh = 0;
			LCDRefresh();
		}
		LCDSetPos(row,col);
        
		LCDPrintfFont(eyeFontBoldWhite," Welcome to the EyeBot M7.\n\n RoBIOS version: ");
		LCDPrintfFont(eyeFontDefaultYellow,"%s\n\n\n",ROBIOS_VERSION);
		LCDGetPos(&row,&col);
		LCDTextBar(row, col, fb.cursor.xmax, 0, M6MAIN_BATTBAR_COLOR);
		OSInfoMisc(&misc);
		if(misc.vbatt_8 < vbatt_min_8)
			misc.vbatt_8 = vbatt_min_8;
		if(misc.vbatt_8 > vbatt_max_8)
			misc.vbatt_8 = vbatt_max_8;
		p = 100 * (misc.vbatt_8 - vbatt_min_8) / (vbatt_max_8 - vbatt_min_8);
		LCDTextBar(row, col, fb.cursor.xmax, p, M6MAIN_BATTBAR_COLOR);
        
        //displaying the battery status
		LCDSetPrintf(row, col+1, "Battery: %s\n\n", misc.vbatt);
		// displaying the IP Address of the ethernet connection, default to 10.0.0.1 if none
		LCDPrintfFont(eyeFontBoldWhite," IP-Addr : ");
		ipAddress[0] = '\0';
		sprintf(ipAddress,"%s",execute("ifconfig eth0"));
		if (ipAddress[68] == 'i')
		{
			LCDPrintfFont(eyeFontDefaultYellow,"%s\n", execute("ifconfig eth0 | sed -rn 's/.*r:([^ ]+) .*/\\1/p' | awk '{printf \"%s \",$0} END {print \"\"}'"));
		}
		else
		{
			LCDPrintfFont(eyeFontDefaultYellow,"10.0.0.1\n");
		}
        //displaying the hostname
		gethostname(hostname,M6MAIN_HOSTNAME_LEN);
		LCDPrintfFont(eyeFontBoldWhite," Hostname: ");
		LCDPrintfFont(eyeFontDefaultYellow,"%s\n", hostname);
        
        //displaying the mac address
        //using fopen
		printfile("/sys/class/net/eth0/address", "MAC-Addr");
        
        //displaying the SSID
        LCDPrintfFont(eyeFontBoldWhite, " Wifi SSID: ");
        
        LCDPrintfFont(eyeFontDefaultYellow, "%s\n", temp);
        
        LCDPrintfFont(eyeFontBoldWhite, " Wifi IP: ");
        
        LCDPrintfFont(eyeFontDefaultYellow, "10.1.1.1\n", temp);
     
		LCDRefresh2();
		
		switch(keycode)
		{
			case KEY1:
			{
				page_info();
				LCDClear();
				keycode = 0x0;
				LCDMenu(" INFO"," HARDWARE"," SOFTWARE"," DEMO");
				break;
			}
			case KEY2:
			{
				show_hdt();
				LCDClear();
				keycode = 0x0;
				LCDMenu(" INFO"," HARDWARE"," SOFTWARE"," DEMO");
				break;
			}
			case KEY3:
			{
				show_exec();
				LCDClear();
				keycode = 0x0;
				LCDMenu(" INFO"," HARDWARE"," SOFTWARE"," DEMO");
				break;
			}
			case KEY4:
			{
				demoPage();
				LCDClear();
				keycode = 0x0;
				LCDMenu(" INFO"," HARDWARE"," SOFTWARE"," DEMO");
				break;
			}
		}
		usleep(LOOPSLEEPTIME);
	}
	while(keycode!=KEY_ESCAPE);

	LCDClear();

	return 0;
}

void getParams()
{
	robiosParams.irEnable = -1;
	robiosParams.runProg = -1;
	robiosParams.progName = NULL;
	robiosParams.irCode = NULL;
	FILE *paramFile;
	paramFile = fopen("/home/pi/EyeBot_BB/usr/robios_params.txt","r");
	if (paramFile == NULL)
	{
		fprintf(stderr,"WARNING: paramater file not found. Using default values and creating new one\n");
		paramFile = fopen("/usr/Eyebot/robios_params.txt","w");
		robiosParams.irCode = "786";
		robiosParams.irEnable = 1;
		robiosParams.runProg = 0;
		robiosParams.progName = "-1";
		robiosParams.irDelay = 5;
		if (paramFile == NULL)
		{
			fprintf(stderr,"ERROR: unable to create parameter file\n");
		}
		else
		{
			fprintf(paramFile,"irEnabled %d\n",robiosParams.irEnable);
			fprintf(paramFile,"irCode %s\n",robiosParams.irCode);
			fprintf(paramFile,"runProg %d\n",robiosParams.runProg);
			fprintf(paramFile,"progName %s\n",robiosParams.progName);
			fprintf(paramFile,"irDelay %d\n",robiosParams.irDelay);
		}
	}
	else
	{
		char *line = (char *)malloc(200);
		char *inputA = (char *)malloc(200);
		char *inputB = (char *)malloc(200);
		while (fgets(line,200,paramFile) != NULL)
		{
			sscanf(line,"%s %s\n",inputA,inputB);
			if (strcmp(inputA,"irEnabled")==0)
			{
				robiosParams.irEnable = atoi(inputB);
				if (robiosParams.irEnable == 0)
				{
					fprintf(stderr,"IR input disabled by default\n");
				}
				else if (robiosParams.irEnable == 1)
				{
					fprintf(stderr,"IR input enabled by default\n");
				}
			}
			else if (strcmp(inputA,"irCode")==0)
			{
				robiosParams.irCode = (char *)calloc(1,200);
				memcpy(robiosParams.irCode,inputB,strlen(inputB));
				fprintf(stderr,"IR code: %s\n",robiosParams.irCode);
			}
			else if (strcmp(inputA,"runProg")==0)
			{
				robiosParams.runProg = atoi(inputB);
				if (robiosParams.runProg == 0)
				{
					fprintf(stderr,"Run program disabled\n");
				}
				else if (robiosParams.runProg == 1)
				{
					fprintf(stderr,"Run program enabled\n");
				}
			}
			else if (strcmp(inputA,"progName")==0)
			{
				robiosParams.progName = (char *)calloc(1,200);
				memcpy(robiosParams.progName,inputB,strlen(inputB));
				fprintf(stderr,"Startup program name: %s\n",robiosParams.progName);
			}
			else if (strcmp(inputA,"irDelay")==0)
			{
				robiosParams.irDelay = atoi(inputB);
				fprintf(stderr,"irDelay set to %d\n",robiosParams.irDelay);
			}
		}
		free(line);
		free(inputA);
		free(inputB);
	}
	if (robiosParams.irCode == NULL)
	{
		robiosParams.irCode = (char *)calloc(1,10);
		robiosParams.irCode = "786";
	}
	if (robiosParams.progName == NULL)
	{
		robiosParams.progName = (char *)calloc(1,200);
		robiosParams.progName = "-1";
	}
	if (robiosParams.irEnable == -1)
	{
		robiosParams.irEnable = 1;
		IRTVInit(robiosParams.irCode);
	}
	else if (robiosParams.irEnable == 1)
	{
		IRTVInit(robiosParams.irCode);
	}
	if (robiosParams.runProg == -1)
	{
		robiosParams.runProg = 0;
	}
	else if (robiosParams.runProg == 1)
	{
		char *processName = calloc(1,300);
		sprintf(processName,"/home/pi/EyeBot_BB/usr/%s",robiosParams.progName);
		if (access(processName,F_OK) != -1)
		{
			runProcess(processName);
		}
		else
		{
			fprintf(stderr,"ERROR: unable to run startup program\n");
			robiosParams.runProg = 0;
		}
	}
	if (robiosParams.irDelay < 1)
	{
		robiosParams.irDelay = 1;
	}
	IRTVSetHoldDelay(robiosParams.irDelay);
	if (paramFile != NULL)
	{
		fclose(paramFile);
	}
}

int main(int argc, char *argv[])
{
	//the two lines below are required to ensure that the touch input works correctly
	/*execute("gpio mode 0 in");
	execute("gpio mode 0 up");*/
	getParams();
	if (robiosParams.runProg == 1)
	{
		exit(0);
	}
	keycode = 0x0;
	screenRefresh = 0;
	int windowBorder = 0;
	struct sigaction new_act, old_act;
	SPIHandle handle = SPIInit(0);
	//Enable multi-thread support
	if (XInitThreads() == 0)
	{
		fprintf(stderr,"\nERROR: cannot enable multi-thread support\n");
		exit(EXIT_FAILURE);
	}
	/* initialize I/O */
	if (argc > 1)
	{
		windowBorder = atoi(argv[1]);
	}
	if(LCDInit()<0)
	{
		printf("LCDInit Error! Exiting!\n");
		return -1;
	}
	if(KEYInit()<0)
	{
		printf("KEYInit Error! Exiting!\n");
		return -1;
	}

	/* setup new sigchld handler */
	new_act.sa_handler = sigchld_handler;
	memset((void*)&new_act.sa_mask,0x0,sizeof(new_act.sa_mask));
	new_act.sa_flags = SA_NOCLDSTOP; /* only terminated child! */
	/* get old handler */
	sigaction(SIGCHLD, 0x0, &old_act);
	/* replace only if not ignored!? */
	if(old_act.sa_handler != SIG_IGN)
		sigaction(SIGCHLD, &new_act, 0x0);
	else
		printf("Cannot set new SIGCHLD handler!");
	keycode = 0x0;
	/* show main page */
	page_main();
	/*if (IRTVGetStatus()==1)
	{
		IRTVRelease();
	}*/
	/* release I/O */
	LCDRelease();
	KEYRelease();
	SPIRelease(handle);

	return 0;
}
