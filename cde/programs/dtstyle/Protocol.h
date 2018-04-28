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
/* $XConsortium: Protocol.h /main/5 1996/03/25 00:53:06 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Protocol.h
 **
 **   Project:     DT 3.0
 **
 **  This file contains function definitions for the corresponding .c
 **  file
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _protocol_h
#define _protocol_h

extern Window   smWindow;


/* External Interface */


extern void MwmReparentNotify(
                            Widget w,
                            XtPointer client_data,
                            XEvent *event ) ;
extern void InitDtstyleProtocol( void ) ;
extern void GetSessionSaveMode(  unsigned char **mode ) ;
extern void SetWindowProperties( void ) ;
extern void SmSaveHomeSession(  
                                int origStartState,
                                int origConfirmMode);
extern void SmDefaultAudioSettings( void ) ;
extern void SmDefaultScreenSettings( void ) ;
extern void SmDefaultKeyboardSettings( void ) ;
extern void SmNewAudioSettings(
                                int volume,
                                int tone,
                                int duration ) ;

extern void SmNewScreenSettings(
                                int timeout,
                                int blanking,
                                int interval,
                                int exposures ) ;
extern void SmNewLockSettings(
                                int lock,
                                int cover ) ;

extern void SmNewKeyboardSettings(
                                int keyClickPercent,
                                int  autoRepeat) ;
extern void SmNewPointerSettings(
                                char *pointerString) ;
extern void SmNewStartupSettings( void ) ;
extern void SmNewFontSettings( 
                                char *fontResourceString) ;

extern void SmNewPreeditSettings( 
                                char *preeditResourceString) ;


#endif /* _protocol_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
