/*
 * params.h
 *
 *  Created on: 7 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		params.h
 * \author  Remi KEAT
 * \brief   Defines main parameters
 */

#ifndef PARAMS_H_
#define PARAMS_H_

#define VEHICLE 1
#define PLATFORM 2
#define WALKER 3

#define DEBUG 1

#define NUMBER_TRY 10
#define HDT_MAX_NAMECHAR 80
#define LCD_MENU_STRLENGTH 32 /* for storage declaration */
#define LCD_LIST_STRLENGTH 64 /* for storage declaration */
#define MENU_HEIGHT 38
#define KEYTM_MAX_REGIONS 32
#define VERSION "1.0"
#define MACHINE_SPEED 700000000
#define MACHINE_TYPE VEHICLE
#define MACHINE_NAME "EyeBot"
#define ID 1
#define CPU_ARCH "ARM"
#define CPU_BOGOMIPS "?"
#define CPU_MHZ "700"
#define CPU_NAME "AM37x 1GHz ARM Cortex-A8 compatible"
#define LIBM6OS_VERSION "1.0"
#define LCD_TYPE BEAGLETOUCH
#define LCD_HEIGHT 240
#define LCD_WIDTH 320
#define LCD_MAX_LIST_ITEM 6
#define HDT_FILE "hdt.txt"

#define HDT_MAX_PATHCHAR 256
#define HDT_MAX_FILECHAR 40
#define HDT_MAX_READBUFF 128

#endif /* PARAMS_H_ */
