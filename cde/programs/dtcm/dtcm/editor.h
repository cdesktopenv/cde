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
**  editor.h
**
**  static char sccsid[] = "@(#)editor.h 1.12 94/03/03 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: editor.h /main/4 1996/05/23 11:18:40 rswiston $
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

#ifndef _EDITOR_H
#define _EDITOR_H

#include <csa.h>
#include "ansi_c.h"
#include "calendar.h"
#include "misc.h"
#include "dssw.h"
#include "rfp.h"
#include "reminders.h"

/*******************************************************************************
**
**  Editor structures
**
*******************************************************************************/
typedef struct {
	int			appt_count;
	CSA_entry_handle	*appt_head;
	Widget			appt_list;
	Widget			appt_list_sw;
	Widget			base_form_mgr;
	Calendar		*cal;
	Widget			change_button;
	Widget			clear_button;
	Widget			close_button;
	Widget			delete_button;
	Boolean			doing_drag;
	Pixmap			drag_bitmap;
	Widget			drag_icon;
	Widget			drag_source;
	Pixmap			drag_mask;
	DSSW			dssw;
	int			dsswFlags;
	Boolean			editor_is_up;
	Boolean			editor_view_is_up;
	Widget			expand_ui_button;
	Widget			frame;
	Widget			help_button;
	int			initialX;
	int			initialY;
	Widget			insert_button;
	Widget			list_label;
	Widget			message_text;
	Reminders		reminders;
	RFP			rfp;
	int			rfpFlags;
	Widget			separator1;
	Widget			separator2;
	Widget			view_form_mgr;
	Widget			view_frame;
	Widget			view_form;
	Widget			view_list_label;
	Widget			view_list;
	Widget			view_sw_mgr;
	Widget			view_cancel_button;
	Widget			view_help_button;
	Glance			view_list_glance;
	Tick			view_list_date;
} Editor;

/*******************************************************************************
**
**  Editor external function declarations
**
*******************************************************************************/
extern void	add_to_appt_list	P((CSA_entry_handle, Editor*));
extern void	add_all_appt		P((Editor*));
extern void	build_editor_list	P((Editor*, Tick, Glance, CSA_entry_handle**,
					   CSA_uint32*));
extern int	build_editor_view	P((Editor*, Glance, Boolean));
extern Boolean	editor_change		P((Dtcm_appointment*, CSA_entry_handle,
					   CSA_entry_handle*, Calendar*));
extern void	editor_clean_up		P((Editor*));
extern Boolean	editor_created		P((Editor*));
extern Boolean	editor_delete		P((CSA_entry_handle, Calendar*));
extern Boolean	editor_insert		P((Dtcm_appointment*, CSA_entry_handle*,
					   Calendar*));
extern CSA_entry_handle editor_nth_appt	P((Editor*, int));
extern Boolean	editor_showing		P((Editor*));
extern Boolean	editor_view_showing	P((Editor*));
extern void	get_editor_vals		P((Editor*));
extern void	set_editor_defaults	P((Editor*, Tick, Tick, Boolean));
extern void	set_editor_title	P((Editor*, char *));
extern void	set_editor_vals		P((Editor*, Tick, Tick));
extern void	show_editor		P((Calendar*, Tick, Tick, Boolean));
extern void	show_editor_view	P((Calendar*, Glance));
extern void	e_make_editor		P((Calendar*));

#endif
