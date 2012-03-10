/* $XConsortium: Screen.h /main/4 1995/10/30 13:12:42 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Screen.h
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
#ifndef _screen_h
#define _screen_h

/* External Interface */


extern void popup_screenBB( Widget shell) ;
extern void restoreScreen( Widget shell, XrmDatabase db) ;
extern void saveScreen( int fd) ;


#endif /* _screen_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
