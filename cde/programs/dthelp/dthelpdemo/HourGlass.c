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
/* $XConsortium: HourGlass.c /main/3 1995/11/08 09:18:28 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        HourGlass.c
 **
 **   Project:     CDE dthelpdemo sample program.
 **
 **   Description: This module contains a simple function for
 **                creating an hourglass cursor.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992, 1993, 1994
 **      Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/


#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#define time32_width 32
#define time32_height 32
#define time32_x_hot 15
#define time32_y_hot 15
static unsigned char time32_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f,
   0x8c, 0x00, 0x00, 0x31, 0x4c, 0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x32,
   0x4c, 0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x32,
   0x8c, 0x00, 0x00, 0x31, 0x0c, 0x7f, 0xfe, 0x30, 0x0c, 0xfe, 0x7f, 0x30,
   0x0c, 0xfc, 0x3f, 0x30, 0x0c, 0xf8, 0x1f, 0x30, 0x0c, 0xe0, 0x07, 0x30,
   0x0c, 0x80, 0x01, 0x30, 0x0c, 0x80, 0x01, 0x30, 0x0c, 0x60, 0x06, 0x30,
   0x0c, 0x18, 0x18, 0x30, 0x0c, 0x04, 0x20, 0x30, 0x0c, 0x02, 0x40, 0x30,
   0x0c, 0x01, 0x80, 0x30, 0x8c, 0x00, 0x00, 0x31, 0x4c, 0x80, 0x01, 0x32,
   0x4c, 0xc0, 0x03, 0x32, 0x4c, 0xf0, 0x1f, 0x32, 0x4c, 0xff, 0xff, 0x32,
   0xcc, 0xff, 0xff, 0x33, 0x8c, 0xff, 0xff, 0x31, 0xfe, 0xff, 0xff, 0x7f,
   0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00};

#define time32m_width 32
#define time32m_height 32
static unsigned char time32m_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xcf, 0x00, 0x00, 0xf3, 0x6e, 0x00, 0x00, 0x76, 0x6e, 0x00, 0x00, 0x76,
   0x6e, 0x00, 0x00, 0x76, 0x6e, 0x00, 0x00, 0x76, 0x6e, 0x00, 0x00, 0x76,
   0xce, 0x00, 0x00, 0x73, 0x8e, 0x7f, 0xfe, 0x71, 0x0e, 0xff, 0xff, 0x70,
   0x0e, 0xfe, 0x7f, 0x70, 0x0e, 0xfc, 0x3f, 0x70, 0x0e, 0xf8, 0x1f, 0x70,
   0x0e, 0xe0, 0x07, 0x70, 0x0e, 0xe0, 0x07, 0x70, 0x0e, 0x78, 0x1e, 0x70,
   0x0e, 0x1c, 0x38, 0x70, 0x0e, 0x06, 0x60, 0x70, 0x0e, 0x03, 0xc0, 0x70,
   0x8e, 0x01, 0x80, 0x71, 0xce, 0x00, 0x00, 0x73, 0x6e, 0x80, 0x01, 0x76,
   0x6e, 0xc0, 0x03, 0x76, 0x6e, 0xf0, 0x1f, 0x76, 0x6e, 0xff, 0xff, 0x76,
   0xee, 0xff, 0xff, 0x77, 0xcf, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


#define time16_x_hot 7
#define time16_y_hot 7
#define time16_width 16
#define time16_height 16
static unsigned char time16_bits[] = {
   0x00, 0x00, 0xfe, 0x7f, 0x14, 0x28, 0x14, 0x28, 0x14, 0x28, 0x24, 0x24,
   0x44, 0x22, 0x84, 0x21, 0x84, 0x21, 0x44, 0x22, 0x24, 0x24, 0x14, 0x28,
   0x94, 0x29, 0xd4, 0x2b, 0xfe, 0x7f, 0x00, 0x00};

#define time16m_width 16
#define time16m_height 16
static unsigned char time16m_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f,
   0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f,
   0xfe, 0x7f, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff};


/********    End Public Function Declarations    ********/

