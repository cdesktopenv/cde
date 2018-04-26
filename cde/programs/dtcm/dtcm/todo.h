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
**  todo.h
**
**  static char sccsid[] = "@(#)todo.h 1.24 94/11/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: todo.h /main/4 1996/05/23 11:17:27 rswiston $
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

#ifndef _TODO_H
#define _TODO_H

#include <csa.h>
#include "ansi_c.h"
#include "calendar.h"
#include "misc.h"
#include "dssw.h"
#include "rfp.h"
#include "reminders.h"

/*******************************************************************************
**
**  ToDo structures
**
*******************************************************************************/
struct TodoView {
	Widget		view_item_number;
	Widget		view_item_toggle;
	Boolean		modified;
	CSA_entry_handle	appt;
	struct TodoView	*next;
};
typedef struct TodoView TodoView;

typedef enum { VIEW_ALL, VIEW_PENDING, VIEW_COMPLETED } todo_view_op;
	
typedef struct {
	Widget			base_form_mgr;
	Calendar		*cal;
	Widget			change_button;
	Widget			clear_button;
	Widget			close_button;
	Widget			completed_toggle;
	Boolean			completed_val;
	Widget			delete_button;
	DSSW			dssw;
	Widget			expand_ui_button;
	Widget			frame;
	Widget			help_button;
	Widget			insert_button;
	Widget			list_label;
	Widget			message_text;
	Reminders		reminders;
	RFP			rfp;
	int			rfpFlags;
	Widget			separator1;
	Widget			separator2;
	CSA_entry_handle	*todo_head;
	int			todo_count;
	Widget			todo_list;
	Widget			todo_list_sw;
	Boolean			todo_is_up;
	Glance			view_list_glance;
	Tick			view_list_date;
	Boolean			todo_view_is_up;
	todo_view_op		view_filter;
	Widget			view_form_mgr;
	Widget			view_form;
	Widget			view_frame;
	Widget			view_filter_menu;
	Widget			view_list_label;
	TodoView		*view_list;
	Widget			view_sw_mgr;
	Boolean			view_list_modified;
	Widget			view_apply_button;
	Widget			view_ok_button;
	Widget			view_cancel_button;
	Widget			view_help_button;
	Widget			drag_icon;
	Widget			drag_source;
	int			initialX;
	int			initialY;
	Boolean			doing_drag;
} ToDo;

/*******************************************************************************
**
**  ToDo external function declarations
**
*******************************************************************************/
extern void	add_to_todo_list	P((CSA_entry_handle, ToDo*));
extern void	add_all_todo		P((ToDo*));
extern void	build_todo_list		P((ToDo*, Tick, Glance, CSA_entry_handle**,
					   CSA_uint32*, todo_view_op));
extern int	build_todo_view		P((ToDo*, Glance, Boolean));
extern void	get_todo_vals		P((ToDo*));
extern void	todo_hide		P((ToDo*));
extern void	set_todo_defaults	P((ToDo*));
extern void	set_todo_title		P((ToDo*, char*));
extern void	set_todo_vals		P((ToDo*));
extern void	show_todo		P((Calendar*));
extern void	t_make_todo		P((Calendar*));
extern void	show_todo_view		P((Calendar*, todo_view_op));
extern void	todo_clean_up		P((ToDo*));
extern Boolean	todo_showing		P((ToDo*));
extern Boolean	todo_view_showing	P((ToDo*));
extern CSA_entry_handle	t_nth_appt	P((ToDo*, int));
extern Boolean  todo_insert           P((Dtcm_appointment*, CSA_entry_handle*,
					Calendar*));

#endif
