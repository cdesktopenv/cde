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
/* $XConsortium: convert5-4.h /main/1 1996/04/21 19:22:58 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CONVERT5_4_H
#define _CONVERT5_4_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "cm.h"
#include "rtable4.h"

/*
 * conversion routines for rpc.cmsd version 5 to rpc.cmsd version 4 data types
 */
extern CSA_return_code _DtCm_cmsattrs_to_apptdata P((
				uint num_attrs,
				cms_attribute *attrs,
				Appt_4 *appt4));

extern CSA_return_code _DtCm_attrs_to_apptdata P((
				uint size,
				CSA_attribute *attrs,
				Appt_4 *appt));

extern CSA_return_code _DtCm_cms_entry_to_appt4	P((
				cms_entry *entry,
				Appt_4 **appt4));

extern CSA_return_code _DtCm_attrs_to_appt4 P((
				uint num_attrs,
				CSA_attribute *attrs,
				Appt_4 **appt4_r));

extern CSA_return_code _DtCm_scope_to_options4 P((
				CSA_enum scope,
				Options_4 *opt));

extern CSA_return_code _DtCm_csaaccesslist_toaccessentry4 P((
				CSA_access_list alist,
				Access_Entry_4 **a4));

extern int _DtCm_string_to_eventtype4 P((
				char *val,
				Event_Type_4 *tag));

extern int _DtCm_rtype_to_interval4 P((
				int val,
				Interval_4 *period));

extern int _DtCm_status_to_apptstatus4 P((
				int val,
				Appt_Status_4 *astat));

extern int _DtCm_classification_to_privacy4 P((
				int val,
				Privacy_Level_4 *privacy));

extern CSA_return_code _DtCm_attrs_to_eventtype4 P((
				cms_attribute_value	*type,
				cms_attribute_value	*stype,
				Tag_4			*tag));

extern CSA_return_code _DtCm_remove_reminder P((char *rem, Appt_4 *appt));

extern CSA_return_code _DtCm_add_reminder P((
				char *rem,
				CSA_reminder * val,
				Appt_4 *appt));

extern int _DtCmAccessRightToV4AccessType P((unsigned int access));

#endif
