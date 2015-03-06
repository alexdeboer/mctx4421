/*
 * system.h
 *
 *  Created on: 14 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		system.h
 * \brief   Header file for system functions
 * \author  Remi KEAT
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "types.h"
#include "string.h"
#include <sys/sysinfo.h>
#include <stdio.h>

char* execute(char* command);
char *OSVersion(void);
int OSMachineSpeed(void);
int OSMachineType(void);
char* OSMachineName(void);
unsigned char OSMachineID(void);
int OSError(char *msg, int number, bool deadend);
int OSInfoCPU (info_cpu_t* infoCPU);
int OSInfoMem (info_mem_t* infoMem);
int OSInfoProc (info_proc_t* infoProc);
int OSInfoMisc (info_misc_t* infoMisc);

#endif /* SYSTEM_H_ */
