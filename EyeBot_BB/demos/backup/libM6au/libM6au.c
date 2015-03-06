/**
 * \file libM6au.c
 *
 * Source code of the EyebotM6 library for audio routines.
 *
 * Implemets the audio routines of the EyebotM6 "RoBIOS" libray libM6.
 *
 * For more information see
 * http://robotics.ee.uwa.edu.au/eyebot/doc/API/library.html
 *
 * \date 2007
 * \author Thomas Sommer
 * \version 0.50
 *
 */

#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/time.h>

#include "libM6au-types.h"
#include "libM6au.h"
#include "libM6au-intern.h"

#define LIBM6_AU_DEVICE			"/dev/dsp"
//#define LIBM6_AU_SAMPLE_RATE		11025
//#define LIBM6_AU_SAMPLE_BITS		8
#define LIBM6_AU_SAMPLE_RATE		44100
#define LIBM6_AU_SAMPLE_BITS		16

#define LIBM6_AU_SAMPLE_CHANNELS	1
#define LIBM6_AU_SAMPLE_FORMAT		1
#define LIBM6_AU_SAMPLE_ALIGN		1

#define MICSAMPLESIZE 10

/* globals: */

/* Audio device states: */
int m6_state_playing = 0;
int m6_state_recording = 0;
int m6_state_beeping = 0;
int m6_terminate_copythread = 0;
/* Arguments for the feed threads: */
copy_sample_args_t m6_play_sample_args;
copy_sample_args_t m6_record_sample_args;
copy_file_args_t m6_play_file_args;
copy_file_args_t m6_record_file_args;
play_tone_args_t m6_play_tone_args;
/* Protectors of above variables: */
pthread_mutex_t m6_playing_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m6_recording_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m6_beeping_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m6_termcopy_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ------------------------------------------------------------------------- */
int AUPlaySample (char *sample, long length)
/**
 * Plays a sound sample. The actual writing to the audio device is handled
 * in a separate thread, so the playback is non-blocking.
 * The function will fail if another sample is played (via the libM6au
 * routines).
 * The audio parameters are fixed to: 8 bit, mono, 11025 sample rate, pcm.
 * \param sample        defined as the contents of a canonical WAV/PCM file
 *    			(for details, see libM6au-types.h)
 * \param length	length of the sample in bytes.
 * \return              '0' on successfully preparing playback - the actual
 *                      writing may fail, though. '-1' on failure.
 * \todo
 *  	more meaningful return values,
 *  	document exact sound capabilities (formats,...),
 */
{
  const int error_code = -1;
  const char *device_name = LIBM6_AU_DEVICE;
  const char *this_code = "AUPlaySample()";
  int device_descriptor;
  pthread_t thread;
  copy_sample_args_t *args;     /* arguments for thread */
  header_t params;              /* header data as a structure */

  check_header (sample, length);
  extract_header_params (&params, sample);
  /*print_header_params (&params); */

  pthread_mutex_lock (&m6_playing_mutex);
  if (m6_state_playing)
    {
      pthread_mutex_unlock (&m6_playing_mutex);
      fprintf (stderr, "%s: Already playing other sample - aborted!\n",
               this_code);
      return error_code;
    }
  else
    {
      m6_state_playing = 1;
      pthread_mutex_unlock (&m6_playing_mutex);
    }

  if ((device_descriptor = open (device_name, O_WRONLY)) == -1)
    {
      pthread_mutex_lock (&m6_playing_mutex);
      m6_state_playing = 0;
      pthread_mutex_unlock (&m6_playing_mutex);
      perror (device_name);
      return error_code;
    }

  if (set_audio_device (device_descriptor, &params) == -1)
    {
      pthread_mutex_lock (&m6_playing_mutex);
      m6_state_playing = 0;
      pthread_mutex_unlock (&m6_playing_mutex);
      perror (device_name);
      close (device_descriptor);
      return error_code;
    }

  /* access to global is ok, since nobody will access it without activating
   * the state first, and the state is mutex-protected ... */
  args = &m6_play_sample_args;
  args->fd = device_descriptor;
  args->buf = (void *) (sample + (12 + 24 + 8));
  args->count = params.data_size;
  args->mutex_p = &m6_playing_mutex;
  args->state_p = &m6_state_playing;

  pthread_create (&thread, NULL, play_sample_thread, (void *) (args));

  return 0;                     /* this says nothing about whether the
                                 * actual playback was successful! */
}

