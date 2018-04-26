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
**  find.h
**
**  static char sccsid[] = "@(#)find.h 1.8 94/11/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: find.h /main/3 1995/11/03 10:26:18 rswiston $
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

#ifndef _FIND_H
#define _FIND_H

#include "ansi_c.h"

typedef struct tick_list {
	Tick tick;
	struct tick_list *next;
} Tick_list;

typedef enum {search_all, search_range} Searchset;

typedef struct {
	Widget          	frame;
	Widget          	form;
	Widget			apptstr_label;
	Widget			apptstr;
	Widget			search_rc_mgr;
	Widget			search_all;
	Widget			search_range;
	Widget			search_from;
	Widget			search_tolabel;
	Widget			search_to;
	Widget			find_button;
	Widget			help_button;
	Widget			date_label;
	Widget			time_label;
	Widget			what_label;
	Widget			find_list;
	Widget			find_list_sw;
	Widget			show_button;
	Widget          	cancel_button;
	Widget			find_message;
	Tick_list		*ticks;
	Searchset		search_set;
	Tick			search_from_date;
	Tick			search_to_date;
}Find;

extern caddr_t	make_find	P((Calendar*));
static Tick f_get_searchdate	P((Widget, Props*));

#endif
