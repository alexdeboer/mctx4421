/*
 * key.h
 *
 *  Created on: 7 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		key.h
 * \author	Remi KEAT
 * \brief		Header file for the key functions
 */

#ifndef KEY_H_
#define KEY_H_

#include "lcd.h"
#include <stdio.h>

/* pre-defined key constants - usable for standard & region map */
#define KEY1 0x00000001
#define KEY2 0x00000002
#define KEY3 0x00000004
#define KEY4 0x00000008

/* special keys (escape & list menu)! */
#define KEY_ESCAPE 0x80000000
#define KEY_LISTTL 0x40000000
#define KEY_LISTUP 0x20000000
#define KEY_LISTDN 0x10000000

/* special pre-defined key constants for standard list menu! */
#define KEY_LIST1 0x00000010
#define KEY_LIST2 0x00000020
#define KEY_LIST3 0x00000040
#define KEY_LIST4 0x00000080
#define KEY_LIST5 0x00000100
#define KEY_LIST6 0x00000200
#define KEY_LIST7 0x00000400
#define KEY_LIST8 0x00000800

/* key library states - arg for KEYIdle() */
#define KEY_GOIDLE 1
#define KEY_NOIDLE 0
#define KEY_STATE -1

/* key library states - arg for KEYIdle() */
#define KEY_GOIDLE 1
#define KEY_NOIDLE 0
#define KEY_STATE -1

/* key touchmap types/key modes */
#define KEYTM_UNKNOWN  0x00
#define KEYTM_CLASSIC  0x01
#define KEYTM_STANDARD 0x02
#define KEYTM_REGIONS  0x03
#define KEYTM_LISTMENU 0x04

/* reserve 0x0 for timed-out keycode! no key pressed! */
#define KEY_TIMEOUT 0x00000000

/* i assume 32 simultaneous keys is impossible! :p */
#define KEY_INVALID 0xFFFFFFFF

int KEYInit(void);
int KEYRelease(void);
int KEYIdle(int idle);
keymode_t KEYSetTM(keymode_t mode);
keymode_t KEYGetTM(touch_map_t** ptouch_map);
int KEYSetRegion(int index, m6key_box_t *region);
int KEYGetRegion(int index, m6key_box_t *region);
int KEYNoTouch(touch_event_t* rawtouch);
int KEYGetRAW(touch_event_t* rawtouch);
keycode_t KEYDecode(touch_event_t* rawtouch);
keycode_t KEYWait(keycode_t excode);
keycode_t KEYRead(void);
keycode_t KEYGet(void);
coord_pair_t KEYGetXY(void);
int activate_escape(int escape);

#endif /* KEY_H_ */
