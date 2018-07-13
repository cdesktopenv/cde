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
**  dssw.h
**
**  static char sccsid[] = "@(#)dssw.h 1.18 94/11/07 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: dssw.h /main/4 1996/05/23 11:18:54 rswiston $
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

#ifndef _DSSW_H
#define _DSSW_H

#include <csa.h>
#include "ansi_c.h"
#include "calendar.h"

#define DATE_LEN	100
#define START_STOP_LEN	16
#define WHAT_LEN	256

/*******************************************************************************
**
**  DSSW enumerated types
**
*******************************************************************************/
typedef enum {
	TIME_AM,
	TIME_PM
} Time_block_op;

/*******************************************************************************
**
**  DSSW structures
**
*******************************************************************************/
typedef struct {
	char		val[START_STOP_LEN];
	Time_block_op	block;
} Time_val;

typedef struct {
	Calendar		*cal;
	Widget			date_label;
	Widget			date_text;
	char			date_val[DATE_LEN];
	Widget			dssw_form_mgr;
	Widget			parent;
	Widget			start_am;
	Widget			start_ampm_rc_mgr;
	Widget			start_label;
	Widget			start_menu;
	Widget			*start_menu_widget_list;
	int			start_menu_widget_count;
	Widget			start_pm;
	Widget			start_text;
	Time_val		start_val;
	Widget			stop_am;
	Widget			stop_ampm_rc_mgr;
	Widget			stop_label;
	Widget			stop_menu;
	Widget			*stop_menu_widget_list;
	int			stop_menu_widget_count;
	Widget			stop_pm;
	Widget			stop_text;
	Time_val		stop_val;
	Widget			what_label;
	Widget			what_text;
	Widget			what_scrollwindow;
	char			what_val[WHAT_LEN];
	Boolean			show_notime_selection;
	Boolean			show_allday_selection;
} DSSW;

/* form-to-appt flags */
#define DSSW_NEXT_DAY 1

/*******************************************************************************
**
**  DSSW external function declarations
**
*******************************************************************************/
extern void	build_dssw		P((DSSW*, Calendar*, Widget, Boolean, Boolean));
extern Boolean	dssw_appt_to_form	P((DSSW*, CSA_entry_handle));
extern Boolean	dssw_attrs_to_form	P((DSSW*, Dtcm_appointment*));
extern Boolean	dssw_form_to_appt	P((DSSW*, Dtcm_appointment*, char*,
					   Tick));
extern Boolean	dssw_form_flags_to_appt	P((DSSW*, Dtcm_appointment*, char*,
					   Tick, int *));
extern Boolean	dssw_form_to_todo	P((DSSW*, Dtcm_appointment*, char*,
					   Tick));
extern void	load_dssw_times		P((DSSW*, Tick, Tick, Boolean));
extern void	get_dssw_times		P((DSSW*));
extern void	get_dssw_vals		P((DSSW*, Tick));
extern void	set_dssw_defaults	P((DSSW*, Tick, Boolean));
extern void	set_dssw_times		P((DSSW*));
extern void	set_dssw_vals		P((DSSW*, Tick));
extern void	set_dssw_menus		P((DSSW*, Props *));

#endif
