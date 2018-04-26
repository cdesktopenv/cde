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
/* $XConsortium: HourGlass.c /main/5 1996/06/21 17:22:09 ageorge $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        HourGlass.c
 **
 **   Project:     dt Dt Utility function
 **
 **   Description: This module contains a simple function for
 **                creating an hourglass cursor.
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*****************************************************************************
 *
 * (c) Copyright 1989,1990 OPEN SOFTWARE FOUNDATION, INC.
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY
 * ALL RIGHTS RESERVED
 *
 * 	THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED
 * AND COPIED ONLY IN ACCORDANCE WITH THE TERMS OF SUCH LICENSE AND
 * WITH THE INCLUSION OF THE ABOVE COPYRIGHT NOTICE.  THIS SOFTWARE OR
 * ANY OTHER COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE
 * AVAILABLE TO ANY OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF THE
 * SOFTWARE IS HEREBY TRANSFERRED.
 *
 * 	THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT
 * NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY OPEN SOFTWARE
 * FOUNDATION, INC. OR ITS THIRD PARTY SUPPLIERS
 *
 * 	OPEN SOFTWARE FOUNDATION, INC. AND ITS THIRD PARTY SUPPLIERS,
 * ASSUME NO RESPONSIBILITY FOR THE USE OR INABILITY TO USE ANY OF ITS
 * SOFTWARE .   OSF SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, AND OSF EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Notice:  Notwithstanding any other lease or license that may pertain to,
 * or accompany the delivery of, this computer software, the rights of the
 * Government regarding its use, reproduction and disclosure are as set
 * forth in Section 52.227-19 of the FARS Computer Software-Restricted
 * Rights clause.
 *
 * (c) Copyright 1989,1990 Open Software Foundation, Inc.  Unpublished - all
 * rights reserved under the Copyright laws of the United States.
 *
 * RESTRICTED RIGHTS NOTICE:  Use, duplication, or disclosure by the
 * Government is subject to the restrictions as set forth in subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software clause
 * at DFARS 52.227-7013.
 *
 * Open Software Foundation, Inc.
 * 11 Cambridge Center
 * Cambridge, MA   02142
 * (617)621-8700
 *
 * RESTRICTED RIGHTS LEGEND:  This computer software is submitted with
 * "restricted rights."  Use, duplication or disclosure is subject to the
 * restrictions as set forth in NASA FAR SUP 18-52.227-79 (April 1985)
 * "Commercial Computer Software- Restricted Rights (April 1985)."  Open
 * Software Foundation, Inc., 11 Cambridge Center, Cambridge, MA  02142.  If
 * the contract contains the Clause at 18-52.227-74 "Rights in Data General"
 * then the "Alternate III" clause applies.
 *
 * (c) Copyright 1989,1990 Open Software Foundation, Inc.
 * ALL RIGHTS RESERVED
 *
 *
 * Open Software Foundation is a trademark of The Open Software  
 * Foundation, Inc.
 *
 * OSF is a trademark of Open Software Foundation, Inc.
 * OSF/Motif is a trademark of Open Software Foundation, Inc.
 * Motif is a trademark of Open Software Foundation, Inc.
 * DEC is a registered trademark of Digital Equipment Corporation
 * DIGITAL is a registered trademark of Digital Equipment Corporation
 * X Window System is a trademark of the Massachusetts Institute of Technology
 *
 *****************************************************************************
 *************************************<+>*************************************/


#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include "DtSvcLock.h"

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


/********    Public Function Declarations    ********/

extern Cursor _DtGetHourGlassCursor( 
                        Display *dpy) ;
extern void _DtTurnOnHourGlass( 
                        Widget w) ;
extern void _DtTurnOffHourGlass( 
                        Widget w) ;

/********    End Public Function Declarations    ********/

/*************************************<->*************************************
 *
 *  Cursor _DtGetHourGlassCursor ()
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
Cursor 
_DtGetHourGlassCursor(
        Display *dpy )
{
    unsigned char *bits;
    unsigned char *maskBits;
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

    _DtSvcProcessLock();
    if (waitCursor != 0) {
      _DtSvcProcessUnlock();
      return(waitCursor);
    }

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
	width = time32_width;
	height = time32_height;
	bits = time32_bits;
	maskBits = time32m_bits;
	xHotspot = time32_x_hot;
	yHotspot = time32_y_hot;
    }
    else
    {
	width = time16_width;
	height = time16_height;
	bits = time16_bits;
	maskBits = time16m_bits;
	xHotspot = time16_x_hot;
	yHotspot = time16_y_hot;
    }

    pixmap = XCreateBitmapFromData (dpy, 
		     DefaultRootWindow(dpy), (char*) bits, 
		     width, height);

  
    maskPixmap = XCreateBitmapFromData (dpy, 
		     DefaultRootWindow(dpy), (char*) maskBits, 
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

    _DtSvcProcessUnlock();
    return (waitCursor);
}

 
/*************************************<->*************************************
 *
 *  void DtSetHourGlass
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

void 
_DtTurnOnHourGlass(
        Widget w )
{
    Cursor cursor;
    
    cursor = _DtGetHourGlassCursor(XtDisplay(w));

    XDefineCursor(XtDisplay(w), XtWindow(w), cursor);
    XFlush(XtDisplay(w));
}


/*************************************<->*************************************
 *
 *  void _DtTurnOffHourGlass
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

void 
_DtTurnOffHourGlass(
        Widget w )
{
   
    XUndefineCursor(XtDisplay(w), XtWindow(w));
    XFlush(XtDisplay(w));
}














