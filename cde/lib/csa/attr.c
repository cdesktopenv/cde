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
/* $XConsortium: attr.c /main/1 1996/04/21 19:21:41 drk $ */
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
#include <sys/types.h>
#include <unistd.h>
#include "attr.h"
#include "cmsdata.h"
#include "nametbl.h"
#include "free.h"
#include "misc.h"
#include "iso8601.h"
#include "lutil.h"

/*
 * calendar attributes defined by the library
 * Note: index zero is not used
 */
char *_CSA_calendar_attribute_names[] = {
	NULL,
	"-//XAPIA/CSA/CALATTR//NONSGML Access List//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Calendar Name//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Calendar Owner//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Calendar Size//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Character Set//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Country//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Date Created//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Language//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Number Entries//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Product Identifier//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Time Zone//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Version//EN",
	"-//XAPIA/CSA/CALATTR//NONSGML Work Schedule//EN",
	"-//CDE_XAPIA_PRIVATE/CSA/CALATTR//NONSGML Server Version//EN",
	"-//CDE_XAPIA_PRIVATE/CSA/CALATTR//NONSGML Data Version//EN",
	"-//CDE_XAPIA_PRIVATE/CSA/CALATTR//NONSGML Calendar Delimiter//EN"
};

/*
 * entry attributes defined by the library
 * Note: index zero is not used
 */
char *_CSA_entry_attribute_names[] = {
	NULL,
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Attendee List//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Audio Reminder//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Classification//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Date Completed//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Date Created//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Description//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Due Date//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML End Date//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Exception Dates//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Exception Rule//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Flashing Reminder//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Last Update//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Mail Reminder//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Number Recurrences//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Organizer//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Popup Reminder//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Priority//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Recurrence Rule//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Recurring Dates//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Reference Identifier//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Sequence Number//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Sponsor//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Start Date//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Status//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Subtype//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Summary//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Time Transparency//EN",
	"-//XAPIA/CSA/ENTRYATTR//NONSGML Type//EN",
	"-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Show Time//EN",
	"-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Type//EN",
	"-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Times//EN",
	"-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Interval//EN",
	"-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Occurrence Number//EN",
	"-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Sequence End Date//EN",
	"-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Entry Delimiter//EN"
};

/*
 * Values for entry attribute CSA_ENTRY_ATTR_SUBTYPE
 */
char *_CSA_entry_subtype_values[] = {
	"-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Appointment//EN",
	"-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Class//EN",
	"-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Holiday//EN",
	"-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Meeting//EN",
	"-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Miscellaneous//EN",
	"-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Phone Call//EN",
	"-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Sick Day//EN",
	"-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Special Occasion//EN",
	"-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Travel//EN",
	"-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Vacation//EN",
};

/* list of calendar attributes and value type */
_DtCmAttrInfo _CSA_cal_attr_info[] =
{
	/* first element is not used */
	{ 0, -1, 0, _DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_CAL_ATTR_ACCESS_LIST_I, CSA_VALUE_ACCESS_LIST,	1,
		_DtCm_old_attr_unknown, B_FALSE, B_FALSE },
	{ CSA_CAL_ATTR_CALENDAR_NAME_I, CSA_VALUE_STRING,	1,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_CAL_ATTR_CALENDAR_OWNER_I, CSA_VALUE_CALENDAR_USER,	1,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_CAL_ATTR_CALENDAR_SIZE_I,	CSA_VALUE_UINT32,	4,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_CAL_ATTR_CHARACTER_SET_I,	CSA_VALUE_STRING,	4,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_CAL_ATTR_COUNTRY_I,	CSA_VALUE_STRING,	0,
		_DtCm_old_attr_unknown, B_FALSE, B_FALSE },
	{ CSA_CAL_ATTR_DATE_CREATED_I,	CSA_VALUE_DATE_TIME,	4,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_CAL_ATTR_LANGUAGE_I,	CSA_VALUE_STRING,	0,
		_DtCm_old_attr_unknown, B_FALSE, B_FALSE },
	{ CSA_CAL_ATTR_NUMBER_ENTRIES_I, CSA_VALUE_UINT32,	1,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_CAL_ATTR_PRODUCT_IDENTIFIER_I, CSA_VALUE_STRING,	1,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_CAL_ATTR_TIME_ZONE_I,	CSA_VALUE_STRING,	4,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_CAL_ATTR_VERSION_I,	CSA_VALUE_STRING,	1,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_CAL_ATTR_WORK_SCHEDULE_I,	CSA_VALUE_OPAQUE_DATA,	0,
		_DtCm_old_attr_unknown, B_FALSE, B_FALSE },
	{ CSA_X_DT_CAL_ATTR_SERVER_VERSION_I, CSA_VALUE_UINT32,	1,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_X_DT_CAL_ATTR_DATA_VERSION_I, CSA_VALUE_UINT32,	1,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_X_DT_CAL_ATTR_CAL_DELIMITER_I, CSA_VALUE_STRING,	-1,
		_DtCm_old_attr_unknown, B_TRUE, B_TRUE }
};

