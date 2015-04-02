/*
 * misc.c
 *
 *  Created on: 15 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		misc.c
 * \brief   Defines misc functions
 * \author  Remi KEAT
 */

#include "misc.h"
#include <stdio.h>
#include <stdlib.h>

void strcpy_n(char* dest, const char* src, size_t n)
{
	//TODO: Correct to consider length
	int length = strlen(src);
	if (n < length)
	{
		char *temp = (char *)malloc(length+1);
		strcpy(temp,src);
		temp[n] = '\0';
		strcpy(dest, temp);
		free(temp);
	}
	else
	{
		strcpy(dest,src);
	}
}

/**
 * \brief   Returns the maximum value
 * \param   double a, double b
 * \return  the maximum value as a double
 */
int max(int a, int b)
{
	//fprintf(stderr,"Max:\na: %d\nb: %d\n",a,b);
	if (a >= b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

/**
 * \brief   trims the string by exchanging the first newlin char with the null byte
 * \param   src: the string to be trimmed
 */
void trimline(char *src)
{
	int length = strlen(src);
	int i;
	for (i = 0; i < length; i++)
	{
		if (src[i] == '\n' || src[i] == '\r')
		{
			src[i] = '\0';
			break;
		}
	}
}
