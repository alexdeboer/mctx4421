/**
 * \file test-libM6au.c
 *
 * EyebotM6 library for audio routines internal test and demo file.
 *
 * tests / demonstrates the functions therein.
 *
 *
 * \todo
 *
 * \date 2006
 * \author Thomas Sommer
 * \version 0.40
 *
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "libM6au.h"

#define DONTCARE 789

/* prototypes: */
int evaluate_test (char *title, int value, int expected_value);
int testAUPlayFile (int config, int expect);
int testAURecordFile (int config, int expect);
int testAUPlaySample (int config, int expect);
int testAURecordSample (int config, int expect);
int testAUCheckRecord (int config, int expect);
int testAUCheckSample (int config, int expect);
int testAUTone (int config, int expect);
int testAUCheckTone (int config, int expect);
int testAUBeep (int config, int expect);
int testAUCaptureMic (int config, int expect);


typedef struct {
	int freq;
	int len;
} tone;


int playtune (tone * tune);

int musicdemo (void);


/* Globals: */
int verbose = 0;                /* 1: more than just results */


/* ------------------------------------------------------------------------- */
int main (int argc, char *argv[])
/* Currently does not evaluate any args - all is hardcoded. */
/* Main just calls the tests.
 * Test result is 0(=success) if as expected
 */
{

  /* (beware of simultaneously running tests that write to the same files) */

  musicdemo();

  testAUBeep (1, DONTCARE);
  sleep (1);
  testAUBeep (1, DONTCARE);
  sleep (1);

  printf ("[Three tones]\n");
  testAUTone (1, 0);
  testAUTone (2, 0);
  testAUTone (1, 0);

  printf ("[Long tone:]\n");
  testAUTone (3, 0);


/*
  printf ("[Testing while nothing plays/records:]\n");
  testAUCheckSample (1, 1);
  testAUCheckRecord (1, 1);

  printf ("[Playing sample and testing:]\n");
  testAUPlaySample (1, DONTCARE);
  testAUCheckSample (1, 0);
  testAUCheckRecord (1, 1);
  testAUCheckSample (2, DONTCARE);
  testAUCheckSample (1, 1);
  testAUCheckRecord (1, 1);

  printf ("[Recording sample and testing:]\n");
  testAURecordSample (1, DONTCARE);
  testAUCheckSample (1, 1);
  testAUCheckRecord (1, 0);
  testAUCheckRecord (2, DONTCARE);
  testAUCheckSample (1, 1);
  testAUCheckRecord (1, 1);

  printf ("[Playing file and testing:]\n");
  testAUPlayFile (1, DONTCARE);
  testAUCheckSample (1, 0);
  testAUCheckRecord (1, 1);
  testAUCheckSample (2, DONTCARE);
  testAUCheckSample (1, 1);
  testAUCheckRecord (1, 1);

  printf ("[Recording file and testing:]\n");
  testAURecordFile (1, DONTCARE);
  testAUCheckSample (1, 1);
  testAUCheckRecord (1, 0);
  testAUCheckRecord (2, DONTCARE);
  testAUCheckSample (1, 1);
  testAUCheckRecord (1, 1);

  printf ("[Playing file collision:]\n");
  testAUPlayFile (1, DONTCARE);
  testAUCheckSample (1, 0);
  testAUPlayFile (1, DONTCARE);
  testAUCheckSample (1, 0);
  testAUCheckSample (2, DONTCARE);

  printf ("[Testing while nothing plays/records:]\n");
  testAUCheckSample (1, 1);
  testAUCheckRecord (1, 1);

*/

  /* we need to wait a while so the threads can finish... */
  printf("...zzzZZZzzz...\n");
  sleep(1);

  return 0;
}

