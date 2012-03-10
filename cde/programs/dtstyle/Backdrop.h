/* $XConsortium: Backdrop.h /main/4 1995/10/30 13:07:56 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Backdrop.h
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
#ifndef _backdrop_h
#define _backdrop_h


/* External Interface */


extern void BackdropDialog( Widget parent) ;
extern void CheckWorkspace( void ) ;
extern void restoreBackdrop( Widget shell, XrmDatabase db) ;
extern void saveBackdrop( int fd) ;


#endif /* _backdrop_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

