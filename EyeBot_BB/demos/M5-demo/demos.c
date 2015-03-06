/* ----------------------------------------------------------------- */
/* EyeBot Standard DEMOS                                             */
/* File: demos.c                                                     */
/* Called from: EyeBot main menu button "Demo" OUT OF ROM            */
/* Author: Thomas Braunl, 2003                                       */
/*         Thomas Braunl, 2000                                       */
/*         Thomas Lampart                                            */
/*         Klaus  Schmitt                                            */
/* ----------------------------------------------------------------- */
#define DEMOVERSION "6.3"
#include "include/eyebot.h"
#include "math.h"

int MK;  /* hardware version */

int MenuSelect (char *str, int min, int max)
/* select an item from a list                                */
/* select string with up to 6 items, selected by \n. 0 = END */
{ int line, k;

  LCDClear ();
  LCDPutString ("Menu Select " DEMOVERSION "\n");
  LCDPutString (str);
  LCDMenu ("+", "-", "Sel", "END");

  line = min;
  LCDSetString (line, 15, "*");

  while ((k = KEYGet ()) != KEY4 && k != KEY3)
    { LCDSetString (line, 15, " ");
      if (k == KEY1) line++;
      if (k == KEY2) line--;
      if (line < min) line = max;
      if (line > max) line = min;
      LCDSetString (line, 15, "*");
    }
  if (k == KEY4) return 0;			/* END */
  else return line;
}


/* from libradio/radiodmo.c */
void radiodemo (void);
int bluetoothsetup(void);
int WiFiSetUp(void);

/* forward declarations */
int musicdemo (void);
int camdemo1 (void);
int camdemo2 (void);
int camdemo3 (void);
void camdemo (void);
int audiodemo (void);
void drivedemo (void);
void radio_menu(void);



/* ************************************** */
/* DEMO MAIN PROGRAM                      */
/* ************************************** */

int main (void)
/* main demo selection */
{ int sel;

  MK = OSMachineType(); /* store machine version */
  do
  { sel = MenuSelect ("Camera\nAudio\nNetwork\nDrive", 1, 4);
    switch (sel)
    { case 1: camdemo(); break;
      case 2: audiodemo(); break;
      case 3: radio_menu(); break;
      case 4: drivedemo(); break;
    }
  } while (sel != 0);		/* 0 = END */
  return 0;
}

void radio_menu(void)
{/* branch to individual radio demos */
  int k;
  do
  { LCDClear ();
    LCDMenu ("Rad", "Blu", "WLa", "END");
    LCDPutString ("Select\nRadio Demo\n  or\nSetup for\n  BlueTooth or\n  WLAN");
    k = KEYGet();
    switch (k)
    { case KEY1: radiodemo(); break;
      case KEY2: bluetoothsetup(); break;
      case KEY3: WiFiSetUp(); break;
    }
  } while (k != KEY4);
}


void camdemo(void)
/* branch to individual camera demos */
{ int k;
  do
  { LCDClear ();
    LCDMenu ("Gry", "Col", "FPS", "END");
    LCDPutString ("Cam Demos\n");
    k = KEYGet();
    switch (k)
    { case KEY1: camdemo1(); break;
      case KEY2: camdemo2(); break;
      case KEY3: camdemo3(); break;
    }
  } while (k != KEY4);
}

/* -------------------------------------------  */

int audiodemo (void)
/* a little audio demo */
{ int b;
  int recorddemo (void);

  extern BYTE SAM_Ready[];	/* Voice says "Ready" sample */

  LCDClear ();
  LCDMenu ("Mus", "Mic", "WAV", "END");
  LCDPutString ("AudioDemos:\n");

  while (1)
  { b = KEYRead ();
    switch (b)
    { case KEY1: musicdemo ();
                 LCDClear ();
                 LCDMenu ("Mus", "Mic", "WAV", "END");
                 LCDPutString ("AudioDemos:\n");
                 break;

      case KEY2: recorddemo ();
                 LCDClear ();
                 LCDMenu ("Mus", "Mic", "WAV", "END");
                 LCDPutString ("AudioDemos:\n");
                 break;

      case KEY3: AUPlaySample (SAM_Ready);
                 while (!AUCheckSample ()) { }
                 break;

      case KEY4: return (0);

      default: break;
    }
  }
  return (0);
}


#define noteC  1114
#define noteD  1250
#define noteE  1403
#define noteF  1487
#define noteG  1669
#define noteA  1873
#define noteH  2103
#define noteC2 2228

#define dur4   200		/* ~ 1/4 note duration */
tone scale[] = { {noteC, dur4} , {noteD, dur4} , {noteE, dur4} , {noteF, dur4} ,
                 {noteG, dur4} , {noteA, dur4} , {noteH, dur4} , {noteC2, dur4},
								 {0, 0} };
