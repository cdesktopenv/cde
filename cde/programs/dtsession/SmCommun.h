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
/* $XConsortium: SmCommun.h /main/5 1996/01/04 19:29:28 barstow $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmCommun.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations
 **   needed for Communication for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smcommun_h
#define _smcommun_h
 
/* 
 *  #include statements 
 */


/* 
 *  #define statements 
 */

/* 
 * typedef statements 
 */

/*
 *  External variables  
 */
extern char SM_DISPLAY_LOCK[];
extern char SM_XSESSION_EXIT[];
extern char SM_RESOURCES_RELOAD[];
extern char SM_SAVE_SESSION[];

/*  
 *  External Interface  
 */
extern void StartMsgServer(void) ;
extern void ProcessClientMessage(XEvent *);
extern void ProcessScreenSaverMessage(XEvent *);
extern void RestoreDefaults(Atom) ;
extern void ProcessPropertyNotify(XEvent *) ;
extern void ProcessReloadActionsDatabase(void);
extern void ProcessEvent(Widget, XtPointer, XEvent *, Boolean *);

#endif /*_smcommun_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
