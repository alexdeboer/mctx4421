/*
 * hdt.c
 *
 *  Created on: 13 Jun 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file    hdt.c
 * \brief   Defines functions used by the HDT system
 * \author  Remi KEAT
 */

#include "hdt.h"

/** 20080917 - azman@ee.uwa.edu.au */

extern __ssize_t getline (char **__restrict __lineptr,
	size_t *__restrict __n, FILE *__restrict __stream);

typedef struct _HDTTypes_t
{
	char *pTitle;
}
HDTTypes_t;

static HDTTypes_t HDTEntryTypes[HDT_MAX_COUNT] =
{
	{HDT_TABLE},
	{HDT_PSD},
	{HDT_SERVO},
	{HDT_MOTOR},
	{HDT_ENCODER},
	{HDT_DRIVE},
	{HDT_COMPASS},
	{HDT_IRTV},
	{HDT_CAM},
	{HDT_ADC},
	{HDT_COM}
};

/* ------------------------------------------------------------------------- */
/**
	\brief		checks all HDT entries in given filename. will not
						check for specific entry (only check entry headers).
	\param		char* filename : name of HDT file to be checked
	\return		int retVal :\n
						-1 if incorrect HDT entry found\n
						(number of entries) if otherwise
*/
int HDTValidate(char *filename)
{
	char *readbuff, *testbuff, *ptoken;
	int intable = 0, retval = 0, loop;
	int readcount, buffcount = HDT_MAX_READBUFF*sizeof(char);
	FILE* pfile = fopen(filename,"rt");

	if(!pfile)
	{
		printf("HDTValidate: Cannot open file '%s'!\n",filename);
		return -1;
	}
	readbuff = (char*) malloc(buffcount);
	while((readcount=getline(&readbuff,&buffcount,pfile))!=-1)
	{
		if(readbuff[0]=='#') /* comment character! */
			continue;
		readbuff[readcount-1] = 0x0;
		testbuff = malloc(readcount*sizeof(char));
		strcpy(testbuff,readbuff);
		ptoken = strtok(testbuff," \r\n");
		if(!ptoken)
		{
			/* should we check? */
		}
		else if(intable)
		{
			if(strcmp(ptoken,"END")==0)
				intable = 0;
		}
		else if(strcmp(ptoken,HDT_TABLE)==0)
		{
			intable = 1;
			retval++;
		}
		else
		{
			for(loop=1;loop<HDT_MAX_COUNT;loop++)
			{
				if(strcmp(ptoken,HDTEntryTypes[loop].pTitle)==0)
				{
					retval++;
					break;
				}
			}
			/* found anything? */
			if(loop==HDT_MAX_COUNT)
			{
				printf("Invalid line: %s\n",readbuff);
				retval = -1;
			}
		}
		free(testbuff);
		if(retval<0) break;
	}
	free(readbuff);
	fclose(pfile);
	return retval;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Copy all entries to deventry. user need to free the
						allocated memory by using free(deventry->buffer).
						return value may be less than count.
	\param		char* filename : name of HDT file to be checked for listing
	\param    HDTEntry_t* deventry : storage structure for the entry
	\param    int count : number of deventry storage supplied
	\return		int retVal :\n
						-1 on failure\n
						(number of entries) on success
*/
int HDTListEntry(char *filename, HDTEntry_t *deventry, int count)
{
	char *readbuff;
	int readcount, buffcount = HDT_MAX_READBUFF*sizeof(char);
	int ecount = 0, loop;
	FILE* hdtfile = fopen(filename,"rt");

	if(!hdtfile)
	{
		printf("HDTListEntry: Cannot open file '%s'!\n",filename);
		return -1;
	}

	readbuff = (char*) malloc(buffcount);
	while((readcount=getline(&readbuff,&buffcount,hdtfile))!=-1 &&
		ecount < count)
	{
		if(readcount>buffcount)
			buffcount = readcount;
		/* compare all types! */
		for(loop=0;loop<HDT_MAX_COUNT;loop++)
		{
			/* find if devname is first in line! */
			if(strstr(readbuff,HDTEntryTypes[loop].pTitle)==readbuff)
			{
				if(ecount<count)
				{
					deventry[ecount].length = buffcount;
					deventry[ecount].buffer = readbuff;
					/* delete the newline char! */
					deventry[ecount].buffer[readcount-1] = 0x0;
					ecount++;
					/* re-allocate buffer */
					readbuff = (char*) malloc(buffcount);
				}
				break;
			}
		}
	}
	if(readcount<0) free(readbuff);

	return ecount;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		finds an entry in the hdt file that matches given name
						and copies the entry to given structure. the newline
						character is replaced by null. user need to free the
						allocated memory by using free(deventry->buffer).
	\param		void* hdtfile : hdt file fopen with "rt" flag
	\param		char* devname : name of entry to search for
	\param    HDTEntry_t* deventry : storage structure for the entry
	\return		int retVal :\n
						-1 on failure (no entry found)\n
						[entry length] on success
*/
int HDTFindEntry(void *hdtfile, char *devname, HDTEntry_t *deventry)
{
	char *readbuff;
	int readcount, buffcount = HDT_MAX_READBUFF*sizeof(char);
	readbuff = (char*) malloc(buffcount);
	while((readcount=getline(&readbuff,&buffcount,(FILE*)hdtfile))!=-1)
	{
		/* find if devname is first in line! */
		if(strstr(readbuff,devname)==readbuff)
		{
			deventry->length = buffcount;
			deventry->buffer = readbuff;
			/* delete the newline char! */
			deventry->buffer[readcount-1] = 0x0;
			break;
		}
	}
	if(readcount<0) free(readbuff);
	return readcount;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		finds a table in the hdt file that matches given name and copies the table data to given structure.
	\param		void* hdtfile : hdt file (fopen with "rt" flag)
	\param    char* tabname : name of table to search for
	\param    HDTTable_t* tabentry : storage structure for the table
	\return		int retVal :\n
						-1 on failure (no table found)\n
						[table size] on success
*/
int HDTFindTable(void *hdtfile, char *tabname, HDTTable_t *tabentry)
{
	char *readbuff, *ptest1;
	int *pdata = 0x0;
	int data, count=-1, findtable=1, loadtable=0;
	int readcount, buffcount = HDT_MAX_READBUFF*sizeof(char);
	readbuff = (char*) malloc(buffcount);
	while((readcount=getline(&readbuff,&buffcount,(FILE*)hdtfile))!=-1)
	{
		/* delete newline char! */
		readbuff[readcount-1] = 0x0;
		/* find table entry! */
		if(findtable&&strstr(readbuff,"TABLE")==readbuff)
		{
			/* get table name from quoted string! */
			/* setup token finding */
			ptest1 = strtok(readbuff,"\"\n"); /* quote & newline delims */
			/* get token a.k.a. label! */
			ptest1 = strtok(0x0,"\"\n");
			if(ptest1)
			{
				/* check table name */
				if(strcmp(ptest1,tabname)==0)
				{
					strcpy(tabentry->name,ptest1);
					count = 0;
					findtable = 0;
					loadtable = 1;
					tabentry->size = 0;
					tabentry->data = 0x0;
				}
			}
		}
		else if(loadtable)
		{
			if(strstr(readbuff,"END ")==readbuff)
			{
				loadtable=0;
				break;
			}
			ptest1 = strtok(readbuff," ,\n\r");
			while(ptest1)
			{
				data = atoi(ptest1);
				count++;
				pdata = (int*) realloc(pdata,count*sizeof(int));
				pdata[count-1] = data;
				tabentry->data = pdata;
				tabentry->size = count;
				ptest1 = strtok(0x0," ,\n\r");
			}
		}
	}
	if(loadtable)
	{
		if(pdata) free(pdata);
	}
	free(readbuff);
	return count;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		load all tables needed by pdevices - if found.
						the return value is a pointer to the first table.
						the tables are in a	linked list allocated with dynamic memory.
						use HDTClearTable	to free up the resources.
	\param		char* filename : hdt file to open
	\param    HDTDevice_t* pdevices : devices with tablename in linked list
	\return		HDTTable_t* table :\n
						0x0 on failure (no table found)\n
						(pointer to first table) if found
*/
HDTTable_t* HDTLoadTable(char *filename, HDTDevice_t *pdevices)
{
	FILE *pfile;
	HDTTable_t hdtTable, *ptable, *pfirst = 0x0, *plast = 0x0;

	pfile = fopen(filename,"rt");
	if(!pfile)
	{
		printf("Cannot open filename \"%s\"!\n",filename);
		return 0x0;
	}

	while(pdevices) /* for all devices! */
	{
		/* look in existing! */
		ptable = pfirst;
		while(ptable)
		{
			if(strcmp(ptable->name,pdevices->tabname)==0)
				break;
			ptable = ptable->pnext;
		}
		if(!ptable)
		{
			if(HDTFindTable(pfile,pdevices->tabname,&hdtTable)<0)
			{
				printf("Cannot find table \"%s\" for Device \"%s\"!\n",
					pdevices->tabname,pdevices->name);
			}
			else
			{
				/* create dynamic copy for table & insert to list */
				ptable = malloc(sizeof(HDTTable_t));
				*ptable = hdtTable;
				ptable->pnext = 0x0; /* last item in list */
				if(!plast) pfirst = ptable;
				else plast->pnext = ptable;
				plast = ptable;
			}
		}
		/* link table to device */
		pdevices->ptable = ptable;
		pdevices = pdevices->pnext;
	}
	fclose(pfile);
	return pfirst;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Free the allocated resources for the tables created by HDTLoadTable.
	\param		HDTTable_t* ptables : tables to be cleared
	\return		int retVal : always 0
*/
int HDTClearTable(HDTTable_t *ptables)
{
	HDTTable_t *ptemp;
	while(ptables)
	{
		ptemp = ptables->pnext;
		if(ptables->data)
			free(ptables->data);
		free(ptables);
		ptables = ptemp;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		load all <device> entry found in the hdt file if devname is	null.
						else, load only the requested device. the return value is a pointer to the first <device>.
						the <devices> are	in a linked list allocated with dynamic memory.
						use	HDTClear<device> to free up the resources.
	\param		char* filename : hdt file to open
	\param		char* devname : device semantics
	\return		HDTDevCAM_t* handle :\n
						0x0 on failure (no <device> found)\n
						(pointer to first <device>) if found
*/
HDTDevCAM_t* HDTLoadCAM(char *filename, char *devname)
{
	FILE *pfile;
	HDTEntry_t hdtEntry;
	HDTDevCAM_t *pcam, *pfirst=0x0, *plast=0x0;
	char *tokenbuff, *ptoken;
	int length;
	int errorHDT;

	pfile = fopen(filename,"rt");
	if(!pfile)
	{
		printf("Cannot open filename \"%s\"!\n",filename);
		return 0x0;
	}

	while(1)
	{
		if((length=HDTFindEntry(pfile,HDT_CAM,&hdtEntry))<0)
			break;

		errorHDT = 0;
		/* create camera data! - and fill with zeros! */
		pcam = malloc(sizeof(HDTDevCAM_t));
		memset((void*)pcam,0x00,sizeof(HDTDevCAM_t));
		/* separate token processing buffer */
		tokenbuff = malloc(length);
		strcpy(tokenbuff,hdtEntry.buffer);
		ptoken = strtok(tokenbuff,"\"\n");
		/* skip camera entry */
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get CAM_LABEL value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(pcam->name,ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get CAM_REGADDR value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%x",&pcam->regaddr);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get CAM_UCB1400IO value!\n");
			errorHDT = 1;
		}
		else
		{
			pcam->ucb1400io = atoi(ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get CAM_WIDTH value!\n");
			errorHDT = 1;
		}
		else
		{
			pcam->width = atoi(ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get CAM_HEIGHT value!\n");
			errorHDT = 1;
		}
		else
		{
			pcam->height = atoi(ptoken);
		}
		free(tokenbuff);
		free(hdtEntry.buffer);
		/* check error entry */
		if(errorHDT)
		{
			free(pcam);
			continue;
		}
		if(devname)
		{
			if(strcmp(pcam->name,devname)==0)
			{
				pfirst = pcam;
				break;
			}
			free(pcam);
		}
		else
		{
			/* link into main list */
			if(!plast) pfirst = pcam;
			else plast->pnext = pcam;
			/* insert a first item in list */
			plast = pcam;
		}
	}
	fclose(pfile);

	return pfirst;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Free the allocated resources for the <device> created by HDTLoad<device>.
	\param		HDTDevCAM_t* pdevs : <device> list to be cleared
	\return		int retVal : always 0
*/
int HDTClearCAM(HDTDevCAM_t *pdevs)
{
	HDTDevCAM_t *pcam;
	while(pdevs)
	{
		pcam = pdevs->pnext;
		free(pdevs);
		pdevs = pcam;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		load all <device> entry found in the hdt file if devname is	null.
						else, load only the requested device. the return value is a pointer to the first <device>.
						the <devices> are	in a linked list allocated with dynamic memory.
						use	HDTClear<device> to free up the resources.
	\param		char* filename : hdt file to open
	\param		char* devname : device semantics
	\return		HDTDevMOTOR_t* handle :\n
						0x0 on failure (no <device> found)\n
						(pointer to first <device>) if found
*/
HDTDevMOTOR_t* HDTLoadMOTOR(char *filename, char *devname)
{
	FILE *pfile;
	HDTEntry_t hdtEntry;
	HDTDevMOTOR_t *pmotor, *pfirst=0x0, *plast=0x0;
	char *tokenbuff, *ptoken;
	int length;
	int errorHDT;

	pfile = fopen(filename,"rt");
	if(!pfile)
	{
		printf("Cannot open filename \"%s\"!\n",filename);
		return 0x0;
	}

	while(1)
	{
		if((length=HDTFindEntry(pfile,HDT_MOTOR,&hdtEntry))<0)
			break;

		errorHDT = 0;
		/* create motor data! - and fill with zeros! */
		pmotor = malloc(sizeof(HDTDevMOTOR_t));
		memset((void*)pmotor,0x00,sizeof(HDTDevMOTOR_t));
		/* separate token processing buffer */
		tokenbuff = malloc(length);
		strcpy(tokenbuff,hdtEntry.buffer);
		ptoken = strtok(tokenbuff,"\"\n");
		/* skip motor entry */
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get MOTOR_LABEL value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(pmotor->name,ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get MOTOR_REGADDR value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%x",&pmotor->regaddr);
		}
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get MOTOR_FREQ value!\n");
			errorHDT = 1;
		}
		else
		{
			pmotor->freq = atoi(ptoken);
		}
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get MOTOR_TABLENAME value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(pmotor->tabname,ptoken);
		}
		free(tokenbuff);
		free(hdtEntry.buffer);
		/* check error entry */
		if(errorHDT)
		{
			free(pmotor);
			continue;
		}
		if(devname)
		{
			if(strcmp(pmotor->name,devname)==0)
			{
				pfirst = pmotor;
				break;
			}
			free(pmotor);
		}
		else
		{
			/* link into main list */
			if(!plast) pfirst = pmotor;
			else plast->pnext = pmotor;
			/* insert a first item in list */
			plast = pmotor;
		}
	}
	fclose(pfile);

	return pfirst;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Free the allocated resources for the <device> created by HDTLoad<device>.
	\param		HDTDevMOTOR_t* pdevs : <device> list to be cleared
	\return		int retVal : always 0
*/
int HDTClearMOTOR(HDTDevMOTOR_t *pdevs)
{
	HDTDevMOTOR_t *pmotor;
	while(pdevs)
	{
		pmotor = pdevs->pnext;
		free(pdevs);
		pdevs = pmotor;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		load all <device> entry found in the hdt file if devname is	null.
						else, load only the requested device. the return value is a pointer to the first <device>.
						the <devices> are	in a linked list allocated with dynamic memory.
						use	HDTClear<device> to free up the resources.
	\param		char* filename : hdt file to open
	\param		char* devname : device semantics
	\return		HDTDevENCODER_t* handle :\n
						0x0 on failure (no <device> found)\n
						(pointer to first <device>) if found
*/
HDTDevENCODER_t* HDTLoadENCODER(char *filename, char *devname)
{
	FILE *pfile;
	HDTEntry_t hdtEntry;
	HDTDevENCODER_t *penc, *pfirst=0x0, *plast=0x0;
	char *tokenbuff, *ptoken;
	int length;
	int errorHDT;

	pfile = fopen(filename,"rt");
	if(!pfile)
	{
		printf("Cannot open filename \"%s\"!\n",filename);
		return 0x0;
	}

	while(1)
	{
		if((length=HDTFindEntry(pfile,HDT_ENCODER,&hdtEntry))<0)
			break;

		errorHDT = 0;
		/* create encoder data! - and fill with zeros! */
		penc = malloc(sizeof(HDTDevENCODER_t));
		memset((void*)penc,0x00,sizeof(HDTDevENCODER_t));
		/* separate token processing buffer */
		tokenbuff = malloc(length);
		strcpy(tokenbuff,hdtEntry.buffer);
		ptoken = strtok(tokenbuff,"\"\n");
		/* skip encoder entry */
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get ENCODER_LABEL value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(penc->name,ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get ENCODER_REGADDR value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%x",&penc->regaddr);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get ENCODER_CLICKSPM value!\n");
			errorHDT = 1;
		}
		else
		{
			penc->clickspm = atoi(ptoken);
		}
		ptoken = strtok(0x0," \"\n");
		if(!ptoken)
		{
			printf("Cannot get ENCODER_MAXSPEED value!\n");
			errorHDT = 1;
		}
		else
		{
			penc->maxspeed = atoi(ptoken);
		}
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get ENCODER_MOTORNAME value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(penc->motorname,ptoken);
		}
		free(tokenbuff);
		free(hdtEntry.buffer);
		/* check error entry */
		if(errorHDT)
		{
			free(penc);
			continue;
		}
		if(devname)
		{
			if(strcmp(penc->name,devname)==0)
			{
				pfirst = penc;
				break;
			}
			free(penc);
		}
		else
		{
			/* link into main list */
			if(!plast) pfirst = penc;
			else plast->pnext = penc;
			/* insert a first item in list */
			plast = penc;
		}
	}
	fclose(pfile);

	return pfirst;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Free the allocated resources for the <device> created by HDTLoad<device>.
	\param		HDTDevENCODER_t* pdevs : <device> list to be cleared
	\return		int retVal : always 0
*/
int HDTClearENCODER(HDTDevENCODER_t *pdevs)
{
	HDTDevENCODER_t *penc;
	while(pdevs)
	{
		penc = pdevs->pnext;
		free(pdevs);
		pdevs = penc;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Link the encoders to the motors.
	\param		HDTDevENCODER_t* pencoders : list of encoders
	\param		HDTDevMOTOR_t* pmotors : list of motors
	\return		int retVal :\n
						0 on success\n
						Negative value on failure (number of unconnected link)
*/
int HDTLinkENC2MOT(HDTDevENCODER_t *pencoders, HDTDevMOTOR_t *pmotors)
{
	int errorlevel = 0;
	while(pencoders)
	{
		HDTDevMOTOR_t *ptemp = pmotors;
		while(ptemp)
		{
			if(strcmp(pencoders->motorname,ptemp->name)==0)
			{
				pencoders->pmotor = ptemp;
				break;
			}
			ptemp = ptemp->pnext;
		}
		if(!ptemp)
		{
			printf("Cannot find motor \"%s\" for encoder \"%s\"!\n",
				pencoders->motorname, pencoders->name);
			errorlevel--;
		}
		pencoders = pencoders->pnext;
	}
	return errorlevel;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		load all <device> entry found in the hdt file if devname is	null.
						else, load only the requested device. the return value is a pointer to the first <device>.
						the <devices> are	in a linked list allocated with dynamic memory.
						use	HDTClear<device> to free up the resources.
	\param		char* filename : hdt file to open
	\param		char* devname : device semantics
	\return		HDTDevIRTV_t* handle :\n
						0x0 on failure (no <device> found)\n
						(pointer to first <device>) if found
*/
HDTDevPSD_t* HDTLoadPSD(char *filename, char *devname)
{
	FILE *pfile;
	HDTEntry_t hdtEntry;
	HDTDevPSD_t *ppsd, *pfirst=0x0, *plast=0x0;
	char *tokenbuff, *ptoken;
	int length;
	int errorHDT;

	pfile = fopen(filename,"rt");
	if(!pfile)
	{
		printf("Cannot open filename \"%s\"!\n",filename);
		return 0x0;
	}

	while(1)
	{
		if((length=HDTFindEntry(pfile,HDT_PSD,&hdtEntry))<0)
			break;

		errorHDT = 0;
		/* create psd data! */
		ppsd = malloc(sizeof(HDTDevPSD_t));
		memset((void*)ppsd,0x00,sizeof(HDTDevPSD_t));
		/* separate token processing buffer */
		tokenbuff = malloc(length);
		strcpy(tokenbuff,hdtEntry.buffer);
		ptoken = strtok(tokenbuff,"\"\n");
		/* skip psd entry */
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get PSD_LABEL value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(ppsd->name,ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get PSD_REGADDR value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%x",&ppsd->regaddr);
		}
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get PSD_TABLENAME value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(ppsd->tabname,ptoken);
		}
		free(tokenbuff);
		free(hdtEntry.buffer);
		/* check error entry */
		if(errorHDT)
		{
			free(ppsd);
			continue;
		}
		if(devname)
		{
			if(strcmp(ppsd->name,devname)==0)
			{
				pfirst = ppsd;
				break;
			}
			free(ppsd);
		}
		else
		{
			/* link into main list */
			if(!plast) pfirst = ppsd;
			else plast->pnext = ppsd;
			/* insert a first item in list */
			plast = ppsd;
		}
	}
	fclose(pfile);

	return pfirst;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Free the allocated resources for the <device> created by HDTLoad<device>.
	\param		HDTDevPSD_t* pdevs : <device> list to be cleared
	\return		int retVal : always 0
*/
int HDTClearPSD(HDTDevPSD_t *pdevs)
{
	HDTDevPSD_t *ppsd;
	while(pdevs)
	{
		ppsd = pdevs->pnext;
		free(pdevs);
		pdevs = ppsd;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		load all <device> entry found in the hdt file if devname is	null.
						else, load only the requested device. the return value is a pointer to the first <device>.
						the <devices> are	in a linked list allocated with dynamic memory.
						use	HDTClear<device> to free up the resources.
	\param		char* filename : hdt file to open
	\param		char* devname : device semantics
	\return		HDTDevSERVO_t* handle :\n
						0x0 on failure (no <device> found)\n
						(pointer to first <device>) if found
*/
HDTDevSERVO_t* HDTLoadSERVO(char *filename, char *devname)
{
	FILE *pfile;
	HDTEntry_t hdtEntry;
	HDTDevSERVO_t *psrv, *pfirst=0x0, *plast=0x0;
	char *tokenbuff, *ptoken;
	int length;
	int errorHDT;

	pfile = fopen(filename,"rt");
	if(!pfile)
	{
		printf("Cannot open filename \"%s\"!\n",filename);
		return 0x0;
	}

	while(1)
	{
		if((length=HDTFindEntry(pfile,HDT_SERVO,&hdtEntry))<0)
			break;

		errorHDT = 0;
		/* create servo data! - and fill with zeros! */
		psrv = malloc(sizeof(HDTDevSERVO_t));
		memset((void*)psrv,0x00,sizeof(HDTDevSERVO_t));
		/* separate token processing buffer */
		tokenbuff = malloc(length);
		strcpy(tokenbuff,hdtEntry.buffer);
		ptoken = strtok(tokenbuff,"\"\n");
		/* skip servo entry */
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get SERVO_LABEL value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(psrv->name,ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get SERVO_REGADDR1 value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%x",&psrv->regaddr);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get SERVO_FREQ value!\n");
			errorHDT = 1;
		}
		else
		{
			psrv->freq = atoi(ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get SERVO_MINTIME value!\n");
			errorHDT = 1;
		}
		else
		{
			psrv->mintime = atoi(ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get SERVO_MAXTIME value!\n");
			errorHDT = 1;
		}
		else
		{
			psrv->maxtime = atoi(ptoken);
		}
		free(tokenbuff);
		free(hdtEntry.buffer);
		/* check error entry */
		if(errorHDT)
		{
			free(psrv);
			continue;
		}
		if(devname)
		{
			if(strcmp(psrv->name,devname)==0)
			{
				pfirst = psrv;
				break;
			}
			free(psrv);
		}
		else
		{
			/* link into main list */
			if(!plast) pfirst = psrv;
			else plast->pnext = psrv;
			/* insert a first item in list */
			plast = psrv;
		}
	}
	fclose(pfile);

	return pfirst;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Free the allocated resources for the <device> created by HDTLoad<device>.
	\param		HDTDevSERVO_t* pdevs : <device> list to be cleared
	\return		int retVal : always 0
*/
int HDTClearSERVO(HDTDevSERVO_t *pdevs)
{
	HDTDevSERVO_t *psrv;
	while(pdevs)
	{
		psrv = pdevs->pnext;
		free(pdevs);
		pdevs = psrv;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		load all <device> entry found in the hdt file if devname is	null.
						else, load only the requested device. the return value is a pointer to the first <device>.
						the <devices> are	in a linked list allocated with dynamic memory.
						use	HDTClear<device> to free up the resources.
	\param		char* filename : hdt file to open
	\param		char* devname : device semantics
	\return		HDTDevDRIVE_t* handle :\n
						0x0 on failure (no <device> found)\n
						(pointer to first <device>) if found
*/
HDTDevDRIVE_t* HDTLoadDRIVE(char *filename, char *devname)
{
	FILE *pfile;
	HDTEntry_t hdtEntry;
	HDTDevDRIVE_t *pdrv, *pfirst=0x0, *plast=0x0;
	char *tokenbuff, *ptoken;
	int length;
	int errorHDT;

	pfile = fopen(filename,"rt");
	if(!pfile)
	{
		printf("Cannot open filename \"%s\"!\n",filename);
		return 0x0;
	}

	while(1)
	{
		if((length=HDTFindEntry(pfile,HDT_DRIVE,&hdtEntry))<0)
			break;

		errorHDT = 0;
		/* create servo data! - and fill with zeros! */
		pdrv = malloc(sizeof(HDTDevDRIVE_t));
		memset((void*)pdrv,0x00,sizeof(HDTDevDRIVE_t));
		/* separate token processing buffer */
		tokenbuff = malloc(length);
		strcpy(tokenbuff,hdtEntry.buffer);
		ptoken = strtok(tokenbuff,"\"\n");
		/* skip servo entry */
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get DRIVE_LABEL value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(pdrv->name,ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get DRIVE_TYPE value!\n");
			errorHDT = 1;
		}
		else
		{
			if(strcmp(ptoken,HDT_DIFF_STR)==0)
			{
				pdrv->drivetype = DIFFERENTIAL_DRIVE;
				ptoken = strtok(0x0," \n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_WHEELDIST value!\n");
					errorHDT = 1;
				}
				else
				{
					sscanf(ptoken,"%d",&pdrv->wheeldist1);
				}
				ptoken = strtok(0x0,"\"\n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_ENC1 value!\n");
					errorHDT = 1;
				}
				else
				{
					strcpy(pdrv->encname1,ptoken);
				}
				ptoken = strtok(0x0,"\"\n");
				/* skip space! */
				ptoken = strtok(0x0,"\"\n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_ENC2 value!\n");
					errorHDT = 1;
				}
				else
				{
					strcpy(pdrv->encname2,ptoken);
				}
			}
			else if(strcmp(ptoken,HDT_ACKM_STR)==0)
			{
				pdrv->drivetype = ACKERMAN_DRIVE;
				ptoken = strtok(0x0," \n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_SWHEELDIST value!\n");
					errorHDT = 1;
				}
				else
				{
					sscanf(ptoken,"%d",&pdrv->wheeldist1);
				}
				ptoken = strtok(0x0," \n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_DWHEELDIST value!\n");
					errorHDT = 1;
				}
				else
				{
					sscanf(ptoken,"%d",&pdrv->wheeldist2);
				}
				ptoken = strtok(0x0," \"\n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_AXESDIST value!\n");
					errorHDT = 1;
				}
				else
				{
					sscanf(ptoken,"%d",&pdrv->axesdist);
				}
				ptoken = strtok(0x0,"\"\n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_ENC1 value!\n");
					errorHDT = 1;
				}
				else
				{
					strcpy(pdrv->encname1,ptoken);
				}
				ptoken = strtok(0x0,"\"\n");
				/* skip space! */
				ptoken = strtok(0x0,"\"\n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_ENC2 value!\n");
					errorHDT = 1;
				}
				else
				{
					strcpy(pdrv->encname2,ptoken);
				}
			}
			else if(strcmp(ptoken,HDT_OMNI_STR)==0)
			{
				pdrv->drivetype = OMNI_DRIVE;
				ptoken = strtok(0x0," \n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_WHEELDIST value!\n");
					errorHDT = 1;
				}
				else
				{
					sscanf(ptoken,"%d",&pdrv->wheeldist1);
				}
				ptoken = strtok(0x0," \"\n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_AXESDIST value!\n");
					errorHDT = 1;
				}
				else
				{
					sscanf(ptoken,"%d",&pdrv->axesdist);
				}
				ptoken = strtok(0x0,"\"\n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_ENC1 value!\n");
					errorHDT = 1;
				}
				else
				{
					strcpy(pdrv->encname1,ptoken);
				}
				ptoken = strtok(0x0,"\"\n");
				/* skip space! */
				ptoken = strtok(0x0,"\"\n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_ENC2 value!\n");
					errorHDT = 1;
				}
				else
				{
					strcpy(pdrv->encname2,ptoken);
				}
				ptoken = strtok(0x0,"\"\n");
				/* skip space! */
				ptoken = strtok(0x0,"\"\n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_ENC3 value!\n");
					errorHDT = 1;
				}
				else
				{
					strcpy(pdrv->encname3,ptoken);
				}
				ptoken = strtok(0x0,"\"\n");
				/* skip space! */
				ptoken = strtok(0x0,"\"\n");
				if(!ptoken)
				{
					printf("Cannot get DRIVE_ENC4 value!\n");
					errorHDT = 1;
				}
				else
				{
					strcpy(pdrv->encname4,ptoken);
				}
			}
			else
			{
				printf("Invalid DRIVE_TYPE value %s!\n",ptoken);
				errorHDT = 1;
			}
		}
		free(tokenbuff);
		free(hdtEntry.buffer);
		/* check error entry */
		if(errorHDT)
		{
			free(pdrv);
			continue;
		}
		if(devname)
		{
			if(strcmp(pdrv->name,devname)==0)
			{
				pfirst = pdrv;
				break;
			}
			free(pdrv);
		}
		else
		{
			/* link into main list */
			if(!plast) pfirst = pdrv;
			else plast->pnext = pdrv;
			/* insert a first item in list */
			plast = pdrv;
		}
	}
	fclose(pfile);

	return pfirst;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Free the allocated resources for the <device> created by HDTLoad<device>.
	\param		HDTDevDRIVE_t* pdevs : <device> list to be cleared
	\return		int retVal : always 0
*/
int HDTClearDRIVE(HDTDevDRIVE_t *pdevs)
{
	HDTDevDRIVE_t *pdrv;
	while(pdevs)
	{
		pdrv = pdevs->pnext;
		free(pdevs);
		pdevs = pdrv;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Link the drives to the encoders.
	\param		HDTDevDRIVE_t* pdrives : list of drive methods
	\param		HDTDevENCODER_t* pencoders : list of encoders
	\return		int retVal :\n
						0 on success\n
						Negative value on failure (number of unconnected link)
*/
int HDTLinkDRV2ENC(HDTDevDRIVE_t *pdrives, HDTDevENCODER_t *pencoders)
{
	int errorlevel = 0;
	HDTDevENCODER_t *ptemp;

	while(pdrives)
	{
		switch(pdrives->drivetype)
		{
		case OMNI_DRIVE:
			/* third quad */
			ptemp = pencoders;
			while(ptemp)
			{
				if(strcmp(pdrives->encname3,ptemp->name)==0)
				{
					pdrives->penc3 = ptemp;
					break;
				}
				ptemp = ptemp->pnext;
			}
			if(!ptemp)
			{
				printf("Cannot find encoder \"%s\" for drive \"%s\"!\n",
					pdrives->encname3, pdrives->name);
				errorlevel--;
			}
			/* fourth quad */
			ptemp = pencoders;
			while(ptemp)
			{
				if(strcmp(pdrives->encname4,ptemp->name)==0)
				{
					pdrives->penc4 = ptemp;
					break;
				}
				ptemp = ptemp->pnext;
			}
			if(!ptemp)
			{
				printf("Cannot find encoder \"%s\" for drive \"%s\"!\n",
					pdrives->encname4, pdrives->name);
				errorlevel--;
			}
		case DIFFERENTIAL_DRIVE:
		case ACKERMAN_DRIVE:
			/* first quad */
			ptemp = pencoders;
			while(ptemp)
			{
				if(strcmp(pdrives->encname1,ptemp->name)==0)
				{
					pdrives->penc1 = ptemp;
					break;
				}
				ptemp = ptemp->pnext;
			}
			if(!ptemp)
			{
				printf("Cannot find encoder \"%s\" for drive \"%s\"!\n",
					pdrives->encname1, pdrives->name);
				errorlevel--;
			}
			/* second quad */
			ptemp = pencoders;
			while(ptemp)
			{
				if(strcmp(pdrives->encname2,ptemp->name)==0)
				{
					pdrives->penc2 = ptemp;
					break;
				}
				ptemp = ptemp->pnext;
			}
			if(!ptemp)
			{
				printf("Cannot find encoder \"%s\" for drive \"%s\"!\n",
					pdrives->encname2, pdrives->name);
				errorlevel--;
			}
			break;
		default:
			printf("Unknown drive type! [%d]\n",pdrives->drivetype);
			errorlevel--;
		}
		pdrives = pdrives->pnext;
	}
	return errorlevel;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		load all <device> entry found in the hdt file if devname is	null.
						else, load only the requested device. the return value is a pointer to the first <device>.
						the <devices> are	in a linked list allocated with dynamic memory.
						use	HDTClear<device> to free up the resources.
	\param		char* filename : hdt file to open
	\param		char* devname : device semantics
	\return		HDTDevIRTV_t* handle :\n
						0x0 on failure (no <device> found)\n
						(pointer to first <device>) if found
*/
HDTDevIRTV_t* HDTLoadIRTV(char *filename, char *devname)
{
	FILE *pfile;
	HDTEntry_t hdtEntry;
	HDTDevIRTV_t *pirtv, *pfirst=0x0, *plast=0x0;
	char *tokenbuff, *ptoken;
	int length;
	int errorHDT;

	pfile = fopen(filename,"rt");
	if(!pfile)
	{
		printf("Cannot open filename \"%s\"!\n",filename);
		return 0x0;
	}

	while(1)
	{
		if((length=HDTFindEntry(pfile,HDT_IRTV,&hdtEntry))<0)
			break;

		errorHDT = 0;
		/* create irtv data! */
		pirtv = malloc(sizeof(HDTDevIRTV_t));
		memset((void*)pirtv,0x00,sizeof(HDTDevIRTV_t));
		/* separate token processing buffer */
		tokenbuff = malloc(length);
		strcpy(tokenbuff,hdtEntry.buffer);
		ptoken = strtok(tokenbuff,"\"\n");
		/* skip irtv entry */
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get IRTV_LABEL value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(pirtv->name,ptoken);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get IRTV_TYPE value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%d",&pirtv->type);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get IRTV_LENGTH value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%d",&pirtv->length);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get IRTV_TOGMASK value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%x",&pirtv->togmask);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get IRTV_INVMASK value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%x",&pirtv->invmask);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get IRTV_MODE value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%d",&pirtv->mode);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get IRTV_BUFFSIZE value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%d",&pirtv->buffsize);
		}
		ptoken = strtok(0x0," \n");
		if(!ptoken)
		{
			printf("Cannot get IRTV_DELAY value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%d",&pirtv->delay);
		}
		free(tokenbuff);
		free(hdtEntry.buffer);
		/* check error entry */
		if(errorHDT)
		{
			free(pirtv);
			continue;
		}
		if(devname)
		{
			if(strcmp(pirtv->name,devname)==0)
			{
				pfirst = pirtv;
				break;
			}
			free(pirtv);
		}
		else
		{
			/* link into main list */
			if(!plast) pfirst = pirtv;
			else plast->pnext = pirtv;
			/* insert a first item in list */
			plast = pirtv;
		}
	}
	fclose(pfile);

	return pfirst;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Free the allocated resources for the <device> created by HDTLoad<device>.
	\param		HDTDevIRTV_t* pdevs : <device> list to be cleared
	\return		int retVal : always 0
*/
int HDTClearIRTV(HDTDevIRTV_t *pdevs)
{
	HDTDevIRTV_t *pdev;
	while(pdevs)
	{
		pdev = pdevs->pnext;
		free(pdevs);
		pdevs = pdev;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		load all <device> entry found in the hdt file if devname is	null.
						else, load only the requested device. the return value is a pointer to the first <device>.
						the <devices> are	in a linked list allocated with dynamic memory.
						use	HDTClear<device> to free up the resources.
	\param		char* filename : hdt file to open
	\param		char* devname : device semantics
	\return		HDTDevADC_t* handle :\n
						0x0 on failure (no <device> found)\n
						(pointer to first <device>) if found
*/
HDTDevADC_t* HDTLoadADC(char *filename, char *devname)
{
	FILE *pfile;
	HDTEntry_t hdtEntry;
	HDTDevADC_t *padc, *pfirst=0x0, *plast=0x0;
	char *tokenbuff, *ptoken;
	int length;
	int errorHDT;

	pfile = fopen(filename,"rt");
	if(!pfile)
	{
		printf("Cannot open filename \"%s\"!\n",filename);
		return 0x0;
	}

	while(1)
	{
		if((length=HDTFindEntry(pfile,HDT_ADC,&hdtEntry))<0)
			break;

		errorHDT = 0;
		/* create adc data! */
		padc = malloc(sizeof(HDTDevADC_t));
		memset((void*)padc,0x00,sizeof(HDTDevADC_t));
		/* separate token processing buffer */
		tokenbuff = malloc(length);
		strcpy(tokenbuff,hdtEntry.buffer);
		ptoken = strtok(tokenbuff,"\"\n");
		/* skip irtv entry */
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get ADC_LABEL value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(padc->name,ptoken);
		}
		ptoken = strtok(0x0," \"\n");
		if(!ptoken)
		{
			printf("Cannot get ADC_CONST value!\n");
			errorHDT = 1;
		}
		else
		{
			sscanf(ptoken,"%d",&padc->denom);
		}
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get ADC_PROCNAME value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(padc->procname,ptoken);
		}
		free(tokenbuff);
		free(hdtEntry.buffer);
		/* check error entry */
		if(errorHDT)
		{
			free(padc);
			continue;
		}
		if(devname)
		{
			if(strcmp(padc->name,devname)==0)
			{
				pfirst = padc;
				break;
			}
			free(padc);
		}
		else
		{
			/* link into main list */
			if(!plast) pfirst = padc;
			else plast->pnext = padc;
			/* insert a first item in list */
			plast = padc;
		}
	}
	fclose(pfile);

	return pfirst;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Free the allocated resources for the <device> created by HDTLoad<device>.
	\param		HDTDevADC_t* pdevs : <device> list to be cleared
	\return		int retVal : always 0
*/
int HDTClearADC(HDTDevADC_t *pdevs)
{
	HDTDevADC_t *pdev;
	while(pdevs)
	{
		pdev = pdevs->pnext;
		free(pdevs);
		pdevs = pdev;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		load all <device> entry found in the hdt file if devname is null.
						else, load only the requested device. the return value is a pointer to the first <device>.
						the <devices> are	in a linked list allocated with dynamic memory.
						use	HDTClear<device> to free up the resources.
	\param		char* filename : hdt file to open
	\param		char *devname : device semantics
	\return		HDTDevCOM_t* handle :\n
						0x0 on failure (no <device> found)\n
						(pointer to first <device>) if found
*/
HDTDevCOM_t* HDTLoadCOM(char *filename, char *devname)
{
	FILE *pfile;
	HDTEntry_t hdtEntry;
	HDTDevCOM_t *pcom, *pfirst=0x0, *plast=0x0;
	char *tokenbuff, *ptoken;
	int length;
	int errorHDT;

	pfile = fopen(filename,"rt");
	if(!pfile)
	{
		printf("Cannot open filename \"%s\"!\n",filename);
		return 0x0;
	}

	while(1)
	{
		if((length=HDTFindEntry(pfile,HDT_COM,&hdtEntry))<0)
			break;

		errorHDT = 0;
		/* create irtv data! */
		pcom = malloc(sizeof(HDTDevCOM_t));
		memset((void*)pcom,0x00,sizeof(HDTDevCOM_t));
		/* separate token processing buffer */
		tokenbuff = malloc(length);
		strcpy(tokenbuff,hdtEntry.buffer);
		ptoken = strtok(tokenbuff,"\"\n");
		/* skip irtv entry */
		ptoken = strtok(0x0,"\"\n");
		if(!ptoken)
		{
			printf("Cannot get SERIAL_LABEL value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(pcom->name,ptoken);
		}
		ptoken = strtok(0x0," \"\n");
		if(!ptoken)
		{
			printf("Cannot get SERIAL_DEVICE value!\n");
			errorHDT = 1;
		}
		else
		{
			strcpy(pcom->devname,ptoken);
		}
		free(tokenbuff);
		free(hdtEntry.buffer);
		/* check error entry */
		if(errorHDT)
		{
			free(pcom);
			continue;
		}
		if(devname)
		{
			if(strcmp(pcom->name,devname)==0)
			{
				pfirst = pcom;
				break;
			}
			free(pcom);
		}
		else
		{
			/* link into main list */
			if(!plast) pfirst = pcom;
			else plast->pnext = pcom;
			/* insert a first item in list */
			plast = pcom;
		}
	}
	fclose(pfile);

	return pfirst;
}
/* ------------------------------------------------------------------------- */
/**
	\brief		Free the allocated resources for the <device> created by HDTLoad<device>.
	\param		HDTDevCOM_t* pdevs : <device> list to be cleared
	\return		int retVal : always 0
*/
int HDTClearCOM(HDTDevCOM_t *pdevs)
{
	HDTDevCOM_t *pdev;
	while(pdevs)
	{
		pdev = pdevs->pnext;
		free(pdevs);
		pdevs = pdev;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