/* list of entry attributes and value type */
_DtCmAttrInfo _CSA_entry_attr_info[] =
{
	/* first element is not used */
	{ 0, -1, 0, _DtCm_old_attr_unknown, B_TRUE, B_TRUE },
	{ CSA_ENTRY_ATTR_ATTENDEE_LIST_I,	CSA_VALUE_ATTENDEE_LIST,
	  0,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_AUDIO_REMINDER_I,	CSA_VALUE_REMINDER,
	  1,	_DtCm_old_attr_beep_reminder,	B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_CLASSIFICATION_I,	CSA_VALUE_UINT32,
	  2,	_DtCm_old_attr_privacy,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_DATE_COMPLETED_I,	CSA_VALUE_DATE_TIME,
	  4,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_DATE_CREATED_I,	CSA_VALUE_DATE_TIME,
	  4,	_DtCm_old_attr_unknown,		B_TRUE, B_TRUE },
	{ CSA_ENTRY_ATTR_DESCRIPTION_I,		CSA_VALUE_STRING,
	  4,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_DUE_DATE_I,		CSA_VALUE_DATE_TIME,
	  4,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_END_DATE_I,		CSA_VALUE_DATE_TIME,
	  1,	_DtCm_old_attr_duration,	B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_EXCEPTION_DATES_I,	CSA_VALUE_DATE_TIME_LIST,
	  4,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_EXCEPTION_RULE_I,	CSA_VALUE_STRING,
	  0,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_FLASHING_REMINDER_I,	CSA_VALUE_REMINDER,
	  1,	_DtCm_old_attr_flash_reminder,	B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_LAST_UPDATE_I,		CSA_VALUE_DATE_TIME,
	  4,	_DtCm_old_attr_unknown,		B_TRUE, B_TRUE },
	{ CSA_ENTRY_ATTR_MAIL_REMINDER_I,	CSA_VALUE_REMINDER,
	  1,	_DtCm_old_attr_mail_reminder,	B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I,	CSA_VALUE_UINT32,
	  4,	_DtCm_old_attr_unknown,		B_TRUE, B_TRUE },
	{ CSA_ENTRY_ATTR_ORGANIZER_I,		CSA_VALUE_CALENDAR_USER,
	  1,	_DtCm_old_attr_author,		B_TRUE, B_TRUE },
	{ CSA_ENTRY_ATTR_POPUP_REMINDER_I,	CSA_VALUE_REMINDER,
	  1,	_DtCm_old_attr_popup_reminder,	B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_PRIORITY_I,		CSA_VALUE_UINT32,
	  4,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_RECURRENCE_RULE_I,	CSA_VALUE_STRING,
	  4,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_RECURRING_DATES_I,	CSA_VALUE_DATE_TIME_LIST,
	  0,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I, CSA_VALUE_OPAQUE_DATA,	
	  1,	_DtCm_old_attr_id,		B_TRUE, B_TRUE },
	{ CSA_ENTRY_ATTR_SEQUENCE_NUMBER_I,	CSA_VALUE_UINT32,
	  0,	_DtCm_old_attr_unknown,		B_TRUE, B_TRUE },
	{ CSA_ENTRY_ATTR_SPONSOR_I,		CSA_VALUE_CALENDAR_USER,
	  4,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_START_DATE_I,		CSA_VALUE_DATE_TIME,
	  1,	_DtCm_old_attr_time,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_STATUS_I,		CSA_VALUE_UINT32,
	  2,	_DtCm_old_attr_status,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_SUBTYPE_I,		CSA_VALUE_STRING,
	  1,	_DtCm_old_attr_type2,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_SUMMARY_I,		CSA_VALUE_STRING,
	  1,	_DtCm_old_attr_what,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_TIME_TRANSPARENCY_I,	CSA_VALUE_SINT32,
	  4,	_DtCm_old_attr_unknown,		B_FALSE, B_FALSE },
	{ CSA_ENTRY_ATTR_TYPE_I,		CSA_VALUE_UINT32,
	  1,	_DtCm_old_attr_type,		B_FALSE, B_FALSE },
	{ CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,	CSA_VALUE_SINT32,
	  2,	_DtCm_old_attr_showtime,	B_FALSE, B_FALSE },
	{ CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,	CSA_VALUE_SINT32,
	  1,	_DtCm_old_attr_repeat_type,	B_FALSE, B_TRUE },
	{ CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,	CSA_VALUE_UINT32,
	  1,	_DtCm_old_attr_repeat_times,	B_FALSE, B_TRUE },
	{ CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I, CSA_VALUE_UINT32,
	  3,	_DtCm_old_attr_repeat_nth_interval, B_FALSE, B_TRUE },
	{ CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I, CSA_VALUE_SINT32,
	  3,	_DtCm_old_attr_repeat_nth_weeknum, B_FALSE, B_TRUE },
	{ CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I, CSA_VALUE_DATE_TIME,
	  3,	_DtCm_old_attr_end_date,	B_FALSE, B_TRUE },
	{ CSA_X_DT_ENTRY_ATTR_ENTRY_DELIMITER_I, CSA_VALUE_STRING,
	  -1,	_DtCm_old_attr_unknown,		B_TRUE, B_TRUE }
};

/*****************************************************************************
 * forward declaration of static functions used within the file
 *****************************************************************************/
static CSA_return_code check_predefined_attrs(
	int			fversion,
	uint			num_attrs,
	cms_attribute		*attrs,
	boolean_t		checkreadonly,
	_DtCmNameTable		*tbl,
	uint			num_defined,
	_DtCmAttrInfo		*our_attrs);

static CSA_return_code convert_cms_user_to_csa_user(char *from,
	CSA_calendar_user **to);

static CSA_return_code hash_entry_attrs(uint num_attrs, CSA_attribute *csaattrs,
	cms_attribute *cmsattrs, uint *hnum, cms_attribute **hattrs, uint *num);

static CSA_return_code hash_cal_attrs(uint num_attrs, CSA_attribute *csaattrs,
	cms_attribute *cmsattrs, uint *hnum, cms_attribute **hattrs, uint *num);

static CSA_return_code _DtCm_check_hashed_entry_attributes(
	int		fvers,
	uint		num_attrs,
	cms_attribute	*attrs,
	CSA_flags	utype);

static CSA_return_code _DtCm_check_hashed_cal_attributes(
	int		fvers,
	uint		num_attrs,
	cms_attribute	*attrs,
	char		*owner,
	char		*cname,
	boolean_t	checkreadonly,
	boolean_t	firsttime,
	boolean_t	csatype);

static CSA_return_code _CheckNameAtHost(char *owner, char *value);

static CSA_return_code _CheckCalendarOwner(char *owner, int type, char *name);

static CSA_return_code _CheckCalendarName(char *owner, char *cname,
					cms_attribute_value *val);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

/*
 * For each calendar attribute, if it is a predefined attribute,
 * check that the data type is correct.
 * If checkreadonly is B_TRUE, also check that it's not readonly.
 */
extern CSA_return_code
_DtCm_check_cal_csa_attributes(
	int		fvers,
	uint		num_attrs,
	CSA_attribute	*attrs,
	char		*cname,
	boolean_t	checkreadonly,
	boolean_t	firsttime,
	boolean_t	checkattrnum)
{
	CSA_return_code stat;
	uint		hnum;
	cms_attribute	*hattrs;
	uint		realnum;

	if ((stat = hash_cal_attrs(num_attrs, attrs, NULL, &hnum, &hattrs,
	    &realnum)) != CSA_SUCCESS)
		return (stat);

	if (checkattrnum == B_TRUE && realnum == 0)
		return (CSA_E_INVALID_PARAMETER);

	stat = _DtCm_check_hashed_cal_attributes(fvers, hnum, hattrs,
		NULL, cname, checkreadonly, firsttime, B_TRUE);

	free(hattrs);

	return (stat);
}

extern CSA_return_code
_DtCm_check_cal_cms_attributes(
	int		fvers,
	uint		num_attrs,
	cms_attribute	*attrs,
	char		*owner,
	char		*cname,
	boolean_t	checkreadonly,
	boolean_t	firsttime,
	boolean_t	checkattrnum)
{
	CSA_return_code stat;
	uint		hnum;
	cms_attribute	*hattrs;
	uint		realnum;

	if ((stat = hash_cal_attrs(num_attrs, NULL, attrs, &hnum, &hattrs,
	    &realnum)) != CSA_SUCCESS)
		return (stat);

	if (checkattrnum == B_TRUE && realnum == 0)
		return (CSA_E_INVALID_PARAMETER);

	stat = _DtCm_check_hashed_cal_attributes(fvers, hnum, hattrs,
		owner, cname, checkreadonly, firsttime, B_FALSE);

	free(hattrs);

	return (stat);
}

extern CSA_return_code
_DtCm_check_entry_attributes(
	int		fvers,
	uint		num_attrs,
	CSA_attribute	*attrs,
	CSA_flags	utype,
	boolean_t	checkattrnum)
{
	CSA_return_code	stat;
	uint		hnum;
	cms_attribute	*hattrs;
	uint		realnum;

	if ((stat = hash_entry_attrs(num_attrs, attrs, NULL, &hnum, &hattrs,
	    &realnum)) != CSA_SUCCESS)
		return (stat);

	if (checkattrnum == B_TRUE && realnum == 0)
		return (CSA_E_INVALID_PARAMETER);

	stat = _DtCm_check_hashed_entry_attributes(fvers, hnum, hattrs,
		utype);

	free(hattrs);

	return (stat);
}

