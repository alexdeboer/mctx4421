/**************************************************************************************************
| File:         libM6thread.c
| Author/Date:  SSTeoh / 27/6/08
| Description:  Multitasking functions for Eyebot M6
| 				For more information see http://robotics.ee.uwa.edu.au/eyebot/doc/API/library.html
***************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include "../libM6time/libM6time.h"
#include "libM6thread.h"
#include "list.h"

struct list_head *pos, *q;
struct m6tcb M6threadlist;
unsigned short M6MultitaskFlags = 0;


/*
 * Description	: Initialize semaphore with given start value
 * Input		: (sem) pointer to a semaphore
 *				  (val) start value
 * Output       : NONE
 */
int OSSemInit( struct sem *sem, int val )
{
	sem_t *psemtmp;

	psemtmp = (sem_t *)(sem);
	sem_init (psemtmp, 0, val);
	return 0;
}

/*
 * Description	: Do semaphore P (down) operation
 * Input		: (sem) pointer to a semaphore
 * Output       : NONE
 */
int OSSemP( struct sem *sem )
{
	sem_t *psemtmp;

	psemtmp = (sem_t *)(sem);
	sem_wait(psemtmp);
	return 0;
}

/*
 * Description	: Do semaphore V (up) operation
 * Input		: (sem) pointer to a semaphore
 * Output       : NONE
 */
int OSSemV( struct sem *sem )
{
	sem_t *psemtmp;

	psemtmp = (sem_t *)(sem);
	sem_post(psemtmp);
	return 0;
}


/*
 * Description	: Initialize multithreading environment
 * Input		: not used - always set to PREEMPT mode
 * Output       : 0=ok, 1=MT already initialized
 */
int OSMTInit( BYTE mode )
{
	if (!(M6MultitaskFlags & MULTITASK_INITIALIZED)) {
		INIT_LIST_HEAD(&M6threadlist.list);
		M6MultitaskFlags = MULTITASK_INITIALIZED;
		return 0;
	}
	else {
		printf("MT already initialized\n");
		return 1;
	}
}

/*
  Description	: Initialize new thread, tcb is initialized and inserted in scheduler queue but not set to READY
  Input:        : (name) pointer to thread name
                  (code) thread start address
                  (stksize) size of thread stack - not used
                  (pri)  thread priority - not used
                  (uid)  thread user id
  Output       	: pointer to initialized thread control block
 */
struct tcb *OSSpawn (char *name, void (*code)(void), int stksiz, int pri, int uid)
{
	struct m6tcb *ptmp;

	ptmp = (struct m6tcb *)malloc(sizeof(struct m6tcb));
	ptmp->stat = TASK_NEW;
	ptmp->code = code;
	ptmp->uid = uid;
	ptmp->hThread = 0;
	list_add(&(ptmp->list), &(M6threadlist.list));
	return (struct tcb *)ptmp;
}

/*
 * Description	: Set status of given thread to READY
 * Input		: pointer to thread
 * Output       : NONE
 */
int OSReady(struct tcb *thread)
{
	struct m6tcb *ptmp;
	pthread_t htmp;

	ptmp = (struct m6tcb *)(thread);
	if (ptmp->stat == TASK_NEW) {
   		pthread_create(&htmp, NULL, ptmp->code, NULL);	//create thread
		pthread_kill(htmp, SIGSTOP);					//suspend the thread
		ptmp->hThread = htmp;
	}
	ptmp->stat = TASK_READY;

	return 0;
}



/*
 * Description	: Enable thread switching in preemptive mode
 * Input		: NONE
 * Output       : NONE
 */
int OSPermit()
{
	struct m6tcb *ptmp;

	M6MultitaskFlags |= MULTITASK_PERMITTED;

	list_for_each_entry(ptmp, &M6threadlist.list, list) {
		if (ptmp->stat == TASK_READY) {
			pthread_kill(ptmp->hThread, SIGCONT);
			ptmp->stat = TASK_RUNNING;
		}
	}
	OSReschedule();
 	return 0;
}


/*
 * Description	: Get the UID of the given thread
 * Input		: pointer to thread
 * Output       : thread UID or -1 on error
 */
int OSGetUID( struct tcb *thread )
{
	struct m6tcb *ptmp;

	if (thread) {
		ptmp = (struct m6tcb *)(thread);
		return ptmp->uid;
	}
	else { //get current thread id
		list_for_each_entry(ptmp, &M6threadlist.list, list) {
			if (ptmp->hThread == pthread_self())
				return ptmp->uid;
		}
	}
	return -1;
}


/*
 * Description	: Disable thread switching in preemptive mode - not implemented
 * Input		: NONE
 * Output       : NONE
 */
