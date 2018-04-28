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
/* $XConsortium: DisplayAreaI.h /main/2 1996/05/09 03:41:47 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        DisplayAreaI.h
 **
 **   Project:     Cde Help System
 **
 **   Description: Defines the Display Area structures and defines.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef	_DtHelpDisplayAreaI_h
#define	_DtHelpDisplayAreaI_h

typedef	struct {
	int	 reason;
	XEvent	*event;
	Window	 window;
	char	*specification;
	int	 hyper_type;
	int	 window_hint;
} DtHelpHyperTextStruct;

#endif /* _DtHelpDisplayAreaI_h */
