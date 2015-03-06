#include "include/eyebot.h"
#include <stdio.h>
#include <stdlib.h>
#define LOWEST_FPS_INDEX FPS15
#define HIGHEST_FPS_INDEX FPS0_9375

#define NO_HUE -1
#define SECONDS 2
extern int MK;
int RGBtoHue (BYTE r, BYTE g, BYTE b);
#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)


int camdemo1 (void)
/* the famous original camera demo */
{ image img1, img2, img3;
  image *ip1, *ip2, *ip3, *ipsave;
  int i = 0;
  int co;
  int slots[] = { 0, 0, 0, 0 };
  int ticks, oldticks, fps;
  int version, c;

  LCDClear ();
  oldticks = OSGetCount ();

  version = CAMInit(NORMAL);
  if (version == NOCAM)
  { LCDPrintf("M%d: No Camera !!\a", MK);
    KEYWait (ANYKEY);
    return (-1);
  }

  while (version >= INITERROR)	/* In case CamInit failed the first time */
  { LCDPrintf("I%d ", version);
    OSWait (CAMWAIT);
    version = CAMInit(NORMAL);
  }

  /* CAMSet (FPS7_5, 0, 0);  set camera speed */
  LCDClear ();
  LCDMenu (" + ", " - ", "Nxt", "END");

  ip1 = &img1;
  ip2 = &img2;
  ip3 = &img3;
  LCDSetPos (6, 15);
  LCDSetChar (0, 15, '*');

  while (1)
  { c = KEYRead ();
    switch (c)
    { case KEY1:		/* change slot i */
        slots[i]++;
        if (slots[i] > 4) slots[i] = 0;
        break;

      case KEY2:		/* change slot i */
        slots[i]--;
        if (slots[i] < 0) slots[i] = 4;
        break;

      case KEY3:		/* change i */
        LCDSetChar (i, 15, ' ');
        i++;
        if (i > 3) i = 0;
        LCDSetChar (i, 15, '*');
        break;

      case KEY4: return (0);

      default: break;
    }

    if (version < COLCAM) CAMGetFrame (ip1);
      else CAMGetColFrame ((colimage *) ip1, 1);

    for (co = 0; co <= 3; co++)
	  { switch (slots[co])	/* do and display operator for each slot */
	    { case 0: LCDSetString (co, 10, "Norm.");
                break;

        case 1: IPLaplace (ip1, ip2);
                ipsave = ip2;
                ip2 = ip1;
                ip1 = ipsave;
                LCDSetString (co, 10, "Lapl.");
                break;

        case 2: IPSobel (ip1, ip2);
                ipsave = ip2;
                ip2 = ip1;
                ip1 = ipsave;
                LCDSetString (co, 10, "Sobel");
                break;

        case 3: IPDither (ip1, ip2);
                ipsave = ip2;
                ip2 = ip1;
                ip1 = ipsave;
                LCDSetString (co, 10, "Dith.");
                break;

        case 4: IPDiffer (ip1, ip3, ip2);
                ipsave = ip3;
                ip3 = ip1;
                ip1 = ipsave;
                ipsave = ip2;
                ip2 = ip1;
                ip1 = ipsave;
                LCDSetString (co, 10, "Diff.");
                break;

        default: break;
	    }
    }

    LCDPutGraphic (ip1);
    /* display center value */
    LCDSetString (4, 10, "pix:");
    LCDSetPos (4, 14);
    LCDPutHex1 ((*ip1)[31][41]);

    /* calculate frames per second (in 1/10 per sec) */
    ticks = OSGetCount ();
    fps = 1000 / (ticks - oldticks);
    oldticks = ticks;

    LCDSetString (5, 10, "fps:");
    LCDSetPos (6, 10);
    LCDPutIntS (fps / 10, 2);
    LCDPutChar ('.');
    LCDPutChar ('0' + (fps % 10));	/* fractional part */
  }

  CAMRelease();
  return 0;
}


