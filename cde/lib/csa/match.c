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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: match.c /main/2 1999/10/14 17:15:21 mgreess $ */
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
#include "nametbl.h"
#include "match.h"
#include "cm.h"
#include "appt4.h"
#include "attr.h"
#include "convert4-5.h"
#include "iso8601.h"
#include "misc.h"

/*****************************************************************************
 * forward declaration of static functions
 *****************************************************************************/
static boolean_t match_this_attribute(
			Appt_4 *appt,
			cms_attribute attr,
			CSA_enum op);

static Attribute_4 *get_appt_reminder(Attribute_4 *attr, char *oldattr);

static boolean_t match_names(char *target, uint num_names, char **names);

static boolean_t match_old_reminder_attribute(
			Attribute_4 *rem4,
			cms_attribute_value *val,
			CSA_enum op);

static boolean_t is_same_string(char *str1, char *str2);

static boolean_t contain_substr(char *str1, char *str2);

static boolean_t is_same_opaque_data(CSA_opaque_data *d1, CSA_opaque_data *d2);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

/*
 * new array of cms_attribute type is allocated.
 * and CSA_attribute_value is converted to cms_attribtue_value type
 * If conflicts in the search criteria that lead to a no
 * match situation is detected, no_match will be set to B_TRUE and the
 * function return CSA_SUCCESS right away.  The calling
 * function should always check the return status and the no_match
 * parameter first before using any of the other output parameters.
 * If CSA_E_INVALID_ATTRIBUTE is returned, the caller should treat
 * it as a no_match case.
 */
