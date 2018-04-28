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
/* $TOG: SmProtocol.c /main/8 1998/08/05 13:38:17 samborn $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmProtocol.c
 **
 **  Project:     HP DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file contains all modules that handle the initialization and
 **  interning of all atoms used by the session manager.
 **
 **
 *****************************************************************************
 *************************************<+>*************************************/
/*                                                               
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1993, 1994, 1996 Hewlett-Packard Company        
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.       
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                      
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                                
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <Dt/Wsm.h>
#include <Dt/WsmP.h>
#include <Dt/Connect.h>
#include <Dt/SessionM.h>
#include <Dt/SessionP.h>
#include "Sm.h"
#include "SmProtocol.h"


/*
 * Global variable definitions
 */
Atom     XaWmProtocols;
Atom     XaWmSaveYourself;
Atom     XaWmState;
Atom     XaWmDtHints;
Atom     XaSmSaveMode;
Atom     XaSmRestoreMode;
Atom     XaSmRestoreDir;
Atom     XaSmStartAckWindow;
Atom     XaSmStopAckWindow;
Atom     XaWmWindowAck;
Atom     XaWmExitSession;
Atom     XaWmLockDisplay;
Atom     XaWmReady;
Atom     XaSmWmProtocol;
Atom	 XaVsmInfo;
Atom     XaDtSmStmProtocol;
Atom     XaDtSmSaveToHome;
Atom     XaDtSmStateChange;
Atom     XaDtSmRestoreDefault;
Atom     XaDtSmLockChange;
Atom	 XaDtSmStateInfo;
Atom	 XaDtSmSaverInfo;
Atom     XaDtSmScreenInfo;
Atom     XaDtSmAudioInfo;
Atom     XaDtSmKeyboardInfo;
Atom     XaDtSmFontInfo;
Atom     XaDtSmPointerInfo;
Atom	 XaSmScreenSaveRet;
Atom     XaDtSmPreeditInfo;


/*************************************<->*************************************
 *
 *  InitProtocol ()
 *
 *
 *  Description:
 *  -----------
 *  Handles interning of atoms used by the session manager.  All should
 *  already exist because the session manager is a toolkit application.
 *  Also handles setting of selection mechanism used by session manager to
 *  distribute unique file names.
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void 
InitProtocol( void )
{
    enum { XA_WM_PROTOCOLS, XA_WM_SAVE_YOURSELF,
	   XA_DT_SAVE_MODE, XA_WM_STATE,
	   XA_DT_SM_STM_PROTOCOL, XA_DT_SM_SAVE_TO_HOME,
	   XA_DT_SM_STATE_CHANGE, XA_DT_SM_RESTORE_DEFAULT,
	   XA_DT_SM_LOCK_CHANGE, XA_DT_SM_SCREEN_INFO,
	   XA_DT_SM_STATE_INFO, XA_DT_SM_SAVER_INFO,
	   XA_DT_SM_AUDIO_INFO, XA_DT_SM_KEYBOARD_INFO,
	   XA_DT_SM_FONT_INFO, XA_DT_SM_POINTER_INFO,
	   XA_DT_SM_PREEDIT_INFO, NUM_ATOMS };
    static char *atom_names[] = { _XA_WM_PROTOCOLS, _XA_WM_SAVE_YOURSELF,
	   _XA_DT_SAVE_MODE, _XA_WM_STATE,
	   _XA_DT_SM_STM_PROTOCOL, _XA_DT_SM_SAVE_TO_HOME,
	   _XA_DT_SM_STATE_CHANGE, _XA_DT_SM_RESTORE_DEFAULT,
	   _XA_DT_SM_LOCK_CHANGE, _XA_DT_SM_SCREEN_INFO,
	   _XA_DT_SM_STATE_INFO, _XA_DT_SM_SAVER_INFO,
	   _XA_DT_SM_AUDIO_INFO, _XA_DT_SM_KEYBOARD_INFO,
	   _XA_DT_SM_FONT_INFO, _XA_DT_SM_POINTER_INFO,
	   _XA_DT_SM_PREEDIT_INFO };

    Atom atoms[XtNumber(atom_names)];

    XInternAtoms(smGD.display, atom_names, XtNumber(atom_names), False, atoms);

    XaWmProtocols = atoms[XA_WM_PROTOCOLS];
    XaWmSaveYourself = atoms[XA_WM_SAVE_YOURSELF];
    XaSmSaveMode = atoms[XA_DT_SAVE_MODE];
    XaWmState = atoms[XA_WM_STATE];
    XaDtSmStmProtocol = atoms[XA_DT_SM_STM_PROTOCOL];
    XaDtSmSaveToHome = atoms[XA_DT_SM_SAVE_TO_HOME];
    XaDtSmStateChange = atoms[XA_DT_SM_STATE_CHANGE];
    XaDtSmRestoreDefault = atoms[XA_DT_SM_RESTORE_DEFAULT];
    XaDtSmLockChange = atoms[XA_DT_SM_LOCK_CHANGE];
    XaDtSmScreenInfo = atoms[XA_DT_SM_SCREEN_INFO];
    XaDtSmStateInfo = atoms[XA_DT_SM_STATE_INFO];
    XaDtSmSaverInfo = atoms[XA_DT_SM_SAVER_INFO];
    XaDtSmAudioInfo = atoms[XA_DT_SM_AUDIO_INFO];
    XaDtSmKeyboardInfo = atoms[XA_DT_SM_KEYBOARD_INFO];
    XaDtSmFontInfo = atoms[XA_DT_SM_FONT_INFO];
    XaDtSmPointerInfo = atoms[XA_DT_SM_POINTER_INFO];
    XaDtSmPreeditInfo = atoms[XA_DT_SM_PREEDIT_INFO];
}