tone melody1[] = { {noteC, dur4} , {noteG, dur4} , {noteC, dur4} , {noteA, dur4} ,
                   {noteC, dur4} , {noteH, dur4} , {noteC, dur4} , {noteF, dur4} ,
									 {noteG, dur4} , {noteC, dur4} , {0, 0} };

int musicdemo (void)
{ int key;
  int playtune (tone * tune);

  LCDClear ();
  LCDMenu ("Sca", "Mel", "", "END");
  LCDPutString ("MusicDemos:\n");

  while (1)
  { key = KEYRead ();
    switch (key)
    { case KEY1: playtune (&scale[0]);
                 break;

      case KEY2: playtune (&melody1[0]);
                 break;

      case KEY3: break;

      case KEY4: return (0);

      default: break;
    }
  }
  return (0);
}


int playtune (tone * tune)
{ int i = 0;
  int k = KEY1;

  while ((tune[i].freq != 0) && (tune[i].len != 0) && (k != KEY4))
  { AUTone ((tune[i].freq), tune[i].len);
    while (!AUCheckTone ()) { }
    i++;
    k = KEYRead ();
  }
  return (0);
}


/* ------------------------------------------------ */


int recorddemo (void)
#define RBUFLEN 20000
{ BYTE buffer[RBUFLEN];
  int choice;

  LCDClear ();
  LCDMenu ("Live", "Rec", "Play", "END");
  LCDPutString ("MicrophoneDemos:\n");

  while (1)
  { choice = KEYRead ();
    switch (choice)
    { case KEY1: LCDMenu (" ", " ", " ", "STOP");
                 LCDSetString (2, 0, "Volume: ");
                 while (KEYRead () != KEY4)
                 { LCDSetString (2, 0, "Micro:       ");
                   LCDSetPos (2, 7);
                   LCDPutInt (AUCaptureMic ());
                   OSWait (20);
                 }
                 LCDClear ();
                 LCDMenu ("Live", "Rec", "Play", "END");
                 LCDPutString ("MicrophoneDemos:\n");
                 break;

      case KEY2: if (RBUFLEN > 1024)
                 { LCDSetString (2, 0, "Recording!");
                   AURecordSample (buffer, RBUFLEN, 11000);
                   while (!AUCheckRecord ()) { }
                   LCDSetString (2, 0, "          ");
                 }
                 else
                 { LCDSetString (2, 0, "low memory!");
                   OSWait (100);
                   LCDSetString (2, 0, "           ");
                 }
                 break;

      case KEY3: LCDSetString (2, 0, "Playing!");
                 AUPlaySample (buffer);
                 while (!AUCheckSample ()) { }
                 LCDSetString (2, 0, "        ");
                 break;

      case KEY4: return (0);

      default: break;
    }
  }
  return (0);
}


/* ------------------------------------------------ */



#define DIST   0.4
#define SPEED  0.1
#define TSPEED 1.0

void drivedemo ()
/* Driving Demo: Drive a distance, then come back */
/* Author: Thomas Braunl, June 2000               */
{ VWHandle vw;
  PositionType pos;
  int i, camera, show, key;
  image picture;

  do
  { LCDClear ();
    LCDMenu ("Drv", "Cam", "", "END");
    key = KEYGet();
    show = (key==KEY2);
    if (key == KEY4) return;
  } while (key == KEY3);

  if (show)
  { camera = CAMInit (NORMAL);
    CAMSet (FPS1_875, 0, 0); /* set slower frame rate for driving */
    if (camera == NOCAM)
    { LCDPrintf("M%d: No Camera !!\a", MK);
      OSWait (200);
      return;
    }
    if (camera >= INITERROR)
    { LCDPrintf ("M%d: Camera init.\nerror!\n\a", MK);
      OSWait (200);
      return;
    }
  }

  vw = VWInit (VW, 1);
  VWStartControl (vw, 7, 0.3, 7, 0.1);
  OSSleep (100);		/* delay to remove hand */

  for (i = 0; i < 4; i++)	/* do 2 drives + 2 turns twice */
  { if (i % 2 == 0)
    { LCDSetString (0, 10, "Drive");
      VWDriveStraight (vw, DIST, SPEED);
    }
    else
    { LCDSetString (0, 10, "Turn ");
      VWDriveTurn (vw, M_PI, TSPEED);
    }
    while (!VWDriveDone (vw))
    { if (show)
      { CAMGetFrame (&picture);
        LCDPutGraphic (&picture);
      }
      else OSWait(33);
      VWGetPosition (vw, &pos);
      LCDSetPrintf (1, 10, "%4.1f", pos.x);
      LCDSetPrintf (2, 10, "%4.1f", pos.y);
      LCDSetPrintf (3, 10, "%5.0f", (pos.phi * 180.0 / M_PI));
    }
  }
  OSWait (200);
  if (show) CAMRelease ();
  VWRelease (vw);
}