extern CSA_return_code
_DtCmHashCriteria(
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
	CSA_enum	**hops)
{
	int	i, j, index, starti = -1, endi = -1;
	time_t	tstart1, tstart2, tend1, tend2;
	time_t	tick;
	char	*name;
	cms_attribute_value	*val, *hval;
	boolean_t keep;

	*hnum = 0;
	*hattrs = NULL;
	*hops = NULL;
	*no_match = B_FALSE;

	/*
	 * Find time range.
	 */
	tstart1 = _DtCM_BOT;
	tstart2 = _DtCM_EOT;
	tend1 = _DtCM_BOT;
	tend2 = _DtCM_EOT;
	*id = 0;
	for (i = 0, j = 0; i < num_attrs; i++) {
		keep = B_TRUE;
		name = (csaattrs ? csaattrs[i].name : cmsattrs[i].name.name);
		if ((index = _DtCm_get_index_from_table(tbl, name)) == -1)
			return (CSA_E_INVALID_ATTRIBUTE);

		val = (csaattrs ? ((cms_attribute_value *)csaattrs[i].value) :
			cmsattrs[i].value);

		if (index == CSA_ENTRY_ATTR_START_DATE_I && val == NULL) {
			if (ops == NULL || ops[i] == CSA_MATCH_EQUAL_TO) {
				*no_match = B_TRUE;
				return (CSA_SUCCESS);
			} else if (ops && (ops[i] == CSA_MATCH_ANY ||
			    ops[i] == CSA_MATCH_NOT_EQUAL_TO)) {
				/* match any start date, no need to set
				 * the range since the range is default to
				 * the largest range already
				 */
				starti = i;
				keep = B_FALSE;
			} 
		} else if (index == CSA_ENTRY_ATTR_START_DATE_I && val) {
			starti = i;
			keep = B_FALSE;
			if (_csa_iso8601_to_tick(val->item.date_time_value,
			    &tick))
				return (CSA_E_INVALID_DATE_TIME);

			if (ops == NULL || ops[i] == CSA_MATCH_EQUAL_TO) {
				tstart1 = tick - 1;
				tstart2 = tick + 1;
			} else switch (ops[i]) {
				/*
				 *case CSA_MATCH_ANY:
				 *case CSA_MATCH_NOT_EQUAL_TO:
				 *	this will be really expensive
				 *	tstart1 = _DtCM_BOT;
				 *	tstart2 = _DtCM_EOT;
				 *	break;
				 */
				case CSA_MATCH_NOT_EQUAL_TO:
					keep = B_TRUE;
					break;
				case CSA_MATCH_LESS_THAN:
					tstart2 = tick;
					break;
				case CSA_MATCH_LESS_THAN_OR_EQUAL_TO:
					tstart2 = tick + 1;
					break;
				case CSA_MATCH_GREATER_THAN:
					tstart1 = tick;
					break;
				case CSA_MATCH_GREATER_THAN_OR_EQUAL_TO:
					tstart1 = tick - 1;
					break;
			}
		} else if (index == CSA_ENTRY_ATTR_END_DATE_I && val) {
			endi = i;
			keep = B_FALSE;
			if (_csa_iso8601_to_tick(val->item.date_time_value,
			    &tick))
				return (CSA_E_INVALID_DATE_TIME);

			if (ops == NULL || ops[i] == CSA_MATCH_EQUAL_TO) {
				tend1 = tick - 1;
				tend2 = tick + 1;
			} else switch (ops[i]) {
				/*
				 *case CSA_MATCH_ANY:
				 *case CSA_MATCH_NOT_EQUAL_TO:
				 *	this will be really expensive
				 *	old_range.key1 = _DtCM_BOT;
				 *	old_range.key2 = _DtCM_EOT;
				 *	break;
				 */
				case CSA_MATCH_NOT_EQUAL_TO:
					keep = B_TRUE;
					break;
				case CSA_MATCH_LESS_THAN:
					tend2 = tick;
					break;
				case CSA_MATCH_LESS_THAN_OR_EQUAL_TO:
					tend2 = tick + 1;
					break;
				case CSA_MATCH_GREATER_THAN:
					tend1 = tick;
					break;
				case CSA_MATCH_GREATER_THAN_OR_EQUAL_TO:
					tend1 = tick - 1;
					break;
			}
		} else if (index == CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I) {
			keep = B_FALSE;
			if (val && val->item.opaque_data_value &&
			    val->item.opaque_data_value->data) {
				*id = atoi((char *)val->\
					item.opaque_data_value->data);
			}
		}

		if (keep == B_TRUE) {
			if (*hattrs == NULL) {
				if ((*hattrs = (cms_attribute *)calloc(1,
				    sizeof(cms_attribute)*(num_attrs-i)+
				    sizeof(char *))) == NULL)
					return (CSA_E_INSUFFICIENT_MEMORY);
				if ((hval = (cms_attribute_value *)calloc(1,
				    sizeof(cms_attribute_value)*(num_attrs-i)))
				    == NULL) {
					free(*hattrs);
					return (CSA_E_INSUFFICIENT_MEMORY);
				} else {
					*((char**)(*hattrs)) = (char *)&hval[0];
					*hattrs = (cms_attribute *)\
						((char *)(*hattrs)+sizeof(char*));
				}
				if (ops && (*hops = (CSA_enum *)calloc(1,
				    sizeof(CSA_enum)*(num_attrs-i))) == NULL) {
					free(*hattrs);
					free(hval);
					return (CSA_E_INSUFFICIENT_MEMORY);
				}
			}

			(*hattrs)[j].name.name = name;
			(*hattrs)[j].name.num = index;
			if (val) {
				(*hattrs)[j].value = &hval[j];
				(*hattrs)[j].value->type = val->type;
				if (csaattrs &&
				    val->type == CSA_VALUE_CALENDAR_USER) {
					if (csaattrs[i].value->item.calendar_user_value)
						(*hattrs)[j].value->item.\
						calendar_user_value =
						    csaattrs[i].value->item.\
						    calendar_user_value->\
						    user_name;
				} else
					(*hattrs)[j].value->item.sint32_value =
						val->item.sint32_value;
			}
			if (ops != NULL)
				(*hops)[j] = ops[i];
			j++;
		}
	}

	if (j == 0) {
		if (*hattrs) _DtCmFreeHashedArrays(*hnum, *hattrs, *hops);
		*hnum = 0;
		*hattrs = NULL;
		*hops = NULL;
	} else {
		*hnum = j;
	}
 
	if (starti == -1)
		*no_start_time_range = B_TRUE;
	else
		*no_start_time_range = B_FALSE;

	if (endi == -1)
		*no_end_time_range = B_TRUE;
	else
		*no_end_time_range = B_FALSE;

	/* do some optimization here: if an end time range is
	 * specified, since start time cannot be > end time,
	 * set end of start time range to end of end time range
	 * if the original end of start time range is > the
	 * end of end time range
	 */
	if (endi != -1 && start2 > end2)
		tstart2 = tend2;

	*start1 = tstart1;
        *start2 = tstart2;
	*end1 = tend1;
	*end2 = tend2;

	return (CSA_SUCCESS);
}

