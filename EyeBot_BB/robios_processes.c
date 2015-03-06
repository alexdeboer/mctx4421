/** File written by Stuart Howard */
/** most function from 20080717 - azman@ee.uwa.edu.au */
/** Based on code by Thomas Sommer */

#include <signal.h>
#include <wait.h>

#include <eyebot.h>
#include "header.h"

static int refresh = 0;

void sigchld_handler(int signum)
{
	int status;
	pid_t  pid;
	process_t *pproc;

	if(proclist.count>0)
	{
		pid = waitpid(WAIT_ANY,&status,WNOHANG); //non-blocking 
		if(pid <0)
		{
			fprintf(stdout,"Error! Got SIGCHLD, but no child returned!\n");
		}
		else if(pid>0)
		{
			if(WIFEXITED(status)||WIFSIGNALED(status))
			{
				//remove process from list!
				pproc = del_process(pid);
				fprintf(stdout,"Program %s %s [%d].\n", pproc->name,
					WIFEXITED(status)?"exited":"terminated",status);
				free(pproc);
				//process_suspend = 0;
				refresh = 1;
			}
			else if(WIFSTOPPED(status))
			{
				pproc = get_process(pid);
				fprintf(stdout,"Program %s suspended.\n", pproc->name);
			}
			else
			{
				//don't think there could be any other condition...
				pproc = get_process(pid);
				fprintf(stdout,"Signal from %d: %s?\n",pid,pproc->name);
			}
		}
		else
		{
			fprintf(stdout,"Huh?! Got SIGCHLD, but returned PID 0!\n");
		}
	}
}

/** 
 * Add a new process to the process list. Return the current number
 * 		number of processes in the list.
 */
pid_t add_process(process_t *pproc)
{
	if(proclist.plast)
		proclist.plast->pnext = pproc;
	else
		proclist.pfirst = pproc;
	proclist.plast = pproc;
	proclist.count++;
	return proclist.count;
}

int chk_process(void)
{
	int status, count = 0;
	pid_t pid;
	process_t *ptest, *pprev = 0x0, *ptemp;
	ptest = proclist.pfirst;
	while(ptest)
	{
		pid = waitpid(ptest->pid, &status, WNOHANG | WUNTRACED);
		if(ptest->pid==pid&&WIFEXITED(status))
		{
			ptemp = ptest->pnext;
			// modify link
			if(pprev) pprev->pnext = ptest->pnext;
			else proclist.pfirst = ptest->pnext;
			// check last pointer
			if(ptest==proclist.plast)
				proclist.plast = pprev;
			free(ptest);
			ptest = ptemp;
			count++;
			proclist.count--;
		}
		else
		{
			pprev = ptest;
			ptest = ptest->pnext;
		}
	}
	return count;
}

process_t* del_process(pid_t pid)
{
	process_t *ptest, *pprev = 0x0;
	ptest = proclist.pfirst;
	while(ptest)
	{
		if(ptest->pid==pid)
		{
			// modify link
			if(pprev) pprev->pnext = ptest->pnext;
			else proclist.pfirst = ptest->pnext;
			// check last pointer
			if(ptest==proclist.plast)
				proclist.plast = pprev;
			proclist.count--;
			break;
		}
		pprev = ptest;
		ptest = ptest->pnext;
	}
	return ptest;
}

process_t *getProcess(int i)
{
	int j=0;
	int procCount = proclist.count;
	process_t *ptest;
	ptest = proclist.pfirst;
	if (i >= procCount)
	{
		return 0x0;
	}
	for (j=0;j<procCount;j++)
	{
		if (i==j)
		{
			return ptest;
		}
		ptest = ptest->pnext;
	}
	return 0x0;
}

process_t* get_process(pid_t pid)
{
	process_t *ptest;
	ptest = proclist.pfirst;
	while(ptest)
	{
		if(ptest->pid==pid)
			break;
		ptest = ptest->pnext;
	}
	return ptest;
}

void loadMenuItems(listmenu_t *listmenu)
{
	int procCount = proclist.count;
	int i=0;
	process_t *proc;
	if (procCount==0)
	{
		return;
	}
	for (i=0;i<procCount;i++)
	{
		proc = getProcess(i);
		listmenuAddItem(listmenu,proc->name,proc);
	}
}

void processMenu()
{
	keycode = 0x0;
	refresh = 0;
	listmenu_t listmenu, *listtemp;
	int exit = 0;
	listtemp = LCDGetList();
	LCDClear();
	listmenuInit(&listmenu,"Process List");
	loadMenuItems(&listmenu);
	LCDList(&listmenu);
	LCDRefresh();
	while (!exit)
	{
		LCDRefresh();
		LCDMenu("","","","BACK");
		if (proclist.count == 0)
		{
			LCDMenu("RUN","","","BACK");
			break;
		}
		//keycode = KEYGet();
		switch (keycode)
		{
			case KEY1:
			{
				keycode = 0x0;
				break;
			}
			case KEY4:
			{
				exit = 1;
				keycode = 0x0;
				LCDMenu("RUN","","","BACK");
				break;
			}
			case KEY_LISTUP:
			{
				LCDListScrollUp();
				keycode = 0x0;
				break;
			}
			case KEY_LISTDN:
			{
				LCDListScrollDown();
				keycode = 0x0;
				break;
			}
			default:
			{
				int test = 0;
				while(keycode&&!(keycode&KEY_LIST1))
				{
					keycode >>= 1; test++;
				}
				LCDListIndex(test);
				keycode = 0x0;
			}
		}
		usleep(LOOPSLEEPTIME);
	}
	
	LCDClear();
	LCDList(listtemp);
}

int load_proc(listmenu_t *plist)
{
	int loop, status;
	process_t *pproc;
	pid_t pid;
	rgb_t stat_color;

	// always update process list!
	chk_process();

	if(proclist.count>0)
	{
		// check previous items!
		if(plist->pitems)
		{
			free(plist->pitems);
			plist->pitems = 0x0;
		}
		// setup filemenu list
		listmenuInit(plist,"Process List");
		pproc = proclist.pfirst;
		for(loop=0; loop<plist->size; loop++)
		{
			pid = waitpid(pproc->pid, &status, WNOHANG | WUNTRACED);
			stat_color = LCD_BLUE;
			if(WIFEXITED(status)||WIFSIGNALED(status))
				stat_color = LCD_RED;
			else if(WIFSTOPPED(status))
				stat_color = LCD_GREEN;
			listmenuAddItem(plist,"pproc->name",(void *) pproc);
			pproc = pproc->pnext;
		}
		LCDList(plist);
	}
	
	return proclist.count;
}

void runProcess(char *execfile)
{
	pid_t ppid = fork();
	process_t *pproc = malloc(sizeof(process_t));
	if (IRTVGetStatus()==1)
	{
		IRTVRelease();
	}
	if (ppid<0)
	{
		fprintf(stdout,"ERROR: Unable to fork the process\n");
	}
	else if (ppid == 0)
	{
		//child process
		execl(execfile,execfile,NULL);
		exit(EXIT_FAILURE);
	}
	else
	{
		//parent process
		char *name = malloc(MAX_FILECHAR);
		sprintf(name,"%d: %s",ppid,execfile);
		pproc->pid = ppid;
		strcpy_n(pproc->name,name,MAX_FILECHAR);
		pproc->pnext = NULL;
		add_process(pproc);
		free(name);
	}
}
