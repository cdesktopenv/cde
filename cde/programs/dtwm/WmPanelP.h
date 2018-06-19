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
/******************************<+>*************************************
 **********************************************************************
 **
 **  File:        WmPanelP.h
 **
 **  Project:     HP/Motif Workspace Manager (dtwm)
 **
 **  Description:
 **  -----------
 **  This file contains private declarations for the Front Panel
 **  subsystem.
 **
 **
 **********************************************************************
 **
 ** (c) Copyright 1992 HEWLETT-PACKARD COMPANY
 ** ALL RIGHTS RESERVED
 **
 **********************************************************************
 **********************************************************************
 **
 **
 **********************************************************************
 ******************************<+>*************************************/

#ifndef _WmPanelP_h
#define _WmPanelP_h

#include <Xm/XmP.h>
#include "WmGlobal.h"

#include "DataBaseLoad.h"

typedef struct PanelData * WmPanelistObject;

#define O_Panel(o) panel.form


#include <time.h>
#if defined(_AIX)
#include <sys/time.h> /* needed for timeval */
#endif


typedef struct
    {
    String	 pchResName;	/* match res_name member of WM_CLASS prop */
    Widget	 wControl;	/* control associated with client */
    Window	 winParent;	/* window to reparent client to */
    Position	 x, y;		/* position of client in winParent */
    Dimension	 width, height;	/* required size of client */
    ClientData	*pCD;		/* filled in when managed, else NULL */
    }	WmFpEmbeddedClientData, *WmFpEmbeddedClientList;

typedef struct _WmFpPushRecallClientData
{
   String	    pchResName;		/* match res_name member of WM_CLASS prop */
   Widget	    wControl;		/* control associated with client */
   ClientData	  * pCD;		/* filled in when managed, else NULL */
   struct timeval   tvTimeout;		/* Time for client to start */
} WmFpPushRecallClientData, *WmFpPushRecallClientList;



void    WmSubpanelPosted (Display *, Window);
Widget  WmPanelistAllocate(Widget, XtPointer, XtPointer);
void    WmPanelistShow (Widget);
void    WmPanelistSetWorkspace (Widget, int);
void    WmFrontPanelSetBusy (Boolean);
Widget  WmPanelistWindowToSubpanel (Display *, Window);
void    WmFrontPanelSessionSaveData ();



#endif /* _WmPanelP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
