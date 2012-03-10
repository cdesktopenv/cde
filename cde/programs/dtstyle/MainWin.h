/* $XConsortium: MainWin.h /main/4 1995/10/30 13:11:00 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        MainWin.h
 **
 **   Project:     DT 3.0
 **
 **  This file contains function definitions for the corresponding .c
 **  file
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _mainwin_h
#define _mainwin_h

/* external variable definitions */

extern char * _dt_version;

/* External Interface */


extern void init_mainWindow( Widget shell) ;
extern void callCancels( void ) ;
extern void activateCB_exitBtn( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
extern void restoreMain( Widget shell, XrmDatabase db) ;
extern void saveMain( int fd) ;


#endif /* _mainwin_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