/* ------------------------------------------------------------------------- */
int AURecordSample (char *buffer, long length)
/**
 * Records a sound sample into the provided buffer.
 * The audio parameters are fixed to: 8 bit, mono, 11025 sample rate, pcm.
 * (see top of this file)
 * \param buffer        memory region that receives the sample, which is
 *                      defined as the contents of a canonical WAV/PCM file,
 *    			(for details, see libM6au-types.h)
 * \param length        bytes to sample, including the header (44 bytes).
 * \return              '0' on successfully preparing recording - the actual
 *                      recording may fail, though. '-1' on failure.
 * \todo
 *      hardcoded filenames
 *	think about an easy way to set up the quality if needed:
 *	(some combination of sampling rate, sample size and channels)
 */
{
  const char *this_code = "AURecordSample()";
  const char *device_name = LIBM6_AU_DEVICE;
  const int error_code = -1;
  int device_descriptor;
  //char header[12 + 24 + 8];
  pthread_t thread;
  copy_sample_args_t *args;     /* arguments for thread */
  header_t params;              /* header data as a structure */

  pthread_mutex_lock (&m6_recording_mutex);
  if (m6_state_recording)
    {
      pthread_mutex_unlock (&m6_recording_mutex);
      fprintf (stderr, "%s: Already recording other sample - aborted!\n",
               this_code);
      return error_code;
    }
  else
    {
      m6_state_recording = 1;
      pthread_mutex_unlock (&m6_recording_mutex);
    }

  if ((device_descriptor = open (device_name, O_RDONLY)) == -1)
    {
      pthread_mutex_lock (&m6_recording_mutex);
      m6_state_recording = 0;
      pthread_mutex_lock (&m6_recording_mutex);
      perror (device_name);
      return error_code;
    }

  params.audio_format = LIBM6_AU_SAMPLE_FORMAT;
  params.channels = LIBM6_AU_SAMPLE_CHANNELS;
  params.sample_rate = LIBM6_AU_SAMPLE_RATE;
  params.block_align = LIBM6_AU_SAMPLE_ALIGN;
  params.bits_per_sample = LIBM6_AU_SAMPLE_BITS;
  params.byte_rate = params.sample_rate * 8 / params.bits_per_sample;
  params.riff_size = length - 8;
  params.fmt_size = 24 - 8;
  params.data_size = length - (12 + 24) - 8;
  //create_header (header, &params);
  //print_header_params (&params);
  //memcpy (buffer, header, 12 + 24 + 8);

  if (set_audio_device (device_descriptor, &params) != 0)
    {
      pthread_mutex_lock (&m6_recording_mutex);
      m6_state_recording = 0;
      pthread_mutex_lock (&m6_recording_mutex);
      perror (device_name);
      close (device_descriptor);
      return error_code;
    }

  /* access to global is ok, since nobody will access it without activating
   * the state first, and the state is mutex-protected ... */
  args = &m6_record_sample_args;
  args->fd = device_descriptor;
  args->buf = (void *) (buffer);
  args->count = length;
  args->mutex_p = &m6_recording_mutex;
  args->state_p = &m6_state_recording;

  pthread_create (&thread, NULL, record_sample_thread, (void *) (args));

  return 0;                     /* this says nothing about whether the
                                 * actual recording was successful! */
}

