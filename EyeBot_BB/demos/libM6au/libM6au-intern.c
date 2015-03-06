/**
 * \file libM6au-intern.c
 *
 * EyebotM6 library for audio routines internal header file.
 *
 * Header file for common routines of the official "RoBIOS" library routines.
 * Only those appear in "libM6au" for clarity.
 *
 * For more information see
 * http://robotics.ee.uwa.edu.au/eyebot/doc/API/library.html
 *
 *
 * \todo
 * 	(see implementation)
 *
 * \date 2006
 * \author Thomas Sommer
 * \version 0.50
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/kd.h>

#include "libM6au-types.h"
#include "libM6au-intern.h"

#define M6AU_DEV_DEBUG 0        /* prints additional information */

extern copy_sample_args_t m6_play_sample_args;
extern copy_sample_args_t m6_record_sample_args;
extern copy_file_args_t m6_play_file_args;
extern copy_file_args_t m6_record_file_args;


/* ------------------------------------------------------------------------- */
int le2int (char *char_list, int length)
/**
 * Interprets a list of characters as a little-endian integer.
 * \param char_list	An array of characters (not a string!) that contains
 * 			a number in little endian order.
 * \param lenght	The length char_list.
 * \return		The integer value of the interpreted data.
 * \todo
 *	error handling is too crude...
 */
{
  int i, val;
  val = 0;

  if (sizeof (int) < length)
    {
      fprintf (stderr, "le2int(): The integer type cannot hold %d bytes!\n",
               length);
      exit (-1);
    }

  for (i = length - 1; i >= 0; i--)
    val = (val << 8) | char_list[i];

  return val;
}


/* ------------------------------------------------------------------------- */
/*int be2int (char *char_list, int length)*/
/* interprets a list of characters as a big-endian integer */
/*
{
  int i, val;
  val = 0;

  for (i = 0; i < length; i++)
    val = (val << 8) | char_list[i];
  return val;
}
*/

/* ------------------------------------------------------------------------- */
int int2le (int val, int length, char *char_list)
/**
 * Generates the little endian representation of an integer
 * \param val		the integer value to be converted.
 * \param lenght	The length char_list.
 * \param char_list	An array of characters (not a string!) that contains
 * 			a the result in little endian order.
 * \return		0 always
 * \todo
 *	error handling is too crude...
 */
{
  int i;

  if (sizeof (int) < length)
    {
      fprintf (stderr, "int2le(): The integer type cannot hold %d bytes!\n",
               length);
      exit (-1);
    }

  for (i = 0; i < length; i++)
    char_list[i] = (val >> (i * 8)) & 255;
  return 0;
}


/* ------------------------------------------------------------------------- */
/*int int2be (int val, int length, char *char_list)*/
/* gives the big endian representation of an integer */
/*
{
  int i;

  for (i = 0; i < length; i++)
    char_list[i] = (val >> (length - 1 - i)) & 255;
  return 0;
}
*/

/* ------------------------------------------------------------------------- */
int set_audio_device (int device_fd, header_t * params)
/**
 * Sets the audio device to the given parameters. The device has to be open
 * already.
 * The audio device is set as well as possible. However the settings are
 * not guaranteed!
 * \param device_fd	File descriptor of the audio device.
 * \param params	Structure containing the parameters. For simplicity,
 * 			a structure containing RIFF-WAV-PCM header parameters
 * 			is passed, even though some of its members are not
 * 			audio parameters (like the lenght of the file...).
 * \return		0 on success
 * \todo
 *	better pass a specific structure
 *	handle wrong settings better
 */
{
  const int error_code = -1;


  if (request_ioctl
	  (device_fd, SNDCTL_DSP_CHANNELS, "SNDCTL_DSP_CHANNELS",
       				params->channels) == -1)
    return error_code;

  if (request_ioctl
	  (device_fd, SNDCTL_DSP_SPEED, "SNDCTL_DSP_SPEED",
       				params->sample_rate) == -1)
    return error_code;

  if (request_ioctl (device_fd, SNDCTL_DSP_SETFMT, "SNDCTL_DSP_SETFMT",
                     params->bits_per_sample) == -1)
    return error_code;

  return 0;
}

