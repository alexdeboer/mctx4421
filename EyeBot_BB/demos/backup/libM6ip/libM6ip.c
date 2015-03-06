/*
 *   Image Processing Library
 *   Thomas Braunl, Univ. Stuttgart, 1996
 *   Additional Code by Gerrit Heitsch
 *   Adapted by Michael Rudolph
 *   derived after Braunl et al. : Parallele Bildverarbeitung, Addison-Wesley
 *
 *   : improc.c,v 1.5 2000/09/07 04:08:34 hoey-d Exp
 *
 */

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "libM6ip.h"

static int  width = 352;
static int  height = 288;
static BYTE black = 0x00;
static BYTE white = 0xFF;


/*
 *    IPL_init(IPL_CONFIG * config)
 *
 *    initialisation routine of the IPL. Set the image width and height,
 *    the byte codes for the colors black and white.
 */

// Axel: this seems not to be a robios function, therfore it is commented out
//
// void IPInit(IPL_CONFIG * config)
// {
//     InitialiseRobot();
//     width  = config -> imageWidth;
//     height = config -> imageHeight;
//     black  = config -> colorBlack;
//     white  = config -> colorWhite;
// }

int IPLaplace(image *imageIn, image *imageOut)
{

    int i;
    int delta;

    for (i = width; i < (height-1)*width; i++)
    {
        delta  = abs(4 * (*imageIn)[0][i] -
                         (*imageIn)[0][i-1] -
                         (*imageIn)[0][i+1] -
                         (*imageIn)[0][i-width] -
                         (*imageIn)[0][i+width]);

        if (delta > white)
            (*imageOut)[0][i] = white;
        else
            (*imageOut)[0][i] = (BYTE)delta;
    }
return 0;
}


int IPDiffer(image *image1, image *image2, image *imageOut)
{

    int i;
    int delta;

    for (i = 0; i < width * height; i++)
    {
        delta = (*image1)[0][i] - (*image2)[0][i];
        if (delta < 0)
            delta = - delta;
        (*imageOut)[0][i] = (BYTE)delta;
    }
	return 0;
}


int IPSobel(image *imageIn, image *imageOut)
{

    int i;
    int grad;
    int deltaX, deltaY;

    memset((*imageOut)[0], 0, width);

    for (i = width; i < (height-1)*width; i++)
    {
        deltaX = 2*(*imageIn)[0][i+1] + (*imageIn)[0][i-width+1] +
                 (*imageIn)[0][i+width+1] - 2*(*imageIn)[0][i-1] -
                 (*imageIn)[0][i-width-1] - (*imageIn)[0][i+width-1];


        deltaY = (*imageIn)[0][i-width-1] + 2*(*imageIn)[0][i-width] +
                 (*imageIn)[0][i-width+1] - (*imageIn)[0][i+width-1] -
                 2*(*imageIn)[0][i+width] - (*imageIn)[0][i+width+1];

        grad = (abs(deltaX) + abs(deltaY)) / 3;
        if (grad > white)
            grad = white;

        (*imageOut)[0][i] = (BYTE)grad;
    }

    memset((*imageOut)[0] + i, 0, width);
	return 0;
}



/* 2x2 ordered dithering, uses only upper left pixel in 2x2 areas      */
int IPDither(image *imageIn, image *imageOut)
{

    int x, y, i;
    int thres = white / 5;
    memcpy(*imageOut, *imageIn, width * height);

    for (y = 0; y < height-1; y+=2)
    {
        for (x = 0; x < width-1; x+=2)
        {
            i = x + width * y;

            if ((*imageIn)[0][i] <   thres)
                (*imageOut)[0][i] = black;
            else
                (*imageOut)[0][i] = white;

            if ((*imageIn)[0][i] < 3*thres)
                (*imageOut)[0][i+1] = black;
            else
                (*imageOut)[0][i] = white;

            if ((*imageIn)[0][i] < 4*thres)
                (*imageOut)[0][i+width] = black;
            else
                (*imageOut)[0][i] = white;

            if ((*imageIn)[0][i] < 2*thres)
                (*imageOut)[0][i+width+1] = black;
            else
                (*imageOut)[0][i] = white;
        }
    }
	return 0;
}


int IPColor2Grey(colimage *src, image *dest)
{

	int row, column;
	for (row = 1; row < imagerows-1; row++)
		for (column = 1; column < imagecolumns-1; column++)
		{
			int val;
			val  = (*src)[row][column][0];
			val += (*src)[row][column][1];
			val += (*src)[row][column][2];
			val /= 3;
			(*dest)[row][column] = val;
		}

	return 0;
}