extern void _DtCmFreeHashedArrays(
	CSA_uint32	hnum,
	cms_attribute	*hattrs,
	CSA_enum	*hops)
{
	char	*ptr, *ptr1;

	if (hattrs) {
		ptr = ((char *)hattrs - sizeof(char *));
		ptr1 = *((char **)ptr);
		free(ptr1);
		free(ptr);
	}
	if (hops) free(hops);
}

/*
 * The algorithm used now simply goes through each criterion.
 * Probably need to optimize this.
 */
extern Appt_4 *
_DtCm_match_appts(
	Appt_4		*appts,
	long		id,
	boolean_t	no_end_time_range,
	time_t		end1,
	time_t		end2,
	CSA_uint32	num_attrs,
	cms_attribute	*attrs,
	CSA_enum	*ops)
{
	Appt_4	*head = NULL;
	Appt_4	*freelist = NULL;
	Appt_4	*last, *nptr;
	time_t	endtick;

	if (id == 0 && num_attrs == 0 && no_end_time_range)
		return (appts);

	while (appts != NULL) {
		nptr = appts->next;

		endtick = appts->appt_id.tick + appts->duration;

		if ((id > 0 && id != appts->appt_id.key) ||
		    (!no_end_time_range && (appts->duration == 0 ||
		     endtick < end1 || endtick >= end2)) ||
		    (num_attrs > 0 && !_DtCm_match_one_appt(appts, num_attrs,
		     attrs, ops)))
		{
			/* not match */
			appts->next = freelist;
			freelist = appts;
		} else {
			/* match */
			appts->next = NULL;

			if (head == NULL) {
				head = appts;
				last = appts;
			} else {
				last->next = appts;
				last = appts;
			}
		}

		appts = nptr;
	}

	_DtCm_free_appt4(freelist);
	return (head);
}

extern boolean_t
_DtCm_match_one_appt(
	Appt_4 *appt,
	uint num_attrs,
	cms_attribute *attrs,
	CSA_enum *ops)
{
	int		i;

	for (i = 0; i < num_attrs; i++) {
		if (match_this_attribute(appt, attrs[i],
		    (ops ? ops[i] : CSA_MATCH_EQUAL_TO)) == B_FALSE)
			return (B_FALSE);
	}

	/* match all given attributes */
	return (B_TRUE);
}

extern Reminder_4 *
_DtCm_match_reminders(Reminder_4 *rems, uint num_names, char **names)
{
	Reminder_4 *head = NULL;
	Reminder_4 *freelist = NULL;
	Reminder_4 *nptr;
	
	if (rems == NULL)
		return (NULL);

	if (names == NULL || num_names == 0) {
		return (rems);
	}

	while (rems != NULL) {
		nptr = rems->next;

		if (match_names(rems->attr.attr, num_names, names) == B_TRUE) {
			rems->next = head;
			head = rems;
		} else {
			rems->next = freelist;
			freelist = rems;
		}

		rems = nptr;
	}

	_DtCm_free_reminder4(freelist);
	return (head);
}

