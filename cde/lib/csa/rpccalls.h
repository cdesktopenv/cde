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
/* $XConsortium: rpccalls.h /main/1 1996/04/21 19:24:24 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _RPCCALLS_H
#define _RPCCALLS_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "csa.h"
#include "entry.h"

/*
 * interface to cm rpc calls
 */

extern CSA_return_code _DtCm_rpc_open_calendar P((Calendar *cal));

extern CSA_return_code _DtCm_rpc_create_calendar P((
				Calendar *cal,
				CSA_uint32 num_attrs,
				CSA_attribute * attrs));

extern CSA_return_code _DtCm_rpc_delete_calendar P((Calendar *cal));

extern CSA_return_code _DtCm_rpc_register_client P((
				Calendar *cal,
				unsigned long type));

extern CSA_return_code _DtCm_rpc_unregister_client P((
				Calendar *cal,
				unsigned long type));

extern CSA_return_code _DtCm_rpc_enumerate_sequence P((
				Calendar *cal,
				_DtCm_libentry *entry,
				time_t start,
				time_t end,
				_DtCm_libentry **elist_r));

extern CSA_return_code _DtCm_rpc_lookup_entries P((
				Calendar *cal,
				CSA_uint32 size,
				CSA_attribute * attrs,
				CSA_enum *ops,
				_DtCm_libentry **entries_r));

extern CSA_return_code _DtCm_rpc_lookup_entry_by_id P((
				Calendar *cal,
				_DtCm_libentry *entry));

extern CSA_return_code _DtCm_rpc_lookup_reminder P((
				Calendar *cal,
				time_t tick,
				CSA_uint32 num_names,
				char **reminder_names,
				CSA_uint32 *num_rems,
				CSA_reminder_reference **rems));

extern CSA_return_code _DtCm_rpc_set_cal_attrs P((
				Calendar *cal,
				CSA_uint32 num_attrs,
				CSA_attribute * attrs));

extern CSA_return_code _DtCm_rpc_get_cal_attrs P((
				Calendar	*cal,
				int		index,
				CSA_uint32	num_attrs,
				char		**names));

extern CSA_return_code _DtCm_rpc_insert_entry P((
				Calendar *cal,
				CSA_uint32 num_attrs,
				CSA_attribute * attrs,
				_DtCm_libentry **entry_r));

extern CSA_return_code _DtCm_rpc_delete_entry P((
				Calendar *cal,
				_DtCm_libentry *entry,
				CSA_enum scope));

extern CSA_return_code _DtCm_rpc_update_entry P((
				Calendar *cal,
				_DtCm_libentry *oentry,
				CSA_uint32 num_attrs,
				CSA_attribute * attrs,
				CSA_enum scope,
				_DtCm_libentry **nentry));

extern CSA_return_code _DtCm_rpc_list_calendar_attributes P((
				Calendar	*cal,
				CSA_uint32	*number_names,
				char		***names_r));

extern CSA_return_code _DtCm_rpc_list_calendars P((
				char			*location,
				CSA_uint32		*number_names,
				CSA_calendar_user	**names_r));

extern CSA_return_code _DtCm_do_unregistration P((
				_DtCm_Connection *conn,
				char *cal,
				unsigned long update_type));

extern CSA_return_code _DtCm_do_registration P((
				_DtCm_Connection *conn,
				char *cal,
				unsigned long update_type));

#endif