/* ------------------------------------------------------------------------- */
int request_ioctl (int device_fd, int request_code, char *request_title,
                   int value)
/**
 * Perfoms an ioctl on a device and compares the result with the request.
 * \param device_fd	file descriptor of the device.
 * \param request_code	ioctl code of the request.
 * \param request_title a string representation of the request code.
 * \param value		a value for the request.
 * \return		0 on success, -1 on failure.
 * \todo:
 * 	handle verbose mode better
 */
{
  int granted_value = value;
  const int error_code = -1;

  if (ioctl (device_fd, request_code, &granted_value) == -1)
    {
      fprintf (stderr, "Unable to fullfill %s:='%i'\n", request_title, value);
      perror ("ioctl");
      return error_code;
    }
  else if (granted_value != value)
    {
      fprintf (stderr, "Request %s:='%i' resulted in '%i'\n",
               request_title, value, granted_value);
      perror ("ioctl");
      return error_code;
    }
#if M6AU_DEV_DEBUG
  else
    fprintf (stdout, "Request %s:='%i' granted.\n", request_title,
             granted_value);
#endif

  return 0;
}

/* ------------------------------------------------------------------------- */
void create_header (char *header, header_t * params)
/**
 * Creates a header from the data of the supplied structure.
 * Will write blindly - use check_header() to veryfy result.
 * \param header	address to write the header string to
 * 			(not null-terminated!)
 * \param params	structure containing the header data.
 * \todo
 * 	test
 */
{
  /*const char *this_code = "create_header()"; */
  char temp[4 + 1];
  char *fmt_chunk = header + 12;
  char *data_chunk = header + 12 + 24;

  sprintf (temp, "RIFF");
  memcpy (&header[0], &temp[0], 4);
  int2le (params->riff_size, 4, temp);
  memcpy (&header[4], &temp[0], 4);
  sprintf (temp, "WAVE");
  memcpy (&header[8], &temp[0], 4);

  sprintf (temp, "fmt ");
  memcpy (&fmt_chunk[0], &temp[0], 4);
  int2le (params->fmt_size, 4, temp);
  memcpy (&fmt_chunk[4], &temp, 4);
  int2le (params->audio_format, 2, temp);
  memcpy (&fmt_chunk[8], &temp, 2);
  int2le (params->channels, 2, temp);
  memcpy (&fmt_chunk[10], &temp, 2);
  int2le (params->sample_rate, 4, temp);
  memcpy (&fmt_chunk[12], &temp, 4);
  int2le (params->byte_rate, 4, temp);
  memcpy (&fmt_chunk[16], &temp, 4);
  int2le (params->block_align, 2, temp);
  memcpy (&fmt_chunk[20], &temp, 2);
  int2le (params->bits_per_sample, 2, temp);
  memcpy (&fmt_chunk[22], &temp, 2);

  sprintf (temp, "data");
  memcpy (&data_chunk[0], &temp[0], 4);
  int2le (params->data_size, 4, temp);
  memcpy (&data_chunk[4], &temp[0], 4);

#if M6AU_DEV_DEBUG
  int i;
  printf ("created this header:\n");
  for (i = 0; i < (12 + 24 + 8); i++)
    fprintf (stdout, " %i:'%c'=0x%X ", i, header[i], header[i]);
  printf ("\n");
#endif

  return;
}

/* ------------------------------------------------------------------------- */
void extract_header_params (header_t * header, char *file)
/**
 * Extracts parameters from the header of a canonical RIFF-WAV-PCM file
 * into a structure, so they can be processed more easily.
 * \param header	address of a structure to write the info to.
 * \param file		contents of a canonical RIFF-WAV-PCM file.
 * 			(see libM6au-types.h for details)
 * \todo
 */
{
  char *fmt_chunk;              /* address of the format chunk */
  char *data_chunk;             /* address of the data chunk */

  fmt_chunk = &file[12];        /* format chunck follows immediately */
  data_chunk = &file[12 + 24];  /* data chunck follows immediately */

  header->riff_size = le2int (&file[4], 4);

  header->fmt_size = le2int (&fmt_chunk[4], 4);
  header->audio_format = le2int (&fmt_chunk[8], 2);
  header->channels = le2int (&fmt_chunk[10], 2);
  header->sample_rate = le2int (&fmt_chunk[12], 4);
  header->byte_rate = le2int (&fmt_chunk[16], 4);
  header->block_align = le2int (&fmt_chunk[20], 2);
  header->bits_per_sample = le2int (&fmt_chunk[22], 2);

  header->data_size = le2int (&data_chunk[4], 4);

  return;
}