extern CSA_return_code
_DtCm_check_operator(
	uint		size,
	CSA_attribute	*csaattrs,
	cms_attribute	*cmsattrs,
	CSA_enum	*ops)
{
	int	i;
	char	*name;
	cms_attribute_value *val;

	for (i = 0; i < size; i++) {

		if ((name = csaattrs ? csaattrs[i].name : cmsattrs[i].name.name)
		    == NULL)
			continue;

		val = (csaattrs ? ((cms_attribute_value *)csaattrs[i].value) :
			cmsattrs[i].value);

		/* don't support matching on CSA_VALUE_OPAQUE_DATA,
		 * CSA_VALUE_ACCESS_LIST, CSA_VALUE_ATTENDEE_LIST,
		 * and CSA_VALUE_DATE_TIME_LIST data type
		 */
		if (val && (val->type == CSA_VALUE_ACCESS_LIST ||
		    val->type == CSA_VALUE_ATTENDEE_LIST ||
		    val->type == CSA_VALUE_DATE_TIME_LIST ||
		    (val->type == CSA_VALUE_OPAQUE_DATA &&
		    (strcmp(name, CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER) ||
		    (ops && ops[i] != CSA_MATCH_EQUAL_TO)))))
			return (CSA_E_UNSUPPORTED_ENUM);

		/* all other test depends on ops being not NULL */
		if (ops == NULL)
			continue;

		/* unknown operator */
		if (ops[i] < CSA_MATCH_ANY || ops[i] > CSA_MATCH_CONTAIN)
			return (CSA_E_INVALID_ENUM);

		/*
		 * if an operator other than CSA_MATCH_ANY, or
		 * CSA_MATCH_EQUAL_TO, CSA_MATCH_NOT_EQUAL_TO
		 * is specified, must specify a value
		 */
		if (val == NULL && (ops[i] != CSA_MATCH_ANY &&
		    ops[i] != CSA_MATCH_EQUAL_TO &&
		    ops[i] != CSA_MATCH_NOT_EQUAL_TO))
			return (CSA_E_INVALID_ENUM);

		/*
		 * only support CSA_MATCH_ANY and CSA_MATCH_EQUAL_TO
		 * for CSA_VALUE_REMINDER, CSA_VALUE_CALENDAR_USER,
		 * and CSA_VALUE_DATE_TIME_RANGE data type
		 */
		if (val && (val->type == CSA_VALUE_REMINDER ||
		    val->type == CSA_VALUE_CALENDAR_USER ||
		    val->type == CSA_VALUE_DATE_TIME_RANGE) &&
		    (ops[i] != CSA_MATCH_ANY &&
		     ops[i] != CSA_MATCH_EQUAL_TO))
			return (CSA_E_UNSUPPORTED_ENUM);

		/*
		 * only support CSA_MATCH_ANY, CSA_MATCH_EQUAL_TO,
		 * CSA_MATCH_NOT_EQUAL_TO and CSA_MATCH_CONTAIN
		 * for strings, other op does not make sense
		*/
		if (val && val->type == CSA_VALUE_STRING &&
		    (ops[i] > CSA_MATCH_NOT_EQUAL_TO &&
		    ops[i] < CSA_MATCH_CONTAIN))
			return (CSA_E_UNSUPPORTED_ENUM);

		/* CSA_MATCH_CONTAIN applies to CSA_VALUE_STRING type only */
		if (ops[i] == CSA_MATCH_CONTAIN && val &&
		    val->type != CSA_VALUE_STRING)
			return (CSA_E_UNSUPPORTED_ENUM);

	}

	return (CSA_SUCCESS);
}

extern boolean_t
_DtCm_match_time_attribute(
	cms_attribute_value *val1,
	cms_attribute_value *val2,
	CSA_enum op)
{
	cms_attribute_value	tval1, tval2;

	if ((val1 && val1->type != CSA_VALUE_DATE_TIME) ||
	    (val2 && val2->type != CSA_VALUE_DATE_TIME))
		return (B_FALSE);

	if (val1) {
		tval1.type = CSA_VALUE_SINT32;
		if (_csa_iso8601_to_tick(val1->item.date_time_value,
		    (time_t *)&tval1.item.sint32_value))
			return (B_FALSE);
	}

	if (val2) {
		tval2.type = CSA_VALUE_SINT32;
		if (_csa_iso8601_to_tick(val2->item.date_time_value,
		    (time_t *)&tval2.item.sint32_value))
			return (B_FALSE);
	}

	return (_DtCm_match_sint32_attribute((val1 ? &tval1 : NULL),
		(val2 ? &tval2 : NULL), op));
}

extern boolean_t
_DtCm_match_time_duration_attribute(
	cms_attribute_value *val1,
	cms_attribute_value *val2,
	CSA_enum op)
{
	cms_attribute_value	tval1, tval2;

	if ((val1 && val1->type != CSA_VALUE_TIME_DURATION) ||
	    (val2 && val2->type != CSA_VALUE_TIME_DURATION))
		return (B_FALSE);

	if (val1) {
		tval1.type = CSA_VALUE_SINT32;
		if (_csa_iso8601_to_duration(val1->item.time_duration_value,
		    (time_t *)&tval1.item.sint32_value))
			return (B_FALSE);
	}

	if (val2) {
		tval2.type = CSA_VALUE_SINT32;
		if (_csa_iso8601_to_duration(val2->item.time_duration_value,
		    (time_t *)&tval2.item.sint32_value))
			return (B_FALSE);
	}

	return (_DtCm_match_sint32_attribute((val1 ? &tval1 : NULL),
		(val2 ? &tval2 : NULL), op));
}

