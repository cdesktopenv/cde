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
/* $XConsortium: OWsync.h /main/2 1996/05/08 20:12:22 drk $ */
/*******************************************************************************
**
**  OWsync.h
**
**  Defines functions to synchronize CDE and OpenWindows resources.
**
**  Copyright 1995 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*
 * External function declarations
 */

extern void	OWsyncColorResources(
			Display *	display,
			int 		monitorType, 
			ColorSet * 	colors);

extern void	OWsyncLocaleResources(
			Display *	display);

