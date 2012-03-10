/* $XConsortium: WmHelp.h /main/4 1995/11/01 11:39:43 rswiston $ */
#ifdef WSM
/* 
 * (c) Copyright 1987, 1988, 1989, 1990, 1992 HEWLETT-PACKARD COMPANY 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = ""
#endif
#endif

#include "WmGlobal.h"
/*
 *    Various types of help available.
 */

#define WM_DT_HELP_VOLUME 			"FPanel"
#define WM_DT_HELP_TOPIC  			"_HOMETOPIC"

#define WM_DT_WSRENAME_HELP_TOPIC  		"WSRENAME"
#define WM_DT_WSRENAMEERROR_HELP_TOPIC 		"WSRENAMEERROR"
#define WM_DT_ICONBOX_TOPIC  			"ICONBOX"
#define WM_DT_WSPRESENCE_TOPIC  		"WSPRESENCE"

typedef struct _WmHelpTopicData
{
   char *helpVolume;
   char *helpTopic;

} WmHelpTopicData;


/********    Public Function Declarations    ********/

extern Boolean RestoreHelpDialogs(
			      WmScreenData *pSD);
extern void SaveHelpResources(
			      WmScreenData *pSD);
extern Boolean WmDtHelp (
			      String args);
extern Boolean WmDtHelpMode (
			      void);
extern void WmDtHelpOnVersion (
				Widget shell);
extern void WmDtStringHelpCB (Widget  theWidget,
			       XtPointer  client_data,
			       XtPointer  call_data );

extern void WmDtWmStringHelpCB (Widget  theWidget,
			       XtPointer  client_data,
			       XtPointer  call_data );

extern void WmDtDisplayTopicHelp (Widget widget,
                        	char * helpVolume,
                        	char * locationID);

extern void WmDtDisplayStringHelp (Widget widget,
				  char * helpString);

extern void WmDtWmTopicHelpCB (Widget  theWidget,
			       XtPointer  client_data,
			       XtPointer  call_data );

/********    End Public Function Declarations    ********/

/****************************   eof    ***************************/
#endif /* WSM */