extern boolean_t
_DtCm_match_sint32_attribute(
	cms_attribute_value *val1,
	cms_attribute_value *val2,
	CSA_enum op)
{
	if (val1 && val2 && val1->type != val2->type)
		return (B_FALSE);

	switch (op) {
	case CSA_MATCH_ANY:
		if (val1 != NULL)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_EQUAL_TO:
		if (val2 == NULL) {
			if (val1 == NULL)
				return (B_TRUE);
			else
				return (B_FALSE);
		} else {
			if (val1 && val1->item.sint32_value
			    == val2->item.sint32_value)
				return (B_TRUE);
			else
				return (B_FALSE);
		}

	case CSA_MATCH_NOT_EQUAL_TO:
		if (val2 == NULL) {
			if (val1 != NULL)
				return (B_TRUE);
			else
				return (B_FALSE);
		} else {
			if (val1 && val1->item.sint32_value
			    != val2->item.sint32_value)
				return (B_TRUE);
			else
				return (B_FALSE);
		}

	case CSA_MATCH_LESS_THAN:
		if (val1 == NULL)
			return (B_FALSE);
		else if (val2 && val1->item.sint32_value < val2->item.sint32_value)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_LESS_THAN_OR_EQUAL_TO:
		if (val1 == NULL)
			return (B_FALSE);
		else if (val2 && val1->item.sint32_value <= val2->item.sint32_value)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_GREATER_THAN:
		if (val1 == NULL)
			return (B_FALSE);
		else if (val2 && val1->item.sint32_value > val2->item.sint32_value)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_GREATER_THAN_OR_EQUAL_TO:
		if (val1 == NULL)
			return (B_FALSE);
		else if (val2 && val1->item.sint32_value >= val2->item.sint32_value)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_CONTAIN: /* only apply to string type */
	default:
		return (B_FALSE);
	}
}

extern boolean_t
_DtCm_match_uint32_attribute(
	cms_attribute_value *val1,
	cms_attribute_value *val2,
	CSA_enum op)
{
	if (val1 && val2 && val1->type != val2->type)
		return (B_FALSE);

	switch (op) {
	case CSA_MATCH_ANY:
		if (val1 != NULL)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_EQUAL_TO:
		if (val2 == NULL) {
			if (val1 == NULL)
				return (B_TRUE);
			else
				return (B_FALSE);
		} else {
			if (val1 && val1->item.uint32_value
			    == val2->item.uint32_value)
				return (B_TRUE);
			else
				return (B_FALSE);
		}

	case CSA_MATCH_NOT_EQUAL_TO:
		if (val2 == NULL) {
			if (val1 != NULL)
				return (B_TRUE);
			else
				return (B_FALSE);
		} else {
			if (val1 && val1->item.uint32_value
			    != val2->item.uint32_value)
				return (B_TRUE);
			else
				return (B_FALSE);
		}

	case CSA_MATCH_LESS_THAN:
		if (val1 == NULL)
			return (B_FALSE);
		else if (val2 && val1->item.uint32_value < val2->item.uint32_value)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_LESS_THAN_OR_EQUAL_TO:
		if (val1 == NULL)
			return (B_FALSE);
		else if (val2 && val1->item.uint32_value <= val2->item.uint32_value)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_GREATER_THAN:
		if (val1 == NULL)
			return (B_FALSE);
		else if (val2 && val1->item.uint32_value > val2->item.uint32_value)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_GREATER_THAN_OR_EQUAL_TO:
		if (val1 == NULL)
			return (B_FALSE);
		else if (val2 && val1->item.uint32_value >= val2->item.uint32_value)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_CONTAIN: /* only apply to string type */
	default:
		return (B_FALSE);
	}
}

/*
 * The matching value should have been checked by _DtCm_check_operator()
 * already so contains valid value for the corresponding operator.
 * Treat NULL string and string with zero length to be equal.
 * val1 is the string to be matched
 * val2 is the matching string
 */
