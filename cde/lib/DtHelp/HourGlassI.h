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
/* $XConsortium: HourGlassI.h /main/5 1995/10/26 12:27:38 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:       HourGlassI.h
 **
 **   Project:     Cache Creek
 **
 **   Description: Internal include file for HourGlass Library.
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _hourglassI_h
#define _hourglassI_h

/* DtHelpGetHourGlassCursor -
 *
 *   Builds and returns the appropriate HourGlass cursor.
 */

extern Cursor DtHelpGetHourGlassCursor( 
                        Display *dpy) ;

/* DtHelpTurnOnHourGlass -
 *
 *   Gets and displays an hourglass cursor in the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtHelpTurnOnHourGlass( 
                        Widget w) ;
 
 /*  Widget widget;
  *
  * widget is the toplevel shell of the window you want
  * the hourglass cursor to appear in.
  */


/* DtHelpTurnOffHourGlass -
 *
 *   Removes the hourglass cursor from the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtHelpTurnOffHourGlass( 
                        Widget w) ;
  
 /* Widget widget;
  *
  *  widget is the toplevel shell of the window you want
  *  to remove hourglass cursor from.
  */

/* DtHelpTurnOnNoEnter -
 *
 *   Removes the hourglass cursor from the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtHelpTurnOnNoEnter( 
                        Widget w) ;

 /* Widget widget;
  *
  *  widget is the toplevel shell of the window you want
  *  to remove hourglass cursor from.
  */

/* DtHelpTurnOffNoEnter -
 *
 *   Removes the hourglass cursor from the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtHelpTurnOffNoEnter( 
                        Widget w) ;

 /* Widget widget;
  *
  *  widget is the toplevel shell of the window you want
  *  to remove hourglass cursor from.
  */


#endif /* _hourglassI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