/* -------------------------------------------------------------------------- */
int testAUPlaySample (int config, int expect)
/* Current status:
 * config not evaluated
 * retrieves the sample from a file
 */
{
  const int error_code = -1;
  const char *file_name = "./testplay.wav";     /* of the sample file */
  int file_descriptor = -1;     /* of the sample file */
  char *buffer;                 /* buffer to transfer between memory and files */
  long buffer_size = 0;
  long bytes_read = 0;
  int func_return = 0;
  if ((file_descriptor = open (file_name, O_RDONLY)) == -1)
    {
      perror (file_name);
      return error_code;
    }

  buffer_size = (long) lseek (file_descriptor, 0, SEEK_END);
  if ((buffer = malloc (buffer_size)) == NULL)
    {
      fprintf (stderr,
               "Unable to allocate playback buffer (%ld)\n", buffer_size);
      return error_code;
    }

  lseek (file_descriptor, 0, SEEK_SET);
  bytes_read = read (file_descriptor, buffer, buffer_size);
  if (bytes_read < 0)
    {
      perror (file_name);
      free (buffer);
      return error_code;
    }
  else if (bytes_read <= 44)
    {
      printf ("no valid RIFF/WAV/PCM sample\n");
      free (buffer);
      return error_code;
    }
  close (file_descriptor);
  /*printf("File read, now sending to play...\n");*/
  func_return = AUPlaySample (buffer, bytes_read);
  free (buffer);
  evaluate_test ("testAUPlaySample()", 0, expect);
  return 0;
}

/* -------------------------------------------------------------------------- */
int testAURecordSample (int config, int expect)
/* Current status:
 * config not evaluated
 * return not evaluated
 * writes the sample to a file
 */
{
  const int error_code = -1;
  const char *file_name = "./testrec-s.wav";    /* of the sample file */
  int file_descriptor = -1;     /* of the sample file */
  char *buffer = "";
  long buffer_size = 1 << 17;   /* 128 KiB */
  long bytes_read = 0;
  long bytes_written = 0;
  if ((buffer = malloc (buffer_size)) == NULL)
    {
      fprintf (stderr,
               "Unable to allocate capture buffer (%ld)\n", buffer_size);
      return error_code;
    }

  AURecordSample (buffer, buffer_size);
  if ((file_descriptor = creat (file_name, 0664)) == -1)
    {
      fprintf (stderr, "Cannot create recording file '%s'\n", file_name);
      perror (file_name);
      free (buffer);
      return error_code;
    }

  if ((bytes_written = write (file_descriptor, buffer, buffer_size)) <= 0)
    {
      fprintf (stderr, "Error,  bytes_written = '%ld'\n", bytes_read);
      perror (file_name);
      close (file_descriptor);
      free (buffer);
      return error_code;
    }

  close (file_descriptor);
  free (buffer);
  evaluate_test ("testAURecordSample()", 0, expect);
  return 0;
}

/* -------------------------------------------------------------------------- */
int testAUPlayFile (int config, int expect)
/* Current status:
 * return not evaluated
 */
{
  char *thisf = "testAUPlayFile()";
  char *file = "";
  switch (config)
    {
    case 1:                    /*  */
      file = "./testplay.wav";
      break;
    case 2:                    /*  */
      file = "./testplay-long.wav";
      break;
    default:
      fprintf (stderr, "%s: Unsuported config '%d'\n", thisf, config);
    }
  AUPlayFile (file);
  evaluate_test ("testAUPlayFile()", 0, expect);
  return 0;
}

/* ------------------------------------------------------------------------- */
int testAURecordFile (int config, int expect)
/* Current status:
 * config not evaluated
 * return not evaluated
 */
{
  char *file = "/tmp/testrec.wav";
  long len = (1 << 18);         /* 256 KiB */
  long freq = 44100;
  AURecordFile (file, len, freq);
  evaluate_test ("testAURecordFile()", 0, expect);
  return 0;
}

/* ------------------------------------------------------------------------- */
int testAUCheckRecord (int config, int expect)
/* Current status:
 */
{
  char *thisf = "testAUCheckRecord()";
  int func_return = 0;
  switch (config)
    {
    case 1:                    /* check once, be quiet */
      func_return = AUCheckRecord ();
      break;
    case 2:                    /* check  until free, (print elapsed time) */
      func_return = AUCheckRecord ();
      time_t time_0 = time (NULL);
      while (func_return == 0)  /* 0 means recording */
        {
          sleep (1);
          func_return = AUCheckRecord ();
          if (verbose)
            fprintf (stdout, "%s: Check returned '%d'\n", thisf, func_return);
        }
      if (verbose)
        fprintf (stdout, "%s: tried for '%0.f' seconds\n", thisf,
                 difftime (time (NULL), time_0));
      break;
    default:
      fprintf (stderr, "%s: Unsuported config '%d'\n", thisf, config);
    }

  evaluate_test (thisf, func_return, expect);
  return 0;
}

