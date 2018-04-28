/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: ColorEdit.h /main/4 1995/10/30 13:08:12 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ColorEdit.h
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
#ifndef _coloredit_h
#define _coloredit_h

/*  Global structure for Color Editor */

typedef struct _EditData {
  Widget      DialogShell;
  Widget      main_form;
  Widget      sliderForm;
  Widget      oldButton;
  Widget      newButton;
  Widget      grabColor;
  Widget      redLabel;
  Widget      greenLabel;
  Widget      blueLabel;
  Widget      hueLabel;
  Widget      satLabel;
  Widget      valLabel;
  Widget      redScale;
  Widget      greenScale;
  Widget      blueScale;
  Widget      hueScale;
  Widget      satScale;
  Widget      valScale;
  int         current_scale;
  ColorSet    *color_set;
  ColorSet    oldButtonColor;
  XmColorProc calcRGB;
  Pixmap      pixmap25;
  Pixmap      pixmap75;

} EditData;

extern EditData edit;

/* External Interface */


extern void ColorEditor( 
                        Widget parent,
                        ColorSet *color_set) ;
extern void RGBtoHSV( 
#if NeedWidePrototypes
                        unsigned int r,
                        unsigned int g,
                        unsigned int b,
#else
                        unsigned short r,
                        unsigned short g,
                        unsigned short b,
#endif
                        int *h,
                        int *s,
                        int *v) ;
extern void restoreColorEdit( 
                        Widget shell,
                        XrmDatabase db) ;
extern void saveColorEdit( 
                        int fd) ;



#endif /* _coloredit_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
