/**
 * \file libM6au.h
 *
 * EyebotM6 library for audio routines header file.
 *
 * Header file for audio routines of the "RoBIOS" library.
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

#ifndef LIBM6_AU_H
#define LIBM6_AU_H

/** Plays an audio sample. */
int AUPlaySample (char* sample, long length);
/** Records an audio sample. */
int AURecordSample(char* buf, long len);
int AUPlayFile(char* file);
int AURecordFile(char* file, long len, long freq);
/** Checks for audio playback end. */
int AUCheckSample (void);
/** Checks for audio recording end. */
int AUCheckRecord (void);
/** Checks for beep or tone end. */
int AUCheckTone(void);
/** Plays a tone. */
int AUTone(int freq, int msec);
/** Plays a short tone. */
int AUBeep(void);
/** Probes the microphone input. */
int AUCaptureMic(void);
/** Terminate an ongoing copy file thread */
void AUStopPlayRec(void);



#endif /* LIBM6_AU_H */
