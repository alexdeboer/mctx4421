#ifndef _m7_header_H_
#define _m7_header_H_

#define M6MAIN_VERSION "0.92"
#define MAX_FILECHAR 400
#define MAX_USERPROG 32
#define MAX_FULLPATH 320
#define MAX_MSGLEN 64

#define MAX_COOLTIME 300000 /* usec */
#define MAX_WAITTIME 250000 /* usec */
#define MAX_WAITKILL 100000 /* usec */
#define MSG_WAITTIME 2 /* sec */
#define TOUCH_IDLETIME_USEC 250000
#define LOOPSLEEPTIME 250000

//custom structures
typedef struct _process_t
{
	pid_t pid;
	char name[MAX_FILECHAR];
	struct _process_t *pnext; /* linked list */
}
process_t;

typedef struct _proclist_t
{
	int count; /* max is MAX_USERPROG */
	process_t *pfirst, *plast;
}
proclist_t;

typedef struct
{
	int irEnable;
	int runProg;
	int irDelay;
	DeviceSemantics irCode;
	char *progName;
} robios_params;

//demo screen functions
int demoPage(void);

//software screen functions
int show_exec(void);

//hardware screen functions
int show_hdt(void);

//process functions
pid_t add_process(process_t *);
int chk_process(void);
process_t* del_process(pid_t);
process_t* get_process(pid_t);
int procmenu(pid_t);
int load_proc(listmenu_t *);
void sigchld_handler(int);
void runProcess(char *);

//common functions
void IRScrollUp(void);
void IRScrollDown(void);
char *keyboardInput(void);
void trimline(char *src);

//global variables
#ifndef _proclist_
	#define _proclist_
	proclist_t proclist;
	#endif
#endif
keycode_t keycode;
int screenRefresh;
robios_params robiosParams;

//thread variables
pthread_t threadKeyInput;
pthread_t threadIRInput;
pthread_t threadScreenRefresh;