/* ------------------------------------------------------------------------- */
void print_header_params (header_t * header)
/**
 * Extracts parameters from the header of a canonical RIFF-WAV-PCM file
 * into a structure, so they can be processed more easily.
 * \param header	address of a structure to write the info to.
 * \param file		contents of a canonical RIFF-WAV-PCM file.
 * 			(see libM6au-types.h for details)
 * \return		0 on success, -1 on errors
 * \todo
 * 	test
 */
{
  printf ("riff_size = '%li'\n", header->riff_size);
  printf ("fmt_size = '%li'\n", header->fmt_size);
  printf ("data_size = '%li'\n", header->data_size);
  printf ("audio_format = '%i'\n", header->audio_format);
  printf ("channels = '%i'\n", header->channels);
  printf ("sample_rate = '%li'\n", header->sample_rate);
  printf ("byte_rate = '%li'\n", header->byte_rate);
  printf ("block_align = '%i'\n", header->block_align);
  printf ("bits_per_sample = '%i'\n", header->bits_per_sample);

  return;
}

/* ------------------------------------------------------------------------- */
int check_header (char *file, long length)
/**
 * Performs a few checks on a file header to see if it complies to the format
 * of a canonical RIFF-WAV-PCM file.
 * \param file		Contents of a file.
 * \param lenght	Lenght of the file.
 * \return		0 if header seems ok, -1 of it is certainly not
 * \todo
 *	check rates
 *	check lenght values
 * 	test
 */
{
  long hdr_file_length;         /* file length info in header */
  long hdr_data_length;         /* sample length info in header */
  const int error_code = -1;
  const int header_length = 12 + 24 + 8;
  char *this_code = "check_header()";
  char temp_str[4 + 1] = "";

#if M6AU_DEV_DEBUG
  int i;
  printf ("check_header(): header contents:\n");
  for (i = 0; i < 44; i++)
    fprintf (stdout, "%i:\t'%c' = \t0x%X\n", i, file[i], file[i]);
  for (i = 0; i < 44; i++)
    fprintf (stdout, "%c", file[i]);
  printf ("\n");
#endif

  /* Is header complete?: */
  if (length < (long) header_length)
    {
      fprintf (stderr, "%s: File too short: %ld bytes, required: %d.\n",
               this_code, length, header_length);
      return error_code;
    }

  /* Does file length match in RIFF chunk?: */
  hdr_file_length = (long) le2int (&file[4], 4) + 8;
  if (length != hdr_file_length)
    {
      fprintf (stderr,
               "%s: File length mismatch: %ld in RIFF-chunk, %ld in real.\n",
               this_code, hdr_file_length, length);
      return error_code;
    }

  /* RIFF?: */
  memcpy (&temp_str[0], &file[0], 4);
  temp_str[4] = '\0';
  hdr_file_length = (long) le2int (&file[4], 4);
  if (strcmp (temp_str, "RIFF"))
    {
      fprintf (stderr, "%s: Not a RIFF file: (%X %X %X %X).\n",
               this_code, temp_str[0], temp_str[1], temp_str[2], temp_str[3]);
      return error_code;
    }

  /* WAV?: */
  memcpy (&temp_str[0], &file[8], 4);
  temp_str[4] = '\0';
  hdr_file_length = (long) le2int (&file[4], 4);
  if (strcmp (temp_str, "WAVE"))
    {
      fprintf (stderr, "%s: Not a canonical WAV file: (%X %X %X %X).\n",
               this_code, temp_str[0], temp_str[1], temp_str[2], temp_str[3]);
      return error_code;
    }

  /* Does file length match in data chunk?: */
  hdr_data_length = (long) le2int (&file[12 + 24 + 4], 4);
  /* chunk lenght and thus file length must be even, data lenght not ... */
  hdr_file_length = (12 + 24 + 8) + hdr_data_length + (hdr_data_length & 1);
  if (length != hdr_file_length)
    {
      fprintf (stderr,
               "%s: File length mismatch: %ld in data-chunk, %ld in real.\n",
               this_code, hdr_file_length, length);
      return error_code;
    }

  return 0;
}

