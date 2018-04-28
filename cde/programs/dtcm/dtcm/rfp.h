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
**  rfp.h
**
**  static char sccsid[] = "@(#)rfp.h 1.20 95/02/10 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: rfp.h /main/4 1996/05/23 11:17:56 rswiston $
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

#ifndef _RFP_H
#define _RFP_H

#include <csa.h>
#include "ansi_c.h"
#include "calendar.h"
#include "misc.h"

#define FOR_LEN	5

/*******************************************************************************
**
**  Structure for repeat, for scope, and privacy values
**
*******************************************************************************/
typedef struct {
	Widget			for_label;
	Widget			for_menu;
	Widget			for_scope;
	int			for_val;
	Widget			frequency_label;
	Widget			parent;
	Widget			privacy_label;
	Widget			privacy_menu;
	CSA_sint32		privacy_val;
	Calendar		*cal;
	Widget			repeat_form_mgr;
	Widget			repeat_label;
	Widget			repeat_menu;
	Widget			repeat_popup_frame;
	Widget			repeat_popup_menu;
	Widget			repeat_popup_text;
	Widget			repeat_apply_button;
	Widget			repeat_cancel_button;
	int			repeat_nth;
	CSA_sint32		repeat_type;
	char			*recurrence_rule;
	Widget			rfp_form_mgr;
} RFP;

/* Monthly-by-weekday flags */
#define RFP_MBW_LAST 1
#define RFP_MBW_4TH  2

/*******************************************************************************
**
**  RFP (repeat, for, privacy) external function declarations
**
*******************************************************************************/
extern void	build_rfp		P((RFP*, Calendar*, Widget));
extern void	get_rfp_privacy_val	P((RFP*));
extern void	get_rfp_repeat_val	P((RFP*, time_t));
extern void	get_rfp_vals		P((RFP*, time_t));
extern Boolean	rfp_attrs_to_form	P((RFP*, Dtcm_appointment*));
extern Boolean	rfp_appt_to_form	P((RFP*, CSA_entry_handle));
extern Boolean	rfp_form_to_appt	P((RFP*, Dtcm_appointment*, char*));
extern Boolean	rfp_form_flags_to_appt	P((RFP*, Dtcm_appointment*, char*, int *));
extern void	set_rfp_privacy_val	P((RFP*));
extern void	set_rfp_repeat_val	P((RFP*));
extern void	set_rfp_defaults	P((RFP*));
extern void	set_rfp_vals		P((RFP*));
extern void	rfp_set_repeat_values	P((RFP*));
extern void	rfp_init		P((RFP*, Calendar *, Widget));


#endif
