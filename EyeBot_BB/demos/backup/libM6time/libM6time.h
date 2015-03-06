/**************************************************************************************************
| File:         libM6time.h
| Author/Date:  SSTeoh / 27/6/08
| Description:  Header file for libM6time.c
| 				For more information see http://robotics.ee.uwa.edu.au/eyebot/doc/API/library.html
***************************************************************************************************/


#ifndef _LIBM6ATTACHTIMER_H_
#define _LIBM6ATTACHTIMER_H_


// Data types
typedef int TimerHandle;
typedef void (*TimerFnc)(void);


int OSSetTime(int hrs,int mins,int secs);
//        Input:          (hrs) value of hours
//                        (mins) value of minutes
//                        (secs) value of seconds
//		  Output:		  return -1 if failed
//        Semantics:      Set system time


int OSSetDate(int year,int mon,int day);
//        Input:          (year) value of year
//                        (mon) value of month
//                        (day) value of day
//		  Output:		  return -1 if failed
//        Semantics:      Set system time

int OSGetCount(void);
//        Input:          NONE
//        Output:         (return code) number of 1/100 seconds since last reset
//        Semantics:      Get the number of 1/100 seconds since last reset.
//                        Type int is 32 bits, so this value will wrap
//                        around after ~248 days.


long OSGetTime(int *hrs,int *mins,int *secs,int *ticks);
//        Input:          (hrs) pointer to int for hours
//                        (mins) pointer to int for minutes
//                        (secs) pointer to int for seconds
//                        (ticks) pointer to int for ticks
//        Output:         (hrs) value of hours
//                        (mins) value of minutes
//                        (secs) value of seconds
//                        (ticks) value of ticks
//        Semantics:      Get system time, one second has 100 ticks


int OSGetDate(int *yrs,int *mon,int *day);
//        Input:          (yrs) pointer to int for year
//                        (mon) pointer to int for mon
//                        (day) pointer to int for day
//        Output:         (yrs) value of year
//                        (mon) value of mon
//                        (day) value of day
//        Semantics:      Get system date

int OSShowTime(void);
//        Input:          NONE
//        Output:         NONE
//        Semantics:      Print system time and date to display


int OSWait (int n);
//        Input:          (n) time to wait (in 1/100s) 10ms
//        Output:         NONE
//        Semantics:      Busy loop for n*1/100 seconds


void OSSleepms(int n);
//        Input:          (n) time to wait (in 1/100s) 10ms
//        Output:         NONE
//        Semantics:      Busy loop for n*1/100 seconds


TimerHandle OSAttachTimer(int scale, TimerFnc function);
// Input:          (scale) prescale value for 100Hz Timer
//                 (TimerFnc) function to be called periodically
// Output:         (TimerHandle) handle to reference the IRQ-slot
//                 A value of 0 indicates an error
// Semantics:      Attach a user function (void function(void)) to a 10ms timer
//                 The scale parameter adjusts the call frequency (100/scale Hz)of
//				   this routine to  allow many different applications.
// Note:           Execution time of any attached routine (and total time of all
//				   attached routines) has to be significantly < 10ms. Otherwise
//				   timer interrupts will be missed and motor/sensor-timing gets corrupted
//

int OSDetachTimer(TimerHandle handle);
// Input:          (handle) handle of a previous installed timer irq
// Output:         0 = handle not valid
//                 1 = function successfully removed from timer irq list
// Semantics:      Detach a previously installed irq-routine from the irq-list.
//

#endif //_LIBM6ATTACHTIMER_H_

