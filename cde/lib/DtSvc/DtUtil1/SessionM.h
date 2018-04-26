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
/* $TOG: SessionM.h /main/8 1998/07/30 12:10:49 mgreess $ */
/*                                                                           
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company                    
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.      
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                     
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                               
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SessionM.h
 **
 **  Description:
 **  -----------
 **  Contains all variables needed for SM messaging.  All messages serviced,
 **  and the tool class name for the session manager
 **
 **
 **
 *******************************************************************
 **  (c) Copyright Hewlett-Packard Company, 1990.  All rights are  
 **  reserved.  Copying or other reproduction of this program      
 **  except for archival purposes is prohibited without prior      
 **  written consent of Hewlett-Packard Company.		     
 ********************************************************************
 **
 **
 *****************************************************************************
 *************************************<+>*************************************/
#ifndef _SessionM_h
#define _SessionM_h

/*
 * Include files
 */
/* If <X11/Intrinsic.h> is included along with this file, <X11/Intrinsic.h>
   MUST be included first due to "typedef Boolean ..." problems.  The following
	  conditional #define is also part of the solution to this problem. */
#include <X11/X.h>
#include <X11/Intrinsic.h>

/*
 * Tool class for the session manager
 */
#define DtSM_TOOL_CLASS			"SESSIONMGR"

/*
 * SM_STARTUP_CHANGE an SM_STATE parameters
 */
#define DtSM_VERBOSE_MODE		1
#define DtSM_QUIET_MODE			2
#define DtSM_ASK_STATE			0
#define DtSM_HOME_STATE			3
#define DtSM_CURRENT_STATE		4 


/*
 * Atoms for session manager/style manager communication
 */
#define _XA_DT_SM_STM_PROTOCOL			"_DT_SM_STM_PROTOCOL"
#define _XA_DT_SM_SAVE_TO_HOME			"_DT_SM_SAVE_TO_HOME"
#define _XA_DT_SM_STATE_CHANGE			"_DT_SM_STATE_CHANGE"
#define _XA_DT_SM_RESTORE_DEFAULT		"_DT_SM_RESTORE_DEFAULT"
#define _XA_DT_SM_PREFERENCES			"_DT_SM_PREFERENCES"

/*
 * Atoms for lock changes
 */
#define _XA_DT_SM_LOCK_CHANGE		"_DT_SM_LOCK_CHANGE"
#define CoverScreenMask            	(1L<<0)
#define LockOnTimeoutMask          	(1L<<1)

/*
 * Parameters to be sent into session manager
 */

/*
 * Atoms for communicating configuration changes and state
 */
/*
 * Defines needed for getting/setting the sm window property on the root
 */
#define PROP_DT_SM_WINDOW_INFO_ELEMENTS     2
#define _XA_DT_SM_WINDOW_INFO       "_DT_SM_WINDOW_INFO"

/*
 * Defines needed for getting the session manager state off it's window
 */
#define PROP_DT_SM_STATE_INFO_ELEMENTS     13
#define _XA_DT_SM_STATE_INFO       "_DT_SM_STATE_INFO"

/*
 * Defines needed for the screen saver list property.
 */
#define _XA_DT_SM_SAVER_INFO       "_DT_SM_SAVER_INFO"

/*
 * Defines needed for the screen saver property
 */
#define PROP_DT_SM_SCREEN_INFO_ELEMENTS     5
#define _XA_DT_SM_SCREEN_INFO       "_DT_SM_SCREEN_INFO"

/*
 * Defines needed for the audio property
 */
#define PROP_DT_SM_AUDIO_INFO_ELEMENTS     4
#define _XA_DT_SM_AUDIO_INFO       "_DT_SM_AUDIO_INFO"

/*
 * Defines needed for the keyboard property
 */
#define PROP_DT_SM_KEYBOARD_INFO_ELEMENTS     3
#define _XA_DT_SM_KEYBOARD_INFO       "_DT_SM_KEYBOARD_INFO"

/*
 * Define needed for the font info property
 */
