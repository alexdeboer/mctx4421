/**
 * \file libM6au-types.h
 *
 * EyebotM6 library for audio routines types declarations header file.
 *
 * Header file for type definitions of the official "RiBIOS" library routines.
 *
 * For more information see
 * http://robotics.ee.uwa.edu.au/eyebot/doc/API/library.html
 *
 *
 * \todo
 *
 * \date 2006
 * \author Thomas Sommer
 * \version 0.50
 *
 */


#ifndef LIBM6_AU_TYPES_H
#define LIBM6_AU_TYPES_H


/* all members with "canonical" must NOT be set per hand, as theses sizes
 * are used in many places but do not refer to this variable.
 * (actually audio_format could be made non-canonical rather easy, but
 * currently there is no need for it...)
 * canonical RIFF-WAV-PCM means:
 * - 12 byte riff header of type wave
 * - 24 byte fmt chunk, pcm format set, at offset 12.
 * - data chunck at offset 36.
 */
typedef struct
{
  long riff_size;		/* canonical: filesize - 8 */
  long fmt_size;		/* canonical: 24 - 8 = 16 */
  long data_size;               /* size of the actual audio data section */
  int audio_format;		/* canonical: 1 */
  int channels;
  long sample_rate;
  long byte_rate;
  int block_align;
  int bits_per_sample;
} header_t;


/* the following *args_t structures are for passing arguments to threads.*/
typedef struct
{
  int fd;			/* file descriptor */
  void *buf;			/* buffer to read from */
  size_t count;			/* number of bytes to read */
  pthread_mutex_t *mutex_p;	/* mutex for changing the state variable */
  int *state_p;			/* state variable to reset after transfer */
} copy_sample_args_t;

typedef struct
{
  int source_fd;		/* file descriptor */
  int target_fd;		/* file descriptor */
  int buffer_size;		/* transfer buffer size in bytes */
  long count;			/* number of bytes to transfer */
  pthread_mutex_t *mutex_p;	/* mutex for changing the state variable */
  pthread_mutex_t *mutexterm_p;	/* mutex for changing the terminate variable */
  int *state_p;			/* state variable to reset after transfer */
  int *term_p;			/* variable to instruct thread to terminate */
} copy_file_args_t;

typedef struct
{
  int fd;			/* file descriptor */
  int freq;			/* frequency of the tone */
  int msec;			/* duration the tone */
  pthread_mutex_t *mutex_p;	/* mutex for changing the state variable */
  int *state_p;			/* state variable to reset after transfer */
} play_tone_args_t;


#endif

