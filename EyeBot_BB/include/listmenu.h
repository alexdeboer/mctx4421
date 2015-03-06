#ifndef _listmenu_
#define _listmenu_

//listmenu functions
void listmenuInit(listmenu_t *,char *);
void listmenuAddItem(listmenu_t *,char *,void *);
void listmenuFreeItems(listmenu_t *);
int listmenuSize(listmenu_t);
void listmenuToggleScroll(listmenu_t *);
int listmenuGetScroll(listmenu_t);
void listmenuChangeItemText(listmenu_t *listmenu, char *newText,int index);
int listmenuGetIndex(listmenu_t listmenu);
char *listmenuGetItemText(listmenu_t listmenu);

#endif
