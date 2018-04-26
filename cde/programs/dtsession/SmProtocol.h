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
/* $TOG: SmProtocol.h /main/7 1998/08/05 13:38:42 samborn $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmProtocol.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   Protocol Handling for the session manager
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*                                                               
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1993, 1994, 1996 Hewlett-Packard Company        
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.       
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                      
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                                
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _smprotocol_h
#define _smprotocol_h
 
/* 
 *  #include statements 
 */
#include <X11/Xlib.h>


/* 
 *  #define statements 
 */

/*
 * Property types used by the session manager not defined elsewhere
 */
#define	_XA_WM_PROTOCOLS		"WM_PROTOCOLS"
#define _XA_WM_STATE			"WM_STATE"
#define _XA_WM_SAVE_YOURSELF		"WM_SAVE_YOURSELF"



/* 
 * typedef statements 
 */

/*
 *  External variables  
 */

/*
 * Atoms interned by the session manager in order to communicate with
 * the other clients in the session
 */
extern Atom		XaWmProtocols;
extern Atom		XaWmSaveYourself;
extern Atom		XaWmState;
extern Atom     	XaWmDtHints;
extern Atom		XaSmSaveMode;
extern Atom		XaSmRestoreMode;
extern Atom		XaSmRestoreDir;
extern Atom		XaSmStartAckWindow;
extern Atom		XaSmStopAckWindow;
extern Atom		XaWmWindowAck;
extern Atom		XaWmExitSession;
extern Atom		XaWmLockDisplay;
extern Atom		XaWmReady;
extern Atom		XaSmWmProtocol;
extern Atom		XaVsmInfo;
extern Atom    	 	XaDtSmStateInfo;
extern Atom    	 	XaDtSmSaverInfo;
extern Atom     	XaDtSmStmProtocol;
extern Atom     	XaDtSmSaveToHome;
extern Atom     	XaDtSmStateChange;
extern Atom     	XaDtSmRestoreDefault;
extern Atom     	XaDtSmLockChange;
extern Atom     	XaDtSmScreenInfo;
extern Atom     	XaDtSmAudioInfo;
extern Atom     	XaDtSmKeyboardInfo;
extern Atom     	XaDtSmFontInfo;
extern Atom     	XaDtSmPointerInfo;
extern Atom	 	XaSmScreenSaveRet;
extern Atom     	XaDtSmPreeditInfo;

/*  
 *  External Interface  
 */


extern void InitProtocol(void);


#endif /*_smprotocols_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
