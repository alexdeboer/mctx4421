/*
 * lcd.c
 *
 *  Created on: 8 Jul 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file    lcd.c
 * \brief   Defines functions to interact with the LCD screen
 * \author  Remi KEAT
 */

/*#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>*/

//#include <eyebot.h>
#include <lcd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Imlib2.h>

static int ApplicationErrorHandler(Display *display, XErrorEvent *theEvent)
{
	gLCDHandle->X11Error = true;
	return 0;
}

/**
 * \brief   Return the rgb_t color from the color name
 * \param   char* colorName
 * \return  rgb_t color
 */
rgb_t getColor(char* colorName)
{
	XColor color;
	XAllocNamedColor(gLCDHandle->d, gLCDHandle->colormap, colorName, &color, &color);
	return color;
}

/**
 * \brief   Draw a bordered frame
 * \param   int x1 : X-coordinate of top-left pixel
 * \param   int y1 : Y-coordinate of top-left pixel
 * \param   int x2 : X-coordinate of bottom-right pixel
 * \param   int y2 : Y-coordinate of bottom-right pixel
 * \return  int retVal : always 0
 */
int LCDDrawFrame(int x1, int y1, int x2, int y2, rgb_t color)
{
	LCDArea(x1+1, y1+1, x2, y2, color);
	LCDFrame(x1, y1, x2-1, y2-1, LCD_BLACK);
	return 0;
}

/**
 * \brief   Draw the menu
 * \return  int retVal : always 0
 */
int LCDDrawMenu(void)
{
	int i;
	if (gLCDHandle->mode&LCD_SHOWMENU)
	{
		int buttonWidth = gLCDHandle->width/4;
		int lastRow = gLCDHandle->height/gLCDHandle->fontHeight-2;
		int columnWidth = gLCDHandle->width/gLCDHandle->fontWidth/4;
		rgb_t lastFgTextColor = gLCDHandle->fgTextColor;
		rgb_t lastBgTextColor = gLCDHandle->bgTextColor;
		char lastColorflag = gLCDHandle->colorflag;
		char* token;
		char* copy;
		int textLen;
		int row;
		int cur_pos_x = gCurPosX;
		int cur_pos_y = gCurPosY;
		for (i = 0; i < 4; i++)
		{
			row = lastRow;
			LCDTextColor(gLCDHandle->menuItems[i].fgcol, gLCDHandle->menuItems[i].bgcol, LCD_BGCOL_TRANSPARENT);
			LCDDrawFrame(i*buttonWidth, gLCDHandle->height-MENU_HEIGHT, (i+1)*buttonWidth, gLCDHandle->height, gLCDHandle->menuItems[i].bgcol);
			copy = strdup(gLCDHandle->menuItems[i].label);
			textLen = strlen(gLCDHandle->menuItems[i].label);
			while ((token = strsep(&copy, "\n")) != NULL)
			{
				//LCDSetString(row,i*columnWidth+1, token);
				LCDSetPrintf(row,i*columnWidth+1, token);
				textLen -= strlen(token);
				if (textLen != 0)
				{
					row++;
					textLen -= 1;
				}
			}
		}
		LCDTextColor(lastFgTextColor, lastBgTextColor, lastColorflag);
		gCurPosX = cur_pos_x;
		gCurPosY = cur_pos_y;
	}
	return 0;
}

/**
 * \brief   Draw the list
 * \return  int retVal : always 0
 */
int LCDDrawList(void)
{
	int offset;
	int itemIndex;
	EyeFont eyeFont;
	strcpy(eyeFont.fontName,FONTBOLD);
	eyeFont.fontColour = LCD_BLACK;
	if (gLCDHandle->listMenu != NULL && (gLCDHandle->mode&LCD_LISTMENU))
	{
		if (gLCDHandle->listMenu->size != 0)
		{
			int listMaxNumber = 0;
			int i;
			int menuHeight = gLCDHandle->fontHeight*2;
			rgb_t lastFgTextColor = gLCDHandle->fgTextColor;
			rgb_t lastBgTextColor = gLCDHandle->bgTextColor;
			char lastColorflag = gLCDHandle->colorflag;
			listmenu_t*  menulist = gLCDHandle->listMenu;
			LCDDrawFrame(0, 0, gLCDHandle->width, menuHeight, menulist->bgcol);
			LCDTextColor(menulist->fgcol, menulist->bgcol, LCD_BGCOL_TRANSPARENT);
			LCDSetPrintf(0, 0, menulist->title);
			if (menulist->scroll == 0)
			{
				offset = 0;
				listMaxNumber = gLCDHandle->listMenu->size;
			}
			else
			{
				LCDDrawFrame(0, menuHeight, gLCDHandle->width, 2*menuHeight, LCD_RED);
				LCDSetPrintf(2, 1, "SCROLL UP");
				LCDDrawFrame(0, LCD_MAX_LIST_ITEM*menuHeight, gLCDHandle->width, (LCD_MAX_LIST_ITEM+1)*menuHeight, LCD_RED);
				LCDSetPrintf(2*LCD_MAX_LIST_ITEM, 1, "SCROLL DOWN");
				offset = 1;
				listMaxNumber = LCD_MAX_LIST_ITEM;
			}
			for (i = offset; i < listMaxNumber-offset; i++)
			{
				itemIndex = menulist->start+i-offset;
				if (itemIndex < menulist->size && itemIndex >= 0)
				{
					LCDDrawFrame(0, (i+1)*menuHeight, gLCDHandle->width, (i+2)*menuHeight, menulist->pitems[itemIndex].bgcol);
					LCDTextColor(menulist->pitems[itemIndex].fgcol, menulist->pitems[itemIndex].bgcol, LCD_BGCOL_TRANSPARENT);
					LCDSetPrintf(2*(i+1), 2, menulist->pitems[itemIndex].label);
					if (i-offset == menulist->index)
					{
						LCDSetPrintfFont(2*(i+1), 0,eyeFont, "> ");
						LCDSetFont(FONTDEFAULT);
					}
				}
				else
				{
					LCDDrawFrame(0, (i+1)*menuHeight, gLCDHandle->width, (i+2)*menuHeight, menulist->pitems[menulist->size-1].bgcol);
				}
			}
			LCDTextColor(lastFgTextColor, lastBgTextColor, lastColorflag);
			KEYSetTM(KEYTM_LISTMENU);
		}
	}
	return 0;
}

