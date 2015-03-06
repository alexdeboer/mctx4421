/*
 * system.c
 *
 *  Created on: 14 Aug 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		system.c
 * \brief   Defines functions for the system
 * \author  Remi KEAT
 */

#include "system.h"

char* execute(char* command)
{
	FILE* fp;
	char path[1024];
	char output[2056] = "";
	fp = popen(command, "r");
	if (fp == NULL)
	{
		strcat(output, "Failed to run the command");
	}
	else
	{
		while (fgets(path, sizeof(path)-1, fp) != NULL)
		{
			strcat(output, path);
		}
	}
	pclose(fp);
	output[strlen(output)-1] = '\0';
	return output;
}

/**
 * \brief			Returns string containing running RoBIOS version.
 * \return    char* version : OS version
 * \details   Example: "3.1b"
 */
char *OSVersion(void)
{
	char* version = VERSION;
	return version;
}

/**
 * \brief     Inform the user how fast the processor runs.
 * \return		int speed : actual clockrate of CPU in Hz
 */
int OSMachineSpeed(void)
{
	return MACHINE_SPEED;
}

/**
 * \brief     Inform the user in which environment the program runs.
 * \return		int machineType : Type of used hardware
 * \details   Valid values are: VEHICLE, PLATFORM, WALKER
 */
int OSMachineType(void)
{
	return MACHINE_TYPE;
}

/**
 * \brief     Inform the user with which name the Eyebot is titled
 * \return		char* machineName : Name of actual Eyebot
 */
char* OSMachineName(void)
{
	char* machineName = MACHINE_NAME;
	return machineName;
}


/**
 * \brief			Inform the user with which ID the Eyebot is titled
 * \return		unsigned char ID : ID of actual Eyebot
 */
unsigned char OSMachineID(void)
{
	return ID;
}

/**
 * \brief			Print message and number to display then stop processor (deadend) or wait for key
 * \param     char* msg : pointer to message
 * \param     int number : int number
 * \param     BOOL deadend : switch to choose deadend or keywait
 * \details   Valid values are:
 * - 0 = no deadend
 * - 1 = deadend
 *
 * \return    int retVal : Always 0
 */
int OSError(char *msg, int number, bool deadend)
{
	//TODO: To complete
	return 0;
}

/**
 * \brief			Collects infos about the CPU – name, speed, architecture and bogusMips
 * \param     info_cpu_t* infoCPU : pointer to a structure (info_cpu_t) containing the cpu infos
 * \return    int retVal : always 0
 */
int OSInfoCPU (info_cpu_t* infoCPU)
{
	//open the file containing the CPU info
	FILE *cpuInfo = fopen("/proc/cpuinfo","rb");
	int lineLength = 70;
	//declare to strings to store info read from files
	char *line = (char *)malloc(lineLength);
	char *line2 = (char *)malloc(lineLength);
	//size_t size = 0;
	//read the first line from the file containing cpu info
	fgets(line,lineLength,cpuInfo);
	trimline(line);
	//only want the content from the 12th character onwards
	strncpy(line2,line+12,LCD_LIST_STRLENGTH);
	strcpy(infoCPU->name, line2);
	strcpy_n(infoCPU->arch,line2,5);
	//read the second line from the file
	fgets(line,lineLength,cpuInfo);
	trimline(line);
	//again we only want the content from the 11th character
	strncpy(line2,line+11,LCD_LIST_STRLENGTH);
	strcpy(infoCPU->bogomips, line2);
	strcpy(infoCPU->mhz, line2);
	
	fclose(cpuInfo);
	free(line);
	free(line2);
	return 0;
}

/**
 * \brief			Collects infos about the memory
 * \param     info_mem_t* infoMem : pointer to a structure (info_mem_t) which contains the memory infos
 * \return    int retVal : always 0
 */
int OSInfoMem (info_mem_t* infoMem)
{
	strcpy(infoMem->procnum, execute("ps aux | wc -l"));
	int len = 100;
	char str[len];
	snprintf(str, len, "%s", execute("cat /proc/meminfo | sed -rn 's/MemTotal: *()/\\1/p'"));
	strcpy(infoMem->total, str);
	snprintf(str, len, "%s", execute("cat /proc/meminfo | sed -rn 's/MemFree: *()/\\1/p'"));
	strcpy(infoMem->free, str);
	return 0;
}

/**
 * \brief			Collects infos about processes
 * \param 		info_proc_t infoProc : pointer to a structure (info_proc_t) which contains the process infos
 * \return    int retVal : always 0
 */

int OSInfoProc (info_proc_t* infoProc)
{
	//TODO: add info on the processes?
	strcpy(infoProc->num, execute("ps aux | wc -l"));
	return 0;
}

/**
 * \brief			Collects system’s miscellaneous infos – uptime, vbatt
 * \param     info_misc_t infoMisc : pointer to a structure (info_misc_t) which contains the misc infos
 * \return    int retVal : always 0
 */
int OSInfoMisc (info_misc_t* infoMisc)
{
	const int vbatt_max_8 = 12 << 8;
	struct sysinfo info;
	sysinfo(&info);
	int len = 100;
	char str[len];
	snprintf(str, len, "%ld", info.uptime);
	strcpy(infoMisc->uptime, str);
	strcpy(infoMisc->vbatt, "5V");
	//infoMisc->vbatt_8 = vbatt_max_8/2;
	infoMisc->vbatt_8 = vbatt_max_8;
	//TODO: Find info battery
	return 0;
}
