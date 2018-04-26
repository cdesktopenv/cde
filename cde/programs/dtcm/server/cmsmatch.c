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
/* $XConsortium: cmsmatch.c /main/4 1995/11/09 12:42:30 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmsmatch.h"
#include "iso8601.h"
#include "misc.h"
#include "match.h"

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/

static boolean_t _MatchOneAttribute(cms_attribute eattr, cms_attribute mattr,
			int op);

/*****************************************************************************
 * extern functions
 *****************************************************************************/

extern boolean_t
_DtCmsMatchAttributes(
	cms_entry *entry,
	uint num_attrs,
	cms_attribute *attrs,
	CSA_enum *ops)
{
	int		i;

	for (i = 0; i < num_attrs; i++) {
		if (attrs[i].name.num > entry->num_attrs)
			return (B_FALSE);

		if (_MatchOneAttribute(entry->attrs[attrs[i].name.num],
		    attrs[i], (ops ? ops[i] : CSA_MATCH_EQUAL_TO)) == B_FALSE)
			return (B_FALSE);
	}
	return (B_TRUE);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

static boolean_t
_MatchOneAttribute(cms_attribute eattr, cms_attribute mattr, int op)
{
	if (eattr.value == NULL) {
		if (op == CSA_MATCH_EQUAL_TO && mattr.value == NULL)
			return (B_TRUE);
		else
			return (B_FALSE);
	}

	switch (eattr.value->type) {
	case CSA_VALUE_ENUMERATED:
	case CSA_VALUE_SINT32:
		return (_DtCm_match_sint32_attribute(eattr.value, mattr.value,
			op));

	case CSA_VALUE_BOOLEAN:
	case CSA_VALUE_FLAGS:
	case CSA_VALUE_UINT32:
		return (_DtCm_match_uint32_attribute(eattr.value, mattr.value,
			op));

	case CSA_VALUE_STRING:
	case CSA_VALUE_CALENDAR_USER:
	case CSA_VALUE_DATE_TIME_RANGE:
		return (_DtCm_match_string_attribute(eattr.value, mattr.value,
			op));

	case CSA_VALUE_DATE_TIME:
		return (_DtCm_match_time_attribute(eattr.value, mattr.value,
			op));

	case CSA_VALUE_TIME_DURATION:
		return (_DtCm_match_time_duration_attribute(eattr.value,
			mattr.value, op));

	case CSA_VALUE_REMINDER:
		return (_DtCm_match_reminder_attribute(eattr.value, mattr.value,
			op));
	default:
		return (B_FALSE);
	}
}

