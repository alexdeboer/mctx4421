/*
 * lcd.h
 *
 *  Created on: 8 Jul 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file    lcd.h
 * \brief   Header file for the LCD functions
 * \author  Remi KEAT
 */

#ifndef LCD_H_
#define LCD_H_

//#include "hdt.h"
#include "globals.h"
#include <stdarg.h>
//#include <string.h>
#include "key.h"
#include "listmenu.h"

#define LCD_WHITE getColor("white")
#define LCD_SILVER getColor("light gray")
#define LCD_LIGHTGRAY getColor("light gray")
#define LCD_LIGHTGREY getColor("light grey")
#define LCD_GRAY getColor("gray")
#define LCD_DARKGRAY getColor("dark gray")
#define LCD_DARKGREY getColor("dark grey")
#define LCD_BLACK getColor("black")

#define LCD_BLUE getColor("blue")
#define LCD_NAVY getColor("navy")
#define LCD_AQUA getColor("aquamarine")
#define LCD_CYAN getColor("cyan")
#define LCD_TEAL getColor("dark cyan")
#define LCD_FUCHSIA getColor("magenta")
#define LCD_MAGENTA getColor("magenta")
#define LCD_PURPLE getColor("purple")

#define LCD_RED getColor("red")
#define LCD_MAROON getColor("maroon")
#define LCD_YELLOW getColor("yellow")
#define LCD_OLIVE getColor("dark olive green")
#define LCD_LIME getColor("lime green")
#define LCD_GREEN getColor("green")

/* constants for text colorflags */
#define LCD_BGCOL_TRANSPARENT         0x01
#define LCD_BGCOL_NOTRANSPARENT       0x10

#define LCD_BGCOL_INVERSE             0x02
#define LCD_BGCOL_NOINVERSE           0x20

#define LCD_FGCOL_INVERSE             0x04
#define LCD_FGCOL_NOINVERSE           0x40

/* constants for lcd modes */
#define LCD_AUTOREFRESH               0x0001
#define LCD_NOAUTOREFRESH             0x0100

#define LCD_SCROLLING                 0x0002
#define LCD_NOSCROLLING               0x0200

#define LCD_LINEFEED                  0x0004
#define LCD_NOLINEFEED                0x0400

#define LCD_SHOWMENU                  0x0008
#define LCD_HIDEMENU                  0x0800

#define LCD_LISTMENU				  0x0010
#define LCD_CLASSICMENU				  0x1000

#define LCD_FB_ROTATE                 0x0080
#define LCD_FB_NOROTATION             0x8000

/*
 * define fonts using their alias
 * font aliases can be found in the following file:
 * 	/etc/X11/fonts/misc/xfonts-base.alias
 */
#define FONTDEFAULT		"6x13"
#define FONTBOLD		"6x13bold"

int LCDInit();
int LCDClear(void);
int LCDSetMode(hword_t mode);
hword_t LCDGetMode(void);
int LCDResetMode(hword_t mode);
int LCDMenu(char *string1, char *string2, char *string3, char *string4);
int LCDMenuI(int pos, char *string, rgb_t fgcol, rgb_t bgcol, void* userp);
menuitem_t* LCDMenuItem(int index);
int LCDList(listmenu_t *menulist);
int LCDSetList(listmenu_t* menulist);
listmenu_t* LCDGetList(void);
menurect_t* LCDListBox(int pos);
menuitem_t* LCDListActiveItem(void);
rgb_t getColor(char* colorName);
rgb_t InvertColor(rgb_t color);
int LCDNeedRefresh(void);
int LCDArea(int x1, int y1, int x2, int y2, rgb_t color);
int LCDSetPixel(int x, int y, rgb_t color);
rgb_t LCDGetPixel(int x, int y);
int LCDInvertPixel(int x, int y);
int LCDLine(int x1, int y1, int x2, int y2, rgb_t color);
int LCDLineInvert(int x1, int y1, int x2, int y2);
int LCDAreaInvert(int x1, int y1, int x2, int y2);
int LCDFrame(int x1, int y1, int x2, int y2, rgb_t color);
int LCDTextColor(rgb_t fgcol, rgb_t bgcol, char colorflags);
int LCDPrintf(const char *format, ...);
int LCDPrintfFont(EyeFont eyeFont,const char *format, ...);
int LCDSetPrintf(int row, int column, const char *format, ...);
int LCDSetPrintfFont(int row, int column, EyeFont eyeFont, const char *format, ...);
int LCDPutChar(char c);
int LCDSetChar(int row, int column, char c);
//int LCDPutString(char *string);
//int LCDSetString(int row, int column, char *string);
//int LCDSetStringFont(int row, int column, char *string, EyeFont eyeFont);
int LCDPutHex(int val);
int LCDPutHex1(int val);
int LCDPutInt(int val);
int LCDPutIntS(int val, int spaces);
int LCDPutFloat(float val);
int LCDPutFloatS(float val, int spaces, int decimals);
int LCDSetPos(int row, int column);
int LCDGetPos(int *row, int *column);
rect_t LCDTextBar(int row, int column, int length, int fill, rgb_t color);
int LCDRelease();
int LCDRefresh(void);
int LCDGetFBInfo(fbinfo_t* pinfo);
int LCDListCount(void);
int LCDListIndex(int index);
int LCDListScrollUp(void);
int LCDListScrollDown(void);
int LCDPutImageRGB(int xpos, int ypos, int xsize, int ysize, byte_t *data);
int LCDPutImageGrey(int xpos, int ypos, int xsize, int ysize, byte_t *data);
int LCDSetFont(char *fontAlias);

#endif /* LCD_H_ */
