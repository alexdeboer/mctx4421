#include "eyebot.h"

int main(int argc, char **argv)
{
	fprintf(stderr,"Demo start\n");
	int exit = 0;
    
	fprintf(stderr,"Initialising LCD...");
    
	if(LCDInit(0)<0)
	{
        fprintf(stderr,"inside loop");
		printf("LCDInit Error! Exiting!\n");
		return -1;
	}
    
	fprintf(stderr,"Done\n");
    
    for (i=0; i<10; i++)
    {
        printf("Hello World!\n");			//This print to the console terminal
        LCDPrintf ("Hello World!\n");		//This print to the EyeBot LCD display
    }
    OSWait(1000);
	fprintf(stderr,"Exiting!\n");
    
    LCDRelease();
    
	return 0;
}
