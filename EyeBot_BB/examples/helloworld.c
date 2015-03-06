#include <stdio.h>
#include "eyebot.h"


int mainbis4()
{
	int i;

	LCDInit(); 				//Initialize the LCD, use big font

	for (i=0; i<10; i++)
	{
      	printf("Hello World!\n");			//This print to the console terminal
      	LCDPrintf ("Hello World!\n");		//This print to the EyeBot LCD display
	}
	OSWait(1000);
	LCDRelease();

	return 0;
}
