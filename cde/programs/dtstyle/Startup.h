/* $XConsortium: Startup.h /main/4 1995/10/30 13:12:58 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Startup.h
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
#ifndef _startup_h
#define _startup_h

/* External Interface */


extern void popup_startupBB( Widget shell) ;
extern void restoreStartup( Widget shell, XrmDatabase db) ;
extern void saveStartup( int fd) ;


#endif /* _startup_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