/* ------------------------------------------------------------------------- */
int AUPlayFile (char *file)
/**
 * Plays a sound sample from a file.
 * This is no wrapper for AUPlaySample(), as the data transfer is buffered.
 * \param file          file name the data is read from.
 * 			(canonical WAV/PCM)
 * \return              '0' on successfully preparing playback - the actual
 *                      writing may fail, though. '-1' on failure.
 * \todo
 * 	document exact sound capabilities (formats,...),
 *      hardcoded filenames,
 *      determine good buffer size
 */
{
  const char *this_code = "AUPlayFile()";
  const int error_code = -1;
  const char *device_name = LIBM6_AU_DEVICE;
  pthread_t thread;
  copy_file_args_t *args;       /* arguments for thread */
  header_t params;              /* header data as a structure */
  const int buffer_size = 1024; /* buffer to transfer between file and dev */
  char *file_name = file;       /* of the sample file */
  int file_descriptor;          /* of the sample file */
  int device_descriptor;
  int bytes_read;
  const int header_length = 12 + 24 + 8;
  char header[header_length];
  off_t file_length;

  /* file first as this does not affect audio device: */
  if ((file_descriptor = open (file_name, O_RDONLY)) == -1)
    {
      perror (file_name);
      return error_code;
    }

  bytes_read = read (file_descriptor, header, header_length);
  if (bytes_read < header_length)
    {
      fprintf (stderr, "%s: Could only read %d bytes of %s - aborted!\n",
               this_code, bytes_read, file);
      close (file_descriptor);
      return error_code;
    }

  file_length = lseek (file_descriptor, 0, SEEK_END);
  if (check_header (header, file_length) != 0)
    {
      fprintf (stderr, "%s: Unsupported header in %s  - aborted!\n",
               this_code, file);
      close (file_descriptor);
      return error_code;
    }

  extract_header_params (&params, header);

  pthread_mutex_lock (&m6_playing_mutex);
  if (m6_state_playing)
    {
      pthread_mutex_unlock (&m6_playing_mutex);
      fprintf (stderr, "%s: Already playing other sample - aborted!\n",
               this_code);
      close (file_descriptor);
      return error_code;
    }
  else
    {
      m6_state_playing = 1;
      pthread_mutex_unlock (&m6_playing_mutex);
    }

  if ((device_descriptor = open (device_name, O_WRONLY)) == -1)
    {
      pthread_mutex_lock (&m6_playing_mutex);
      m6_state_playing = 0;
      pthread_mutex_unlock (&m6_playing_mutex);
      perror (device_name);
      close (file_descriptor);
      return error_code;
    }

  if (set_audio_device (device_descriptor, &params) != 0)
    {
      pthread_mutex_lock (&m6_recording_mutex);
      m6_state_recording = 0;
      pthread_mutex_lock (&m6_recording_mutex);
      perror (device_name);
      close (device_descriptor);
      close (file_descriptor);
      return error_code;
    }

  pthread_mutex_lock (&m6_termcopy_mutex);
  m6_terminate_copythread = 0;
  pthread_mutex_unlock (&m6_termcopy_mutex);

  /* the copy funcion expects this: */
  lseek (file_descriptor, header_length, SEEK_SET);

  /* access to global is ok, since nobody will access it without activating
   * the state first, and the state is mutex-protected ... */
  args = &m6_play_file_args;
  args->source_fd = file_descriptor;
  args->target_fd = device_descriptor;
  args->buffer_size = buffer_size;
  args->count = params.data_size;
  args->mutex_p = &m6_playing_mutex;
  args->state_p = &m6_state_playing;
  args->mutexterm_p = &m6_termcopy_mutex;
  args->term_p = &m6_terminate_copythread;

  pthread_create (&thread, NULL, copy_file_thread, (void *) (args));

  return 0;                     /* this says nothing about whether the
                                 * actual playback was successful! */
}

