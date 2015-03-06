/********************************************************
| File:         test-libM6time.c
| Author/Date:  SSTeoh / 27/6/08
| Description:  Test program for M6 Time functions
*********************************************************/
#include <stdio.h>
#include <unistd.h> // for sleep()
#include <errno.h>
#include "libM6time.h"


#ifndef _REENTRANT
#error ACKK!!! _REENTRANT NOT DEFINED!
#endif

void Timertask1(void);
void Timertask2(void);
void Timertask3(void);
void Timertask4(void);
void Timertask5(void);
int testOSTimes(void);
int testOSAttachTimer(void);

int cnt1=0, cnt2=0, cnt3=0, cnt4=0, cnt5=0;


void Timertask1(void)
{
	cnt1++;
}
void Timertask2(void)
{
	cnt2++;
}
void Timertask3(void)
{
	cnt3++;
}
void Timertask4(void)
{
	cnt4++;
}
void Timertask5(void)
{
	cnt5++;
}

int testOSAttachTimer (void)
{
	// Test for the following functions:
	// OSAttachTimer(), OSDetachTimer()
	TimerHandle timerhandle1, timerhandle2, timerhandle3, timerhandle4, timerhandle5;
	int i;

	printf("\nAttach 5 functions to the 100Hz timer\n\n");
	printf("Timertask1 increment cnt1 at 1s interval   \n");
	printf("Timertask2 increment cnt2 at 500ms interval\n");
	printf("Timertask3 increment cnt3 at 200ms interval\n");
	printf("Timertask4 increment cnt4 at 100ms interval\n");
	printf("Timertask5 increment cnt5 at 10ms interval \n");

	timerhandle1 = OSAttachTimer(100, &Timertask1);	//Set Timertask1() to run at 1s interval
	timerhandle2 = OSAttachTimer(50, &Timertask2);	//Set Timertask2() to run at 500ms interval
	timerhandle3 = OSAttachTimer(20, &Timertask3);  //Set Timertask3() to run at 200ms interval
	timerhandle4 = OSAttachTimer(10, &Timertask4);  //Set Timertask4() to run at 100ms interval
	timerhandle5 = OSAttachTimer(1, &Timertask5);   //Set Timertask5() to run at 10ms interval

	printf("\nStart cnt:\tcnt1=%3d, cnt2=%3d, cnt3=%3d, cnt4=%3d, cnt5=%3d\n", cnt1, cnt2, cnt3, cnt4, cnt5);
	for ( i=1;i<=10;i++)
	{
		sleep(1);
		printf("After %ds:\tcnt1=%3d, cnt2=%3d, cnt3=%3d, cnt4=%3d, cnt5=%3d\n", i, cnt1, cnt2, cnt3, cnt4, cnt5);
	}

	printf("\nDetach all functions from the 100Hz timer\n");
	OSDetachTimer(timerhandle1);
	OSDetachTimer(timerhandle2);
	OSDetachTimer(timerhandle3);
	OSDetachTimer(timerhandle4);
	OSDetachTimer(timerhandle5);

	sleep(1);
	printf("After 11s:\tcnt1=%3d, cnt2=%3d, cnt3=%3d, cnt4=%3d, cnt5=%3d\n", cnt1, cnt2, cnt3, cnt4, cnt5);

	sleep(1);
	printf("After 12s:\tcnt1=%3d, cnt2=%3d, cnt3=%3d, cnt4=%3d, cnt5=%3d\n", cnt1, cnt2, cnt3, cnt4, cnt5);

  return 0;
}



int testOSTimes (void)
{
	// Test for the following functions:
	// OSSetTime(), OSSetDate(), OSGetCount(), OSGetTime(), OSGetDate() and  OSWait()

  	int hrs, mins, secs, ticks, year, mon, day;
	int cnt1, cnt2;

	printf("==>Test OSGetCount()...\n");
	printf("OSGetCount() = %d\n", OSGetCount());
	printf("Sleep 1s\n");
	sleep(1);
	printf("OSGetCount() = %d\n\n", OSGetCount());


	printf("==>Test OSGetTime()...\n");
	OSGetTime(&hrs, &mins, &secs, &ticks);
	printf("hrs=%d\tmin=%d\tsec=%d\tticks=%d\n\n", hrs, mins, secs, ticks);

	printf("Test OSGetDate()...\n");
	OSGetDate(&year, &mon, &day);
	printf("year=%d\tmon=%d\tday%d\t\n\n", year+1900, mon+1, day);

	printf("==>Test OSShowTime()...\nCurrent date/time = ");
	OSShowTime();

	printf("\n==>Test OSSetTime(10, 10, 10)\n");
	errno = 0;
	if (OSSetTime(10, 10, 10) == -1)
	{
		printf("Set time failed:\n" );
		if ( errno == EINVAL)
			printf("specifies an invalid time\n");
		else if ( errno == EPERM )
			printf("not a privileged user attempted to set the time\n");
	}

	printf("==>Test OSSetDate(2008, 5, 4)\n");
	errno = 0;
	if (OSSetDate(2008, 5, 4) == -1)
	{
		printf("Set time failed:\n" );
		if ( errno == EINVAL)
			printf("specifies an invalid date\n");
		else if ( errno == EPERM )
			printf("not a privileged user attempted to set the date\n");
	}
	printf("New date/time...");
	OSShowTime();

	printf("\n==>Test OSWait(50)...wait for 500ms\n");
	cnt1=OSGetCount();
	OSWait(50); //n*10ms
	cnt2=OSGetCount();
	printf("Start OSGetCount() = %d\n", cnt1);
	printf("End OSGetCount() = %d\n", cnt2);
	printf("Time diff = %dms\n\n", (cnt2-cnt1));

	cnt1=OSGetCount();
	OSWait(100); //n*10ms
	cnt2=OSGetCount();
	printf("Start OSGetCount() = %d\n", cnt1);
	printf("End OSGetCount() = %d\n", cnt2);
	printf("Time diff = %dms\n\n", (cnt2-cnt1));



  return 0;
}

int main(int argc, char* argv[])
{

	printf("Test OS time functions\n");
	printf("-----------------------------------------------\n");
	testOSTimes();

	//printf("Test OSAttachTimer and OSDetechTimer functions\n");
	///printf("-----------------------------------------------\n");
	//testOSAttachTimer();

	return 0;
}


