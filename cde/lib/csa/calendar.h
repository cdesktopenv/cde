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
/* $XConsortium: calendar.h /main/1 1996/04/21 19:21:50 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CALENDAR_H
#define _CALENDAR_H

/*
 * This file contains the internal data structure of calendars.
 */

#include <EUSCompat.h>
#include "ansi_c.h"
#include "csa.h"
#include "connection.h"
#include "nametbl.h"
#include "attr.h"
#include "cm.h"

typedef struct cbq {
	CSA_flags	reason;
	CSA_callback	handler;
	CSA_buffer	client_data;
	struct cbq	*next;
	struct cbq	*prev;
} _DtCmCallbackEntry;

/*
 * Calendar object structure
 */
typedef struct cal {
	void			*handle;
	int			rpc_version;	/* server version */
	int			file_version;
	_DtCmNameTable		*cal_tbl;
	_DtCmNameTable		*entry_tbl;
	char			*name;		/* calendar@location */
	char			*location;
	boolean_t		async_process;	/* asynchronous update enabled*/
	CSA_flags		all_reasons;	/* all events registered */
	CSA_flags		do_reasons;	/* events to process */
	_DtCmCallbackEntry	*cb_list;
	_DtCm_Connection	conn;		/* connection info */
	uint			num_attrs;
	cms_attribute		*attrs;
	boolean_t		got_attrs;
	int			access;
	caddr_t			ehead;		/* list of entries associated */
	caddr_t			etail;		/* with the calendar.	      */
	struct cal		*next;
	struct cal		*prev;
} Calendar;

/* linked list of active calendars */
extern Calendar *_DtCm_active_cal_list;

/* function prototypes */

extern Calendar * _DtCm_new_Calendar P((
		const char *calenadr));

extern void _DtCm_free_Calendar P((
		Calendar *cal));

extern Calendar	*_DtCm_get_Calendar P((
		CSA_session_handle calhandle));

extern uint _DtCm_add_to_entry_list P((
		Calendar *cal,
		caddr_t elist));

extern void _DtCm_remove_from_entry_list P((
		Calendar *cal,
		caddr_t head,
		caddr_t tail));

extern CSA_return_code _DtCm_list_old_cal_attr_names P((
		Calendar *cal,
		CSA_uint32 *num_names_r,
		char **names_r[]));

extern CSA_return_code _DtCm_set_cal_attr P((
		Calendar *cal,
		CSA_attribute attr));

extern CSA_return_code _DtCm_get_all_cal_attrs P((
		Calendar *cal,
		CSA_uint32 *num_attrs,
		CSA_attribute **attrs));

extern CSA_return_code _DtCm_get_cal_attrs_by_name P((
		Calendar *cal,
		CSA_uint32 num_names,
		CSA_attribute_reference *names,
		CSA_uint32 *num_attrs,
		CSA_attribute **attrs));

extern void _DtCm_reset_cal_attrs P((
		Calendar *cal));

extern void _DtCm_count_entry_in_list P((
		caddr_t elist));

#endif