/* --------------------------------------------------- */


int camdemo2 (void)
/* Demo for the color cam only                  */
/* displays selectable color components         */
{ colimage colimg;
  int version;
  int toggle = 1;
  int i, j;
  int cur_hue, mem_hue = 0, d_hue;

  LCDClear ();
  version = CAMInit (WIDE);
  if (version == NOCAM)
  { LCDPrintf("M%d: No Camera !!\a", MK);
    KEYWait (ANYKEY);
    return -1;
  }

  while (version >= INITERROR)	/* In case CamInit failed first time */
  { LCDPrintf("I%d ", version);
    OSWait (CAMWAIT);
    version = CAMInit (WIDE);
  }

  if (version < COLCAM)
  { LCDPrintf("M%d: No Camera !!\a", MK);
    KEYWait (ANYKEY);
    return -1;
  }

  CAMSet (FPS7_5, 0, 0); /* set camera speed */
  LCDClear ();
  LCDMenu ("Grb", "Tog", "", "END");
  LCDSetString (0, 10, "NORMAL");
  LCDSetString (5, 10, "match:");

  while (1)			/* display color of image middle */
  { CAMGetColFrame (&colimg, 0);
    LCDSetPrintf (1, 10, "R: %3d", colimg[31][41][0]);
    LCDSetPrintf (2, 10, "G: %3d", colimg[31][41][1]);
    LCDSetPrintf (3, 10, "B: %3d", colimg[31][41][2]);
    cur_hue =
      RGBtoHue (colimg[31][41][0], colimg[31][41][1], colimg[31][41][2]);
    LCDSetPrintf (4, 10, "h: %3d", cur_hue);

    switch (KEYRead ())
    {
    case KEY1:  /* Grab Color values from middle of pic */
	  mem_hue = cur_hue;
	  LCDSetPrintf (6, 10, "%4d", mem_hue);
	  break;

    case KEY2:  /* switch between normal and filtered display */
	  toggle = !toggle;
	  if (toggle) LCDSetString (0, 10, "NORMAL");
	  else LCDSetString (0, 10, "FILTER");
	  break;

    case KEY3: break;

    case KEY4: CAMRelease ();
               return 0;

    default: break;
    }

    if (toggle) LCDPutColorGraphic (&colimg);	/* display normal */
    else  /* display filtered */
    { for (i = 0; i < imagerows; i++)
	    { for (j = 0; j < imagecolumns; j++)
        { /* calc color value distance */
          cur_hue = RGBtoHue (colimg[i][j][0], colimg[i][j][1], colimg[i][j][2]);
          d_hue = abs(mem_hue - cur_hue);

          if (cur_hue != NO_HUE && d_hue < 30)	/* MATCH: black pixel */
          { colimg[i][j][0] = 0;
            colimg[i][j][1] = 0;
            colimg[i][j][2] = 0;
          }
          else /* no match: white pixel */
          { colimg[i][j][0] = 255;
            colimg[i][j][1] = 255;
            colimg[i][j][2] = 255;
          }
        }
	    }
      LCDPutColorGraphic (&colimg);
    }
  }
}


/* --------------------------------------------------- */