/* ------------------------------------------------------------------------- */
void *play_sample_thread (void *arg)
/**
 * Writes the contents of a buffer to a file.
 * In more specific terms: Plays a sample on an audio device.
 * \param arg		structure containing all neccessary arguments.
 * 			(see libM6au-types.h for details)
 * \return		-
 * \todo
 *
 */
{
  /*const char *this_code = "copy_sample_thread()"; */
  copy_sample_args_t *args;
  args = (copy_sample_args_t *) arg;
  const void *t_ret = NULL;

  /*printf ("T: fd='%d'\n", args->fd); */
  /*printf ("T: count='%d'\n", args->count); */

  /*printf ("T: buf0='%d'\n", ((char *) (args->buf))[0]); */
  /*printf ("T: *ret='%d'\n", *(args->ret)); */

  /*printf ("...playing sample now...\n"); */
  if (write (args->fd, args->buf, args->count) == -1)
    {
      perror ("Audio-Device");
    }
  close (args->fd);
  /*printf ("...playing sample done...\n"); */

  /*printf ("T: buf0='%d'\n", ((char *) (args->buf))[0]); */
  /*printf ("T: *ret='%d'\n", *(args->ret)); */

  /*printf ("T: exit\n"); */
  pthread_mutex_lock (args->mutex_p);
  *(args->state_p) = 0;
  /*printf ("T: *st='%d'\n", *(args->state_p)); */
  pthread_mutex_unlock (args->mutex_p);

  return (void *) t_ret;
}

/* ------------------------------------------------------------------------- */
void *record_sample_thread (void *arg)
/**
 * Writes the contents of a buffer to a file.
 * In more specific terms: Records a sample from an audio device.
 * \param arg		structure containing all neccessary arguments.
 * 			(see libM6au-types.h for details)
 * \return		-
 * \todo
 *
 */
{
  /*const char *this_code = "copy_sample_thread()"; */
  copy_sample_args_t *args;
  args = (copy_sample_args_t *) arg;
  const void *t_ret = NULL;

  /*printf ("T: fd='%d'\n", args->fd); */
  /*printf ("T: count='%d'\n", args->count); */
  /*printf ("T: buf0='%d'\n", ((char *) (args->buf))[0]); */
  /*printf ("T: *ret='%d'\n", *(args->ret)); */

  if (read (args->fd, args->buf, args->count) == -1)
    {
      perror ("Audio-Device");
    }
  close (args->fd);

  /*printf ("T: buf0='%d'\n", ((char *) (args->buf))[0]); */
  /*printf ("T: *ret='%d'\n", *(args->ret)); */

  pthread_mutex_lock (args->mutex_p);
  *(args->state_p) = 0;
  /*printf ("T: *st='%d'\n", *(args->state_p)); */
  pthread_mutex_unlock (args->mutex_p);

  return (void *) t_ret;
}

