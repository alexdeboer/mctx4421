#include <stdlib.h>
#include <eyebot.h>	//need to include eyebot.h so that LCD_MAX_LIST_ITEM calculates correctly

int listmenuSize(listmenu_t listmenu)
{
	return listmenu.size;
}

void listmenuToggleScroll(listmenu_t *listmenu)
{
	if (listmenu->scroll==0)
	{
		listmenu->scroll = 1;
	}
	else
	{
		listmenu->scroll = 0;
	}
}

int listmenuGetScroll(listmenu_t listmenu)
{
	return listmenu.scroll;
}

void listmenuInit(listmenu_t *listmenu,char *name)
{
	fbinfo_t fb;
	LCDGetFBInfo(&fb);
	strcpy_n(listmenu->title,name,LCD_LIST_STRLENGTH);
	listmenu->fgcol = LCD_WHITE;
	listmenu->bgcol = LCD_BLUE;
	listmenu->size = 0;
	listmenu->start = 0;
	listmenu->left = 0;
	listmenu->width = fb.cursor.xmax;
	listmenu->no_empty = 0;
	listmenu->scroll = 0;
	listmenu->index = 0;
	//listmenu->count = 0;
}

void listmenuAddItem(listmenu_t *listmenu,char *name,void *plink)
{
	if (listmenu->size == 0)
	{
		listmenu->pitems = malloc(sizeof(menuitem_t));
	}
	else
	{
		listmenu->pitems = realloc(listmenu->pitems,(listmenuSize(*listmenu)+1)*sizeof(menuitem_t));
	}
	if (!listmenu->pitems)
	{
			printf("ERROR: unable to allocate memory for listmenu\n");
			listmenu->pitems == NULL;
			return;
	}
	listmenu->size+=1;
	strcpy_n(listmenu->pitems[listmenuSize(*listmenu)-1].label,name,LCD_LIST_STRLENGTH);
	listmenu->pitems[listmenuSize(*listmenu)-1].fgcol = LCD_RED;
	listmenu->pitems[listmenuSize(*listmenu)-1].bgcol = LCD_AQUA;
	listmenu->pitems[listmenuSize(*listmenu)-1].plink = plink;
	if (listmenuSize(*listmenu) > LCD_MAX_LIST_ITEM)
	{
		listmenu->scroll = 1;
	}
}

void listmenuFreeItems(listmenu_t *listmenu)
{
	free(listmenu->pitems);
	listmenu->size = 0;
	listmenu->scroll = 0;
}

void listmenuChangeItemText(listmenu_t *listmenu, char *newText,int index)
{
	if (index < 0 || index >= listmenu->size)
	{
		fprintf(stderr,"WARNING: attempted to change text of item outside bounds\n");
		return;
	}
	else
	{
		strcpy_n(listmenu->pitems[index].label,newText,LCD_LIST_STRLENGTH);
	}
}

int listmenuGetIndex(listmenu_t listmenu)
{
	return listmenu.index;
}

char *listmenuGetItemText(listmenu_t listmenu)
{
	int index = listmenu.index;
	return listmenu.pitems[index].label;
}
