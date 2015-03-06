#include <eyebot.h>
#include "header.h"
#include <string.h>

void IRScrollUp(void)
{
	int index = listmenuGetIndex(*gLCDHandle->listMenu);
	if (index > 0)
	{
		LCDListIndex(index-1);
	}
	else if (gLCDHandle->listMenu->start > 0)
	{
		LCDListScrollUp();
	}
}

void IRScrollDown(void)
{
	int index = listmenuGetIndex(*gLCDHandle->listMenu);
	int maxNumb = 0;
	if (gLCDHandle->listMenu->scroll == 1)
	{
		maxNumb = LCD_MAX_LIST_ITEM - 2;
	}
	else
	{
		maxNumb = gLCDHandle->listMenu->size;
	}
	if (index < maxNumb-1)
	{
		LCDListIndex(index+1);
	}
	else
	{
		LCDListScrollDown();
	}
}

char *keyboardInput(void)
{
	XEvent event;
	int exit = 0;
	int returnSize = 20;
	int length = 0;
	char *retInput = calloc(returnSize,1);
	char *input = calloc(1,1);
	listmenu_t *listtemp = LCDGetList();
	LCDClear();
	LCDSetMode(LCD_CLASSICMENU);
	LCDMenu("","","","");
	LCDRefresh();
	while (exit == 0)
	{
		LCDSetPrintf(1,1,"Press enter to exit\nInput:\n%s",retInput);
		LCDRefresh();
		XNextEvent(gLCDHandle->d,&event);
		switch (event.type)
		{
			case KeyRelease:
			{
				KeySym keysym;
				XLookupString(&event.xkey,input,10,&keysym,NULL);
				if (input[0] == '\n' || input[0] == '\r')
				{
					exit = 1;
				}
				else if ((input[0] >= 'a'&& input[0] <= 'z')||(input[0] >= 'A'&& input[0] <= 'Z')||(input[0] >= '0'&& input[0] <= '9')||input[0] == ' ')
				{
					if (length==returnSize-1)
					{
						break;
					}
					length++;
					strcat(retInput,input);
					retInput[length] = '\0';
				}
				else if (input[0] == 8)
				{
					if (length == 1)
					{
						retInput[0] = '\0';
						length--;
					}
					else if (length > 1)
					{
						retInput[length-1] = '\0';
						length--;
					}
				}
				break;
			}
		}
		LCDClear();
		fprintf(stderr,"Length: %d\n",length);
	}
	LCDSetMode(LCD_LISTMENU);
	LCDSetList(listtemp);
	//free(input);
	return retInput;
}
