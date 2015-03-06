/*
 * timer.c
 *
 *  Created on: 1 Jul 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file    timer.c
 * \brief   Defines functions related to the timer
 * \author  Remi KEAT
 */

#include "timer.h"

/**
 * \brief   Busy loop for n*1/100 seconds.
 * \param   int n : time to wait
 * \return  int retVal : always 0
 */
int OSWait(int n)
{
	fd_set in_fds;
	struct timeval tv;
	if (gLCDHandle != NULL)
	{
		LCDRefresh();
		FD_ZERO(&in_fds);
		FD_SET(gLCDHandle->fd, &in_fds);
		tv.tv_sec = 0;
		tv.tv_usec = 10000*n;
		if (select(gLCDHandle->fd+1, &in_fds, 0, 0, &tv))
		{
			LCDNeedRefresh();
		}
	}
	else
	{
		usleep(10000*n);
	}
	return 0;
}
