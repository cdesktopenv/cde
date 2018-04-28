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
/* $XConsortium: entry.h /main/1 1996/04/21 19:23:13 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _ENTRY_H
#define _ENTRY_H

/*
 * This file contains the internal data structure of entries.
 */

#include <EUSCompat.h>
#include "ansi_c.h"
#include "cm.h"
#include "calendar.h"
#include "rtable4.h"

typedef struct _libentry {
	void		*handle;	/* for sanity check */
	Calendar	*cal;		/* associated calendar */
	cms_entry	*e;
	boolean_t	filled;
	struct _libentry *next;
	struct _libentry *prev;
} _DtCm_libentry;

/* function prototypes */

extern _DtCm_libentry *_DtCm_get_libentry P((CSA_entry_handle entryh));

extern CSA_return_code _DtCm_make_libentry P((
				cms_entry	*cmsentry,
				_DtCm_libentry	**entry_r));

extern CSA_return_code _DtCm_get_entry_detail P((_DtCm_libentry *entry));

extern CSA_return_code _DtCm_get_entry_attr_names P((
				_DtCm_libentry *entry,
				CSA_uint32 *num_names_r,
				char **names_r[]));

extern CSA_return_code _DtCm_get_entry_attrs_by_name P((
				_DtCm_libentry *entry,
				CSA_uint32 num_names,
				CSA_attribute_reference *names,
				CSA_uint32 *num_attrs,
				CSA_attribute **attrs));

extern CSA_return_code _DtCm_get_all_entry_attrs P((
				_DtCm_libentry *entry,
				CSA_uint32 *num_attrs,
				CSA_attribute **attrs));

extern CSA_return_code _DtCm_libentry_to_entryh P((
				_DtCm_libentry *elist,
				CSA_uint32 *size,
				CSA_entry_handle **entries_r));

extern CSA_return_code _DtCmCmsentriesToLibentries P((
				_DtCmNameTable	**tbl,
				cms_entry	*entries,
				_DtCm_libentry	**libentries));

extern CSA_return_code _DtCm_appt4_to_libentries P((
				char		*calname,
				Appt_4		*appt4,
				_DtCm_libentry	**libentries));

extern CSA_return_code _DtCm_libentries_to_appt4 P((
				_DtCm_libentry *libentries,
				Appt_4 **appt4));

extern CSA_return_code _DtCm_reminder4_to_csareminder P((
				Reminder_4 *r4,
				CSA_uint32 *num_rems,
				CSA_reminder_reference **rems));

extern _DtCm_libentry *_DtCm_convert_entry_wheader P((_DtCm_libentry *entry));

extern void _DtCm_free_libentries_from_list P((
				_DtCm_libentry *head,
				_DtCm_libentry *tail));

extern void _DtCm_free_libentries P((_DtCm_libentry *entries));

extern void _DtCm_free_entry_handles P((
				uint num_entries,
				CSA_entry_handle *entries));

extern void _DtCm_free_reminder_references P((
				uint num_rems,
				CSA_reminder_reference *rems));

extern void _DtCm_free_entry_content P((uint dummy, _DtCm_libentry *entry));

extern CSA_return_code _DtCm_cms2csa_reminder_ref P((
				cms_reminder_ref	*cmsrems,
				CSA_uint32		*num_rems,
				CSA_reminder_reference	**csarems));

#endif
