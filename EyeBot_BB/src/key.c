/*
 * key.c
 *
 *  Created on: 7 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		key.c
 * \author	Remi KEAT
 * \brief		Defines functions for the key input
 */

//TODO: changes this include: ideally we don't want to include everything to do with eyebot.h
#include <eyebot.h>	//need to include eyebot.h so that LCD_MAX_LIST_ITEM calculates correctly

static keycode_t key_esc = KEY_INVALID;

/**
 * \brief   Open the evdev device file for reading touch events.
 * Load the key configuration file (if found), else use the hardcoded default value.
 * \return  int retVal : 0 on success\n
 * Negative value on failure
 */
int KEYInit(void)
{
	gTouchMap = malloc(sizeof(touch_map_t));
	KEYSetTM(KEYTM_CLASSIC);
	return 0;
}

/**
 * \brief   Set mode for key touch map.
 * \param   keymode_t mode : Requested touch map mode
 * \return  keymode_t retMod : Current touch map mode
 */
keymode_t KEYSetTM(keymode_t mode)
{
	int i;
	int buttonWidth = gLCDHandle->width/4;
	gTouchMap->mode = mode;
	int menuHeight = gLCDHandle->fontHeight*2;
	//TODO: Put other modes
	int offset = 0;
	if (gLCDHandle->listMenu)
	{
		if (gLCDHandle->listMenu->scroll != 0)
		{
			offset = 1;
		}
	}
	for (i = 0; i < KEYTM_MAX_REGIONS; i++)
	{
		gTouchMap->rect[i].tl.x = 0;
		gTouchMap->rect[i].tl.y = 0;
		gTouchMap->rect[i].br.x = 0;
		gTouchMap->rect[i].br.x = 0;
	}
	switch (mode)
	{
		case KEYTM_LISTMENU:
			for (i = 4; i < LCD_MAX_LIST_ITEM+3-offset; i++)
			{
				gTouchMap->rect[i].tl.x = 0;
				gTouchMap->rect[i].tl.y = (i+offset-4+1)*menuHeight;
				gTouchMap->rect[i].br.x = gLCDHandle->width;
				gTouchMap->rect[i].br.y = (i+offset-4+2)*menuHeight;
			}
			break;
		default:
			break;
	}
	for (i = 0; i < 4; i++)
	{
		gTouchMap->rect[i].tl.x = i*buttonWidth;
		gTouchMap->rect[i].tl.y = gLCDHandle->height-MENU_HEIGHT;
		gTouchMap->rect[i].br.x = (i+1)*buttonWidth;
		gTouchMap->rect[i].br.y = gLCDHandle->height;
	}
	if (offset != 0)
	{
		gTouchMap->rect[KEYTM_MAX_REGIONS-2].tl.x = 0;
		gTouchMap->rect[KEYTM_MAX_REGIONS-2].tl.y = 1*menuHeight;
		gTouchMap->rect[KEYTM_MAX_REGIONS-2].br.x = gLCDHandle->width;
		gTouchMap->rect[KEYTM_MAX_REGIONS-2].br.y = 2*menuHeight;
		gTouchMap->rect[KEYTM_MAX_REGIONS-1].tl.x = 0;
		gTouchMap->rect[KEYTM_MAX_REGIONS-1].tl.y = LCD_MAX_LIST_ITEM*menuHeight;
		gTouchMap->rect[KEYTM_MAX_REGIONS-1].br.x = gLCDHandle->width;
		gTouchMap->rect[KEYTM_MAX_REGIONS-1].br.y = (LCD_MAX_LIST_ITEM+1)*menuHeight;
	}
	return mode;
}

/**
 * \brief   Get current mode and touch map (region map).
 * \param   touch_map_t** ptouch_map : Pointer to a touch_map_t structure
 * \return  keymode_t retMod : Current touch map mode
 */
keymode_t KEYGetTM(touch_map_t** ptouch_map)
{
	if (ptouch_map != 0x0)
	{
		*ptouch_map = gTouchMap;
	}
	return gTouchMap->mode;
}

/**
 * \brief   Manually set region data into current touch map.
 * Only used in KEYTM_REGIONS mode. If region is 0x0, resets the touch map
 * (mode becomes KEYTM_UNKNOWN).
 * \param   int index : Index for region
 * \param   m6key_box_t* region : Pointer to a region data
 * \return  int retVal : 0 on success\n
 * Negative value on failure
 */
int KEYSetRegion(int index, m6key_box_t *region)
{
	gTouchMap->rect[index] = *region;
	return 0;
}

/**
 * \brief   Copy specific region data out from the current touch map.
 * Only used in KEYTM_REGIONS mode.
 * \param   int index : Index for region
 * \param   m6key_box_t* region : Pointer to a storage for region data
 * \return  int retVal : 0 on success\n
 * Negative value on failure
 */
int KEYGetRegion(int index, m6key_box_t *region)
{
	*region = gTouchMap->rect[index];
	return 0;
}

