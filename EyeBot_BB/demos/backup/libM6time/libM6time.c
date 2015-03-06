/**************************************************************************************************
| File:         libM6time.c
| Author/Date:  SSTeoh / 27/6/08
| Description:  Time functions for Eyebot M6
| 				For more information see http://robotics.ee.uwa.edu.au/eyebot/doc/API/library.html
***************************************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h> // for sleep()

#include "libM6time.h"
#include "libM6os.h"

#ifndef _REENTRANT
#error ACKK!!! _REENTRANT NOT DEFINED!
#endif

void* timer_thread(void *arg);

typedef struct _m6attachtimer_t {
	TimerFnc code;
	int timescale;
}
m6attachtimer_t;


//Time functions
//-----------------

//Get the number of 1/100 seconds since last reset
int OSGetCount(void)
{
	struct tms OsTMS;
	return (int)(times(&OsTMS));

	//struct timespec ts;
	//clock_gettime(CLOCK_MONOTONIC, &ts);
	//return ((ts.tv_sec*1000)+(ts.tv_nsec/1000000));
}

//Get system time, one second has 100 ticks
long OSGetTime(int *hrs,int *mins,int *secs,int *ticks)
{
  time_t curtime;
  struct tm *loctime;
  struct tms OsTMS;

  /* Get the current time.  */
  curtime = time (NULL);

  /* Convert it to local time representation.  */
  loctime = localtime (&curtime);

  *hrs = loctime->tm_hour;
  *mins = loctime->tm_min;
  *secs = loctime->tm_sec;
  *ticks = (int)(times(&OsTMS) % 100);

  return 0;
}

//Get system date
int OSGetDate(int *yrs,int *mon,int *day)
{

  time_t curtime;
  struct tm *loctime;

  /* Get the current time.  */
  curtime = time (NULL);

  /* Convert it to local time representation.  */
  loctime = localtime (&curtime);

  *yrs = loctime->tm_year;
  *mon = loctime->tm_mon;
  *day = loctime->tm_mday;

  return 0;
}

//Print system time to display
int OSShowTime(void)
{
  time_t curtime;
  struct tm *loctime;

  /* Get the current time.  */
  curtime = time (NULL);

  /* Convert it to local time representation.  */
  loctime = localtime (&curtime);

  /* Print out the date and time in the standard format.  */
  printf ("%s", asctime (loctime));

  return 0;
}

//Busy loop for n*1/100 seconds
int OSWait (int n)
{
	struct timespec reqtv, remtv;

	reqtv.tv_sec = n/100;
	reqtv.tv_nsec = (n % 100)*10000000;
	//reqtv.tv_sec = n/1000;                 	//1ms wait
	//reqtv.tv_nsec = (n % 1000)*1000000;
	nanosleep(&reqtv, &remtv);

	return 0;
}


//Set system clock to given time
int OSSetTime(int hrs,int mins,int secs)
{
  time_t curtime;
  struct timeval newtv;
  time_t newtime;
  struct tm *loctime;

  /* Get the current time.  */
  curtime = time (NULL);

  /* Convert it to local time representation.  */
  loctime = localtime (&curtime);

  loctime->tm_hour = hrs;
  loctime->tm_min = mins;
  loctime->tm_sec = secs;

  if ((newtime = (mktime(loctime))) == -1)
  	printf("invalid time value\n");

  newtv.tv_sec = newtime;
  newtv.tv_usec = 0;

  return settimeofday(&newtv, NULL);
}


//Set system clock to given date
int OSSetDate(int year,int mon,int day)
{
  time_t curtime;
  struct timeval newtv;
  time_t newtime;
  struct tm *loctime;

  /* Get the current time.  */
  curtime = time (NULL);

  /* Convert it to local time representation.  */
  loctime = localtime (&curtime);

  loctime->tm_year = year-1900;
  loctime->tm_mon = mon;
  loctime->tm_mday = day;

  if ((newtime = (mktime(loctime))) == -1)
  	printf("invalid time value\n");

  newtv.tv_sec = newtime;
  newtv.tv_usec = 0;

  return settimeofday(&newtv, NULL);
}


//Busy loop for n ms
void OSSleepms(int n)
{
	struct timespec reqtv, remtv;

	reqtv.tv_sec = n/1000;
	reqtv.tv_nsec = (n % 1000)*1000000;
	nanosleep(&reqtv, &remtv);
}

// Legacy functions for attaching and detaching user functions to a 100Hz timer
//----------------------------------------------------------------------------------

// Timer thread to execute the user function in scale*10ms interval
void* timer_thread(void *args)
{
	int tscale, curtime;
	TimerFnc RunAttachedFunc;
	m6attachtimer_t *timer_data = (m6attachtimer_t*) args;

	tscale = timer_data->timescale;
	RunAttachedFunc = (TimerFnc) timer_data->code;

	while(1)
	{
		RunAttachedFunc();
		curtime = OSGetCount();
		while ((OSGetCount()-curtime) < tscale)
			OSSleepms(1);
		pthread_testcancel();
	}
	return (void*) 0;
}


//Attach user function to the 100Hz timer thread
TimerHandle OSAttachTimer(int scale, TimerFnc function)
{
	static m6attachtimer_t timer_data;
	pthread_t htmp;

	timer_data.code = function;
	timer_data.timescale = scale;
	if (pthread_create(&htmp, NULL, timer_thread, (void*)&timer_data) != 0) {
		printf("Cannot attach function to the timer thread\n");
		return 0;
	}
	OSWait(1);
	return (TimerHandle) htmp;
}


//Detach user function from the 100Hz timer thread
int OSDetachTimer(TimerHandle handle)
{
	if((pthread_cancel((pthread_t)handle))!=0) {
		printf("Cannot detach function from the timer thread\n");
    	return 0;
	}
	else {
		pthread_join((pthread_t)handle, NULL);
	}
	return 1;
}


