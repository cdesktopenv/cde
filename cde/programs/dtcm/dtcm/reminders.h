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
**  reminders.h
**
**  static char sccsid[] = "@(#)reminders.h 1.15 93/10/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: reminders.h /main/3 1995/11/03 10:32:24 rswiston $
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

#ifndef _REMINDERS_H
#define _REMINDERS_H

#include <X11/Intrinsic.h>
#include <csa.h>
#include "ansi_c.h"
#include "misc.h"

#define REMINDER_SCOPE_LEN	5
#define MAILTO_LEN		100
#define BEEP_FLAG		"bp"
#define FLASH_FLAG		"fl"
#define MAIL_FLAG		"ml"
#define POPUP_FLAG		"op"

typedef enum {
	REMINDERS_OK,
	ADVANCE_BLANK,
	ADVANCE_CONTAINS_BLANK,
	ADVANCE_NONNUMERIC
} Reminder_val_op;


/*******************************************************************************
**
**  Reminders structure will hold widgets for the "Reminders Display"
**
*******************************************************************************/
typedef struct {
	Boolean			selected;
	Time_scope_menu_op	scope;
	int			scope_val;
} Reminders_val;

typedef struct {
	Widget			parent;
	Widget			alarm_label;
	Widget			beep_menu;
	Widget			beep_text;
	Widget			beep_toggle;
	Reminders_val		beep_val;
	Widget			bfpm_form_mgr;
	Calendar		*cal;
	Widget			flash_menu;
	Widget			flash_text;
	Widget			flash_toggle;
	Reminders_val		flash_val;
	Widget			mail_menu;
	Widget			mail_text;
	Widget			mail_toggle;
	Reminders_val		mail_val;
	Widget			mailto_label;
	Widget			mailto_text;
	char			mailto_val[MAILTO_LEN];
	Widget			popup_menu;
	Widget			popup_text;
	Widget			popup_toggle;
	Reminders_val		popup_val;
} Reminders;

/*******************************************************************************
**
**  Editor external function declarations
**
*******************************************************************************/
extern void	build_reminders		P((Reminders*, Calendar*, Widget));
extern Reminder_val_op	get_reminders_vals	P((Reminders*, Boolean));
extern Boolean	reminders_attrs_to_form	P((Reminders*, Dtcm_appointment*));
extern Boolean	reminders_appt_to_form	P((Reminders*, CSA_entry_handle));
extern Boolean	reminders_form_to_appt	P((Reminders*, Dtcm_appointment*, char *));
extern void	set_reminders_defaults	P((Reminders*));
extern void	set_reminders_vals	P((Reminders*, Boolean));
extern void 	reminders_init		P((Reminders*, Calendar*, Widget));


#endif
