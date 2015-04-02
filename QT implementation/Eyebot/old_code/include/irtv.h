/*
 * irtv.h
 *
 *  Created on: 19 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		irtv.h
 * \brief   Header file for the IRTV functions
 * \author  Remi KEAT
 */

#ifndef IRTV_H_
#define IRTV_H_

#include "spi.h"
#include <lirc/lirc_client.h>

//IR remote control button defines
//			Define name			char			config file name
#define		IR_KEY_1			'1'				//KEY_1
#define		IR_KEY_2			'2'				//KEY_2
#define		IR_KEY_3			'3'				//KEY_3
#define		IR_KEY_4			'4'				//KEY_4
#define		IR_KEY_5			'5'				//KEY_5
#define		IR_KEY_6			'6'				//KEY_6
#define		IR_KEY_7			'7'				//KEY_7
#define		IR_KEY_8			'8'				//KEY_8
#define		IR_KEY_9			'9'				//KEY_9
#define		IR_KEY_0			'0'				//KEY_0
#define		IR_KEY_RED			'R'				//KEY_RED
#define		IR_KEY_GREEN		'G'				//KEY_GREEN
#define		IR_KEY_YELLOW		'Y'				//KEY_YELLOW
#define		IR_KEY_BLUE			'B'				//KEY_BLUE
#define		IR_KEY_LEFT			'L'				//KEY_LEFT
#define		IR_KEY_RIGHT		'>'				//KEY_RIGHT
#define		IR_KEY_UP			'U'				//KEY_UP
#define		IR_KEY_DOWN			'D'				//KEY_DOWN
#define		IR_KEY_OK			'O'				//KEY_OK
#define		IR_KEY_POWER		'P'				//KEY_POWER

int IRTVInit(DeviceSemantics semantics);
int IRTVGet(void);
void IRTVRelease(void);
int IRTVGetStatus(void);
DeviceSemantics IRTVGetControllerCode();
void IRTVSetControllerCode(DeviceSemantics semantics);
void IRTVSetHoldDelay(int delay);

#endif /* IRTV_H_ */
