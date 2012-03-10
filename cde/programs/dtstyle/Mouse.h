/* $XConsortium: Mouse.h /main/4 1995/10/30 13:11:19 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Mouse.h
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
#ifndef _mouse_h
#define _mouse_h

/* External Interface */


extern void popup_mouseBB( Widget shell) ;
extern void restoreMouse( Widget shell, XrmDatabase db) ;
extern void saveMouse( int fd) ;


#endif /* _mouse_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
