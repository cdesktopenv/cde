/* $XConsortium: Font.h /main/4 1995/10/30 13:09:39 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Font.h
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
#ifndef _font_h
#define _font_h

/* typedef statements */

typedef struct {
  XmFontList sysFont;
  XmFontList userFont;
  String     sysStr;
  String     userStr;
  XmString   pointSize;
} Fontset;

/* External Interface */


extern void popup_fontBB( Widget shell) ;
extern void restoreFonts( Widget shell, XrmDatabase db) ;
extern void saveFonts( int fd) ;


#endif /* _font_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
