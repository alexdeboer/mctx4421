/* ********************************* */
/* drive-ball.c                      */
/* Drive towards Ball                */
/* Thomas Braunl, UWA, 2002, 2004    */
/* ********************************* */

#include "eyebot.h"
#include <stdlib.h>
#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)
#define NO_HUE -1
#define BALLHUE 41

#define imagecolumns 354
#define imagerows 290

typedef BYTE image[imagerows][imagecolumns];
typedef BYTE colimage[imagerows][imagecolumns][3];

int RGBtoHue(BYTE r, BYTE g, BYTE b)
/* return hue value for RGB color */
{ int hue, delta, max, min;

  max   = MAX(r, MAX(g,b));
  min   = MIN(r, MIN(g,b));
  delta = max - min;
  hue =0; /* init hue*/

  if (2*delta <= max) hue = NO_HUE; /* gray, no color */
  else {
    if (r==max) hue = 42 + 42*(g-b)/delta; /* 1*42 */
    else if (g==max) hue = 126 +42*(b-r)/delta; /* 3*42 */
    else if (b==max) hue = 210 +42*(r-g)/delta; /* 5*42 */
  }
  return (BYTE) hue; /* now: hue is in range [0..252] */
}


void ColSearch(colimage img, int obj_hue, int thres,
               int *pos, int *val)
/* find x position of color object, return pos and value*/
{ int x,y, count, h, distance;
  *pos = -1; *val = 0;  /* init */
  for (x=0;x<imagecolumns;x++)
  { count = 0;
    for (y=0;y<imagerows;y++)
    { h = RGBtoHue(img[y][x][0],img[y][x][1],img[y][x][2]);

      if (h != NO_HUE)
      { distance = abs((int)h-obj_hue);  /* hue distance */
        if (distance > 126) distance = 253 - distance;
        if (distance < thres) count++;
      }
    }
    if (count > *val) { *val = count; *pos = x; }
  }
}


int mainbis5()
{ colimage c;
  VWHandle vw;
  int hue, pos, val, key;
  LCDInit();
  CAMHandle chandle = CAMInit("NORMAL");
  vw = VWInit("VW_DRIVE", 1);
  
  LCDPrintf("DriveToBall Demo\n");
  LCDMenu("STA","","","END");
  key = KEYGet();

  while(key != KEY4) 
  {	LCDMenu(" ","",""," "); /* clear menu */
    do
    { CAMGetFrameRGB(chandle, &c);
      LCDPutImageRGB(0, 0, 100, 100, &c);
      ColSearch(c, BALLHUE, 10, &pos, &val);  /* search image */
      LCDSetPos(1,0);
      LCDPrintf("h%3d p%2d v%2d\n", hue, pos, val);

      if (val < 20)  /* otherwise FINISHED */
        if (pos == -1 || pos < 20) VWDriveTurn    (vw,  0.10, 0.4);  /* left     */
          else if (pos > 60)       VWDriveTurn    (vw, -0.10, 0.4);  /* right    */
            else                   VWDriveStraight(vw,  0.05, 0.1);  /* straight */
      VWDriveWait(vw);  /* finish motion */
    } while (val < 20);
    
    /* ask for another round */
    LCDMenu("STA","","","END");
    key = KEYGet();
  }
  LCDRelease();
  return 0;
}
