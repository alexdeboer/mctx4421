/**************************************************************************************************
| File:         libM6thread.h
| Author/Date:  SSTeoh / 27/6/08
| Description:  Header file for libM6thread.c
| 				For more information see http://robotics.ee.uwa.edu.au/eyebot/doc/API/library.html
***************************************************************************************************/
#ifndef _LIBM6THREAD_H_
#define _LIBM6THREAD_H_

#include "list.h"					//linked list

//Data types:

#if !defined(_BYTE)
#define _BYTE
typedef unsigned char BYTE;
#endif


// M6 Thread control block
struct m6tcb {
	struct list_head list;
	pthread_t hThread;
	void *code;     		// pointer to code
	int pri;                /* thread priority  */
	int stat;               /* thread status    */
	char *name;             /* thread name      */
	int uid;                /* UserID           */
};

//Legacy thread control block
struct tcb {                /* Thread control block */
	struct list_head list;
	pthread_t hThread;
	void *code;     		// pointer to code
	int pri;                /* thread priority  */
	int stat;               /* thread status    */
	char *name;             /* thread name      */
	int uid;                /* UserID           */
//	struct tcb *next;       /* double linked ring list pointers */
//	struct tcb *prev;
//	struct tcb *semq;       /* semaphore ist pointer    */
//	int stkbeg;             /* stack data   */
//	int stkend;
//	int stksiz;
//	char *stackbase;
//	int pri;                /* thread priority  */
//	int stat;               /* thread status    */
//	char *name;             /* thread name      */
//	int uid;                /* UserID           */

};

//Legacy semaphore structure - not used
struct tcbq {               /* thread control block queue anchor */
	struct tcb *next;
	int     pristat;        /* static priority value    */
	int     pridyn;         /* dynamic priority value   */
	int     threads;        /* not used yet             */
};
struct sem {                /* semaphore structure  */
	struct tcbq waitq;      /* list of threads      */
	int count;              /* counter              */
};


#define TASK_NEW        0       /* the stati a thread can have  */
#define TASK_READY      1
#define TASK_SUSPEND    2
#define TASK_RUNNING	3

#define	COOP	0		/* operation mode */
#define PREEMPT	1
#define NOTASK  255
#define	DEFAULT	COOP

#define IDLE_STACKSIZE      128		/* stack sizes */
#define DEFAULT_STACKSIZE   4096
#define SMALL_STACKSIZE     2048

#define MAX_PRI 8               /* MAX and MIN pri allowed for user programs */
#define MIN_PRI 1
#define IDLE_PRI 0              /* this pri is reserved for the system  */


//Flags for M6MultitaskFlags
#define MULTITASK_INITIALIZED 	1
#define MULTITASK_PERMITTED		2


// Legacy functions for OS multithreading
int OSSemInit( struct sem *sem, int val );
int OSSemP( struct sem *sem );
int OSSemV( struct sem *sem );

int OSPermit( void );
int OSForbid( void );
int OSMTInit( BYTE mode );
struct tcb *OSSpawn (char *name, void (*code)(void), int stksiz, int pri, int uid);
int OSGetUID( struct tcb *thread );
int OSMTStatus( void );
int OSReady( struct tcb *thread );
int OSSuspend( struct tcb *thread );
int OSSleep( int hsecs );
int OSYield( void);
int OSReschedule( void );
int OSRun( struct tcb *thread );
int OSKill( struct tcb *thread );
int OSPanic(char *msg);
int OSJoin(struct tcb *thread);
int OSExit(int code);


#endif //_LIBM6THREAD_H_

