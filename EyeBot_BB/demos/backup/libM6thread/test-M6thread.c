#include <stdio.h>
#include <pthread.h>
#include "../libM6time/libM6time.h"
#include "libM6thread.h"

#define ss  1024

struct tcb *t1, *t2;
int cnt1=0, cnt2=0;
struct sem Lcd;

void mytask1()
{
	int id, i;
  	id = OSGetUID(0);
	while (1)
	for ( i=0;i<50; i++)
	{
		cnt1++;
		OSSemP(&Lcd);
		//printf("Task1...%d\n", cnt1);
		OSSemV(&Lcd);
		OSWait(5);
		pthread_testcancel();
	}
}

void mytask2()
{
	int id;
  	id = OSGetUID(0);
	while (1)
	{
		cnt2++;
		OSSemP(&Lcd);
		//printf("Task2...%d\n", cnt2);
		OSSemV(&Lcd);
		OSWait(5);
		pthread_testcancel();
	}
}


int main()
{
   	printf("Initialize MT\n");
	OSMTInit(PREEMPT);

	OSSemInit(&Lcd,1);

   	printf("OSSpawn task1(UID=11) and task 2(UID=22)\n\n");
 	t1 = OSSpawn("t1", mytask1, ss, MIN_PRI, 11);
   	t2 = OSSpawn("t2", mytask2, ss, MIN_PRI, 22);
   	if (!t1 || !t2) printf("spawn failed\n\n");

	printf("UID for task1 = %d\n", OSGetUID(t1));
	printf("UID for task2 = %d\n\n", OSGetUID(t2));

   	printf("t1cnt=%d,  t2cnt=%d\n\n", cnt1, cnt2);

   	printf("OSReady(), OSPermit() task1 and task2\n");
   	OSReady(t1);
   	OSReady(t2);
	OSPermit();

   	OSWait(100);
   	printf("t1cnt=%d,  t2cnt=%d\n", cnt1, cnt2);

   	OSForbid();
   	printf("\nOSForbit()\n");

   	OSWait(100);
   	printf("t1cnt=%d,  t2cnt=%d\n\n", cnt1, cnt2);

	OSPermit();
   	printf("OSPermit()\n");

   	OSWait(100);
   	printf("t1cnt=%d,  t2cnt=%d\n", cnt1, cnt2);

	//Test OSSuspend and OSRun

   	printf("\nOSSuspend() task1\n");
	OSSuspend(t1);

   	OSWait(100);
   	printf("t1cnt=%d,  t2cnt=%d\n\n", cnt1, cnt2);

   	printf("OSRun() task1\n");
    OSRun(t1);

   	OSWait(100);
   	printf("t1cnt=%d,  t2cnt=%d\n\n", cnt1, cnt2);

   	OSKill(t1);
	printf("OSkill() task 1\n");
   	OSKill(t2);
	printf("OSkill() task 2\n");

   	OSWait(100);

   	return 0;
};