extern CSA_return_code
_DtCm_check_entry_cms_attributes(
	int		fvers,
	uint		num_attrs,
	cms_attribute	*attrs,
	CSA_flags	utype,
	boolean_t	checkattrnum)
{
	CSA_return_code	stat;
	uint		hnum;
	cms_attribute	*hattrs;
	uint		realnum;

	if ((stat = hash_entry_attrs(num_attrs, NULL, attrs, &hnum, &hattrs,
	    &realnum)) != CSA_SUCCESS)
		return (stat);

	if (checkattrnum == B_TRUE && realnum == 0)
		return (CSA_E_INVALID_PARAMETER);

	stat = _DtCm_check_hashed_entry_attributes(fvers, hnum, hattrs,
		utype);

	free(hattrs);

	return (stat);
}

/*
 * copy attributes
 * attributes with a name but NULL value is allowed
 * attributes with null names are ignored
 * validity of attributes should be checked before calling this routine
 *
 * Note: the first entry is not used
 */
extern CSA_return_code
_DtCm_copy_cms_attributes(
	uint srcsize,
	cms_attribute *srcattrs,
	uint *dstsize,
	cms_attribute **dstattrs)
{
	int	i, j;
	CSA_return_code stat = CSA_SUCCESS;
	cms_attribute	*attrs;

	if (dstsize == NULL || dstattrs == NULL)
		return (CSA_E_INVALID_PARAMETER);

	*dstsize = 0;
	*dstattrs = NULL;

	if (srcsize == 0)
		return (CSA_SUCCESS);

	if ((attrs = calloc(1, sizeof(cms_attribute) * (srcsize + 1))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	/* firstr element is not used */
	for (i = 1, j = 1; i <= srcsize; i++) {
		if (srcattrs[i].name.name != NULL) {
			if ((stat = _DtCm_copy_cms_attribute(&attrs[j],
			    &srcattrs[i], B_TRUE)) != CSA_SUCCESS)
				break;
			else
				j++;
		}
	}

	if (stat != CSA_SUCCESS && j > 1) {
		_DtCm_free_cms_attributes(j, attrs);
		free(attrs);
	} else {
		*dstsize = j - 1;
		*dstattrs = attrs;
	}

	return(stat);
}

/*
 * Frees the name and value field of the array, but not
 * array itself.
 * note: element 0 is not used
 */
extern void
_DtCm_free_cms_attributes(uint size, cms_attribute *attrs)
{
	int i;

	for (i = 0; i < size; i++) {
		if (attrs[i].name.name) {
			free(attrs[i].name.name);
			attrs[i].name.name = NULL;

			if (attrs[i].value) {
				_DtCm_free_cms_attribute_value(attrs[i].value);
				attrs[i].value = NULL;
			}
		}
	}
}

/*
 * Frees the name and value field of the array, but not
 * array itself.
 */
extern void
_DtCm_free_attributes(uint size, CSA_attribute * attrs)
{
	int i;

	for (i = 0; i < size; i++) {
		if (attrs[i].name) {
			free(attrs[i].name);
			attrs[i].name = NULL;

			if (attrs[i].value) {
				_DtCm_free_attribute_value(attrs[i].value);
				attrs[i].value = NULL;
			}
		}
	}
}

/*
 * Free the value part of the cms_attribute structure.
 * note: element 0 is not used
 */
extern void
_DtCm_free_cms_attribute_values(uint size, cms_attribute *attrs)
{
	int	i;

	for (i = 1; i <= size; i++) {
		if (attrs[i].value) {
			_DtCm_free_cms_attribute_value(attrs[i].value);
			attrs[i].value = NULL;
		}
	}
}

/*
 * Free the value part of the the attribute structure.
 */
extern void
_DtCm_free_attribute_values(uint size, CSA_attribute * attrs)
{
	int	i;

	for (i = 0; i < size; i++) {
		if (attrs[i].value) {
			_DtCm_free_attribute_value(attrs[i].value);
			attrs[i].value = NULL;
		}
	}
}

extern char *
_DtCm_old_reminder_name_to_name(char *oldname)
{
	if (strcmp(oldname, _DtCM_OLD_ATTR_BEEP_REMINDER) == 0)
		return (CSA_ENTRY_ATTR_AUDIO_REMINDER);
	else if (strcmp(oldname, _DtCM_OLD_ATTR_FLASH_REMINDER) == 0)
		return (CSA_ENTRY_ATTR_FLASHING_REMINDER);
	else if (strcmp(oldname, _DtCM_OLD_ATTR_MAIL_REMINDER) == 0)
		return (CSA_ENTRY_ATTR_MAIL_REMINDER);
	else if (strcmp(oldname, _DtCM_OLD_ATTR_POPUP_REMINDER) == 0)
		return (CSA_ENTRY_ATTR_POPUP_REMINDER);
	else
		return (oldname);
}

extern int
_DtCm_old_reminder_name_to_index(char *oldname)
{
	char	*name;

	name = _DtCm_old_reminder_name_to_name(oldname);

	return (_DtCm_get_index_from_table(_DtCm_entry_name_tbl, name));
}

/*
 * Given an attribute name, return the corresponding
 * attribute number that's supported by old backends (v4 and before).
 */
extern CSA_return_code
_DtCm_get_old_attr_by_name(char *name, _DtCm_old_attrs *attr)
{
	int	index;

	index = _DtCm_get_index_from_table(_DtCm_entry_name_tbl, name);
	if (index > 0 && index <= _DtCM_DEFINED_ENTRY_ATTR_SIZE) {

		if (_CSA_entry_attr_info[index].oldattr
		    != _DtCm_old_attr_unknown) {
			*attr = _CSA_entry_attr_info[index].oldattr;
			return (CSA_SUCCESS);
		} else
			return (CSA_E_UNSUPPORTED_ATTRIBUTE);
	} else
		return (CSA_E_INVALID_ATTRIBUTE);
}

/*
 * Given an attribute index, return the corresponding
 * attribute number that's supported by old backends (v4 and before).
 */
extern CSA_return_code
_DtCm_get_old_attr_by_index(int index, _DtCm_old_attrs *attr)
{
	if (index <= _DtCM_DEFINED_ENTRY_ATTR_SIZE) {
		if (_CSA_entry_attr_info[index].oldattr
		    != _DtCm_old_attr_unknown) {
			*attr = _CSA_entry_attr_info[index].oldattr;
			return (CSA_SUCCESS);
		} else
			return (CSA_E_UNSUPPORTED_ATTRIBUTE);
	} else
		return (CSA_E_INVALID_ATTRIBUTE);
}

/*
 * copy attribute
 * the attribute structure should contain valid value
 * a NULL attribute value is valid
 */
extern CSA_return_code
_DtCm_copy_cms_attribute(
	cms_attribute *to,
	cms_attribute *from,
	boolean_t copyname)
{
	CSA_return_code	stat = CSA_SUCCESS;
	char		*name;

	if (to == NULL)
		return (CSA_E_INVALID_PARAMETER);

	/* copy the attribute name */
	if (copyname) {
		if ((name = strdup(from->name.name)) == NULL)
			return(CSA_E_INSUFFICIENT_MEMORY);
	}

	if ((stat = _DtCm_copy_cms_attr_val(from->value, &to->value))
	    == CSA_SUCCESS) {
		if (copyname) {
			to->name.name = name;
			to->name.num = from->name.num;
		}
	} else if (copyname)
		free (name);

	return(stat);
}

extern CSA_return_code
_DtCm_copy_cms_attr_val(cms_attribute_value *from, cms_attribute_value **to)
{
	CSA_return_code	stat = CSA_SUCCESS;
	cms_attribute_value	*val;

	if (to == NULL)
		return (CSA_E_INVALID_PARAMETER);

	/* copy the attribute value */
	if (from == NULL)
		val = NULL;
	else {
		if ((val = (cms_attribute_value *)calloc(1,
		    sizeof(cms_attribute_value))) == NULL)
			return (CSA_E_INSUFFICIENT_MEMORY);

		switch (from->type) {
		case CSA_VALUE_BOOLEAN:
		case CSA_VALUE_ENUMERATED:
		case CSA_VALUE_FLAGS:
		case CSA_VALUE_UINT32:
		case CSA_VALUE_SINT32:
			val->item.uint32_value = from->item.uint32_value;
			break;

		case CSA_VALUE_STRING:
		case CSA_VALUE_DATE_TIME:
		case CSA_VALUE_DATE_TIME_RANGE:
		case CSA_VALUE_TIME_DURATION:
		case CSA_VALUE_CALENDAR_USER:
			if (from->item.string_value)
				val->item.string_value =
					strdup(from->item.string_value);
			else
				val->item.string_value = calloc(1, 1);
			if (val->item.string_value == NULL)
				stat = CSA_E_INSUFFICIENT_MEMORY;
			break;

		case CSA_VALUE_REMINDER:
			if (from->item.reminder_value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				stat = _DtCm_copy_reminder(
					from->item.reminder_value,
					&val->item.reminder_value);
			break;
		case CSA_VALUE_ATTENDEE_LIST:
			stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			break;
		case CSA_VALUE_ACCESS_LIST:
			if (from->item.access_list_value &&
			    (val->item.access_list_value =
			    _DtCm_copy_cms_access_list(
			    from->item.access_list_value)) == NULL) {

				stat = CSA_E_INSUFFICIENT_MEMORY;
			}
			break;
		case CSA_VALUE_DATE_TIME_LIST:
			if (from->item.date_time_list_value &&
			    (val->item.date_time_list_value =
			    _DtCm_copy_date_time_list(
		    	    from->item.date_time_list_value)) == NULL) {

				stat = CSA_E_INSUFFICIENT_MEMORY;
			}
			break;
		case CSA_VALUE_OPAQUE_DATA:
			if (from->item.opaque_data_value) {
				stat = _DtCm_copy_opaque_data(
					from->item.opaque_data_value,
					&val->item.opaque_data_value);
			}
			break;
		default:
			stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			break;
		}

		if (stat != CSA_SUCCESS)
			free(val);
		else
			val->type = from->type;
	}

	if (stat == CSA_SUCCESS) {
		*to = val;
	}

	return(stat);
}

/*
 * copy the attribute name, and convert the attribute value
 */
extern CSA_return_code
_DtCm_cms2csa_attribute(cms_attribute from, CSA_attribute *to)
{
	CSA_return_code		stat;
	char			*name;
	CSA_attribute_value	*val;

	if ((name = strdup(from.name.name)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if ((stat = _DtCm_cms2csa_attrval(from.value, &val)) == CSA_SUCCESS) {
		to->name = name;
		to->value = val;
	} else
		free(name);

	return (stat);
}

extern CSA_return_code
_DtCm_cms2csa_attrval(cms_attribute_value *from, CSA_attribute_value **to)
{
	CSA_return_code	stat = CSA_SUCCESS;
	CSA_attribute_value	*val;

	if (to == NULL)
		return (CSA_E_INVALID_PARAMETER);

	/* copy the attribute value */
	if (from == NULL)
		val = NULL;
	else {
		if ((val = (CSA_attribute_value *)calloc(1,
		    sizeof(CSA_attribute_value))) == NULL)
			return (CSA_E_INSUFFICIENT_MEMORY);

		switch (from->type) {
		case CSA_VALUE_BOOLEAN:
		case CSA_VALUE_ENUMERATED:
		case CSA_VALUE_FLAGS:
		case CSA_VALUE_UINT32:
		case CSA_VALUE_SINT32:
			val->item.uint32_value = from->item.uint32_value;
			break;

		case CSA_VALUE_STRING:
		case CSA_VALUE_DATE_TIME:
		case CSA_VALUE_DATE_TIME_RANGE:
		case CSA_VALUE_TIME_DURATION:
			if (from->item.string_value)
				val->item.string_value =
					strdup(from->item.string_value);
			else
				val->item.string_value = calloc(1, 1);
			if (val->item.string_value == NULL)
				stat = CSA_E_INSUFFICIENT_MEMORY;
			break;

		case CSA_VALUE_REMINDER:
			if (from->item.reminder_value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				stat = _DtCm_copy_reminder(
					from->item.reminder_value,
					&val->item.reminder_value);
			break;
		case CSA_VALUE_CALENDAR_USER:
			if (from->item.calendar_user_value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				stat = convert_cms_user_to_csa_user(
					from->item.calendar_user_value,
					&val->item.calendar_user_value);
			break;
		case CSA_VALUE_ACCESS_LIST:
			stat = _DtCm_cms2csa_access_list(
				from->item.access_list_value,
				&val->item.access_list_value);
			break;
		case CSA_VALUE_ATTENDEE_LIST:
			stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			break;
		case CSA_VALUE_DATE_TIME_LIST:
			if (from->item.date_time_list_value &&
			    (val->item.date_time_list_value =
			    _DtCm_copy_date_time_list(
			    from->item.date_time_list_value)) == NULL) {
				stat = CSA_E_INSUFFICIENT_MEMORY;
			}
			break;
		case CSA_VALUE_OPAQUE_DATA:
			if (from->item.opaque_data_value) {
				stat = _DtCm_copy_opaque_data(
					from->item.opaque_data_value,
					&val->item.opaque_data_value);
			} else
				val->item.opaque_data_value = NULL;
			break;
		default:
			stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			break;
		}

		if (stat != CSA_SUCCESS)
			free(val);
		else
			val->type = from->type;
	}

	if (stat == CSA_SUCCESS) {
		*to = val;
	}

	return(stat);
}

extern void
_DtCm_free_cms_attribute_value(cms_attribute_value *val)
{
	if (val == NULL)
		return;

	switch (val->type) {
	case CSA_VALUE_STRING:
	case CSA_VALUE_DATE_TIME:
	case CSA_VALUE_DATE_TIME_RANGE:
	case CSA_VALUE_TIME_DURATION:
	case CSA_VALUE_CALENDAR_USER:
		if (val->item.string_value)
			free(val->item.string_value);
		break;

	case CSA_VALUE_REMINDER:
		if (val->item.reminder_value)
			_DtCm_free_reminder(val->item.reminder_value);
		break;
	case CSA_VALUE_ACCESS_LIST:
		if (val->item.access_list_value)
			_DtCm_free_cms_access_entry(val->item.access_list_value);
		break;
	case CSA_VALUE_DATE_TIME_LIST:
		if (val->item.date_time_list_value)
			_DtCm_free_date_time_list(
				val->item.date_time_list_value);
		break;
	case CSA_VALUE_OPAQUE_DATA:
		if (val->item.opaque_data_value) {
			_DtCm_free_opaque_data(val->item.opaque_data_value);
		}
		break;
	}
	free(val);
}

extern void
_DtCm_free_attribute_value(CSA_attribute_value *val)
{
	if (val == NULL)
		return;

	switch (val->type) {
	case CSA_VALUE_STRING:
	case CSA_VALUE_DATE_TIME:
	case CSA_VALUE_DATE_TIME_RANGE:
	case CSA_VALUE_TIME_DURATION:
		if (val->item.string_value)
			free(val->item.string_value);
		break;

	case CSA_VALUE_REMINDER:
		if (val->item.reminder_value)
			_DtCm_free_reminder(val->item.reminder_value);
		break;
	case CSA_VALUE_ACCESS_LIST:
		if (val->item.access_list_value)
			_DtCm_free_csa_access_list(val->item.access_list_value);
		break;
	case CSA_VALUE_CALENDAR_USER:
		if (val->item.calendar_user_value) {
			if (val->item.calendar_user_value->user_name)
				free(val->item.calendar_user_value->user_name);
			if (val->item.calendar_user_value->calendar_address)
				free(val->item.calendar_user_value->user_name);
			free(val->item.calendar_user_value);
		}
		break;
	case CSA_VALUE_DATE_TIME_LIST:
		if (val->item.date_time_list_value)
			_DtCm_free_date_time_list(
				val->item.date_time_list_value);
		break;
	case CSA_VALUE_OPAQUE_DATA:
		if (val->item.opaque_data_value) {
			_DtCm_free_opaque_data(val->item.opaque_data_value);
		}
		break;
	}
	free(val);
}

extern CSA_return_code
_DtCm_set_uint32_attrval(uint numval, cms_attribute_value **attrval)
{
	cms_attribute_value	*val;

	if ((val = (cms_attribute_value *)malloc(sizeof(cms_attribute_value)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	val->type = CSA_VALUE_UINT32;
	val->item.uint32_value = numval;

	*attrval = val;

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCm_set_sint32_attrval(int numval, cms_attribute_value **attrval)
{
	cms_attribute_value	*val;

	if ((val = (cms_attribute_value *)malloc(sizeof(cms_attribute_value)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	val->type = CSA_VALUE_SINT32;
	val->item.sint32_value = numval;

	*attrval = val;

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCm_set_string_attrval(
	char *strval,
	cms_attribute_value **attrval,
	CSA_enum type)
{
	cms_attribute_value	*val;

	if (type != CSA_VALUE_STRING && type != CSA_VALUE_DATE_TIME &&
	    type != CSA_VALUE_DATE_TIME_RANGE &&
	    type != CSA_VALUE_TIME_DURATION && type != CSA_VALUE_CALENDAR_USER)
		return (CSA_E_INVALID_PARAMETER);

	if ((val = (cms_attribute_value *)malloc(sizeof(cms_attribute_value)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	val->type = type;

	if (strval == NULL) {
		val->item.string_value = NULL;
	} else if ((val->item.string_value = strdup(strval)) == NULL) {
		free(val);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	*attrval = val;

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCm_set_user_attrval(
	char *user,
	cms_attribute_value **attrval)
{
	cms_attribute_value	*val;

	if (user == NULL) {
		*attrval = NULL;
		return (CSA_SUCCESS);
	}

	if ((val = (cms_attribute_value *)malloc(sizeof(cms_attribute_value)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	val->type = CSA_VALUE_CALENDAR_USER;

	if ((val->item.calendar_user_value = strdup(user)) == NULL) {
		free(val);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	*attrval = val;

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCm_set_reminder_attrval(CSA_reminder *remval, cms_attribute_value **attrval)
{
	cms_attribute_value *val;
	CSA_return_code stat;

	if (remval == NULL)
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);

	if ((val = (cms_attribute_value *)malloc(sizeof(cms_attribute_value)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	val->type = CSA_VALUE_REMINDER;

	if ((stat = _DtCm_copy_reminder(remval, &val->item.reminder_value))
	    != CSA_SUCCESS) {
		free(val);
		return (stat);
	} else {

		*attrval = val;

		return (CSA_SUCCESS);
	}
}

extern CSA_return_code
_DtCm_set_csa_access_attrval(
	cms_access_entry *aval,
	CSA_attribute_value **attrval)
{
	CSA_attribute_value *val;
	CSA_return_code stat = CSA_SUCCESS;

	if ((val = (CSA_attribute_value *)malloc(sizeof(CSA_attribute_value)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	val->type = CSA_VALUE_ACCESS_LIST;

	if (aval == NULL) {

		val->item.access_list_value = NULL;

	} else {
	
		stat = _DtCm_cms2csa_access_list(aval,
			&val->item.access_list_value);

	}

	if (stat == CSA_SUCCESS)
		*attrval = val;
	else
		free(val);

	return (stat);
}

extern CSA_return_code
_DtCm_set_opaque_attrval(CSA_opaque_data *data, cms_attribute_value **attrval)
{
	CSA_return_code stat;
	cms_attribute_value	*val;

	if (data == NULL)
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);

	if ((val = (cms_attribute_value *)malloc(sizeof(cms_attribute_value)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	val->type = CSA_VALUE_OPAQUE_DATA;

	if ((stat = _DtCm_copy_opaque_data(data, &val->item.opaque_data_value))
	    != CSA_SUCCESS) {
		free(val);
		return (stat);
	} else {

		*attrval = val;
		return (CSA_SUCCESS);
	}
}

extern CSA_return_code
_DtCm_set_csa_uint32_attrval(uint numval, CSA_attribute_value **attrval)
{
	CSA_attribute_value	*val;

	if ((val = (CSA_attribute_value *)malloc(sizeof(CSA_attribute_value)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	val->type = CSA_VALUE_UINT32;
	val->item.uint32_value = numval;

	*attrval = val;

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCm_set_csa_string_attrval(
	char *strval,
	CSA_attribute_value **attrval,
	CSA_enum type)
{
	CSA_attribute_value	*val;

	if (type != CSA_VALUE_STRING && type != CSA_VALUE_DATE_TIME &&
	    type != CSA_VALUE_DATE_TIME_RANGE &&
	    type != CSA_VALUE_TIME_DURATION && type != CSA_VALUE_CALENDAR_USER)
		return (CSA_E_INVALID_PARAMETER);

	if ((val = (CSA_attribute_value *)malloc(sizeof(CSA_attribute_value)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	val->type = type;

	if (strval == NULL) {
		val->item.string_value = NULL;
	} else if ((val->item.string_value = strdup(strval)) == NULL) {
		free(val);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	*attrval = val;

	return (CSA_SUCCESS);
}

extern void
_DtCm_free_csa_access_list(CSA_access_list alist)
{
	CSA_access_list nptr;

	while (alist != NULL) {
		nptr = alist->next;

		if (alist->user) {
			if (alist->user->user_name)
				free(alist->user->user_name);
			if (alist->user->calendar_address);
				free(alist->user->calendar_address);
			free(alist->user);
		}

		free(alist);

		alist = nptr;
	}
}

extern void
_DtCm_free_date_time_list(CSA_date_time_list list)
{
	CSA_date_time_entry *nptr;

	while (list != NULL) {
		nptr = list->next;

		if (list->date_time) {
			free(list->date_time);
		}

		free(list);

		list = nptr;
	}
}

extern void
_DtCm_free_cms_access_entry(cms_access_entry *list)
{
	cms_access_entry *nptr;

	while (list != NULL) {
		nptr = list->next;

		if (list->user) {
			free(list->user);
		}

		free(list);

		list = nptr;
	}
}

extern cms_access_entry *
_DtCm_copy_cms_access_list(cms_access_entry *alist)
{
	cms_access_entry	*l, *head, *prev;
	boolean_t	cleanup = B_FALSE;

	prev = head = NULL;
	while (alist != NULL) {
		if ((l = (cms_access_entry *)calloc(1, sizeof(cms_access_entry)))
		    == NULL) {
			cleanup = B_TRUE;
			break;
		}

		if ((l->user = strdup(alist->user)) == NULL) {
			free(l);
			cleanup = B_TRUE;
			break;
		}

		l->rights = alist->rights;
		l->next = NULL;

		if (head == NULL)
			head = l;
		else
			prev->next = l;
		prev = l;

		alist = alist->next;
	}

	if (cleanup == B_TRUE) {
		_DtCm_free_cms_access_entry(head);
		head = NULL;
	}
	return(head);
}

extern CSA_return_code
_DtCm_cms2csa_access_list(
	cms_access_entry *cmslist,
	CSA_access_rights **csalist)
{
	CSA_return_code stat = CSA_SUCCESS;
	CSA_access_rights *to, *head, *prev;

	head = prev = NULL;
	while (cmslist != NULL) {
		if ((to = (CSA_access_rights *)calloc(1,
		    sizeof(CSA_access_rights))) == NULL) {
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		if ((to->user = (CSA_calendar_user *)calloc(1,
		    sizeof(CSA_calendar_user))) == NULL) {
			free(to);
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		if ((to->user->user_name = strdup(cmslist->user)) == NULL) {
			free(to->user);
			free(to);
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		to->rights = cmslist->rights;
		to->next = NULL;

		if (head == NULL)
			head = to;
		else
			prev->next = to;

		prev = to;

		cmslist = cmslist->next;
	}

	if (stat != CSA_SUCCESS) {
		_DtCm_free_csa_access_list(head);
		head = NULL;
	}

	*csalist = head;
	return (stat);
}

extern CSA_return_code
_DtCm_csa2cms_access_list(
	CSA_access_rights *csalist,
	cms_access_entry **cmslist)
{
	CSA_return_code stat = CSA_SUCCESS;
	cms_access_entry *to, *head, *prev;

	head = prev = NULL;
	while (csalist != NULL) {
		if ((to = (cms_access_entry *)calloc(1,
		    sizeof(cms_access_entry))) == NULL) {
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		if (csalist->user->user_name) {
			if ((to->user = strdup(csalist->user->user_name))
			    == NULL) {
				stat = CSA_E_INSUFFICIENT_MEMORY;
				free(to);
				break;
			}
		} else {
			stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			free(to);
			break;
		}

		to->rights = csalist->rights;
		to->next = NULL;

		if (head == NULL)
			head = to;
		else
			prev->next = to;

		prev = to;

		csalist = csalist->next;
	}

	if (stat != CSA_SUCCESS) {
		_DtCm_free_cms_access_entry(head);
		head = NULL;
	}

	*cmslist = head;
	return (stat);
}

extern CSA_date_time_list
_DtCm_copy_date_time_list(CSA_date_time_list dlist)
{
	CSA_date_time_entry	*l, *head, *prev;
	boolean_t	cleanup = B_FALSE;

	prev = head = NULL;
	while (dlist != NULL) {
		if ((l = (CSA_date_time_entry *)calloc(1,
		    sizeof(CSA_date_time_entry))) == NULL) {
			cleanup = B_TRUE;
			break;
		}

		if ((l->date_time = strdup(dlist->date_time)) == NULL) {
			free(l);
			cleanup = B_TRUE;
			break;
		}

		l->next = NULL;

		if (head == NULL)
			head = l;
		else
			prev->next = l;
		prev = l;

		dlist = dlist->next;
	}

	if (cleanup == B_TRUE) {
		_DtCm_free_date_time_list(head);
		head = NULL;
	}
	return(head);
}

extern CSA_return_code
_DtCm_copy_reminder(CSA_reminder *from, CSA_reminder **to)
{
	CSA_reminder *newval;

	if ((newval = (CSA_reminder *)calloc(1, sizeof(CSA_reminder))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if (from->lead_time) {
		if ((newval->lead_time = strdup(from->lead_time)) == NULL) {
			free(newval);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
	} else {
		free(newval);
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);
	}

	if (from->snooze_time)
		newval->snooze_time = strdup(from->snooze_time);
	else
		newval->snooze_time = calloc(1, 1);

	if (newval->snooze_time == NULL) {
		_DtCm_free_reminder(newval);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	newval->repeat_count = from->repeat_count;

	if (from->reminder_data.size > 0) {
		newval->reminder_data.size = from->reminder_data.size;
		if ((newval->reminder_data.data = malloc(
		    newval->reminder_data.size)) == NULL) {
			_DtCm_free_reminder(newval);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
		memcpy(newval->reminder_data.data, from->reminder_data.data,
			from->reminder_data.size);
	}

	*to = newval;
	return (CSA_SUCCESS);
}

extern void
_DtCm_free_reminder(CSA_reminder *val)
{
	if (val == NULL) return;

	if (val->lead_time)
		free(val->lead_time);

	if (val->snooze_time)
		free(val->snooze_time);

	if (val->reminder_data.size > 0)
		free(val->reminder_data.data);

	free(val);
}

extern CSA_return_code
_DtCm_copy_opaque_data(CSA_opaque_data *from, CSA_opaque_data **to)
{
	CSA_opaque_data	*val;

	if ((val = (CSA_opaque_data *)calloc(1, sizeof(CSA_opaque_data)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	if (from->size > 0) {
		val->size = from->size;
		if ((val->data = malloc(from->size)) == NULL) {
			free(val);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
		memcpy(val->data, from->data, from->size);
	} else {
		val->size = 0;
		val->data = NULL;
	}

	*to = val;

	return (CSA_SUCCESS);
}

extern void
_DtCm_free_opaque_data(CSA_opaque_data *val)
{
	if (val == NULL) return;

	if (val->data)
		free(val->data);
	free(val);
}

extern void
_DtCm_get_attribute_types(uint size, int *types)
{
	int	i;

	for (i = 1; i <= size; i++) {
		types[i] = _CSA_entry_attr_info[i].type;
	}
}

/******************************************************************************
 * static functions used within the file
 ******************************************************************************/

/*
 * The passed in attributes are hashed.
 * For each attribute, check
 * 1. type is valid and supported
 * 2. if it's a date time value type, check validity of date time.
 * 3. if it's a reminder value type, check validity of lead time.
 * 4. if it is a defined attribute, check that the data type is correct.
 * 5. if it is a defined attribute and checkreadonly is set, check
 *	that it's not readonly.
 */
static CSA_return_code
check_predefined_attrs(
	int			fver,
	uint			num_attrs,
	cms_attribute		*attrs,
	boolean_t		checkreadonly,
	_DtCmNameTable		*tbl,
	uint			num_defined,
	_DtCmAttrInfo		*our_attrs)
{
	int		i, index, cl;
	CSA_reminder	*rptr;
	time_t		tick;

	for (i = 0; i < num_attrs; i++) {

		if (attrs[i].name.name == NULL)
			continue;

		if (tbl == NULL)
			index = i;
		else
			index = _DtCm_get_index_from_table(tbl,
					attrs[i].name.name); 

		if (index > 0 && index <= num_defined) {

			/* check whether the attribute is supported
			 * in this version
			 */
			if (our_attrs[index].fst_vers == 0 ||
			    (fver < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
			    fver < our_attrs[index].fst_vers))
				return (CSA_E_UNSUPPORTED_ATTRIBUTE);
			else if (our_attrs[index].fst_vers == -1)
				return (CSA_E_INVALID_ATTRIBUTE);

			/* check whether the attribute is readonly */
			if (checkreadonly &&
			    ((fver < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
			    our_attrs[index].nex_ro) ||
			    (fver >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
			    our_attrs[index].ex_ro)))
				return (CSA_E_READONLY);

			/* check data type */
			if (attrs[i].value &&
			    attrs[i].value->type != our_attrs[index].type)
				return (CSA_E_INVALID_ATTRIBUTE_VALUE);

			if (index == CSA_ENTRY_ATTR_CLASSIFICATION_I) {
				cl = attrs[i].value->item.uint32_value;
				if (cl < CSA_CLASS_PUBLIC ||
				    cl > CSA_CLASS_CONFIDENTIAL)
					return (CSA_E_INVALID_ATTRIBUTE_VALUE);
			}

			if (index == CSA_ENTRY_ATTR_TYPE_I) {
				cl = attrs[i].value->item.uint32_value;
				if (cl < CSA_TYPE_EVENT ||
				    cl > CSA_X_DT_TYPE_OTHER)
					return (CSA_E_INVALID_ATTRIBUTE_VALUE);
			}

		} else if (fver < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
			return (CSA_E_INVALID_ATTRIBUTE);
		}

		/* check validity of value type */
		if (attrs[i].value) {
			if (attrs[i].value->type < CSA_VALUE_BOOLEAN ||
			    attrs[i].value->type > CSA_VALUE_OPAQUE_DATA)
				return (CSA_E_INVALID_ATTRIBUTE_VALUE);

			/* cast the sint32_value element to the desired
			 * type to be safe since the value part could
			 * actually be a pointer to a CSA_attribute_value
			 */
			switch (attrs[i].value->type) {
			case CSA_VALUE_DATE_TIME:
				if (attrs[i].value->item.sint32_value == 0
				    || _csa_iso8601_to_tick(
				    (char *)attrs[i].value->item.sint32_value,
				    &tick))
					return (CSA_E_INVALID_DATE_TIME);
				break;
			case CSA_VALUE_REMINDER:
				rptr = (CSA_reminder *)
					attrs[i].value->item.sint32_value;
				if (rptr == NULL || rptr->lead_time == NULL ||
				    _csa_iso8601_to_duration(rptr->lead_time,
							     &tick))
					return (CSA_E_INVALID_ATTRIBUTE_VALUE);
				break;
			case CSA_VALUE_ATTENDEE_LIST:
				return (CSA_E_INVALID_ATTRIBUTE_VALUE);
			}
		}
	}

	return (CSA_SUCCESS);
}

static CSA_return_code
convert_cms_user_to_csa_user(char *from, CSA_calendar_user **to)
{
	CSA_calendar_user *newval;

	if ((newval = (CSA_calendar_user *)calloc(1,
	    sizeof(CSA_calendar_user))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if (from) {
		if ((newval->user_name = strdup(from)) == NULL) {
			free(newval);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
	}

	*to = newval;
	return (CSA_SUCCESS);
}

static CSA_return_code
hash_entry_attrs(
	uint		num_attrs,
	CSA_attribute	*csaattrs,
	cms_attribute	*cmsattrs,
	uint		*hnum,
	cms_attribute	**hattrs,
	uint		*num)
{
	CSA_return_code		stat;
	int			i, j, index, count = 0;
	cms_attribute		*nattrs;
	char			*name;
	cms_attribute_value	*val;

	if ((nattrs = (cms_attribute *)calloc(1,
	    sizeof(cms_attribute)*(num_attrs+_DtCM_DEFINED_ENTRY_ATTR_SIZE+1)))
	    == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	for (i = 0, j = _DtCM_DEFINED_ENTRY_ATTR_SIZE + 1; i < num_attrs; i++) {
		name = (csaattrs ? csaattrs[i].name : cmsattrs[i].name.name);
		if (name == NULL)
			continue;
		else {
			count++;
			val = (csaattrs ?
				((cms_attribute_value *)csaattrs[i].value) :
				cmsattrs[i].value);
		}

		index = _DtCm_get_index_from_table(_DtCm_entry_name_tbl, name);

		if (index > 0) {
			if (cmsattrs) cmsattrs[i].name.num = index;

			nattrs[index].name.name = name;
			nattrs[index].value = val;
		} else {
			nattrs[j].name.name = name;
			nattrs[j++].value = val;
		}
	}

	if (num) *num = count;
	*hnum = j - 1;
	*hattrs = nattrs;
	return (CSA_SUCCESS);
}

static CSA_return_code
hash_cal_attrs(
	uint		num_attrs,
	CSA_attribute	*csaattrs,
	cms_attribute	*cmsattrs,
	uint		*hnum,
	cms_attribute	**hattrs,
	uint		*num)
{
	CSA_return_code		stat;
	int			i, j, index, count = 0;
	cms_attribute		*nattrs;
	char			*name;
	cms_attribute_value	*val;

	if ((nattrs = (cms_attribute *)calloc(1,
	    sizeof(cms_attribute)*(num_attrs+_DtCM_DEFINED_CAL_ATTR_SIZE+1)))
	    == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	for (i = 0, j = _DtCM_DEFINED_CAL_ATTR_SIZE + 1; i < num_attrs; i++) {
		name = (csaattrs ? csaattrs[i].name : cmsattrs[i].name.name);
		if (name == NULL)
			continue;
		else {
			count++;
			val = (csaattrs ?
				((cms_attribute_value *)csaattrs[i].value) :
				cmsattrs[i].value);
		}

		index = _DtCm_get_index_from_table(_DtCm_cal_name_tbl, name);

		if (index > 0) {
			if (cmsattrs) cmsattrs[i].name.num = index;

			nattrs[index].name.name = name;
			nattrs[index].value = val;
		} else {
			nattrs[j].name.name = name;
			nattrs[j++].value = val;
		}
	}

	if (num) *num = count;
	*hnum = j - 1;
	*hattrs = nattrs;
	return (CSA_SUCCESS);
}

static CSA_return_code
_DtCm_check_hashed_cal_attributes(
	int		fvers,
	uint		num_attrs,
	cms_attribute	*attrs,
	char		*owner,
	char		*cname,
	boolean_t	checkreadonly,
	boolean_t	firsttime,
	boolean_t	csatype)
{
	CSA_return_code stat;
	CSA_attribute_value	*csaval;
	cms_attribute_value	*cmsval;
	char			*nattr = NULL; /* calendar name */
	char			*oattr = NULL; /* calendar owner */
	char			*cattr = NULL; /* character set */
	char			*tattr = NULL; /* time zone */

	if (firsttime) {
		if (attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].name.name) {
			if (attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].value == NULL
			    || attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].value->\
			    item.calendar_user_value == NULL)
				return (CSA_E_INVALID_ATTRIBUTE_VALUE);

			if (csatype) {
				csaval = (CSA_attribute_value *)attrs[\
					CSA_CAL_ATTR_CALENDAR_OWNER_I].value;
				stat = _CheckCalendarOwner(owner, csaval->type,
				    (csaval && csaval->item.calendar_user_value?
				    csaval->item.calendar_user_value->user_name:
				    NULL));
			} else {
				cmsval = attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].\
					value;
				stat = _CheckCalendarOwner(owner, cmsval->type,
					cmsval ?
					cmsval->item.calendar_user_value:NULL);
			}

			if (stat != CSA_SUCCESS)
				return (stat);

			oattr = attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].name.name;
			attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].name.name = NULL;
		}

		if (attrs[CSA_CAL_ATTR_CALENDAR_NAME_I].name.name) {
			if (attrs[CSA_CAL_ATTR_CALENDAR_NAME_I].value == NULL ||
			    attrs[CSA_CAL_ATTR_CALENDAR_NAME_I].value->\
			    item.string_value == NULL) {
				if (oattr)
					attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].\
						name.name = oattr;
				return (CSA_E_INVALID_ATTRIBUTE_VALUE);
			}

			if ((stat = _CheckCalendarName(owner, cname,
			    attrs[CSA_CAL_ATTR_CALENDAR_NAME_I].value))
			    != CSA_SUCCESS) {
				if (oattr)
					attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].\
						name.name = oattr;
				return (stat);
			}

			nattr = attrs[CSA_CAL_ATTR_CALENDAR_NAME_I].name.name;
			attrs[CSA_CAL_ATTR_CALENDAR_NAME_I].name.name = NULL;
		}

		if (attrs[CSA_CAL_ATTR_CHARACTER_SET_I].name.name) {
			cattr = attrs[CSA_CAL_ATTR_CHARACTER_SET_I].name.name;
			attrs[CSA_CAL_ATTR_CHARACTER_SET_I].name.name = NULL;
		}

		if (attrs[CSA_CAL_ATTR_TIME_ZONE_I].name.name) {
			tattr = attrs[CSA_CAL_ATTR_TIME_ZONE_I].name.name;
			attrs[CSA_CAL_ATTR_TIME_ZONE_I].name.name = NULL;
		}
	}

	stat = check_predefined_attrs(fvers, num_attrs+1, attrs,
		checkreadonly, NULL, _DtCM_DEFINED_CAL_ATTR_SIZE,
		_CSA_cal_attr_info);

	if (oattr)
		attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].name.name = oattr;

	if (nattr)
		attrs[CSA_CAL_ATTR_CALENDAR_NAME_I].name.name = nattr;

	if (cattr)
		attrs[CSA_CAL_ATTR_CHARACTER_SET_I].name.name = oattr;

	if (tattr)
		attrs[CSA_CAL_ATTR_TIME_ZONE_I].name.name = nattr;

	return (stat);
}

static CSA_return_code
_DtCm_check_hashed_entry_attributes(
	int		fvers,
	uint		num_attrs,
	cms_attribute	*attrs,
	CSA_flags	utype)
{
	CSA_return_code stat;

	if ((stat = check_predefined_attrs(fvers, num_attrs+1, attrs,
	    (utype == 0 ? B_FALSE : B_TRUE), NULL,
	    _DtCM_DEFINED_ENTRY_ATTR_SIZE, _CSA_entry_attr_info))
	    != CSA_SUCCESS) {

		return (stat);
	}

	if (utype == CSA_CB_ENTRY_ADDED) {

		/* make sure the minimum set of attribute is specified */
		if (attrs[CSA_ENTRY_ATTR_START_DATE_I].value == NULL ||
		    attrs[CSA_ENTRY_ATTR_TYPE_I].value == NULL)
			return (CSA_E_INVALID_PARAMETER);

	} else if (utype == CSA_CB_ENTRY_UPDATED) {

		/* type can only be set at insertion time */
		if (attrs[CSA_ENTRY_ATTR_TYPE_I].name.name)
			return (CSA_E_READONLY);
	}

	return (CSA_SUCCESS);
}

static CSA_return_code
_CheckNameAtHost(char *owner, char *value)
{
	char		*ptr, *optr;
	int		res;

	/* check name part first */
	if (owner == NULL) {
		/* get user name of user running the application */
		if ((owner = _DtCmGetUserName()) == NULL)
			return (CSA_E_FAILURE);
	}

	if (optr = strchr(owner, '@')) *optr = '\0';
	if (ptr = strchr(value, '@')) *ptr = '\0';
	res = strcmp(value, owner);
	if (optr) *optr = '@';
	if (ptr) *ptr = '@';

	if (res != 0)
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);

	/* check host too if it's specified */
	if (ptr == NULL)
		return (CSA_SUCCESS);

	ptr++;
	if (strcmp(ptr, (optr ? ++optr : _DtCmGetLocalHost())) == 0)
		return (CSA_SUCCESS);
	else
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);
}

/*
 * check the data type and validity of calendar owner attribute
 */
static CSA_return_code
_CheckCalendarOwner(char *owner, int type, char *user)
{
	if (type != CSA_VALUE_CALENDAR_USER || user == NULL)
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);

	/* make sure user is the same as the one running the application */
	return (_CheckNameAtHost(owner, user));
}

/*
 * check the data type and validity of calendar name attribute
 */
static CSA_return_code
_CheckCalendarName(char *owner, char *cname, cms_attribute_value *val)
{
	CSA_return_code stat;
	char		*ptr, *optr;
	char		user[BUFSIZ];
	int		res;
	boolean_t	isuser;

	if (val->type != CSA_VALUE_STRING)
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);

	/* check that the attribute value is the same as the given
	 * calendar name
	 */
	if ((stat = _CheckNameAtHost(cname, val->item.string_value))
	    != CSA_SUCCESS)
		return (stat);

	/* now make sure if cal name is a user name, it's
	 * the same as that of the calling user
	 */
	if (ptr = strchr(val->item.string_value, '@')) *ptr = '\0';
	isuser = _DtCmIsUserName(val->item.string_value);
	if (ptr) *ptr = '@';

	/* make sure it's the same as the user running the application */
	if (isuser == B_TRUE) {
		sprintf(user, "%s%s", val->item.string_value, (ptr ? ptr : ""));
		return (_CheckNameAtHost(owner, user));
	}

	/* check the host part */
	if (ptr == NULL)
		return (CSA_SUCCESS);
	else
		ptr++;

	if (owner && (optr = strchr(owner, '@')))
		optr++;
	else
		optr = _DtCmGetLocalHost();

	if (strcmp(ptr, optr) == 0)
		return (CSA_SUCCESS);
	else
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);
}

