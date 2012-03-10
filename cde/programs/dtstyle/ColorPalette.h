/* $XConsortium: ColorPalette.h /main/4 1995/10/30 13:09:05 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ColorPalette.h
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
#ifndef _colorpalette_h
#define _colorpalette_h

/* External Interface */


extern Bool AllocatePaletteCells( Widget shell) ;
extern int ReColorPalette( void ) ;
extern void CheckMonitor( Widget shell) ;


#endif /* _colorpalette_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