extern boolean_t
_DtCm_match_string_attribute(
	cms_attribute_value *val1,
	cms_attribute_value *val2,
	CSA_enum op)
{
	if (val1 && val2 && val1->type != val2->type)
		return (B_FALSE);

	if (val1 == NULL) {
		if ((op == CSA_MATCH_EQUAL_TO && val2 == NULL) ||
		    (op == CSA_MATCH_NOT_EQUAL_TO && val2))
			return (B_TRUE);
		else
			return (B_FALSE);
	}

	switch (op) {
	case CSA_MATCH_ANY:
		return (B_TRUE);	/* since val1 != NULL */

	case CSA_MATCH_EQUAL_TO:
		if (val2 == NULL) {
			return (B_FALSE); /* since val1 != NULL */
		} else {
			return (is_same_string(val1->item.string_value,
				val2->item.string_value));
		}

	case CSA_MATCH_NOT_EQUAL_TO:
		if (val2 == NULL) {
			return (B_TRUE); /* since val1 != NULL */
		} else {
			return (!is_same_string(val1->item.string_value,
				val2->item.string_value));
		}

	case CSA_MATCH_CONTAIN:
		/* if val2 is null, will be rejected already */
		return (contain_substr(val1->item.string_value,
			val2->item.string_value));

	case CSA_MATCH_LESS_THAN:
	case CSA_MATCH_LESS_THAN_OR_EQUAL_TO:
	case CSA_MATCH_GREATER_THAN:
	case CSA_MATCH_GREATER_THAN_OR_EQUAL_TO:
	default:
		return (B_FALSE);
	}
}

