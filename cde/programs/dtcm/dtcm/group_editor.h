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
**  group_editor.h
**
**  static char sccsid[] = "@(#)group_editor.h 1.24 94/11/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: group_editor.h /main/4 1996/05/23 11:18:16 rswiston $
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

#ifndef _GEDITOR_H
#define _GEDITOR_H

#include <csa.h>
#include "ansi_c.h"
#include "util.h"
#include "calendar.h"
#include "misc.h"
#include "dssw.h"
#include "rfp.h"

/*******************************************************************************
**
**  Group Editor structures
**
*******************************************************************************/
typedef struct {
	int			appt_count;
	CSA_entry_handle	*appt_head;
	char			*name;
	CSA_session_handle	cal_handle;
	int			version;
	unsigned int		entry_access;
} Access_data;

/*
**  An entry in this structure will point to the big list in the Access_data
**  structure
*/
typedef struct {
	int		entry_idx;
	Tick		tick;
	Access_data	*ad;
} List_data;

typedef struct {
	CmDataList		*access_data;
	Widget			access_list;
	Widget			access_list_label;
	Widget			access_list_sw;
	Widget			appt_list;
	Widget			appt_list_label;
	Widget			appt_list_sw;
	Widget			base_form_mgr;
	Widget			button_rc_mgr;
	Calendar		*cal;
	Widget			change_button;
	Widget			clear_button;
	Widget			close_button;
	Widget			delete_button;
	DSSW			dssw;
	int			dsswFlags;
	Widget			expand_ui_button;
	Widget			frame;
	Boolean			geditor_is_up;
	Widget			help_button;
	Widget			insert_button;
	CmDataList		*list_data;
	Widget			mail_button;
	Widget			message_text;
	RFP			rfp;
	int			rfpFlags;
	Widget			separator1;
	Widget			separator2;
        Pixmap                  drag_bitmap;
        Pixmap                  drag_mask;
        Widget                  drag_icon;
        Widget                  drag_source;
        int                     initialX;
        int                     initialY;
        Boolean                 doing_drag;
} GEditor;

/*******************************************************************************
**
**  Group Editor external function declarations
**
*******************************************************************************/
extern void		add_to_gaccess_list	P((char*, CSA_session_handle,
						   unsigned int, int, GEditor*,
						   Boolean));
extern void		add_all_gappt		P((GEditor*));
extern void		geditor_clean_up	P((GEditor*));
extern CSA_entry_handle	geditor_nth_appt	P((GEditor*, int,
						   Access_data**));
extern Boolean		geditor_showing		P((GEditor*));
extern void		get_geditor_vals	P((GEditor*));
extern void		remove_all_gaccess_data P((GEditor*));
extern void		remove_all_gaccess	P((GEditor*));
extern void		remove_from_gaccess_list P((char*, GEditor*));
extern void		set_geditor_defaults	P((GEditor*, Tick, Tick));
extern void		set_geditor_title	P((GEditor*, char*));
extern void		set_geditor_vals	P((GEditor*, Tick, Tick));
extern void		show_geditor		P((Calendar*, Tick, Tick));

#endif