/*int LCDInit()
{
	int width, height;
	Display *pDisp;
	Screen *pScr;
	
	pDisp = XOpenDisplay(NULL);
	pScr = DefaultScreenOfDisplay(pDisp);
	
	width = pScr->width;
	height = pScr->height;
	
	XCloseDisplay(pDisp);
	
	//LCDInitParams(width,height);
	LCDInitParams(LCD_WIDTH,LCD_HEIGHT);
}*/

/**
 * \brief   Initialize the LCD
 * \return  int retVal : always 0
 */
int LCDInit()
{
	int i,s,lcdNum, width, height;
	gLCDHandle = malloc(sizeof(LCDHandle));
	Display *d;
	Screen *scr;
	Window w;
	XEvent e;
	Hints hints;
	Atom property;

	hints.flags = 2;
	hints.decorations = 0;

	/* Open the connetion with the server */
	d = XOpenDisplay(NULL);
	if (d == NULL) {
		printf("Cannot open the display\n");
		gLCDEnabled = false;
		return -1;
	}
	
	s = DefaultScreen(d);
	scr = DefaultScreenOfDisplay(d);
	lcdNum = s;
	
	/*width = scr->width;
	height = scr->height;*/
	width = LCD_WIDTH;
	height = LCD_HEIGHT;
	/*width = 640;
	height = 480;*/

	/* Create the window */
	w = XCreateSimpleWindow(d, RootWindow(d, s), 0, 0, width,height, 1, BlackPixel(d, s), WhitePixel(d, s));
	XStoreName(d, w, "EyeBot");
	
	property = XInternAtom(d, "_MOTIF_WM_HINTS", True);
	XChangeProperty(d, w, property, property, 32, PropModeReplace, (unsigned char *)&hints, 5);

	GC gc = XCreateGC(d, w, 0, 0);
	Colormap colormap = XDefaultColormap(d, s);

	XFontStruct* fontstruct = XLoadQueryFont(d, FONTDEFAULT);

	Atom delWindow = XInternAtom(d, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(d, w, &delWindow, 1);

	/* Choose the interesting events */
	XSelectInput(d, w, /*ExposureMask | KeyPressMask |*/ KeyReleaseMask | ButtonPressMask | FocusChangeMask);

	/* Show the window */
	XMapWindow(d, w);

	gLCDHandle->d = d;
	gLCDHandle->s = s;
	gLCDHandle->lcdNum = lcdNum;
	gLCDHandle->w = w;
	gLCDHandle->colormap = colormap;
	gLCDHandle->gc = gc;
	gLCDHandle->fontstruct = fontstruct;
	gLCDHandle->fontHeight = fontstruct->max_bounds.ascent+fontstruct->max_bounds.descent;
	gLCDHandle->fontWidth = XTextWidth(fontstruct, "a", 1);
	gLCDHandle->height = height;
	gLCDHandle->width = width;
	gLCDHandle->bgTextColor = LCD_BLACK;
	gLCDHandle->fgTextColor = LCD_WHITE;
	gLCDHandle->startCurPosX = 10;
	gLCDHandle->startCurPosY = gLCDHandle->fontHeight/2-2+gLCDHandle->fontHeight;
	gLCDHandle->fd = ConnectionNumber(d);
	gLCDHandle->mode = LCD_CLASSICMENU;
	gLCDHandle->X11Error = false;
	gCurPosX = gLCDHandle->startCurPosX;
	gCurPosY = gLCDHandle->startCurPosY;
	gLCDHandle->colorflag = 0x00;
	gLCDHandle->scr = scr;

	//set the colour of the button text and the background colour
	//button 1
	gLCDHandle->menuItems[0].fgcol = LCD_BLACK;
	gLCDHandle->menuItems[0].bgcol = LCD_RED;
	//button 2
	gLCDHandle->menuItems[1].fgcol = LCD_BLACK;
	gLCDHandle->menuItems[1].bgcol = LCD_GREEN;
	//button 3
	gLCDHandle->menuItems[2].fgcol = LCD_BLACK;
	gLCDHandle->menuItems[2].bgcol = LCD_YELLOW;
	//button 4
	gLCDHandle->menuItems[3].fgcol = LCD_BLACK;
	gLCDHandle->menuItems[3].bgcol = LCD_BLUE;

	gLCDHandle->listMenu = NULL;
	gLCDEnabled = true;
	XSetFont(gLCDHandle->d,gLCDHandle->gc,gLCDHandle->fontstruct->fid);
	LCDClear();
	return 0;
}

/**
 * \brief   Clear the LCD display and all display buffers.
 * \return  int retVal : always 0
 */
int LCDClear(void)
{
	LCDArea(0, 0, gLCDHandle->width, gLCDHandle->height, LCD_BLACK);
	gCurPosX = gLCDHandle->startCurPosX;
	gCurPosY = gLCDHandle->startCurPosY;
	LCDTextColor(LCD_WHITE, LCD_BLACK, LCD_BGCOL_TRANSPARENT);
	return 0;
}

/**
 * \brief   Update the internal mode flag bits.
 * \param		hword_t mode : LCD Mode flag
 * \return  int retVal : always 0
 */
int LCDSetMode(hword_t mode)
{
	if (mode & LCD_AUTOREFRESH)	gLCDHandle->mode |= LCD_AUTOREFRESH;
	if (mode & LCD_SCROLLING)	gLCDHandle->mode |= LCD_SCROLLING;
	if (mode & LCD_LINEFEED) gLCDHandle->mode |= LCD_LINEFEED;
	if (mode & LCD_SHOWMENU) gLCDHandle->mode |= LCD_SHOWMENU;
	if (mode & LCD_FB_ROTATE) gLCDHandle->mode |= LCD_FB_ROTATE;
	if (mode & LCD_LISTMENU) gLCDHandle->mode |= LCD_LISTMENU;

	if (mode & LCD_NOAUTOREFRESH) gLCDHandle->mode &= ~(LCD_AUTOREFRESH);
	if (mode & LCD_NOSCROLLING) gLCDHandle->mode &= ~(LCD_SCROLLING);
	if (mode & LCD_NOLINEFEED) gLCDHandle->mode &= ~(LCD_LINEFEED);
	if (mode & LCD_HIDEMENU)
	{
		gLCDHandle->mode &= ~(LCD_SHOWMENU);
		gLCDHandle->mode &= ~(LCD_LISTMENU);
	}
	if (mode & LCD_FB_NOROTATION) gLCDHandle->mode &= ~(LCD_FB_ROTATE);
	if (mode & LCD_CLASSICMENU) gLCDHandle->mode &= ~(LCD_LISTMENU);
	return 0;
}

/**
 * \brief   Get the internal mode flag bits.
 * \return  hword_t mode : Current mode flag bits
 */
hword_t LCDGetMode(void)
{
	return gLCDHandle->mode;
}

/**
 * \brief   Reset the internal mode flag bits to a previously saved mode.
 * \param   hword_t mode : Mode flag - bit XORed
 * \return  int retVal : always 0
 */
int LCDResetMode(hword_t mode)
{
	gLCDHandle->mode = mode & 0xFF;
	return 0;
}

/**
 * \brief   Set menu entries in KEY_CLASSIC mode (4-buttons).
 * Also sets the LCD_SHOWMENU flag and refresh the LCD.
 * \param   char* string1 :  Menu entry for KEY1 in classic mode
 * \param   char* string2 :  Menu entry for KEY2 in classic mode
 * \param   char* string3 :  Menu entry for KEY3 in classic mode
 * \param   char* string4 :  Menu entry for KEY4 in classic mode
 * \return  int retVal : always 0
 */
int LCDMenu(char *string1, char *string2, char *string3, char *string4)
{
	strcpy(gLCDHandle->menuItems[0].label, string1);
	strcpy(gLCDHandle->menuItems[1].label, string2);
	strcpy(gLCDHandle->menuItems[2].label, string3);
	strcpy(gLCDHandle->menuItems[3].label, string4);
	LCDSetMode(LCD_SHOWMENU);
	LCDDrawMenu();
	return 0;
}

/**
 * \brief   Set specific menu entry in KEY_CLASSIC mode (index given by pos).
 * Color customization for specific key is now possible (fgcol/bgcol).
 * A user-specific data can be linked to the menu using userp pointer.
 * Will also set the LCD_SHOWMENU flag and refresh the LCD.
 * \param   int pos : Select menu entry in classic mode
 * \param   char* string : Menu entry for the key at specified index
 * \param   rgb_t fgcol : Textcolor for the menu
 * \param   rgb_t bgcol : Background color for the menu
 * \param   void* userp : A general purpose pointer for user-specific data
 * \return  int retVal : always 0
 */
int LCDMenuI(int pos, char *string, rgb_t fgcol, rgb_t bgcol, void* userp)
{
	strcpy(gLCDHandle->menuItems[pos].label, string);
	gLCDHandle->menuItems[pos].fgcol = fgcol;
	gLCDHandle->menuItems[pos].bgcol = bgcol;
	LCDSetMode(LCD_SHOWMENU);
	LCDDrawMenu();
	return 0;
}

/**
 * \brief	Return the menuitem at a given position
 * \param   int index : position of the menuitem
 * \return  menuitem_t* menuItem
 */
menuitem_t* LCDMenuItem(int index)
{
	return &(gLCDHandle->menuItems[index]);
}

/**
 * \brief   Setup the list menu display and update appropriate info in the listmenu_t structure pointed by menulist (e.g. scroll, count).
 * Will also set the LCD_LISTMENU flag and refresh the LCD.
 * \param   listmenu_t* menulist : Listmenu to be used for display
 * \return  int retVal : always 0
 */
int LCDList(listmenu_t *menulist)
{
	gLCDHandle->listMenu = menulist;
	LCDSetMode(LCD_LISTMENU);
	LCDDrawList();
	return 0;
}

/**
 * \brief   Unlike LCDList(), this will blindly assign menulist to the mainlist for display.
 * Doesn't update anything in the menulist structure, nor modify any internal flags.
 * Useful to maintain multiple lists for menu display.
 * \param   listmenu_t* menulist : Listmenu to be used for display
 * \return  int retVal : always 0
 */
int LCDSetList(listmenu_t* menulist)
{
	gLCDHandle->listMenu = menulist;
	LCDSetMode(LCD_LISTMENU);
	LCDDrawList();
	return 0;
}

/**
 * \brief   Get the currently active list menu.
 * \return  listmenu_t* retListMenu : Pointer to listmenu_t structure
 */
listmenu_t* LCDGetList(void)
{
	return gLCDHandle->listMenu;
}

/**
 * \brief   Get the frame info of a specific list item in form of a menurect_t structure.
 * \param   int pos : Index of list item
 * \return  menurect_t* retMenuRect : Pointer to a menurect_t structure
 */
menurect_t* LCDListBox(int pos)
{
	menurect_t* menuRect = malloc(sizeof(menurect_t));
	menuRect->x = 0;
	menuRect->y = (1+pos)*2*gLCDHandle->fontHeight;
	menuRect->height = 2*gLCDHandle->fontHeight;
	menuRect->width = gLCDHandle->width;
	return menuRect;
}

/**
 * \brief   Get the selected menuitem in the list menu – using index & start variable in listmenu_t.
 * Will return  0x0 (NUL) if no item is currently selected.
 * \return  menuitem_t* retMenuItem : Pointer to a menuitem_t structure
 */
menuitem_t* LCDListActiveItem(void)
{
	int index = gLCDHandle->listMenu->index;
	return &(gLCDHandle->listMenu->pitems[index]);
}

/**
 * \brief   Draw a color-filled rectangle with (x1,y1) as top-left coordinate and (x2,y2) as the bottom-right coordinate.
 * \param   int x1 : X-coordinate of top-left pixel
 * \param   int y1 : Y-coordinate of top-left pixel
 * \param   int x2 : X-coordinate of bottom-right pixel
 * \param   int y2 : Y-coordinate of bottom-right pixel
 * \param   rgb_t color : RGB fill color value
 * \return  int retVal : always 0
 */
int LCDArea(int x1, int y1, int x2, int y2, rgb_t color)
{
	XSetForeground(gLCDHandle->d, gLCDHandle->gc, color.pixel);
	XFillRectangle(gLCDHandle->d, gLCDHandle->w, gLCDHandle->gc, x1, y1, x2-x1, y2-y1);
	return 0;
}

/**
 * \brief   Draw a color rectangle frame with (x1,y1) as top-left coordinate and (x2,y2) as the bottom-right coordinate.
 * \param   int x1 : X-coordinate of top-left pixel
 * \param   int y1 : Y-coordinate of top-left pixel
 * \param   int x2 : X-coordinate of bottom-right pixel
 * \param   int y2 : Y-coordinate of bottom-right pixel
 * \param   rgb_t color : RGB fill color value
 * \return  int retVal : always 0
 */
int LCDFrame(int x1, int y1, int x2, int y2, rgb_t color)
{
	XSetForeground(gLCDHandle->d, gLCDHandle->gc, color.pixel);
	XDrawRectangle(gLCDHandle->d, gLCDHandle->w, gLCDHandle->gc, x1, y1, x2-x1, y2-y1);
	return 0;
}

/**
 * \brief   Set the default color for text (including background) and related flags (e.g. for transparent background).
 * \param   rgb_t fgcol : Default color for text
 * \param   rgb_t bgcol : Default color for text background
 * \param   char colorflags : Mode flag for text color
 * \details Valid value for colorflags :
 * - LCD_BGCOL_TRANSPARENT
 * - LCD_BGCOL_INVERSE
 * - LCD_FGCOL_INVERSE
 * - LCD_BGCOL_NOTRANSPARE
 * - LCD_BGCOL_NOINVERSE
 * - LCD_FGCOL_NOINVERSE
 *
 * \return  int retVal : always 0
 */
int LCDTextColor(rgb_t fgcol, rgb_t bgcol, char colorflags)
{
	gLCDHandle->fgTextColor = fgcol;
	gLCDHandle->bgTextColor = bgcol;
	gLCDHandle->colorflag = colorflags;
	return 0;
}

int LCDSetFont(char *fontAlias)
{
	XFreeFont(gLCDHandle->d,gLCDHandle->fontstruct);
	gLCDHandle->fontstruct = XLoadQueryFont(gLCDHandle->d, fontAlias);
	gLCDHandle->fontHeight = gLCDHandle->fontstruct->max_bounds.ascent+gLCDHandle->fontstruct->max_bounds.descent;
	gLCDHandle->fontWidth = XTextWidth(gLCDHandle->fontstruct, "a", 1);
	XSetFont(gLCDHandle->d,gLCDHandle->gc,gLCDHandle->fontstruct->fid);
	return 0;
}

/**
 * \brief   Print formatted string to LCD and refresh LCD. Cursor position is updated.
 * \param   const char* format : Formatted string
 * \return  int retVal : always 0
 */
int LCDPrintfFont(EyeFont eyeFont,const char *format, ...)
{
	char* text;
	va_list args;
	va_start(args, format);
	vasprintf(&text, format, args);
	char* token;
	char* copy = strdup(text);
	int textWidth;
	int textLen = strlen(text);
	LCDSetFont(eyeFont.fontName);
	XSetForeground(gLCDHandle->d, gLCDHandle->gc, eyeFont.fontColour.pixel);
	XSetBackground(gLCDHandle->d, gLCDHandle->gc, gLCDHandle->bgTextColor.pixel);
	while ((token = strsep(&copy, "\n")) != NULL)
	{
		textWidth = XTextWidth(gLCDHandle->fontstruct, token, strlen(token));
		if (gCurPosX+textWidth > gLCDHandle->width)
		{
			gCurPosX = gLCDHandle->startCurPosX;
			gCurPosY += gLCDHandle->fontHeight;
		}
		if (gLCDHandle->colorflag == LCD_BGCOL_TRANSPARENT)
		{
			XSetBackground(gLCDHandle->d, gLCDHandle->gc, LCDGetPixel(gCurPosX, gCurPosY).pixel);
			XDrawImageString(gLCDHandle->d, gLCDHandle->w, gLCDHandle->gc, gCurPosX, gCurPosY, token, strlen(token));
			XSetBackground(gLCDHandle->d, gLCDHandle->gc, gLCDHandle->bgTextColor.pixel);
		}
		else
		{
			XDrawImageString(gLCDHandle->d, gLCDHandle->w, gLCDHandle->gc, gCurPosX, gCurPosY, token, strlen(token));
		}
		textLen -= strlen(token);
		if (textLen != 0)
		{
			gCurPosX = gLCDHandle->startCurPosX;
			gCurPosY += gLCDHandle->fontHeight;
			textLen -= 1;
		}
		else
		{
			gCurPosX += textWidth;
		}
	}
	free(text);
	va_end(args);
	return 0;
}

/**
 * \brief   Print formatted string to LCD and refresh LCD. Cursor position is updated.
 * \param   const char* format : Formatted string
 * \return  int retVal : always 0
 */
int LCDPrintf(const char *format, ...)
{
	char *text;
	va_list args;
	va_start(args,format);
	vasprintf(&text, format, args);
	EyeFont eyeFont;
	eyeFont.fontColour = gLCDHandle->fgTextColor;
	strcpy(eyeFont.fontName,FONTDEFAULT);
	LCDPrintfFont(eyeFont,text);
	free(text);
	va_end(args);
	return 0;
}

/**
 * \brief   LCDPrintf with text position specified.
 * \param   int row : Cursor position
 * \param   int column : Cursor position
 * \param   const char* format : Formatted string
 * \return  int retVal : always 0
 */
int LCDSetPrintf(int row, int column, const char *format, ...)
{
	char* text;
	va_list args;
	va_start(args, format);
	int temp_cur_pos_x = gLCDHandle->startCurPosX+column*gLCDHandle->fontWidth;
	int temp_cur_pos_y = gLCDHandle->startCurPosY+row*gLCDHandle->fontHeight;
	vasprintf(&text, format, args);
	char* token;
	char* copy = strdup(text);
	int textWidth;
	int textLen = strlen(text);
	XSetForeground(gLCDHandle->d, gLCDHandle->gc, gLCDHandle->fgTextColor.pixel);
	XSetBackground(gLCDHandle->d, gLCDHandle->gc, gLCDHandle->bgTextColor.pixel);
	while ((token = strsep(&copy, "\n")) != NULL)
	{
		textWidth = XTextWidth(gLCDHandle->fontstruct, token, strlen(token));
		if (temp_cur_pos_x+textWidth > gLCDHandle->width)
		{
			temp_cur_pos_x = gLCDHandle->startCurPosX;
			temp_cur_pos_y += gLCDHandle->fontHeight;
		}
		if (gLCDHandle->colorflag == LCD_BGCOL_TRANSPARENT)
		{
			XSetBackground(gLCDHandle->d, gLCDHandle->gc, LCDGetPixel(temp_cur_pos_x, temp_cur_pos_y).pixel);
			XDrawImageString(gLCDHandle->d, gLCDHandle->w, gLCDHandle->gc, temp_cur_pos_x, temp_cur_pos_y, token, strlen(token));
			XSetBackground(gLCDHandle->d, gLCDHandle->gc, gLCDHandle->bgTextColor.pixel);
		}
		else
		{
			XDrawImageString(gLCDHandle->d, gLCDHandle->w, gLCDHandle->gc, temp_cur_pos_x, temp_cur_pos_y, token, strlen(token));
		}
		textLen -= strlen(token);
		if (textLen != 0)
		{
			temp_cur_pos_x = gLCDHandle->startCurPosX;
			temp_cur_pos_y += gLCDHandle->fontHeight;
			textLen -= 1;
		}
		else
		{
			temp_cur_pos_x += textWidth;
		}
	}
	gCurPosX = temp_cur_pos_x;
	gCurPosY = temp_cur_pos_y;
	free(text);
	va_end(args);
	return 0;
}

/**
 * \brief   LCDPrintf with text position and text colour specified.
 * \param   int row : Cursor position
 * \param   int column : Cursor position
 * \param	rgb_t colour : the colour of the text
 * \param   const char* format : Formatted string
 * \return  int retVal : always 0
 */
int LCDSetPrintfFont(int row, int column, EyeFont eyeFont, const char *format, ...)
{
	char* text;
	va_list args;
	va_start(args, format);
	LCDSetFont(eyeFont.fontName);
	int temp_cur_pos_x = gLCDHandle->startCurPosX+column*gLCDHandle->fontWidth;
	int temp_cur_pos_y = gLCDHandle->startCurPosY+row*gLCDHandle->fontHeight;
	vasprintf(&text, format, args);
	char* token;
	char* copy = strdup(text);
	int textWidth;
	int textLen = strlen(text);
	XSetForeground(gLCDHandle->d, gLCDHandle->gc, eyeFont.fontColour.pixel);
	XSetBackground(gLCDHandle->d, gLCDHandle->gc, gLCDHandle->bgTextColor.pixel);
	while ((token = strsep(&copy, "\n")) != NULL)
	{
		textWidth = XTextWidth(gLCDHandle->fontstruct, token, strlen(token));
		if (temp_cur_pos_x+textWidth > gLCDHandle->width)
		{
			temp_cur_pos_x = gLCDHandle->startCurPosX;
			temp_cur_pos_y += gLCDHandle->fontHeight;
		}
		XDrawImageString(gLCDHandle->d, gLCDHandle->w, gLCDHandle->gc, temp_cur_pos_x, temp_cur_pos_y, token, strlen(token));
		textLen -= strlen(token);
		if (textLen != 0)
		{
			temp_cur_pos_x = gLCDHandle->startCurPosX;
			temp_cur_pos_y += gLCDHandle->fontHeight;
			textLen -= 1;
		}
		else
		{
			temp_cur_pos_x += textWidth;
		}
	}
	gCurPosX = temp_cur_pos_x;
	gCurPosY = temp_cur_pos_y;
	free(text);
	va_end(args);
	return 0;
}

/**
 * \brief   Write a character to LCD and refresh LCD. Cursor position is updated.
 * \param   char c : Character to be displayed
 * \return  int retVal : always 0
 */
int LCDPutChar(char c)
{
	LCDPrintf("%c", c);
	return 0;
}

/**
 * \brief   LCDPutChar with text position specified.
 * \param   int row : Cursor position
 * \param   int column : Cursor position
 * \param   char c : Character to be displayed
 * \return  int retVal : always 0
 */
int LCDSetChar(int row, int column, char c)
{
	LCDSetPrintf(row, column, "%c", c);
	return 0;
}

/**
 * \brief   Print string to LCD and refresh LCD. Cursor position is updated.
 * \param   char* string : String to be displayed
 * \return  int retVal : always 0
 */
/*int LCDPutString(char *string)
{
	LCDPrintf("%s", string);
	return 0;
}*/

/**
 * \brief   LCDPutString with text position specified.
 * \param   int row : Cursor position
 * \param   int column : Cursor position
 * \param   char* c : String to be displayed
 * \return  int retVal : always 0
 */
/*int LCDSetString(int row, int column, char *string)
{
	LCDSetPrintf(row, column, "%s", string);
	return 0;
}*/

/**
 * \brief   LCDPutString with text position and colour specified.
 * \param   int row : Cursor position
 * \param   int column : Cursor position
 * \param   char* c : String to be displayed
 * \param 	rgb_t colour : the colour of the text
 * \return  int retVal : always 0
 */
/*int LCDSetStringFont(int row, int column, char *string, EyeFont eyeFont)
{
	LCDSetPrintfFont(row, column, eyeFont, "%s", string);
	return 0;
}*/

/**
 * \brief   Print hexadecimal number to LCD and refresh LCD. Cursor position is updated. Utilize LCDPrintf for conversion.
 * \param   int val : Hex number to be displayed
 * \return  int retVal : always 0
 */
int LCDPutHex(int val)
{
	LCDPrintf("%X", val);
	return 0;
}

/**
 * \brief   Print hexadecimal number to LCD and refresh LCD. Cursor position is updated. Utilize LCDPrintf for conversion.
 * \param   int val : Hex number to be displayed
 * \return  int retVal : always 0
 */
int LCDPutHex1(int val)
{
	LCDPrintf("%X", val);
	return 0;
}

/**
 * \brief   Print integer to LCD and refresh LCD. Cursor position is updated.
 * \param   int val : Integer to be displayed
 * \return  int retVal : always 0
 */
int LCDPutInt(int val)
{
	LCDPrintf("%i", val);
	return 0;
}

/**
 * \brief   Print integer to LCD and refresh LCD. Cursor position is updated. Text space usage can be specified (formatting).
 * \param   int val : Integer to be displayed
 * \param   int spaces : Text space for the integer
 * \return  int retVal : always 0
 */
int LCDPutIntS(int val, int spaces)
{
	LCDPrintf("%*i", spaces, val);
	return 0;
}

/**
 * \brief   Print floating-point value to LCD and refresh LCD. Cursor position is updated.
 * \param   int val : Floating-point value to be displayed
 * \return  int retVal : always 0
 */
int LCDPutFloat(float val)
{
	LCDPrintf("%f", val);
	return 0;
}

/**
 * \brief   Print floating-point value to LCD and refresh LCD. Cursor position is updated. Text space usage can be specified (formatting).
 * \param   int val : Floating-point value to be displayed
 * \param   int spaces : Text space for the integer
 * \param   int decimals : Number of decimal points to display
 * \return  int retVal : always 0
 */
int LCDPutFloatS(float val, int spaces, int decimals)
{
	LCDPrintf("%*.*f", spaces, decimals, val);
	return 0;
}

/**
 * \brief   Set the text cursor position to (row, column).
 * \param   int row : Text cursor row index
 * \param   int column : Text cursor column index
 * \return  int retVal : always 0
 */
int LCDSetPos(int row, int column)
{
	gCurPosX = gLCDHandle->startCurPosX+column*gLCDHandle->fontWidth;
	gCurPosY = gLCDHandle->startCurPosY+row*gLCDHandle->fontHeight;
	return 0;
}

/**
 * \brief   Get the text cursor position.
 * \param   int* row : Pointer to cursor row index
 * \param   int* column : Pointer to cursor column index
 * \return  int retVal : always 0
 */
int LCDGetPos(int *row, int *column)
{
	*column = (gCurPosX-gLCDHandle->startCurPosX)/gLCDHandle->fontWidth;
	*row = (gCurPosY-gLCDHandle->startCurPosY)/gLCDHandle->fontHeight;
	return 0;
}

/**
 * \brief   Sets the color of the pixel at (x,y) coordinate to color.
 * \param   int x : X-coordinate of the pixel
 * \param   int y : Y-coordinate of the pixel
 * \param   rgb_t color : RGB color value for the pixel
 * \return  int retVal : always 0
 */
int LCDSetPixel(int x, int y, rgb_t color)
{
	XSetForeground(gLCDHandle->d, gLCDHandle->gc, color.pixel);
	XDrawPoint(gLCDHandle->d, gLCDHandle->w, gLCDHandle->gc, x, y);
	return 0;
}

/**
 * \brief   Get the RGB color value of the pixel at (x,y) coordinate.
 * \param   int x : X-coordinate of the pixel
 * \param   int y : Y-coordinate of the pixel
 * \return  rgb_t color : RGB color value
 */
rgb_t LCDGetPixel(int x, int y)
{
  XImage *image;
  XColor color;
  XErrorHandler old_handler;
  old_handler = XSetErrorHandler(ApplicationErrorHandler);
  image = XGetImage(gLCDHandle->d, gLCDHandle->w, x, y, 1, 1, AllPlanes, XYPixmap);
  if (gLCDHandle->X11Error == True)
  {
  	color = LCD_BLACK;
  	gLCDHandle->X11Error = False;
  }
  else
  {
  	color.pixel = XGetPixel(image, 0, 0);
  }
  XSetErrorHandler(old_handler);
  XFree(image);
  XQueryColor(gLCDHandle->d, gLCDHandle->colormap, &color);
  return color;
}

/**
 * \brief   Invert a RGB color.
 * \param   rgb_t color : RGB color value
 * \return  rgb_t color : RGB color value
 */
rgb_t InvertColor(rgb_t color)
{
	XColor colorInverted;
	colorInverted.red = 0xFFFF - color.red;
	colorInverted.green = 0xFFFF - color.green;
	colorInverted.blue = 0xFFFF -color.blue;
	XAllocColor(gLCDHandle->d, gLCDHandle->colormap, &colorInverted);
	return colorInverted;
}

/**
 * \brief   Bit-invert the color of the pixel at (x,y) coordinate.
 * \param   int x : X-coordinate of the pixel
 * \param   int y : Y-coordinate of the pixel
 * \return  int retVal : always 0
 */
int LCDInvertPixel(int x, int y)
{
	LCDSetPixel(x, y, InvertColor(LCDGetPixel(x, y)));
	return 0;
}

/**
 * \brief   Draw a color line from (x1,y1) to (x2,y2).
 * \param   int x1 : X-coordinate of first pixel
 * \param   int y1 : Y-coordinate of first pixel
 * \param   int x2 : X-coordinate of second pixel
 * \param   int y2 : Y-coordinate of second pixel
 * \param   rgb_t color :  RGB color value for the pixel
 * \return  int retVal : always 0
 */
int LCDLine(int x1, int y1, int x2, int y2, rgb_t color)
{
	XSetForeground(gLCDHandle->d, gLCDHandle->gc, color.pixel);
	XDrawLine(gLCDHandle->d, gLCDHandle->w, gLCDHandle->gc, x1, y1, x2, y2);
	return 0;
}

/**
 * \brief   Draw a line from (x1,y1) to (x2,y2). The line pixels  will invert the color of existing pixels.
 * \param   int x1 : X-coordinate of first pixel
 * \param   int y1 : Y-coordinate of first pixel
 * \param   int x2 : X-coordinate of second pixel
 * \param   int y2 : Y-coordinate of second pixel
 * \return  int retVal : always 0
 */
int LCDLineInvert(int x1, int y1, int x2, int y2)
{
	int x;
	float y;
	for (x = x1; x < x2; x++)
	{
		y = ((float)y1)+((float)(y2-y1))*((float)(x-x1))/((float)(x2-x1));
		LCDInvertPixel(x, (int)y);
	}
	return 0;
}

/**
 * \brief   Draw a rectangle with (x1,y1) as top-left coordinate and (x2,y2) as the bottom-right coordinate.
 *          The pixels in the specified area will invert the color of existing pixels.
 * \param   int x1 : X-coordinate of top-left pixel
 * \param   int y1 : Y-coordinate of top-left pixel
 * \param   int x2 : X-coordinate of bottom-right pixel
 * \param   int y2 : Y-coordinate of bottom-right pixel
 * \return  int retVal : always 0
 */
int LCDAreaInvert(int x1, int y1, int x2, int y2)
{
	int x, y;
	for (x = x1; x < x2; x++)
	{
		for (y = y1; y < y2; y++)
		{
			LCDInvertPixel(x, y);
		}
	}
	return 0;
}

/**
 * \brief   Draw a textbar for text starting at position (row, column) until (row, column+length).
 *          The textbar will take about 25%-50% of text height & width to draw its frame.
 *          The fill parameter will define how much of the text bar should be 'filled' with color (like a progress bar).
 * \param   int row : Start text cursor position
 * \param   int column : Start text cursor position
 * \param   int length : Text length of the bar
 * \param   int fill : Percentage of textbar to be filled
 * \param   rgb_t color : Fill color for the textbar
 * \return  rect_t rect : rect_t structure for the textbar's frame
 */
rect_t LCDTextBar(int row, int column, int length, int fill, rgb_t color)
{
	int old_cur_posX, old_cur_posY;
	old_cur_posX = gCurPosX;
	old_cur_posY = gCurPosY;
	rect_t frame;
	frame.x = gLCDHandle->startCurPosX+column*gLCDHandle->fontWidth;
	frame.y = gLCDHandle->startCurPosY+(row-1.15)*gLCDHandle->fontHeight;
	frame.height = 1.5*gLCDHandle->fontHeight;
	frame.width = length*gLCDHandle->fontWidth;
	LCDFrame(frame.x, frame.y, frame.x+frame.width, frame.y+frame.height, color);
	LCDArea(frame.x, frame.y, frame.x+frame.width*fill/100.0, frame.y+frame.height, color);
	rgb_t backupFgColor = gLCDHandle->fgTextColor;
	rgb_t backupBgColor = gLCDHandle->bgTextColor;
	char colorflag = gLCDHandle->colorflag;
	LCDTextColor(InvertColor(color), color, LCD_BGCOL_TRANSPARENT);
	LCDSetPrintf(row, column+length-5, "%3d%%", fill);
	LCDTextColor(backupFgColor, backupBgColor, colorflag);
	gCurPosX = old_cur_posX;
	gCurPosY = old_cur_posY;
	return frame;
}

/**
 * \brief   Indicate if the LCD need to be refreshed
 * \return  int retVal : non-null value indicate that the LCD need to be refreshed
 */
int LCDNeedRefresh(void)
{
	if (gLCDEnabled)
	{
		XEvent e;
		XNextEvent(gLCDHandle->d, &e);
		switch (e.type)
		{
			case ButtonPress:
				gMousePosX = e.xbutton.x;
				gMousePosY = e.xbutton.y;
				gMouseButton = e.xbutton.button;
				break;
			case Expose:
				gCurPosX = gLCDHandle->startCurPosX;
				gCurPosY = gLCDHandle->startCurPosY;
				gMouseButton = -1;
				LCDDrawMenu();
				return 1;
				break;
			case ClientMessage:
				LCDRelease(*gLCDHandle);
				exit(0);
				break;
		}
	}
	return 0;
}

/**
 * \brief   Release the LCD
 * \return  int retVal : always 0
 */
int LCDRelease()
{
	if (gLCDEnabled)
	{
		XFreeColormap(gLCDHandle->d, gLCDHandle->colormap);
		XDestroyWindow(gLCDHandle->d, gLCDHandle->w);
		/* Close the connexion with the server */
		XCloseDisplay(gLCDHandle->d);
		gLCDEnabled = false;
		free(gLCDHandle);
		gLCDHandle = NULL;
	}
	return 0;
}

/**
 * \brief			Refresh the screen (i.e write display buffers to the framebuffer device). Refreshes the menu
 * \return    int retVal : always 0
 */
int LCDRefresh(void)
{
	LCDDrawMenu();
	LCDDrawList();
	XEvent e;
	while (XPending(gLCDHandle->d))
	{
		LCDNeedRefresh();
	}
	return 0;
}

/**
 * \brief			Refresh the screen (i.e write display buffers to the framebuffer device). Does not refresh the menu
 * \return    int retVal : always 0
 */
int LCDRefresh2(void)
{
	LCDDrawList();
	XEvent e;
	while (XPending(gLCDHandle->d))
	{
		LCDNeedRefresh();
	}
	return 0;
}

/**
 * \brief			Refresh the screen (i.e write display buffers to the framebuffer device). Does not refresh the menu or the listmenu
 * \return    int retVal : always 0
 */
int LCDRefresh3(void)
{
	XEvent e;
	while (XPending(gLCDHandle->d))
	{
		LCDNeedRefresh();
	}
	return 0;
}

/**
 * \brief			Get display information and save to structure pointed by pinfo.
 * Cursor info needs LCDInit() for textsize.
 * \param     fbinfo_t* pinfo : Pointer to storage for screen & cursor info
 * \return    int retVal\n
 *            0 on success\n
 *            Negative value on failure
 */
int LCDGetFBInfo(fbinfo_t* pinfo)
{
	pinfo->screen.xres = gLCDHandle->width;
	pinfo->screen.yres = gLCDHandle->height;
	pinfo->screen.bpp = 0; //TODO: correct the value
	pinfo->cursor.x = gCurPosX;
	pinfo->cursor.xmax = (gLCDHandle->width-gLCDHandle->startCurPosX)/gLCDHandle->fontWidth-1;
	pinfo->cursor.y = gCurPosY;
	pinfo->cursor.ymax = (gLCDHandle->height-gLCDHandle->startCurPosY)/gLCDHandle->fontHeight-1;
	return 0;
}


/**
 * \brief			Get the number of list items supported by the current display (text) configuration.
 * This includes the item for title bar - thus, different from count variable in listmenu_t as updated by an LCDList() call.
 * \return    int listCount : Number of list items (including title box)
 */
int LCDListCount(void)
{
	return LCD_MAX_LIST_ITEM;
}

/**
 * \brief	  		Set the list index.
 * \param     int index : List index
 * \return    int retVal : List index
 */
int LCDListIndex(int index)
{
	gLCDHandle->listMenu->index = index;
	return 0;
}

/**
 * \brief			Scrolls the list display up.
 * Menu index is not altered.
 * If the active menu item goes out of focus, the index becomes negative (no item selected).
 * \return    int retVal : always 0
 */
int LCDListScrollUp(void)
{
	if (gLCDHandle->listMenu->scroll == 0)
	{
		return 0;
	}
	if (gLCDHandle->listMenu->start > 0)
	{
		gLCDHandle->listMenu->start--;
	}
	return 0;
}

/**
 * \brief     Scrolls the list display down.
 * Menu index is not altered.
 * If the active menu item goes out of focus, the index becomes negative (no item selected).
 * \return    int retVal : always 0
 */
int LCDListScrollDown(void)
{
	if (gLCDHandle->listMenu->scroll == 0)
	{
		return 0;
	}
	if (gLCDHandle->listMenu->start < gLCDHandle->listMenu->size-LCD_MAX_LIST_ITEM+2)
	{
		gLCDHandle->listMenu->start++;
	}
	return 0;
}

/**
 * \brief		Place a RGB color image (24bpp) at (xpos,ypos) position on the LCD screen.
 * \param		int xpos : X-coordinate of top-left image position
 * \param   int ypos : Y-coordinate of top-left image position
 * \param   int xsize : Image width
 * \param   int ysize : Image height
 * \param   byte_t* data : Pointer to image data (24-bit per pixel)
 * \return  int retVal : always 0
 */
int LCDPutImageRGB(int xpos, int ypos, int xsize, int ysize, byte_t *data)
{
	XClearWindow(gLCDHandle->d,gLCDHandle->w);
	Imlib_Image img;
	//fprintf(stderr,"	XCreateImage\n");
	static Pixmap pix;
	static int run = 0;
	//Pixmap pix = XCreatePixmap(gLCDHandle->d,DefaultRootWindow(gLCDHandle->d),xsize,ysize,DefaultDepthOfScreen(DefaultScreenOfDisplay(gLCDHandle->d)));
	//XImage *image = XCreateImage(gLCDHandle->d,DefaultVisual(gLCDHandle->d,0),DefaultDepth(gLCDHandle->d,DefaultScreen(gLCDHandle->d)),pix,0,data,640,480,8,0);
	//XImage *image = XCreateImage(gLCDHandle->d,DefaultVisual(gLCDHandle->d,0),DefaultDepth(gLCDHandle->d,DefaultScreen(gLCDHandle->d)),ZPixmap,0,data,320,240,8,0);
	//XImage *image = XCreateImage(gLCDHandle->d,DefaultVisual(gLCDHandle->d,0),1,ZPixmap,0,data,640,480,8,0);
	img = imlib_load_image("temp.jpeg");
	if (!img)
	{
		fprintf(stderr,"	ERROR: unable to read image data\n");
		return -1;
	}
	imlib_context_set_image(img);
	if (run == 0)
	{
		pix = XCreatePixmap(gLCDHandle->d,gLCDHandle->w,xsize,ysize,DefaultDepthOfScreen(gLCDHandle->scr));
		imlib_context_set_display(gLCDHandle->d);
		imlib_context_set_visual(DefaultVisualOfScreen(gLCDHandle->scr));
		imlib_context_set_colormap(DefaultColormapOfScreen(gLCDHandle->scr));
		imlib_context_set_drawable(pix);
		run = 1;
	}
	//fprintf(stderr,"	Done\n	XPutImage\n");
	//XPutImage(gLCDHandle->d,gLCDHandle->w,gLCDHandle->gc,image,0,0,xpos,ypos,xsize,ysize);
	imlib_render_image_on_drawable(0, 0);
	XSetWindowBackgroundPixmap(gLCDHandle->d,gLCDHandle->w, pix);
    //XClearWindow(gLCDHandle->d,gLCDHandle->w);
	//fprintf(stderr,"	Done\n");
	imlib_free_image();
	//XFreePixmap(gLCDHandle->d,pix);
	return 0;
}

/**
 * \brief		Place a RGB color image (24bpp) at (xpos,ypos) position on the LCD screen.
 * \param		int xpos : X-coordinate of top-left image position
 * \param   int ypos : Y-coordinate of top-left image position
 * \param   int xsize : Image width
 * \param   int ysize : Image height
 * \param   byte_t* data : Pointer to image data (24-bit per pixel)
 * \return  int retVal : always 0
 */
int LCDPutImageGrey(int xpos, int ypos, int xsize, int ysize, byte_t *data)
{
	//TODO: Complete
	return 0;
}