#define _XA_DT_SM_FONT_INFO       "_DT_SM_FONT_INFO"

/*
 * Define needed for the font info property
 */
#define _XA_DT_SM_POINTER_INFO       "_DT_SM_POINTER_INFO"

/*
 * Define needed for the preeditType info property
 */
#define _XA_DT_SM_PREEDIT_INFO       "_DT_SM_PREEDIT_INFO"

/*  bit definitions for SmStateInfo.flags */
#define SM_STATE_NONE		0
#define SM_STATE_START     	(1L << 0)
#define SM_STATE_CONFIRM     	(1L << 1)
#define SM_STATE_COMPAT     	(1L << 2)
#define SM_STATE_SEND     	(1L << 3)
#define SM_STATE_COVER     	(1L << 4)   /* Obsolete */
#define SM_STATE_LOTOUT		(1L << 5)   /* Obsolete */
#define SM_STATE_LOTOUTSTAT     (1L << 6)

#define SM_STATE_CYCLETIMEOUT   (1L << 7)
#define SM_STATE_LOCKTIMEOUT    (1L << 8)
#define SM_STATE_SAVERTIMEOUT   (1L << 9)
#define SM_STATE_RANDOM         (1L << 10)
#define SM_STATE_DISP_SESSION	(1L << 11)


#define SM_STATE_ALL	(SM_STATE_START   | SM_STATE_CONFIRM |\
			 SM_STATE_COMPAT  | SM_STATE_SEND    |\
			 SM_STATE_COVER   | SM_STATE_LOTOUT  |\
			 SM_STATE_LOTOUTSTAT | SM_STATE_CYCLETIMEOUT |\
                         SM_STATE_LOCKTIMEOUT | SM_STATE_SAVERTIMEOUT |\
                         SM_STATE_RANDOM | SM_STATE_DISP_SESSION)


/*
 * typedef statements for structures to be returned
 */
typedef struct
{
    int		 flags;
    int		 smStartState;
    int		 smConfirmMode;
    Boolean	 smCompatMode;
    Boolean	 smSendSettings;
    Boolean	 smCoverScreen;    /* Obsolete */
    Boolean	 smLockOnTimeout;  /* Obsolete */
    Boolean	 smLockOnTimeoutStatus;
    int          smCycleTimeout;
    int          smLockTimeout;
    int          smSaverTimeout;
    Boolean      smRandom;
    Boolean	 smDisplaySpecific;	/* If True, the currently running
					   session is for a specific display */

} SmStateInfo;

typedef struct
{
    char *saverList;
} SmSaverInfo;

typedef struct
{
    int flags;
    int smTimeout;
    int smInterval;
    int smPreferBlank;
    int smAllowExp;
} SmScreenInfo;

typedef struct
{
    int flags;
    int smBellPercent;
    unsigned int smBellPitch;
    unsigned int smBellDuration;
} SmAudioInfo;


typedef struct
{
    int flags;
    int smKeyClickPercent;
    int smGlobalAutoRepeat;
} SmKeyboardInfo;

/*
 * Function definitions
 */
extern Status _DtGetSmWindow(Display *, Window, Window *) ;
extern Status _DtGetSmState(Display *, Window, SmStateInfo *);
extern Status _DtSetSmState(Display *, Window, SmStateInfo *);
extern Status _DtGetSmSaver(Display *, Window, SmSaverInfo *);
extern void   _DtSetSmSaver(Display *, Window, SmSaverInfo *);
extern Status _DtGetSmScreen(Display *, Window, Atom, SmScreenInfo *);
extern Status _DtGetSmAudio(Display *, Window, Atom, SmAudioInfo *);
extern Status _DtGetSmKeyboard(Display *, Window, Atom, SmKeyboardInfo *);
extern Status _DtGetSmFont(Display *, Window, Atom, char **);
extern Status _DtGetSmPointer(Display *, Window, Atom, char **);
extern Status _DtGetSmPreedit(Display *, Window, Atom, char **);

#endif /* _SessionM_h */



