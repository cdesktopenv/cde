/* $XConsortium: Audio.h /main/4 1995/10/30 13:07:37 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Audio.h
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
#ifndef _audio_h
#define _audio_h

/* External Interface */


extern void popup_audioBB( Widget shell) ;
extern void restoreAudio( Widget shell, XrmDatabase db) ;
extern void saveAudio( int fd) ;



#endif /* _audio_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