/* ------------------------------------------------------------------------- */
void *copy_file_thread (void *arg)
/**
 * Copies the contents of a file into a file.
 * The offset must be set to the start of the audio samples.
 * \param arg		structure containing all neccessary arguments.
 * 			(see libM6au-types.h for details)
 * \return		-
 * \todo
 *
 */
{
  const char *this_code = "copy_file_thread()";
  copy_file_args_t *args;
  args = (copy_file_args_t *) arg;
  const void *t_ret = NULL;
  char *buffer = "";
  long bytes_to_read;
  long bytes_read;
  long bytes_left;

  //pthread_mutex_lock (args->mutex_p);
  /*printf ("T: &mu='%d'\n", (args->mutex_p)); */
  //pthread_mutex_unlock (args->mutex_p);
  /*printf ("T: &st='%d'\n", (args->state_p)); */
  /*printf ("T: *st='%d'\n", *(args->state_p)); */
  /*printf ("T: sfd='%d'\n", args->source_fd); */
  /*printf ("T: tfd='%d'\n", args->target_fd); */
  /*printf ("T: count='%d'\n", args->count); */

  if ((buffer = malloc (args->buffer_size)) == NULL)
    {
      close (args->source_fd);
      close (args->target_fd);
      pthread_mutex_lock (args->mutex_p);
      *(args->state_p) = 0;
      pthread_mutex_unlock (args->mutex_p);
      fprintf (stderr,
               "%s: Unable to allocate %d bytes for playback buffer.\n",
               this_code, args->buffer_size);
      return (void *) t_ret;
    }

  bytes_left = args->count;

  while (bytes_left)
    {
      bytes_to_read = args->buffer_size;
      if (bytes_to_read > bytes_left)
        bytes_to_read = bytes_left;

      /*fprintf(stderr, "%s: bytes_to_read: %ld\n", this_code, bytes_to_read); */
      if ((bytes_read = read (args->source_fd, buffer, bytes_to_read)) <= 0)
        {
          close (args->source_fd);
          close (args->target_fd);
          pthread_mutex_lock (args->mutex_p);
          *(args->state_p) = 0;
          pthread_mutex_unlock (args->mutex_p);
          free (buffer);
          fprintf (stderr, "%s: More bytes expected in source file!\n",
                   this_code);
          return (void *) t_ret;
        }

      if (write (args->target_fd, buffer, bytes_read) == -1)
        {
          close (args->source_fd);
          close (args->target_fd);
          pthread_mutex_lock (args->mutex_p);
          *(args->state_p) = 0;
          pthread_mutex_unlock (args->mutex_p);
          free (buffer);
          perror ("Audio-Device");
          return (void *) t_ret;
        }

      bytes_left -= bytes_read;


      pthread_mutex_lock (args->mutexterm_p);
	  if (*(args->term_p))
	  {
		  *(args->term_p)=0;
      	  pthread_mutex_unlock (args->mutexterm_p);
          close (args->source_fd);
          close (args->target_fd);
          pthread_mutex_lock (args->mutex_p);
          *(args->state_p) = 0;
          pthread_mutex_unlock (args->mutex_p);
          free (buffer);
          return (void *) t_ret;
	  }
      pthread_mutex_unlock (args->mutexterm_p);
    }

  /*printf ("T: sfd='%d'\n", args->source_fd); */
  /*printf ("T: tfd='%d'\n", args->target_fd); */
  /*printf ("T: count='%d'\n", args->count); */
  /*printf ("T: &mu='%d'\n", (args->mutex_p)); */
  /*printf ("T: &st='%d'\n", (args->state_p)); */
  close (args->source_fd);
  close (args->target_fd);
  pthread_mutex_lock (args->mutex_p);
  *(args->state_p) = 0;
  /*printf ("T: *st='%d'\n", *(args->state_p)); */
  pthread_mutex_unlock (args->mutex_p);
  free (buffer);

  /*printf ("T: exit\n"); */

  return (void *) t_ret;
}

/* ------------------------------------------------------------------------- */
void *play_tone_thread (void *arg)
/**
 * Generates a tone.
 * \param arg		structure containing all neccessary arguments.
 * 			(see libM6au-types.h for details)
 * \return		-
 * \todo
 *
 */
{
  /*const char *this_code = "play_tone_thread()"; */
  play_tone_args_t *args;
  args = (play_tone_args_t *) arg;
  const void *t_ret = NULL;
  int request_value;

  request_value = 1193180 / args->freq; /* truncated */
  if (ioctl (args->fd, KIOCSOUND, request_value) < 0)
    {
      perror (" Ioctl Error ");
      close (args->fd);
      pthread_mutex_lock (args->mutex_p);
      *(args->state_p) = 0;
      pthread_mutex_unlock (args->mutex_p);
      return (void *) t_ret;
    }
  else
    {
      usleep (args->msec * 1000);
      if (ioctl (args->fd, KIOCSOUND, 0) < 0)
        {
          perror (" Ioctl Error ");
          close (args->fd);
          pthread_mutex_lock (args->mutex_p);
          *(args->state_p) = 0;
          pthread_mutex_unlock (args->mutex_p);
          exit (0);
          return (void *) t_ret;
        }
    }
  close (args->fd);
  pthread_mutex_lock (args->mutex_p);
  *(args->state_p) = 0;
  pthread_mutex_unlock (args->mutex_p);

  return (void *) t_ret;
}


