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
/* $XConsortium: HourGlass.h /main/4 1995/10/26 15:23:08 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:       HourGlass.h
 **
 **   Project:     DT
 **
 **   Description: Public include file for HourGlass Library.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _hourglass_h
#define _hourglass_h

/* _DtGetHourGlassCursor -
 *
 *   Builds and returns the appropriate HourGlass cursor.
 */

extern Cursor _DtGetHourGlassCursor( 
                        Display *dpy) ;

/* _DtTurnOnHourGlass -
 *
 *   Gets and displays an hourglass cursor in the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtTurnOnHourGlass( 
                        Widget w) ;
 
 /*  Widget widget;
  *
  * widget is the toplevel shell of the window you want
  * the hourglass cursor to appear in.
  */


/* _DtTurnOffHourGlass -
 *
 *   Removes the hourglass cursor from the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtTurnOffHourGlass( 
                        Widget w) ;
  
 /* Widget widget;
  *
  *  widget is the toplevel shell of the window you want
  *  to remove hourglass cursor from.
  */


#endif /* _hourglass_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */



