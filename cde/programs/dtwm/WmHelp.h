/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* 
 * (c) Copyright 1987, 1988, 1989, 1990, 1992 HEWLETT-PACKARD COMPANY 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 

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
