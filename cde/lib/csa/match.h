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
/* $XConsortium: match.h /main/1 1996/04/21 19:23:51 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _MATCH_H
#define _MATCH_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "cm.h"
#include "rtable4.h"

extern CSA_return_code _DtCmHashCriteria P((
			_DtCmNameTable	*tbl,
			CSA_uint32	num_attrs,
			CSA_attribute	*csaattrs,
			cms_attribute	*cmsattrs,
			CSA_enum	*ops,
			boolean_t	*no_match,
			boolean_t	*no_start_time_range,
			boolean_t	*no_end_time_range,
			time_t		*start1,
			time_t		*start2,
			time_t		*end1,
			time_t		*end2,
			long		*id,
			CSA_uint32	*hnum,
			cms_attribute	**hattrs,
			CSA_enum	**hops));

extern void _DtCmFreeHashedArrays P((
			CSA_uint32	hnum,
			cms_attribute	*hattrs,
			CSA_enum	*hops));

extern Appt_4 *_DtCm_match_appts P((
			Appt_4		*appts,
			long		id,
			boolean_t	no_end_time_range,
			time_t		end1,
			time_t		end2,
			CSA_uint32	num_attrs,
			cms_attribute	*attrs,
			CSA_enum	*ops));

extern boolean_t _DtCm_match_one_appt(
			Appt_4 *appt,
			uint num_attrs,
			cms_attribute * attrs,
			CSA_enum *ops);

extern Reminder_4 *_DtCm_match_reminders P((
			Reminder_4 *rems,
			uint num_names,
			char **names));

extern CSA_return_code _DtCm_check_operator P((
			uint size,
			CSA_attribute *csaattrs,
			cms_attribute *cmsattrs,
			CSA_enum *ops));

extern boolean_t _DtCm_match_sint32_attribute P((
			cms_attribute_value *val1,
			cms_attribute_value *val2,
			CSA_enum op));

extern boolean_t _DtCm_match_uint32_attribute P((
			cms_attribute_value *val1,
			cms_attribute_value *val2,
			CSA_enum op));

extern boolean_t _DtCm_match_time_attribute P((
			cms_attribute_value *val1,
			cms_attribute_value *val2,
			CSA_enum op));

extern boolean_t _DtCm_match_time_duration_attribute P((
			cms_attribute_value *val1,
			cms_attribute_value *val2,
			CSA_enum op));

extern boolean_t _DtCm_match_string_attribute P((
			cms_attribute_value *val1,
			cms_attribute_value *val2,
			CSA_enum op));

extern boolean_t _DtCm_match_reminder_attribute P((
			cms_attribute_value *val1,
			cms_attribute_value *val2,
			CSA_enum op));

#endif
