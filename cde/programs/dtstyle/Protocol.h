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