/*************************************<->*************************************
 *
 *  Cursor GetHourGlassCursor ()
 *
 *
 *  Description:
 *  -----------
 *  Builds and returns the appropriate Hourglass cursor
 *
 *
 *  Inputs:
 *  ------
 *  dpy	= display
 * 
 *  Outputs:
 *  -------
 *  Return = cursor.
 *
 *  Comments:
 *  --------
 *  None. (None doesn't count as a comment)
 * 
 *************************************<->***********************************/
static Cursor GetHourGlassCursor(
  Display *dpy)

{
    char        *bits;
    char        *maskBits;
    unsigned int width;
    unsigned int height;
    unsigned int xHotspot;
    unsigned int yHotspot;
    Pixmap       pixmap;
    Pixmap       maskPixmap;
    XColor       xcolors[2];
    int          scr;
    unsigned int cWidth;
    unsigned int cHeight;
    int		 useLargeCursors = 0;
    static Cursor waitCursor=0;


    if (waitCursor != 0)
      return(waitCursor);

    if (XQueryBestCursor (dpy, DefaultRootWindow(dpy), 
	32, 32, &cWidth, &cHeight))
    {
	if ((cWidth >= 32) && (cHeight >= 32))
	{
	    useLargeCursors = 1;
	}
    }

    if (useLargeCursors)
    {
	width    = time32_width;
	height   = time32_height;
	bits     = (char *) time32_bits;
	maskBits = (char *) time32m_bits;
	xHotspot = time32_x_hot;
	yHotspot = time32_y_hot;
    }
    else
    {
	width    = time16_width;
	height   = time16_height;
	bits     = (char *) time16_bits;
	maskBits = (char *) time16m_bits;
	xHotspot = time16_x_hot;
	yHotspot = time16_y_hot;
    }

    pixmap = XCreateBitmapFromData (dpy, 
		     DefaultRootWindow(dpy), bits, 
		     width, height);

  
    maskPixmap = XCreateBitmapFromData (dpy, 
		     DefaultRootWindow(dpy), maskBits, 
		     width, height);

    xcolors[0].pixel = BlackPixelOfScreen(DefaultScreenOfDisplay(dpy));
    xcolors[1].pixel = WhitePixelOfScreen(DefaultScreenOfDisplay(dpy));

    XQueryColors (dpy, 
		  DefaultColormapOfScreen(DefaultScreenOfDisplay
					  (dpy)), xcolors, 2);

    waitCursor = XCreatePixmapCursor (dpy, pixmap, maskPixmap,
				      &(xcolors[0]), &(xcolors[1]),
				      xHotspot, yHotspot);
    XFreePixmap (dpy, pixmap);
    XFreePixmap (dpy, maskPixmap);

    return (waitCursor);
}

 
/*************************************<->*************************************
 *
 *  void TurnOnHourGlass
 *
 *
 *  Description:
 *  -----------
 *  sets the window cursor to an hourglass
 *
 *
 *  Inputs:
 *  ------
 *  w   = Widget
 *
 *  Outputs:
 *  -------
 *  None
 *
 *  Comments:
 *  --------
 *  None. (None doesn't count as a comment)
 *
 *************************************<->***********************************/
void TurnOnHourGlass(
        Widget w)
{
    Cursor cursor;
    
    cursor = GetHourGlassCursor(XtDisplay(w));

    XDefineCursor(XtDisplay(w), XtWindow(w), cursor);
    XFlush(XtDisplay(w));
}


/*************************************<->*************************************
 *
 *  void TurnOffHourGlass
 *
 *
 *  Description:
 *  -----------
 *  Removed the hourglass cursor from a window
 *
 *
 *  Inputs:
 *  ------
 *  w = Widget
 *
 *  Outputs:
 *  -------
 *  None
 *
 *  Comments:
 *  --------
 *  None. (None doesn't count as a comment)
 *
 *************************************<->***********************************/
void TurnOffHourGlass(
        Widget w)

{
   
    XUndefineCursor(XtDisplay(w), XtWindow(w));
    XFlush(XtDisplay(w));
}














