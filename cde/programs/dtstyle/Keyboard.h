/* $XConsortium: Keyboard.h /main/4 1995/10/30 13:10:26 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Keyboard.h
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
#ifndef _keyboard_h
#define _keyboard_h

/* External Interface */


extern void popup_keyboardBB( Widget shell) ;
extern void restoreKeybd( Widget shell, XrmDatabase db) ;
extern void saveKeybd( int fd) ;


#endif /* _keyboard_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