int OSForbid()
{
	struct m6tcb *ptmp;

	M6MultitaskFlags &= ~MULTITASK_PERMITTED;

	list_for_each_entry(ptmp, &M6threadlist.list, list) {
		if (ptmp->stat == TASK_RUNNING) {
			pthread_kill(ptmp->hThread, SIGSTOP);
			ptmp->stat = TASK_READY;
		}
	}
	return 0;
}


/*
 * Description	: Remove given thread and reschedule
 * Input		: pointer to thread
 * Output       : NONE
 */
int OSKill( struct tcb *thread )
{
	struct m6tcb *ptmp;

	if (thread) {
		ptmp = (struct m6tcb *)(thread);
		if (ptmp->stat != TASK_RUNNING)
			pthread_kill(ptmp->hThread, SIGCONT);
		pthread_cancel(ptmp->hThread);
		list_for_each_safe(pos, q, &M6threadlist.list) {
			if (ptmp == list_entry(pos, struct m6tcb, list)) {
				list_del(pos);
				free(ptmp);
			}
		}
	}
	else { //kill current thread
		pthread_t currthread = pthread_self();
		list_for_each_safe(pos, q, &M6threadlist.list) {
			ptmp = list_entry(pos, struct m6tcb, list);
			if (ptmp->hThread == currthread) {
				if (ptmp->stat != TASK_RUNNING)
					pthread_kill(ptmp->hThread, SIGCONT);
				pthread_cancel(currthread);
				list_del(pos);
	 			free(ptmp);
			}
		}
	}
	return 0;
}

/*
 * Description	: Set status of given thread to SUSPEND
 * Input		: pointer to thread
 * Output       : NONE
 */
int OSSuspend(struct tcb *thread)
{
	struct m6tcb *ptmp;

	if (thread) {
		ptmp = (struct m6tcb *)(thread);
		if (ptmp->stat == TASK_RUNNING) {
			pthread_kill(ptmp->hThread, SIGSTOP);
			ptmp->stat = TASK_SUSPEND;
			//printf("thread suspended\n");
		}
	}
	else { //suspend current thread
		list_for_each_entry(ptmp, &M6threadlist.list, list) {
			if ((ptmp->hThread == pthread_self()) && (ptmp->stat == TASK_RUNNING)) {
				pthread_kill(ptmp->hThread, SIGSTOP);
				ptmp->stat = TASK_SUSPEND;
			}
		}
	}
	return 0;
}

/*
 * Description	: READY given thread and reschedule
 * Input		: pointer to thread
 * Output       : NONE
 */
int OSRun(struct tcb *thread )
{
	struct m6tcb *ptmp;

	OSReady(thread);
	if (M6MultitaskFlags & MULTITASK_PERMITTED) {
		ptmp = (struct m6tcb *)(thread);
		pthread_kill(ptmp->hThread, SIGCONT);
		ptmp->stat = TASK_RUNNING;
		OSReschedule();
	}
	return 0;
}


/*
 * Description	: Let current thread sleep for at least n*10ms
 * Input		: hsecs - number of 10 millisecons to sleep
 * Output       : NONE
 */
int OSSleep( int hsecs )
{
	OSWait(hsecs);
	return 0;
}

/*
 * Description	: Suspend current thread and reschedule
 * Input		: pointer to thread
 * Output       : NONE
 */
int OSYield()
{
	OSSuspend(0);
	OSReschedule();
	return 0;
}

/*
 * Description	: Choose new current thread
 * Input		: pointer to thread
 * Output       : NONE
 */
int OSReschedule()
{
	OSWait(1);
	return 0;
}


/*
 * Description	: Returns actual multitasking mode - not implemented
 * Input		: NONE
 * Output       : Return multitasking status
 */
int OSMTStatus()
{
	return (M6MultitaskFlags & MULTITASK_PERMITTED);
}

/*
 * Description: : Multithreading error, print message to display and stop multitasking
 * Input:    	: pointer to message text
 * Output:      : NONE
 */
int OSPanic(char *msg)
{
	printf("%s", msg);
	pthread_kill(pthread_self(), SIGTERM);
	return 0;
}

/*
 * Description: : Wait for a thread to finish
 * Input:    	: pointer to thread
 * Output:      : NONE
 */
int OSJoin(struct tcb *thread)
{
	struct m6tcb *ptmp;

	ptmp = (struct m6tcb *)(thread);
	if (ptmp->stat == TASK_RUNNING)
		pthread_join(ptmp->hThread, NULL);

	return 0;
}

/*
 * Description: : Kill current thread with given exit code
 * Input:    	: Exit code
 * Output:      : NONE
 */
int OSExit(int code)
{
	OSKill(0);
	printf("Exit code = %d\n", code);
	return 0;
}


