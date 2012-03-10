/* $XConsortium: SaveRestore.h /main/5 1996/03/25 00:53:26 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SaveRestore.h
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

#ifndef _saverestore_h
#define _saverestore_h

/* External Interface */


extern void saveSessionCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
extern Boolean restoreSession( 
                        Widget shell,
                        char *name) ;

extern void restoreFonts( Widget shell, XrmDatabase db) ;
extern void saveFonts( int fd) ;
extern void restoreMain( Widget shell, XrmDatabase db) ;
extern void saveMain( int fd) ;
extern void restoreMouse( Widget shell, XrmDatabase db) ;
extern void saveMouse( int fd) ;
extern void restoreAudio( Widget shell, XrmDatabase db) ;
extern void saveAudio( int fd) ;
extern void restoreScreen( Widget shell, XrmDatabase db) ;
extern void saveScreen( int fd) ;
extern void restoreColor( Widget shell, XrmDatabase db) ;
extern void saveColor( int fd) ;
extern void restoreColorEdit( Widget shell, XrmDatabase db) ;
extern void saveColorEdit( int fd) ;
extern void restoreKeybd( Widget shell, XrmDatabase db) ;
extern void saveKeybd( int fd) ;
extern void restoreStartup( Widget shell, XrmDatabase db) ;
extern void saveStartup( int fd) ;
extern void restoreBackdrop( Widget shell, XrmDatabase db) ;
extern void saveBackdrop( int fd) ;
extern void restoreI18n( Widget shell, XrmDatabase db) ;
extern void saveI18n( int fd) ;


#endif /* _saverestore_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */


