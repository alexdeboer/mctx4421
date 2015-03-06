/*
 * types.h
 *
 *  Created on: 31 Jul 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		types.h
 * \brief		Defines types
 * \author	Remi KEAT
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <X11/Xlib.h>
#include <mpsse.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <mpsse.h>
#include "params.h"
#include "WinTypes.h"

typedef char* DeviceSemantics;
typedef XColor rgb_t;
typedef unsigned short hword_t;
typedef struct mpsse_context* SPIHandle;
typedef unsigned long keycode_t;
typedef unsigned char keymode_t;
typedef unsigned short lcdmode_t;
typedef unsigned char byte_t;

typedef float meterPerSec;
typedef float radPerSec;
typedef float meter;
typedef float radians;
typedef struct
{
	meter x;
	meter y;
	radians phi;
} PositionType;


typedef struct
{
	unsigned long flags;
	unsigned long functions;
	unsigned long decorations;
	long inputMode;
	unsigned long status;
} Hints;

typedef struct
{
	int xres, yres;
	int bpp;
} screen_t;

typedef struct
{
	int x, y;
	int xmax, ymax;
} cursor_t;

typedef struct
{
	screen_t screen;
	cursor_t cursor; /* x & y holds font width & height! */
} fbinfo_t;

typedef struct
{
	char name[40];
	char mhz[20];
	char arch[20];
	char bogomips[20];
} info_cpu_t;

typedef struct
{
	char procnum[20];
	char total[40];
	char free[40];
} info_mem_t;

typedef struct
{
	char num[20];
} info_proc_t;

typedef struct
{
	char uptime[20];
	char vbatt[20];
	int vbatt_8;
} info_misc_t;

/**
 * \brief Structure representing the coordinates of a point
 */
typedef struct
{
	int x, y;
} coord_pair_t;

typedef struct
{
	int active;
	coord_pair_t tl;       //top left
	coord_pair_t br;       //bottom right
} m6key_box_t;

typedef struct
{
	keymode_t mode;
	m6key_box_t rect[KEYTM_MAX_REGIONS];
} touch_map_t;

typedef struct
{
	coord_pair_t point, value;
	//struct timeval instant;
	int sync, status;
} touch_event_t;

typedef struct
{
	char label[LCD_MENU_STRLENGTH];
	rgb_t fgcol, bgcol;
	void *plink; /* link to user data/function! */
} menuitem_t;

typedef struct
{
	char title[LCD_LIST_STRLENGTH];
	rgb_t fgcol, bgcol;
	int size, start, width, left, scroll; /* configure these! */
	int index/*, count*/; /* the library will set & manage these! */
	menuitem_t *pitems; /* pointer to array of 'size' menuitems */
	int no_empty;
} listmenu_t;

/**
 * \brief Structure defining an LCD
 */
typedef struct
{
	int lcdNum;
	Display* d;
	int s;
	Window w;
	Colormap colormap;
	GC gc;
	XFontStruct* fontstruct;
	int fontHeight;
	int fontWidth;
	int height;
	int width;
	int startCurPosX;
	int startCurPosY;
	rgb_t fgTextColor;
	rgb_t bgTextColor;
	char colorflag;
	hword_t mode;
	menuitem_t menuItems[4];
	listmenu_t* listMenu;
	int fd;
	bool X11Error;
	Screen *scr;
} LCDHandle;

/**
 * \brief Structure representing a rectangle
 */
typedef struct
{
	int x, y;
	int height, width;
} rect_t;

/**
 * \brief Structure representing a menu rectangle
 */
typedef rect_t menurect_t;

typedef unsigned int SERVOHandle;
typedef unsigned int MOTORHandle;
typedef unsigned int QUADHandle;
typedef unsigned int PSDHandle;
typedef unsigned int ADCHandle;
typedef unsigned int VWHandle;
typedef unsigned int CAMHandle;

/* an hdt text entry - one line! */
typedef struct _HDTEntry_t
{
	int length;
	char *buffer;
}
HDTEntry_t;

/* assume table of integers only! */
typedef struct _HDTTable_t
{
	struct _HDTTable_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	int size;
	int *data;
}
HDTTable_t;

/* a generic device structure - only valid for dev with tabname! */
typedef struct _HDTDevice_t
{
	struct _HDTDevice_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	char tabname[HDT_MAX_NAMECHAR];
	HDTTable_t *ptable; /* pointer to a table */
}
HDTDevice_t;

typedef struct _HDTDevCAM_t
{
	struct _HDTDevCAM_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	int regaddr, ucb1400io, width, height;
}
HDTDevCAM_t;

typedef struct _HDTDevMOTOR_t
{
	struct _HDTDevMOTOR_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	char tabname[HDT_MAX_NAMECHAR];
	HDTTable_t *ptable; /* pointer to a table */
	int regaddr, freq;
}
HDTDevMOTOR_t;

typedef struct _HDTDevENCODER_t
{
	struct _HDTDevENCODER_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	char motorname[HDT_MAX_NAMECHAR];
	HDTDevMOTOR_t *pmotor;
	int regaddr, clickspm;
	int maxspeed;
}
HDTDevENCODER_t;

typedef struct _HDTDevSERVO_t
{
	struct _HDTDevSERVO_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	int regaddr, freq;
	int mintime, maxtime;
}
HDTDevSERVO_t;

typedef struct _HDTDevPSD_t
{
	struct _HDTDevPSD_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	char tabname[HDT_MAX_NAMECHAR];
	HDTTable_t *ptable; /* pointer to a table */
	int regaddr;
}
HDTDevPSD_t;

typedef struct _HDTDevDRIVE_t
{
	struct _HDTDevDRIVE_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	char encname1[HDT_MAX_NAMECHAR],encname2[HDT_MAX_NAMECHAR];
	char encname3[HDT_MAX_NAMECHAR],encname4[HDT_MAX_NAMECHAR];
	HDTDevENCODER_t *penc1, *penc2, *penc3, *penc4;
	int drivetype;
	int wheeldist1, axesdist, wheeldist2;
}
HDTDevDRIVE_t;

typedef struct _HDTDevIRTV_t
{
	struct _HDTDevIRTV_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	int type, length, togmask, invmask, mode, buffsize, delay;
}
HDTDevIRTV_t;

typedef struct _HDTDevADC_t
{
	struct _HDTDevADC_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	char procname[HDT_MAX_NAMECHAR];
	int denom;
}
HDTDevADC_t;

typedef struct _HDTDevCOM_t
{
	struct _HDTDevCOM_t *pnext;
	char name[HDT_MAX_NAMECHAR];
	char devname[HDT_MAX_NAMECHAR];
}
HDTDevCOM_t;

#define FONTNAMELEN 50

typedef struct 
{
	rgb_t fontColour;
	char fontName[FONTNAMELEN];
} EyeFont;
#endif /* TYPES_H_ */
