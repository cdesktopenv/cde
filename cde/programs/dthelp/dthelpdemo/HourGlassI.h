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
/* $XConsortium: HourGlassI.h /main/3 1995/11/08 09:18:38 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:       HourGlassI.h
 **
 **   Project:     CDE dthelpdemo sample program.
 **
 **   Description: Internal include file for HourGlass Library.
 **
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

#ifndef _hourglassI_h
#define _hourglassI_h

/* TurnOnHourGlass -
 *
 *   Gets and displays an hourglass cursor in the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtHelpTurnOnHourGlass(
    Widget widget);

 /*
  * widget is the toplevel shell of the window you want
  * the hourglass cursor to appear in.
  */


/* TurnOffHourGlass -
 *
 *   Removes the hourglass cursor from the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtHelpTurnOffHourGlass(
    Widget widget);

 /*
  *  widget is the toplevel shell of the window you want
  *  to remove hourglass cursor from.
  */

#endif /* _hourglassI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */








