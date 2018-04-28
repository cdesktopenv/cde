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
/*******************************************************************************
**
**  select.h
**
**  static char sccsid[] = "@(#)select.h 1.6 94/11/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: select.h /main/3 1995/11/03 10:33:23 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _SELECT_H
#define _SELECT_H

#include "ansi_c.h"
#include "browser.h"
#include "calendar.h"

typedef enum {
	daySelect,
	weekSelect,
	monthSelect,
	hourSelect,
	weekdaySelect,
	weekhotboxSelect 
} Selection_unit;

typedef struct {
	int row;
	int col;
	int nunits;
	int active;
	int boxw;
	int boxh;
} Selection;

typedef struct {
	int	x;
	int	y;
} pr_pos;

extern void	activate_selection	P((Selection*));
extern void	browser_deselect	P((Calendar *, Browser*));	
extern void	browser_select		P((Calendar *, Browser*, pr_pos*));	
extern void	calendar_deselect	P((Calendar*));
extern void	calendar_select		P((Calendar*, Selection_unit, caddr_t));
extern void	deactivate_selection	P((Selection *));
extern void	monthbox_deselect	P((Calendar*));	
extern void	monthbox_select		P((Calendar*));
extern void	paint_selection		P((Calendar*));
extern int	selection_active	P((Selection*));
extern void	weekchart_select	P((Calendar*));	

#endif