/* ------------------------------------------------------------------------- */
int AURecordFile (char *file, long length, long sample_rate)
/**
 * Records a sound sample.
 * This is no wrapper for AURecordFile(), as here the data transfer is
 * buffered.
 * \param file          file name the data is written to. (canonical WAV/PCM)
 *                      implicit settings: 16 bit, 1 channel
 * \param len           bytes to sample, including the header (44 bytes).
 * \param sample_rate   samples per second.
 * \return              '0' on successfully preparing recording - the actual
 *                      recording may fail, though. '-1' on failure.
 * \todo
 * 	test actual recording results,
 *      maybe redesign interface (duration is more user oriented than len,
 *      why freq and not channels,bits...),
 *      usually freq cannot be set exactly - signal this to the user in
 *      some friendly way or allow some margin
 *      determine good buffer size
 */
{
  const char *this_code = "AUPRecordFile()";
  const int error_code = -1;
  const char *device_name = LIBM6_AU_DEVICE;
  pthread_t thread;
  copy_file_args_t *args;       /* arguments for thread */
  header_t params;              /* header data as a structure */
  const int buffer_size = 1024; /* buffer to transfer between file and dev */
  char *file_name = file;       /* of the sample file */
  int file_descriptor;          /* of the sample file */
  int device_descriptor;
  const int header_length = 12 + 24 + 8;
  char header[header_length];

  /* check passed parameters: */
  if (length < header_length)
    {
      fprintf (stderr, "%s: Parameter 'length'=%ld too small - aborted!\n",
               this_code, length);
      close (file_descriptor);
      return error_code;
    }
  if ((sample_rate <= 20) || (sample_rate >= 48000))
    {
      fprintf (stderr, "%s: Parameter 'sample_rate'=%ld rejected - aborted!\n",
               this_code, sample_rate);
      close (file_descriptor);
      return error_code;
    }

  params.audio_format = LIBM6_AU_SAMPLE_FORMAT;
  params.channels = LIBM6_AU_SAMPLE_CHANNELS;
  params.sample_rate = sample_rate;
  params.block_align = LIBM6_AU_SAMPLE_ALIGN;
  params.bits_per_sample = LIBM6_AU_SAMPLE_BITS;
  params.byte_rate = params.sample_rate * 8 / params.bits_per_sample;
  params.riff_size = length - 8;
  params.fmt_size = 24 - 8;
  params.data_size = length - (12 + 24) - 8;
  create_header (header, &params);
  //print_header_params (&params);

  pthread_mutex_lock (&m6_recording_mutex);
  if (m6_state_recording)
    {
      pthread_mutex_unlock (&m6_recording_mutex);
      fprintf (stderr, "%s: Already recording other sample - aborted!\n",
               this_code);
      close (file_descriptor);
      return error_code;
    }
  else
    {
      m6_state_recording = 1;
      pthread_mutex_unlock (&m6_recording_mutex);
    }

  if ((file_descriptor = creat (file_name, 0664)) == -1)
    {
      pthread_mutex_lock (&m6_recording_mutex);
      m6_state_recording = 0;
      pthread_mutex_unlock (&m6_recording_mutex);
      perror (file_name);
      return error_code;
    }

  if (write (file_descriptor, header, header_length) == -1)
    {
      pthread_mutex_lock (&m6_recording_mutex);
      m6_state_recording = 0;
      pthread_mutex_unlock (&m6_recording_mutex);
      perror (device_name);
      close (file_descriptor);
      perror ("output file");
      return error_code;
    }

  if ((device_descriptor = open (device_name, O_RDONLY)) == -1)
    {
      pthread_mutex_lock (&m6_recording_mutex);
      m6_state_recording = 0;
      pthread_mutex_unlock (&m6_recording_mutex);
      perror (device_name);
      close (file_descriptor);
      return error_code;
    }

  if (set_audio_device (device_descriptor, &params) != 0)
    {
      pthread_mutex_lock (&m6_recording_mutex);
      m6_state_recording = 0;
      pthread_mutex_lock (&m6_recording_mutex);
      perror (device_name);
      close (device_descriptor);
      close (file_descriptor);
      return error_code;
    }

  /* the copy funcion expects this: */
  /* should be there already: */
  /*lseek (file_descriptor, header_length, SEEK_SET);*/

  pthread_mutex_lock (&m6_termcopy_mutex);
  m6_terminate_copythread = 0;
  pthread_mutex_unlock (&m6_termcopy_mutex);

  /* access to global is ok, since nobody will access it without activating
   * the state first, and the state is mutex-protected ... */
  args = &m6_record_file_args;
  args->source_fd = device_descriptor;
  args->target_fd = file_descriptor;
  args->buffer_size = buffer_size;
  args->count = params.data_size;
  args->mutex_p = &m6_recording_mutex;
  args->state_p = &m6_state_recording;
  args->mutexterm_p = &m6_termcopy_mutex;
  args->term_p = &m6_terminate_copythread;
  pthread_create (&thread, NULL, copy_file_thread, (void *) (args));

  return 0;                     /* this says nothing about whether the
                                 * actual recording was successful! */
}

/* ------------------------------------------------------------------------- */
int AUCheckSample (void)
/**
 * Checks whether there is an ongoing playback.
 * Will only detect playback by AUPlay* routines.
 * \return              '0' if playback takes place, '1' if not.
 * \todo
 *       maybe name in a way that correlates to the result:
 *       AUCheckNotPlaying() or AUCheckSampleEnd()
 */
{
  int state;

  pthread_mutex_lock (&m6_playing_mutex);
  state = m6_state_playing;
  pthread_mutex_unlock (&m6_playing_mutex);

  return !state;
}

/* ------------------------------------------------------------------------- */
int AUCheckRecord (void)
/**
 * Checks whether there is an ongoing recording.
 * Will only detect recording by AURecord* routines.
 * \return              '0' if recording takes place, '1' if not.
 * \todo
 *       maybe name in a way that correlates to the result:
 *       AUCheckNotRecording() or AUCheckRecordEnd()
 */
{
  int state;

  pthread_mutex_lock (&m6_recording_mutex);
  state = m6_state_recording;
  pthread_mutex_unlock (&m6_recording_mutex);

  return !state;
}

