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
/* $XConsortium: convert4-5.h /main/1 1996/04/21 19:22:52 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CONVERT4_5_H
#define _CONVERT4_5_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "cm.h"
#include "rtable4.h"

/*
 * functions to convert v4 data structures
 */
extern CSA_return_code _DtCm_accessentry4_to_cmsaccesslist P((
				Access_Entry_4 *a4,
				cms_access_entry **alist));

extern CSA_return_code _DtCm_appt4_to_attrs P((
				char		*calname,
				Appt_4		*a4,
				uint		num_attrs,
				cms_attribute	*attrs,
				boolean_t	rerule));

extern int _DtCm_eventtype4_to_type P((Event_Type_4 tag));

extern char *_DtCm_eventtype4_to_subtype P((Event_Type_4 tag));

extern CSA_sint32 _DtCm_interval4_to_rtype P((Interval_4 val));

extern CSA_sint32 _DtCm_apptstatus4_to_status P((Appt_Status_4 stat));

extern CSA_sint32 _DtCm_privacy4_to_classification P((Privacy_Level_4 p));

extern CSA_return_code _DtCmAttr4ToReminderValue P((
				Attribute_4		*remval,
				cms_attribute_value	**attrval));

#endif
