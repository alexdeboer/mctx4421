/**
 * \file libM6au-intern.h
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
 * 	(see implemantation)
 *
 * \date 2006
 * \author Thomas Sommer
 * \version 0.50
 * 
 */

#ifndef LIBM6_AU_INTERN_H
#define LIBM6_AU_INTERN_H

/** Extracts information from a RIFF-WAV-PCM header */
void extract_header_params (header_t * header, char *file);
/** Prints out the header parameter structure. */
void print_header_params (header_t * header);
/** Interprets a list of bytes as a little endian integer. */
int le2int (char *char_list, int length);
/*int be2int (char *char_list, int length);*/
/** Generates the little endian representation of an integer. */
int int2le (int val, int length, char *char_list);
/*int int2be (int val, int length, char *char_list);*/
/** Sets the audio device to the given parameters. */
int set_audio_device (int device_fd, header_t *params);
/** Performs an ioctl and evaluatesthe  result. */
int request_ioctl (int device_fd, int request_code, char *request_title,
		   int value);
/** Creates a header from the supplied parameter structure. */
void create_header(char* header, header_t * params);
/** Checks the header of a canonical RIFF-WAV-PCM file. */
int check_header (char* file, long length);
/** Writes an audio sample to an audio device.*/
void *play_sample_thread (void *arg);
/** Reads an audio sample from an audio device. */
void *record_sample_thread (void *arg);
/** Copies a file into another. */
void *copy_file_thread (void *arg);
/** Plays a tone. */
void *play_tone_thread (void *arg);


#endif 