/**
 * \brief   Validate there's no touch on screen surface
 * \param   touch_event_t* rawtouch : pointer to touch_event_t structure this is optional!
 * only if raw data needed! else, use 0x0!
 * \return  int retVal :\n
 * 0 - being touched\n
 * 1 - not touched
 */
int KEYNoTouch(touch_event_t* rawtouch)
{
	//TODO: Consider rawtouch
	if (gMouseButton == 1)
	{
		return 0;
	}
	return 1;
}

/**
 * \brief   Gets raw touch info - a non-blocking function.
 * Mainly used for calibration and testing.
 * \param   touch_event_t* rawtouch : pointer to touch_event_t structure
 * \return  int retVal :\n
 * 0 if sync signal received!\n
 * Negative value if otherwise
 */
int KEYGetRAW(touch_event_t* rawtouch)
{
	//TODO: Change this
	rawtouch->point.x = gMousePosX;
	rawtouch->point.y = gMousePosY;
	rawtouch->status = gMouseButton;
	return 0;
}

/**
 * \brief   Decode raw touch info into a keycode based on the current touch map.
 * Mainly used for testing.
 * \param   touch_event_t* rawtouch : pointer to touch_event_t structure
 * \return  keycode_t keyCode : Status of touch data (variable in rawtouch)
 */
keycode_t KEYDecode(touch_event_t* rawtouch)
{
	int k;
	for (k = 0; k < KEYTM_MAX_REGIONS; k++)
	{
		if (inside(rawtouch->point.x, rawtouch->point.y, gTouchMap->rect[k]))
		{
			if (gLCDHandle->listMenu)
			{
				if (gLCDHandle->listMenu->scroll != 0)
				{
					if (k == KEYTM_MAX_REGIONS-2)
					{
						return KEY_LISTUP;
					}
					else if (k == KEYTM_MAX_REGIONS-1)
					{
						return KEY_LISTDN;
					}
				}
			}
			return 0x01<<k;
		}
	}
	return 0;
}

/**
 * \brief   Close the evdev device file and stop checking any key touch event
 * \return  int retVal : 0 on success\n
 * Negative value on failure
 */
int KEYRelease(void)
{
	free(gTouchMap);
	gTouchMap = NULL;
	return 0;
}

/**
 * \brief		Check if a point is inside a rectangle
 * \param		int x : X-coordinate of the point
 * \param   int y : Y-coordinate of the point
 * \param   m6key_box_t rect : rectangle structure
 * \return  int retVal : non-null if inside
 */

int inside(int x, int y, m6key_box_t rect)
{
	if (x >= rect.tl.x && x <= rect.br.x)
	{
		if (y >= rect.tl.y && y <= rect.br.y)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * \brief		Enable/disable event checking procedure
 * \param		int idle : user request
 * \details Valid values for idle:
 * - KEY_GOIDLE - deactivate event checking
 * - KEY_NOIDLE - activate event checking
 * - KEY_STATE - request current status
 *
 * \return  int status : Idle status of event checking procedure
 */
int KEYIdle(int idle)
{
	return 0;
}

/**
 * \brief		Wait for specific keys only.
 * \param		keycode_t excode : Expected keycode values (bit XORed)
 * \return  keycode_t retKey : Keycode value
 */
keycode_t KEYWait(keycode_t excode)
{
	keycode_t readKey = KEYRead();
	while ((readKey & excode) == 0)
	{
		LCDNeedRefresh();
		readKey = KEYRead();
	}
	gMouseButton = 0;
	return readKey;
}

/**
 * \brief		Wait for a touch event and return keycode (including KEY_INVALID - undefined keycode).
 * \return  keycode_t retKey : Keycode value
 */
keycode_t KEYGet(void)
{
	keycode_t readKey = KEYWait(KEY_INVALID);
	return readKey;
}

/**
 * \brief		Wait for a touch event and return the XY-coordinate.
 * \return  coord_pair_t retCoord : Coordinate pair
 */
coord_pair_t KEYGetXY(void)
{
	coord_pair_t point;
	while (gMouseButton == 0)
	{
		LCDNeedRefresh();
	}
	point.x = gMousePosX;
	point.y = gMousePosY;
	return point;
}

/**
 * \brief		Read a keycode and returns. Function does not wait, thus includes KEY_TIMEOUT.
 * \return  keycode_t retKey : Keycode value
 */
keycode_t KEYRead(void)
{
	int k;
	if (gMouseButton == 1)
	{
		touch_event_t key;
		KEYGetRAW(&key);
		gMouseButton = -1;
		return KEYDecode(&key);
	}
	else if (gMouseButton == -1)
	{
		return KEY_INVALID;
	}
	return 0;
}

int activate_escape(int escape)
{
	if(escape<0)
		return key_esc==KEY_ESCAPE?1:0;
	if(escape)
		key_esc = KEY_ESCAPE;
	else
		key_esc = KEY_INVALID;
	return key_esc==KEY_ESCAPE?1:0;
}
