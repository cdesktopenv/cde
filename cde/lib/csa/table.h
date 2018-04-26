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
/* $XConsortium: table.h /main/1 1996/04/21 19:24:49 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _TABLE_H
#define _TABLE_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "cm.h"
#include "rtable4.h"
#include "entry.h"
#include "connection.h"

/*
 *  Interface to rpc calls of version 2-4
 */

extern CSA_return_code _DtCm_table_create P((Calendar *cal));

extern CSA_return_code _DtCm_table_get_access P((
				Calendar *cal,
				cms_access_entry **acclist));

extern CSA_return_code _DtCm_table_set_access P((
				Calendar *cal,
				CSA_access_list alist));

extern CSA_return_code _DtCm_table_lookup_reminder P((
				Calendar *cal,
				CSA_uint32 num_names,
				char **reminder_names,
				CSA_uint32 *num_rems,
				CSA_reminder_reference **rems));

extern CSA_return_code _DtCm_table_lookup_range P((
				Calendar	*cal,
				long		start1,
				long		start2,
				boolean_t	no_end_time_range,
				long		end1,
				long		end2,
				long		id,
				uint		num_attrs,
				cms_attribute	*attrs,
				CSA_enum	*ops,
				_DtCm_libentry	**appts));

extern CSA_return_code _DtCm_table_lookup P((
				Calendar *cal,
				_DtCm_libentry *entry));

/*
 * This routine looks up events of a repeating sequence.
 * The caller either specifies entry and elist_r or
 * id and key. 
 * If entry is specified, events are converted to cms_entry structures
 * and returned in elist_r, otherwise; the caller is interested only
 * in the key of the first entry which will be returned in the cms_key
 * structure pointed to by key.
 */
extern CSA_return_code _DtCm_table_lookup_key_range P((
				Calendar *cal,
				_DtCm_libentry *entry,
				long id,
				time_t start,
				time_t end,
				cms_key *key,
				_DtCm_libentry **elist_r));

extern CSA_return_code _DtCm_table_insert P((
				Calendar *cal,
				uint num_attrs,
				CSA_attribute * attrs,
				_DtCm_libentry **entries));

extern CSA_return_code _DtCm_table_delete P((
				Calendar *cal,
				_DtCm_libentry *entry,
				CSA_enum scope));

extern CSA_return_code _DtCm_table_update P((
				Calendar *cal,
				_DtCm_libentry *oentry,
				uint num_attrs,
				CSA_attribute * attrs,
				CSA_enum scope,
				_DtCm_libentry **nentry));

extern CSA_return_code _DtCm_table_size P((
				Calendar *cal,
				int *size));

extern CSA_return_code _DtCm_table_unregister_target P((
				_DtCm_Connection *conn,
				char *cal));

extern CSA_return_code _DtCm_table_register_target P((
				_DtCm_Connection *conn,
				char *cal));

#endif
