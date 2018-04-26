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
**  blist.h
**
**  static char sccsid[] = "@(#)blist.h 1.19 94/11/07 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: blist.h /main/3 1995/11/03 10:18:18 rswiston $
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

#ifndef _BLIST_H
#define _BLIST_H

#include "ansi_c.h"
#include "util.h"

/*
 * The list of names we can typically browse is used in three spots - the
 * browse list editor, the multi browser (compare calendars), and the main
 * window to build the browse menu.
 *
 * The list of names actually is a list of the BlistData structures.  The
 * multi browser uses the cal_handle item and therefore, we can't delete the
 * entry from this list until the multi browser is no longer using that pointer.
 */
typedef enum {
	BLIST_INSERT, BLIST_DELETE, BLIST_ACTIVE
} BlistTag;

typedef struct {
	char			*name;
	BlistTag		tag;
	CSA_session_handle	cal_handle;
	unsigned int		user_access;
	int			version;
	Paint_cache		*cache;
	int			cache_size;
} BlistData;

typedef struct {
	Widget		button_rc_mgr;
	Widget		edit_rc_mgr;
	Widget		frame;
	Widget		form;
	Widget		username;
	Widget		username_label;
	Widget		add_button;
	Widget		list_label;
	Widget		browse_list;
	Widget		browse_list_sw;
	Widget		remove_button;
	Widget		ok_button;
	Widget		apply_button;
	Widget		reset_button;
	Widget		cancel_button;
	Widget		help_button;
	Widget		message;
	CmDataList	*blist_data;
	Boolean		bl_pending_message_up;
	Boolean		bl_list_changed;
} Browselist;

typedef struct browser_state {
	char			*cname;
	Pixmap			*glyph;
	struct browser_state	*next;
} BrowserState;

extern void	blist_clean		P((Browselist*, Boolean));
extern void	blist_init_names	P((Calendar*));
extern void	blist_init_ui		P((Calendar*));
extern void	blist_reset		P((Calendar*));
extern void	make_browselist		P((Calendar*));
extern void	show_browselist		P((Calendar*));

#endif
