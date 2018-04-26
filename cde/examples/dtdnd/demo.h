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
/* $TOG: demo.h /main/4 1999/07/20 14:49:21 mgreess $ */
/*****************************************************************************
 *****************************************************************************
 **
 **   File:         demo.h
 **
 **   Description:  Header for foundation of CDE Drag & Drop Demo.
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * Public General-Purpose Demo Function Declarations
 */

extern void	demoDragFinishCallback(Widget, XtPointer, XtPointer);
extern void	demoDragMotionHandler(Widget, XtPointer, XtPointer, XEvent*);
extern void	demoProcessPress(Widget, XEvent*, String*, Cardinal*);
extern void	demoDrawExposeCallback(Widget, XtPointer, XtPointer);

extern XtAppContext	demoAppContext;
extern Widget		demoTopLevel;
