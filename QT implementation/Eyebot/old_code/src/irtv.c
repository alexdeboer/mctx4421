/*
 * irtv.c
 *
 *  Created on: 19 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		irtv.c
 * \brief   Defines IRTV functions
 * \author  Remi KEAT
 */

#include <misc.h>
#include "irtv.h"

char *controllerCode;
int running = 0, holdDelay = 5;

/**
 * \brief	Initializes the IR remote control decoder by calling IRTVInit() with the device name found in the corresponding HDT entry.
 * \param   DeviceSemantics semantics, for the IR remote this will be the controller code
 * \return  int retVal :
 * \details
 * - 0 = ok
 * - 1 = HDT file error
 * - 2 = invalid or missing "IRTV" HDT entry for this semantics
 * - 3 = unable to initialise IR input
 */
int IRTVInit(DeviceSemantics semantics)
{
	if (controllerCode == NULL)
	{
		controllerCode = malloc(10);
	}
	char *IRTVFileName = malloc(200);
	char *command = malloc(200);;
	//set the name for the IRTV config file to use
	sprintf(IRTVFileName,"/home/pi/EyeBot_BB/IRConfigs/%s_lircd.conf",semantics);
	//setup the command that will be executed
	sprintf(command,"sudo cp %s /etc/lirc/lircd.conf",IRTVFileName);
	//replace the config that IR input is currently using with the one we want
	system(command);
	/*
	 * need to call the following system command to allow for the IR input
	 * Restart is used to make sure that if the config file is changed the correct
	 * 	config file is used. If it is not restarted then the previous config file
	 * 	is used. Note: if is says that it failed to stop LIRC in most cases it is
	 * 	fine because it may already have been stopped. If you know that it is 
	 * 	currently running and it fails to stop it then something may be wrong.
	 */
	system("sudo /etc/init.d/lirc restart");
	//Initiate LIRC. If it fails then return and indicate the error
    if(lirc_init("lirc",1)==-1)
    {
		fprintf(stderr,"Unable to initialise IR input");
		//stop the IR input because we were unable to initialise the input
		system("sudo /etc/init.d/lirc stop");
		free(IRTVFileName);
		free(command);
		return 3;
	}
	strcpy(controllerCode,semantics);
	free(IRTVFileName);
	free(command);
	running = 1;
	return 0;
}

/**
 * \brief		Waits for IR input and returns the keycode.
 * \return 	int retVal : Next code from the buffer
 * \details 0 = no key
 */
int IRTVGet(void)
{
	int codeIndex = 24;
	char *code;
	char temp[3] = "";
	int retChar;
	int irDelay = 0;
	//exit if the return value is -1 because the LIRC socket is closed
	if (lirc_nextcode(&code)==-1)
	{
		return 0;
	}
	temp[0] = code[17];
	temp[1] = code[18];
	temp[2] = '\0';
	sscanf(temp,"%x",&irDelay);
	if (irDelay > 0 && irDelay < holdDelay)
	{
		return 0;
	}
	if (code[codeIndex] == 'R')
	{
		if (code[codeIndex+1] == 'E')
		{
			retChar = code[codeIndex];
		}
		else
		{
			retChar = '>';
		}
	}
	else
	{
		retChar = code[codeIndex];
	}
	/*
	 * need to free code because lirc_nextcode allocates memory
	 * this is to avoid possible memory leak errors
	 */
	free(code);
	return retChar;
}

/**
 * \brief		Terminates the remote control decoder and releases the irtv thread.
 */
void IRTVRelease(void)
{
	lirc_deinit();
	/* 
	 * need to call the following command to stop IR input otherwise the pi could 
	 * 		still be controlled with the remote control
	 */
	system("sudo /etc/init.d/lirc stop");
	running = 0;
}

/**
 * \brief		Checks to see if IR input is enabled;
 * \return		int retVal: whether IR input is enabled
 * \details		
 * 	-0: not enabled
 * 	-1: enabled
 */
int IRTVGetStatus()
{
	return running;
}

void IRTVSetControllerCode(DeviceSemantics semantics)
{
	if (controllerCode == NULL)
	{
		controllerCode = (char *)malloc(10);
	}
	strcpy(controllerCode,semantics);
}

DeviceSemantics IRTVGetControllerCode()
{
	/*char *temp = (char *)malloc(10);
	strcpy(temp,controllerCode);
	return temp;*/
	return controllerCode;
}

/**
 * \brief	sets the hold to prevent multiple presses being registered 
 * 			when a IR remote button is pressed
 * \param	int delay: the value to set the delay to, larger value equal larger delay
 */
void IRTVSetHoldDelay(int delay)
{
	holdDelay = delay;
}