extern boolean_t
_DtCm_match_reminder_attribute(
	cms_attribute_value *val1,
	cms_attribute_value *val2,
	CSA_enum op)
{
	CSA_reminder	*r1, *r2;
	cms_attribute_value tv1, tv2;

	if ((val1 && val1->type != CSA_VALUE_REMINDER) ||
	    (val2 && val2->type != CSA_VALUE_REMINDER))
		return (B_FALSE);

	switch (op) {
	case CSA_MATCH_ANY:
		if (val1 != NULL)
			return (B_TRUE);
		else
			return (B_FALSE);

	case CSA_MATCH_EQUAL_TO:
		if (val2 == NULL || val2->item.reminder_value == NULL) {
			if (val1 == NULL || val1->item.reminder_value == NULL)
				return (B_TRUE);
			else
				return (B_FALSE);
		} else if (val1 == NULL || val1->item.reminder_value == NULL) {
			return (B_FALSE);
		} else {
			r1 = val1->item.reminder_value;
			r2 = val2->item.reminder_value;

			if ((r1->repeat_count != r2->repeat_count) ||
			    !is_same_opaque_data(&r1->reminder_data,
			    &r2->reminder_data))
				return (B_FALSE);

			tv1.type = CSA_VALUE_SINT32;
			tv2.type = CSA_VALUE_SINT32;
			tv1.item.sint32_value = 0;
			tv2.item.sint32_value = 0;

			if (r1->lead_time && *r1->lead_time &&
			    _csa_iso8601_to_duration(r1->lead_time,
			    (time_t *)&tv1.item.sint32_value))
				return (B_FALSE);

			if (r2->lead_time && *r2->lead_time &&
			    _csa_iso8601_to_duration(r2->lead_time,
			    (time_t *)&tv2.item.sint32_value))
				return (B_FALSE);

			if (_DtCm_match_sint32_attribute(&tv1, &tv2, op)
			    == B_FALSE)
				return (B_FALSE);

			tv1.item.sint32_value = 0;
			tv2.item.sint32_value = 0;


			if (r1->snooze_time && *r1->snooze_time &&
			    _csa_iso8601_to_duration(r1->snooze_time,
			    (time_t *)&tv1.item.sint32_value))
				return (B_FALSE);
			if (r2->snooze_time && *r2->snooze_time &&
			    _csa_iso8601_to_duration(r2->snooze_time,
			    (time_t *)&tv2.item.sint32_value))
				return (B_FALSE);

			return (_DtCm_match_sint32_attribute(&tv1, &tv2, op));

		}

	/* these are not supported for CSA_VALUE_REMINDER type */
	case CSA_MATCH_NOT_EQUAL_TO:
	case CSA_MATCH_LESS_THAN:
	case CSA_MATCH_LESS_THAN_OR_EQUAL_TO:
	case CSA_MATCH_GREATER_THAN:
	case CSA_MATCH_GREATER_THAN_OR_EQUAL_TO:
	case CSA_MATCH_CONTAIN:
	default:
		return (B_FALSE);
	}
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

static boolean_t
match_this_attribute(Appt_4 *appt, cms_attribute attr, CSA_enum op)
{
	time_t			tick;
	_DtCm_old_attrs		oldtag;
	cms_attribute_value	oldattr;
	cms_attribute_value	matchattr;

	if (attr.name.name == NULL || attr.value == NULL)
		return (B_FALSE);

	if ((attr.name.num > 0 &&
	    _DtCm_get_old_attr_by_index(attr.name.num, &oldtag)) ||
	    (attr.name.num == 0 &&
	    _DtCm_get_old_attr_by_name(attr.name.name, &oldtag)))
		return (B_FALSE);

	switch (oldtag) {
	/* id is matched somewhere else
	case _DtCm_old_attr_id:
	*/
	case _DtCm_old_attr_time:
		/* the only case is to match a non-NULL start date with
		 * CSA_MATCH_NOT_EQUAL_TO
		 */
		_csa_iso8601_to_tick(attr.value->item.date_time_value, &tick);
		if (appt->appt_id.tick == tick)
			return (B_FALSE);
		else
			return (B_TRUE);

	case _DtCm_old_attr_type:
		oldattr.type = CSA_VALUE_UINT32;
		oldattr.item.uint32_value =
			_DtCm_eventtype4_to_type(appt->tag->tag);
		return (_DtCm_match_uint32_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_type2:

		oldattr.type = CSA_VALUE_STRING;
		oldattr.item.string_value =
			_DtCm_eventtype4_to_subtype(appt->tag->tag);
		return (_DtCm_match_string_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_duration:
		/* the only possibility is to match a NULL end date value
		 * or match a non-NULL end date with MATCH_NOT_EQUAL_TO
		 */
		if (op == CSA_MATCH_EQUAL_TO) {
			if (appt->duration == 0)
				return (B_TRUE);
			else
				return (B_FALSE);
		} else if (op == CSA_MATCH_ANY) {
			return (B_TRUE);
		} else if (op == CSA_MATCH_NOT_EQUAL_TO && attr.value == NULL) {
			if (appt->duration == 0)
				return (B_FALSE);
			else
				return (B_TRUE);
		} else if (op == CSA_MATCH_NOT_EQUAL_TO && attr.value) {
			_csa_iso8601_to_tick(attr.value->item.date_time_value,
				&tick);
			if ((appt->appt_id.tick+appt->duration) == tick)
				return (B_FALSE);
			else
				return (B_TRUE);
		} else
			return (B_FALSE);

	case _DtCm_old_attr_repeat_type:

		oldattr.type = CSA_VALUE_SINT32;
		oldattr.item.sint32_value =
			_DtCm_interval4_to_rtype(appt->period.period);
		return (_DtCm_match_sint32_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_what:

		oldattr.type = CSA_VALUE_STRING;
		oldattr.item.string_value = appt->what;
		return (_DtCm_match_string_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_status:

		oldattr.type = CSA_VALUE_UINT32;
		oldattr.item.uint32_value =
			_DtCm_apptstatus4_to_status(appt->appt_status);
		return (_DtCm_match_uint32_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_author:

		oldattr.type = CSA_VALUE_STRING;
		oldattr.item.string_value = appt->author;

		return (_DtCm_match_string_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_beep_reminder:

		return (match_old_reminder_attribute(
			get_appt_reminder(appt->attr,
			_DtCM_OLD_ATTR_BEEP_REMINDER), attr.value, op));

	case _DtCm_old_attr_flash_reminder:

		return (match_old_reminder_attribute(
			get_appt_reminder(appt->attr,
			_DtCM_OLD_ATTR_FLASH_REMINDER), attr.value, op));

	case _DtCm_old_attr_mail_reminder:

		return (match_old_reminder_attribute(
			get_appt_reminder(appt->attr,
			_DtCM_OLD_ATTR_MAIL_REMINDER), attr.value, op));

	case _DtCm_old_attr_popup_reminder:

		return (match_old_reminder_attribute(
			get_appt_reminder(appt->attr,
			_DtCM_OLD_ATTR_POPUP_REMINDER), attr.value, op));

	case _DtCm_old_attr_repeat_times:

		oldattr.type = CSA_VALUE_UINT32;
		oldattr.item.uint32_value = appt->ntimes;
		return (_DtCm_match_uint32_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_showtime:

		oldattr.type = CSA_VALUE_SINT32;
		oldattr.item.sint32_value = appt->tag->showtime;
		return (_DtCm_match_sint32_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_privacy:

		oldattr.type = CSA_VALUE_UINT32;
		oldattr.item.uint32_value =
			_DtCm_privacy4_to_classification(appt->privacy);
		return (_DtCm_match_uint32_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_repeat_nth_interval:

		oldattr.type = CSA_VALUE_UINT32;
		oldattr.item.uint32_value = appt->period.nth;
		return (_DtCm_match_uint32_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_repeat_nth_weeknum:

		oldattr.type = CSA_VALUE_SINT32;
		oldattr.item.sint32_value = appt->period.nth;
		return (_DtCm_match_sint32_attribute(&oldattr, attr.value, op));

	case _DtCm_old_attr_end_date:

		oldattr.type = CSA_VALUE_SINT32;
		oldattr.item.sint32_value = appt->period.enddate;

		if (attr.value) {
			matchattr.type = CSA_VALUE_SINT32;
			_csa_iso8601_to_tick(attr.value->item.date_time_value,
				(time_t *)&matchattr.item.sint32_value);
		}

		return (_DtCm_match_sint32_attribute(&oldattr,
			(attr.value ? &matchattr : NULL), op));
	defalut:
		return (B_FALSE);
	}
}

static Attribute_4 *
get_appt_reminder(Attribute_4 *attr4, char *oldattr)
{

	while (attr4 != NULL) {
		if (attr4->attr && !strcmp(attr4->attr, oldattr))
			return (attr4);
		else
			attr4 = attr4->next;
	}

	return (NULL);
}

static boolean_t
match_names(char *oldattr, uint num_names, char **names)
{
	int	i;
	char	*attrname;

	if (oldattr == NULL)
		return (B_FALSE);

	attrname = _DtCm_old_reminder_name_to_name(oldattr);

	for (i = 0; i < num_names; i++) {
		if (names[i] && (strcmp(names[i], attrname) == 0)) {
			return (B_TRUE);
		}
	}

	return (B_FALSE);
}

static boolean_t
match_old_reminder_attribute(
	Attribute_4 *rem4,
	cms_attribute_value *val,
	CSA_enum op)
{
	cms_attribute_value	oldattr;
	CSA_reminder	rem;	
	char		ltstring[BUFSIZ];

	if (rem4 != NULL) {
		rem.snooze_time = NULL;
		rem.repeat_count = 0;
		rem.reminder_data.size = (rem4->clientdata ?
						strlen(rem4->clientdata) : 0);
		rem.reminder_data.data = (unsigned char *)rem4->clientdata;

		_csa_duration_to_iso8601((rem4->value ? atoi(rem4->value) : 0),
			ltstring);
		rem.lead_time = ltstring;

		oldattr.type = CSA_VALUE_REMINDER;
		oldattr.item.reminder_value = &rem;
	}

	return (_DtCm_match_reminder_attribute((rem4 ? &oldattr : NULL),
		val, op));
}

static boolean_t
is_same_string(char *str1, char *str2)
{
	/* val2 is a NULL string */
	if (str2 == NULL || *str2 == 0) {
		if (str1 == NULL || *str1 == 0)
			return (B_TRUE);
		else
			return (B_FALSE);
	} else {
		if (str1 && strcmp(str1, str2) == 0)
			return (B_TRUE);
		else
			return (B_FALSE);
	}
}

static boolean_t
contain_substr(char *str1, char *str2)
{
	int	i, len;

	if (str2 == NULL || *str2 == '\0')
		return (B_TRUE);

	if (str1 == NULL || *str1 == '\0') {
		return (B_FALSE);
	} else {
		len = strlen(str2);

		for (i = 0; str1[i] != '\0'; i++) {
			if (strncasecmp(&str1[i], str2, len) == 0)
				return (B_TRUE);
		}
		return (B_FALSE);
	}
}

static boolean_t
is_same_opaque_data(CSA_opaque_data *d1, CSA_opaque_data *d2)
{
	/* val2 is a NULL string */
	if (d2 == NULL || d2->size == 0) {
		if (d1 == NULL || d1->size == 0)
			return (B_TRUE);
		else
			return (B_FALSE);
	} else {
		if (d1 && d1->size > 0) {
			if (d1->size == d2->size) {
				if (memcmp(d1->data, d2->data, d1->size) == 0)
					return (B_TRUE);
				else
					return (B_FALSE);
			} else
				return (B_FALSE);
		} else
			return (B_FALSE);
	}
}