/* ------------------------------------------------------------------------- */
int AUCheckTone (void)
/**
 * Checks whether there is an ongoing beep or tone.
 * Will only detect playback by AUTone and AUBeep routines.
 * \return              '0' if recording takes place, '1' if not.
 * \todo
 *       maybe name in a way that correlates to the result:
 *       AUCheckNotBeeping() or AUCheckBeepEnd()
 */
{
  int state;

  pthread_mutex_lock (&m6_beeping_mutex);
  state = m6_state_beeping;
  pthread_mutex_unlock (&m6_beeping_mutex);

  return !state;
}


void AUStopPlayRec(void)
/**
 * Terminate an ongoing copy file thread
 */
{
  	pthread_mutex_lock (&m6_termcopy_mutex);
	m6_terminate_copythread = 1;
  	pthread_mutex_unlock (&m6_termcopy_mutex);
}

/* ------------------------------------------------------------------------- */
int AUTone (int freq, int msec)
/**
 * Generates a tone.
 *
 * \param freq          tone frequency in Hz.
 * \param msec          tone duration in ms.
 * \return              '0' on success
 *                      '-1' on failure.
 * \todo
 */
{
  const int error_code = -1;
  const char *this_code = "AUTone()";
  const char *device_name = "/dev/dsp";     /* be root for this */
  int device_descriptor = -1;
  header_t params;              /* header data as a structure */

  int bufsize;
  int p=0;
  double phase = 0.0;
  short *buf;
  double step;
  int tcnt;

  if ((device_descriptor = open (device_name, O_WRONLY)) == -1)
  {
    perror (device_name);
    return error_code;
  }

  params.audio_format = LIBM6_AU_SAMPLE_FORMAT;
  params.channels = LIBM6_AU_SAMPLE_CHANNELS;
  params.sample_rate = LIBM6_AU_SAMPLE_RATE;
  params.block_align = LIBM6_AU_SAMPLE_ALIGN;
  params.bits_per_sample = LIBM6_AU_SAMPLE_BITS;
  params.byte_rate = params.sample_rate * 8 / params.bits_per_sample;

  if (set_audio_device (device_descriptor, &params) != 0)
  {
    perror (device_name);
    close (device_descriptor);
    return error_code;
  }

  bufsize = (LIBM6_AU_SAMPLE_RATE+1)/freq;
  if ( (buf=malloc(bufsize*sizeof(short))) == NULL)
  {
    fprintf (stderr, "%s: Unable to allocate play tone buffer\n",this_code);
    close (device_descriptor);
  	return error_code;
  }

  step = 2.0 * M_PI * (double)freq / (double)LIBM6_AU_SAMPLE_RATE;
  //printf("freq=%d, bufsize=%d, step=%8.2lf\n", freq, bufsize, step);

  while (p < bufsize)
  {
  	buf[p++] = (short) (sin(phase) * 10000.0);
    phase += step;
  }

  tcnt = msec*LIBM6_AU_SAMPLE_RATE/(1000*bufsize);
  while (tcnt-- > 0)
  {
    if (write(device_descriptor, buf, bufsize*(LIBM6_AU_SAMPLE_BITS/8)) != bufsize*(LIBM6_AU_SAMPLE_BITS/8) )
  	{
      fprintf (stderr, "%s: Could not write to sound device\n",this_code);
      close (device_descriptor);
  	  free(buf);
  	  return error_code;
  	}
  }

  free(buf);
  close(device_descriptor);

  return 0;                 /* says nothing about whether play was successful */

}

/* ------------------------------------------------------------------------- */
int AUBeep (void)
/**
 * Generates a short tone (beep).
 *
 * \return              '0' always.
 * \todo test,
 */
{
  /* provided we have a console: */
  printf ("\a");
  /* or use AUTone() to avoid user preferences... */
  return 0;
}

/* ------------------------------------------------------------------------- */
int AUCaptureMic (void)
/**
 * Grab current microphone input value.
 * Get several audio samples from microphone and return the average value
 */
{
  const char *this_code = "AUCaptureMic()";
  int mic_value = 0;
  long acc_mic_value = 0;
  char *buffer = "";
  int i;

  	if ((buffer = malloc (MICSAMPLESIZE)) == NULL) {
    	fprintf (stderr, "%s: Create buffer error!\n", this_code);
		return -1;
	}

	if (AURecordSample (buffer, MICSAMPLESIZE) != 0) {
    	fprintf (stderr, "%s: Get sample error!\n", this_code);
		return -1;
	}
	while (!AUCheckRecord()) ;

	for ( i=0;i<MICSAMPLESIZE;i++ )
		acc_mic_value += (long)*buffer++;

	mic_value = (int)(acc_mic_value/MICSAMPLESIZE);

  	return mic_value;
}