/* ------------------------------------------------------------------------- */
int testAUCheckSample (int config, int expect)
/* Current status:
 */
{
  char *thisf = "testAUCheckSample()";
  int func_return = 0;
  switch (config)
    {
    case 1:                    /* check once, be quiet */
      func_return = AUCheckSample ();
      break;
    case 2:                    /* check until free, (print elapsed time) */
      func_return = AUCheckSample ();
      time_t time_0 = time (NULL);
      while (func_return == 0)  /* 0 means recording */
        {
          sleep (1);
          func_return = AUCheckSample ();
          if (verbose)
            fprintf (stdout, "%s: Check returned '%d'\n", thisf, func_return);
        }
      if (verbose)
        fprintf (stdout, "%s: tried for '%0.f' seconds\n", thisf,
                 difftime (time (NULL), time_0));
      break;
    default:
      fprintf (stderr, "%s: Unsuported config '%d'\n", thisf, config);
    }

  evaluate_test (thisf, func_return, expect);
  return 0;
}

/* -------------------------------------------------------------------------- */
int testAUTone (int config, int expect)
/* Current status:
  */
{
  char *thisf = "testAUTone()";
  int func_return = 0;

  switch (config)
    {
    case 1:                    /* a */
      func_return = AUTone (600, 1000);
      break;
    case 2:                    /* higher, shorter */
      func_return = AUTone (800, 500);
      break;
    case 3:                    /* low, long */
      func_return = AUTone (1000, 5000);
      break;
    default:
      fprintf (stderr, "%s: Unsuported config '%d'\n", thisf, config);
    }

  evaluate_test (thisf, func_return, expect);
  return 0;
}

/* -------------------------------------------------------------------------- */
int testAUCheckTone (int config, int expect)
/* Current status:
 */
{
  char *thisf = "testAUCheckTone()";
  int func_return = 0;
  switch (config)
    {
    case 1:                    /* check once, be quiet */
      func_return = AUCheckTone ();
      break;
    case 2:                    /* check every second until free, print elapsed time */
      func_return = AUCheckTone ();
      time_t time_0 = time (NULL);
      while (func_return == 0)  /* 0 means recording */
        {
          sleep (1);
          func_return = AUCheckTone ();
        }
      if (verbose)
        fprintf (stdout, "%s: tried for '%0.f' seconds\n", thisf,
                 difftime (time (NULL), time_0));
      break;
    default:
      fprintf (stderr, "%s: Unsuported config '%d'\n", thisf, config);
    }

  evaluate_test (thisf, func_return, expect);
  return 0;
}

/* -------------------------------------------------------------------------- */
int testAUBeep (int config, int expect)
/* Current status:
 * config not evaluated
 */
{
  AUBeep ();
  evaluate_test ("testAUBeep()", 0, expect);
  return 0;
}

/* ------------------------------------------------------------------------- */
int testAUCaptureMic (int config, int expect)
/* Current status:
 * config not evaluated
 */
{
  AUCaptureMic ();
  evaluate_test ("testAUCaptureMic()", 0, expect);
  return 0;
}


/* ------------------------------------------------------------------------- */
int evaluate_test (char *title, int value, int expected_value)
{
  if (expected_value == DONTCARE)
    {
      fprintf (stdout, "   %s executed.\n", title);
    }
  else
    {
      if (value == expected_value)
        {
          fprintf (stdout, "   %s successful.\n", title);
        }
      else
        {
          fprintf (stdout, "!!!%s failed.\n", title);
        }
    }
  return 0;
}


/* ========================================================================= */



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

  playtune (&scale[0]);

  sleep(5);
  playtune (&melody1[0]);


/*
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

*/
  return (0);
}


int playtune (tone * tune)
{ int i = 0;
  //int k = KEY1;

  while ((tune[i].freq != 0) && (tune[i].len != 0) )
  { AUTone ((tune[i].freq), tune[i].len);

	//while (!AUCheckTone ()) { }
    i++;
    //k = KEYRead ();

  }
  return (0);
}

