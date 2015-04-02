/*
 * hdt.h
 *
 *  Created on: 10 Jun 2013
 *      Author: Remi KEAT
 */

/**
 * \file    hdt.h
 * \brief   Header file for the HDT functions
 * \author  Remi KEAT
 */

#ifndef HDT_H_
#define HDT_H_

#include "types.h"
#include <stdio.h> /* FILE */
#include <stdlib.h> /* malloc() */
#include <QString>

#define HDT_IDX_TABLE	0
#define HDT_IDX_PSD		1
#define HDT_IDX_SERVO	2
#define HDT_IDX_MOTOR	3
#define HDT_IDX_ENCODER	4
#define HDT_IDX_DRIVE	5
#define HDT_IDX_COMPASS	6
#define HDT_IDX_IRTV	7
#define HDT_IDX_CAM		8
#define HDT_IDX_ADC		9
#define HDT_IDX_COM		10
#define HDT_MAX_COUNT	11

#define HDT_TABLE "TABLE"
#define HDT_PSD "PSD"
#define HDT_SERVO "SERVO"
#define HDT_MOTOR "MOTOR"
#define HDT_ENCODER "ENCODER"
#define HDT_DRIVE "DRIVE"
#define HDT_COMPASS "COMPASS"
#define HDT_IRTV "IRTV"
#define HDT_CAM "CAM"
#define HDT_ADC "ADC"
#define HDT_COM "SERIAL"

#define DIFFERENTIAL_DRIVE 0
#define ACKERMAN_DRIVE 1
#define ACKERMANN_DRIVE 1
#define SYNCHRO_DRIVE 2
#define TRICYCLE_DRIVE 3
#define OMNI_DRIVE 4

#define HDT_DIFF_STR "DIFFERENTIAL"
#define HDT_ACKM_STR "ACKERMANN"
#define HDT_OMNI_STR "OMNI"

int HDTValidate(char *filename);
int HDTListEntry(char *filename, HDTEntry_t *deventry, int count);
int HDTFindEntry(void *hdtfile, char *devname, HDTEntry_t *deventry);
int HDTFindTable(void *hdtfile, char *tabname, HDTTable_t *tabentry);

HDTTable_t HDTLoadTable(QString filename, HDTDevice_t *pdevices);
int HDTClearTable(HDTTable_t *ptables);

HDTDevCAM_t* HDTLoadCAM(char *filename, char *devname);
int HDTClearCAM(HDTDevCAM_t *pdevs);
HDTDevMOTOR_t* HDTLoadMOTOR(char *filename, char *devname);
int HDTClearMOTOR(HDTDevMOTOR_t *pdevs);
HDTDevENCODER_t* HDTLoadENCODER(char *filename, char *devname);
int HDTClearENCODER(HDTDevENCODER_t *pdevs);
int HDTLinkENC2MOT(HDTDevENCODER_t *pencoders, HDTDevMOTOR_t *pmotors);

HDTDevPSD_t HDTLoadPSD(QString filename, QString devname);
int HDTClearPSD(HDTDevPSD_t *pdevs);

HDTDevSERVO_t* HDTLoadSERVO(char *filename, char *devname);
int HDTClearSERVO(HDTDevSERVO_t *pdevs);
HDTDevDRIVE_t* HDTLoadDRIVE(char *filename, char *devname);
int HDTClearDRIVE(HDTDevDRIVE_t *pdevs);
int HDTLinkDRV2ENC(HDTDevDRIVE_t *pdrives, HDTDevENCODER_t *pencoders);
HDTDevIRTV_t* HDTLoadIRTV(char *filename, char *devname);
int HDTClearIRTV(HDTDevIRTV_t *pdevs);
HDTDevADC_t* HDTLoadADC(char *filename, char *devname);
int HDTClearADC(HDTDevADC_t *pdevs);
HDTDevCOM_t* HDTLoadCOM(char *filename, char *devname);
int HDTClearCOM(HDTDevCOM_t *pdevs);

#endif /* HDT_H_ */