int camdemo3 (void)
/* camera demo allowing frame-rate setting */
/* Jason Foo, Jacky Baltes, Thomas Braunl  */
{ int fps, key, count, clock, version;
  BOOL display = TRUE, drive = FALSE;
  const char *speed;
  colimage pic;
  VWHandle vw = 0;
  int i, j, k;
  FILE *stream;

  LCDClear ();
  LCDMenu ("FPS", "Dis", "Upl", "END");
  
  version = CAMInit(NORMAL);
  if (version == NOCAM)
  { LCDPrintf("No Camera !!\a");
    KEYWait (ANYKEY);
    return (-1);
  }

  while (version >= INITERROR)	/* In case CamInit failed the first time */
  { LCDPrintf("I%d ", version);
    OSWait (CAMWAIT);
    version = CAMInit(NORMAL);
  }
  
  LCDClear ();
  LCDSetPrintf(4,10, "Cam %d", version);

  fps = LOWEST_FPS_INDEX;
  count = 0;
  clock = OSGetCount();
  while (CAMSet (fps, 0, 0) != 0) fps++;

  do
  { CAMGetColFrame (&pic, FALSE);
    key = KEYRead();
    
   /* Image Upload */
   if (key == KEY3)
    { stream = fopen("ser1", "w");
      fprintf (stream, "P6\n%d %d\n255\n", imagecolumns, imagerows);
      for (i = 0; i < imagerows; i++)
      { LCDSetPrintf(0,10, "Line%2d", i);
        for (j = 0; j < imagecolumns; j++)
        { for (k = 0; k < 3; k++)
            putc (pic[i][j][k], stream);
        }
        OSWait(14); /* slow down data transfer */
      }
      fclose (stream);
      AUBeep ();
    }

    if (display) LCDPutColorGraphic (&pic); /* Note: display may destroy! */

    /* Speed display */
    LCDSetPrintf (0,10, "%6d\n", ++count);
    if (OSGetCount () - clock >= 100 * SECONDS)
    { LCDSetPos (1, 10);
      LCDPrintf ("%4.1fHz\n", (double) count / SECONDS);
      count = 0;
      clock = OSGetCount ();
    }

    switch (fps)
    { case FPS60: speed = "60    "; break;
      case FPS30: speed = "30    "; break;
      case FPS15: speed = "15    "; break;
      case FPS7_5: speed = "7.5   "; break;
      case FPS3_75: speed = "3.75  "; break;
      case FPS1_875: speed = "1.875 "; break;
      case FPS0_9375: speed = "0.9375"; break;
      case FPS0_46875: speed = "0.4688"; break;
      default: speed = "?     "; break;
    }
    LCDSetPrintf (2,10, "%6s\n", speed);

    /* change fps */
    if (key == KEY1)
    { if (++fps > HIGHEST_FPS_INDEX) fps = LOWEST_FPS_INDEX;
      LCDSetPos(0,0);
      while (CAMSet (fps, 0, 0) != 0) LCDPutString("camset ");
    }

    /* toggle driving or DISPLAY */
    if (key == KEY2)
    { display = !display;
      if (display) LCDSetString(5,10, "      ");
        else       LCDSetString(5,10, "no Dis");
    }

/*    { drive = !drive;
      if (drive)
      { LCDSetString(5,10, "Drive");
        vw = VWInit (VW, 1);
        VWStartControl (vw, 7, 0.3, 7, 0.1);
        VWSetSpeed (vw, 0.1, 0);
      }
      else
      { LCDSetString(5,10, "     ");
        VWRelease (vw);
      }
    }
*/
  } while (key != KEY4);

  CAMRelease ();
  if (drive) VWRelease (vw);
  return 0;
}


/* ----------------------------------------------------- */

int RGBtoHue (BYTE r, BYTE g, BYTE b)
/* Convert RGB to HSI color model */
/* Thomas Braunl, 1998            */
{ int hue=NO_HUE; /* sat, val, */
  int delta, max, min;

  max = MAX (r, MAX (g, b));
  min = MIN (r, MIN (g, b));
  delta = max - min;

  /* val   = max;
     if (max != 0) sat = delta / max;
     else sat = 0;
     if (sat == 0) hue = NO_HUE;
   */

  if (3 * delta <= max)
    hue = NO_HUE;
  else
    { if (r == max)
      hue = 42 + 42 * (g - b) / delta;	/* 1*42 */
      else if (g == max)
       hue = 126 + 42 * (b - r) / delta;	/* 3*42 */
      else if (b == max)
       hue = 210 + 42 * (r - g) / delta;	/* 5*42 */
       /* now: hue is in range [0..252] */
    }
  return hue;
}


