/**
 * File: keylcd-demo.c
 *
 * Example program to use some simple key and lcd library functions
 *
 */
#include <stdio.h>
#include "eyebot.h"


int mainbis3()
{
	keycode_t key;
	int done=0;
	int ret;

	//Initialize the lcd
	if((ret=LCDInit())<0)
	{
		printf("Error: Unable to initialize the LCD [%d]\n",ret);
		return -1;
	}
	//Initialize the touch key
	if((ret=KEYInit())<0)
	{
		printf("Error: Unable to initialize the touch KEY [%d]\n",ret);
		return -1;
	}

  	LCDClear ();
  	LCDMenu ("Key 1", "Key 2", "Key 3", "END");

  	while (!done)
  	{
		key = KEYRead ();

  	  	switch (key)
  	  	{
			case KEY1:
      			LCDPutString ("You have pressed KEY 1");
  	  	  		break;

  	  	  	case KEY2:
      			LCDPutString ("You have pressed KEY 2");
  	  	  	 	break;

  	  	  	case KEY3:
      			LCDPutString ("You have pressed KEY 3");
		  		break;

  	  	  	case KEY4:
      			LCDPutString ("You have pressed KEY 4...END");
		  		done=1;
				break;

  	  	  	default:
		  		break;
  	  	}
  	  	OSWait(100);
  	}
	LCDRelease();
	KEYRelease();

  	return (0);
}



