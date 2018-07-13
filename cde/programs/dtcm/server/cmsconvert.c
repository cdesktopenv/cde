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
/* $XConsortium: cmsconvert.c /main/5 1996/10/03 10:38:06 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmsconvert.h"
#include "convert5-4.h"
#include "convert4-5.h"
#include "attr.h"
#include "appt4.h"
#include "iso8601.h"
#include "lutil.h"
#include "cmsdata.h"

/*
 * Routines to convert data types used in version 5 to version 4.
 */

/*
 * forward declaration of functions used in the file
 */
static Abb_Appt_4 * _MakeAbbrAppt();

static CSA_return_code _AttrsToAbbrAppt(uint size, cms_attribute *attrs,
					Abb_Appt_4 *appt);

static uint _ConvertV5Access(uint v5rights, boolean_t strictmode);

static uint _ConvertV4Access(uint v4rights);

static char * _ConvertReminderName(char *rname);

static CSA_return_code _Appt4ToCmsentryAttrs(char *calname, Appt_4 *a4,
				uint *num_attrs, cms_attribute **attrs_r);
/*****************************************************************************
 * extern functions
 *****************************************************************************/

extern CSA_return_code
_DtCmsCmsentriesToAppt4ForClient(cms_entry *entries, Appt_4 **appt)
{
	CSA_return_code	stat = CSA_SUCCESS;
	Appt_4	*newappt, *prev, *head;

	if (appt == NULL)
		return (CSA_E_INVALID_PARAMETER);

	head = prev = NULL;
	while (entries != NULL) {
		if ((newappt = _DtCm_make_appt4(B_TRUE)) == NULL)
			break;

		if ((stat = _DtCmsAttrsToAppt4(entries->num_attrs + 1,
		    entries->attrs, newappt, B_FALSE)) != CSA_SUCCESS) {
			_DtCm_free_appt4(newappt);
			break;
		} else
			newappt->appt_id.key = entries->key.id;

		if (head == NULL)
			head = newappt;
		else
			prev->next = newappt;

		prev = newappt;
		entries = entries->next;
	}

	if (stat != CSA_SUCCESS) {
		_DtCm_free_appt4(head);
		head = NULL;
	}

	*appt = head;
	return (stat);
}

extern CSA_return_code
_DtCmsCmsentriesToAbbrAppt4ForClient(cms_entry *entries, Abb_Appt_4 **abbr)
{
	CSA_return_code	stat = CSA_SUCCESS;
	Abb_Appt_4	*newabbr, *prev, *head;

	if (abbr == NULL)
		return (CSA_E_INVALID_PARAMETER);

	head = prev = NULL;
	while (entries != NULL) {
		if ((newabbr = _MakeAbbrAppt()) == NULL)
			break;

		if ((stat = _AttrsToAbbrAppt(entries->num_attrs, entries->attrs,
		    newabbr)) != CSA_SUCCESS) {
			_DtCm_free_abbrev_appt4(newabbr);
			break;
		} else
			newabbr->appt_id.key = entries->key.id;

		if (head == NULL)
			head = newabbr;
		else
			prev->next = newabbr;

		prev = newabbr;
		entries = entries->next;
	}

	if (stat != CSA_SUCCESS) {
		_DtCm_free_abbrev_appt4(head);
		head = NULL;
	}

	*abbr = head;
	return (stat);
}

extern Access_Entry_4 *
_DtCmsConvertV5AccessList(cms_access_entry *cmslist, boolean_t strictmode)
{
	Access_Entry_4	*l, *head, *prev;
	boolean_t	cleanup = B_FALSE;

	prev = head = NULL;
	while (cmslist != NULL) {
		if ((l = (Access_Entry_4 *)calloc(1, sizeof(Access_Entry_4)))
		    == NULL) {
			cleanup = B_TRUE;
			break;
		}

		if ((l->who = strdup(cmslist->user)) == NULL) {
			free(l);
			cleanup = B_TRUE;
			break;
		}
		l->access_type = _ConvertV5Access(cmslist->rights, strictmode);

		if (head == NULL)
			head = l;
		else
			prev->next = l;
		prev = l;

		cmslist = cmslist->next;
	}

	if (cleanup == B_TRUE) {
		_DtCm_free_access_list4(head);
		head = NULL;
	}
	return (head);
}

extern cms_access_entry *
_DtCmsConvertV4AccessList(Access_Entry_4 *alist)
{
	cms_access_entry	*l, *head, *prev;
	boolean_t		cleanup = B_FALSE;

	prev = head = NULL;
	while (alist != NULL) {
		if ((l = (cms_access_entry *)calloc(1,
		    sizeof(cms_access_entry))) == NULL) {
			cleanup = B_TRUE;
			break;
		}

		if ((l->user = strdup(alist->who)) == NULL) {
			free(l);
			cleanup = B_TRUE;
			break;
		}
		l->rights = _ConvertV4Access(alist->access_type);

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
	return (head);
}

extern CSA_return_code
_DtCmsReminderRefToReminder(
	cms_reminder_ref	*rems,
	Reminder_4		**r4)
{
	CSA_return_code	stat = CSA_SUCCESS;
	Reminder_4	*head, *prev, *rptr;
	char		buf[30];

	prev = head = NULL;
	while (rems != NULL) {

		if ((rptr = (Reminder_4 *)calloc(1, sizeof(Reminder_4)))
		    == NULL) {
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		rptr->appt_id.tick = rems->key.time;
		rptr->appt_id.key = rems->key.id;
		rptr->tick = rems->runtime;

		if ((rptr->attr.attr=_ConvertReminderName(rems->reminder_name))
		    == NULL) {
			free(rptr);
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		if ((rptr->attr.value = calloc(1,1)) == NULL) {
			free(rptr->attr.attr);
			free(rptr);
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		if ((rptr->attr.clientdata = calloc(1,1)) == NULL) {
			free(rptr->attr.value);
			free(rptr->attr.attr);
			free(rptr);
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		if (head == NULL)
			head = rptr;
		else
			prev->next = rptr;
		prev = rptr;

		rems = rems->next;
	}

	if (stat != CSA_SUCCESS) {
		_DtCm_free_reminder4(head);
		head = NULL;
	}

	*r4 = head;

	return(stat);
}

extern CSA_return_code
_DtCmsV4ReminderToReminderRef(
	char			*calname,
	Reminder_4		*r4,
	_DtCmsEntryId		*ids,
	cms_reminder_ref	**rems)
{
	CSA_return_code		stat = CSA_SUCCESS;
	cms_reminder_ref	*head, *prev, *rptr;
	char			buf[BUFSIZ];

	prev = head = NULL;
	while (r4 != NULL) {

		if ((rptr = (cms_reminder_ref *)calloc(1,
		    sizeof(cms_reminder_ref))) == NULL) {
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		rptr->key.time = r4->appt_id.tick;
		rptr->key.id = r4->appt_id.key;
		rptr->runtime = r4->tick;

		if ((rptr->reminder_name = strdup(
		    _DtCm_old_reminder_name_to_name(r4->attr.attr)))
		    == NULL) {
			free(rptr);
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		sprintf(buf, "%ld:%s@%s", ids->id, calname,
			_DtCmGetHostAtDomain());

		if ((rptr->entryid = strdup(buf)) == NULL) {
			free(rptr->reminder_name);
			free(rptr);
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		if (head == NULL)
			head = rptr;
		else
			prev->next = rptr;
		prev = rptr;

		r4 = r4->next;
		ids = ids->next;
	}

	if (stat != CSA_SUCCESS) {
		_DtCmsFreeReminderRef(head);
		head = NULL;
	}

	*rems = head;

	return(stat);
}

extern CSA_return_code
_DtCmsCmsAccessToV4Access(cms_access_entry *alist, Access_Entry_4 **a4)
{
	CSA_return_code	stat = CSA_SUCCESS;
	Access_Entry_4	*to, *prev, *head;

	head = prev = NULL;
	while (alist != NULL) {
		if ((to = (Access_Entry_4 *)calloc(1, sizeof(Access_Entry_4)))
		    == NULL) {
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		if ((to->access_type = _DtCmAccessRightToV4AccessType(
		    alist->rights)) < 0) {
			stat = CSA_E_INVALID_FLAG;
			break;
		}

		if (alist->user == NULL) {
			stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			break;
		}

		if ((to->who = strdup(alist->user)) == NULL) {
			free(to);
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		if (head == NULL)
			head = to;
		else
			prev->next = to;

		prev = to;

		alist = alist->next;
	}

	if (stat != CSA_SUCCESS) {
		_DtCm_free_access_list4(head);
		head = NULL;
	}

	*a4 = head;
	return (stat);
}

extern CSA_return_code
_DtCmsAppt4ToCmsentriesForClient(
	char		*calname,
	Appt_4		*appt,
	cms_entry	**e_r)
{
	CSA_return_code	stat = CSA_SUCCESS;
        cms_entry	*entry, *head, *prev;

	if (e_r == NULL)
		return(CSA_E_INVALID_PARAMETER);

	prev = head = NULL;
	while (appt != NULL) {

		if ((entry = (cms_entry *)calloc(1, sizeof(cms_entry))) == NULL)
			break;

		if ((stat = _Appt4ToCmsentryAttrs(calname, appt,
		    &entry->num_attrs, &entry->attrs)) == CSA_SUCCESS) {
			entry->key.time = appt->appt_id.tick;
			entry->key.id = appt->appt_id.key;
		} else {
			free(entry);
			break;
		}

		if (head == NULL)
			head = entry;
		else
			prev->next = entry;

		prev = entry;

		appt = appt->next;
	}

	if (stat != CSA_SUCCESS) {
		_DtCm_free_cms_entries(head);
		head = NULL;
	}

	*e_r = head;
	return(stat);
}

extern CSA_return_code
_DtCmsAppt4ToCmsentry(
	char		*calname,
	Appt_4		*a4,
	cms_entry	**entry_r,
	boolean_t	rerule)
{
	CSA_return_code	stat;
        cms_entry	*entry;

	if (a4 == NULL)
		return(CSA_E_INVALID_PARAMETER);

	if ((entry = _DtCm_make_cms_entry(_DtCm_entry_name_tbl)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	entry->key.time = a4->appt_id.tick;
	entry->key.id = a4->appt_id.key;

	if ((stat = _DtCm_appt4_to_attrs(calname, a4, entry->num_attrs,
	    entry->attrs, rerule)) != CSA_SUCCESS) {
		_DtCm_free_cms_entry(entry);
		return (stat);
	}

	*entry_r = entry;
	return (CSA_SUCCESS);
}

/*
 * This routine is used either to convert attrs from clients for insert
 * or update; or to convert appts for sending to clients
 * Note: the reference id is not converted
 */
extern CSA_return_code
_DtCmsAttrsToAppt4(
	uint		size,
	cms_attribute	*attrs,
	Appt_4		*appt,
	boolean_t	check)
{
	CSA_return_code	stat = CSA_SUCCESS;
	char		*string;
	time_t		endtick;
	int		i, enddateindex = -1, interval = -1, wknum;
	int		typeindex = -1, stypeindex = -1;
	boolean_t	set_wknum = B_FALSE;

	/*
	 * this routine is called to convert valid v2 to v4 attributes
	 * so no checking of CSA_E_INVALID_ATTRIBUTE and
	 * CSA_E_UNSUPPORTED_ATTRIBUTE is necessary
	 */

	for (i = 0; i < size && stat == CSA_SUCCESS; i++) {
		switch (attrs[i].name.num) {
		case CSA_ENTRY_ATTR_START_DATE_I:
			if (check == B_TRUE && attrs[i].value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else if (_csa_iso8601_to_tick(attrs[i].value->\
			    item.date_time_value, &appt->appt_id.tick))
				stat = CSA_E_INVALID_DATE_TIME;
			break;

		case CSA_ENTRY_ATTR_END_DATE_I: 
			enddateindex = i;
			break;

		case CSA_ENTRY_ATTR_TYPE_I:
			typeindex = i;
			break;

		case CSA_ENTRY_ATTR_SUBTYPE_I:
			stypeindex = i;
			break;

		case CSA_ENTRY_ATTR_SUMMARY_I:

			if (attrs[i].value->item.string_value) {
				if ((string = strdup(attrs[i].value->item.\
				    string_value)) == NULL)
					stat = CSA_E_INSUFFICIENT_MEMORY;
				else {
					if (appt->what)
						free(appt->what);
					appt->what = string;
				}
			}
			break;

		case CSA_ENTRY_ATTR_ORGANIZER_I:
			free(appt->author);
			if ((appt->author = strdup(attrs[i].value->\
			    item.calendar_user_value)) == NULL)
				stat = CSA_E_INSUFFICIENT_MEMORY;
			break;

		case CSA_ENTRY_ATTR_AUDIO_REMINDER_I:

			if (attrs[i].value == NULL)
				stat = _DtCm_remove_reminder(
					_DtCM_OLD_ATTR_BEEP_REMINDER, appt);
			else if (attrs[i].value->item.reminder_value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				stat = _DtCm_add_reminder(
					_DtCM_OLD_ATTR_BEEP_REMINDER,
					attrs[i].value->item.reminder_value,
					appt);
			break;

		case CSA_ENTRY_ATTR_FLASHING_REMINDER_I:

			if (attrs[i].value == NULL)
				stat = _DtCm_remove_reminder(
					_DtCM_OLD_ATTR_FLASH_REMINDER, appt);
			else if (attrs[i].value->item.reminder_value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				stat = _DtCm_add_reminder(
					_DtCM_OLD_ATTR_FLASH_REMINDER,
					attrs[i].value->item.reminder_value,
					appt);
			break;

		case CSA_ENTRY_ATTR_MAIL_REMINDER_I:

			if (attrs[i].value == NULL)
				stat = _DtCm_remove_reminder(
					_DtCM_OLD_ATTR_MAIL_REMINDER, appt);
			else if (attrs[i].value->item.reminder_value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				stat = _DtCm_add_reminder(
					_DtCM_OLD_ATTR_MAIL_REMINDER,
					attrs[i].value->item.reminder_value,
					appt);
			break;

		case CSA_ENTRY_ATTR_POPUP_REMINDER_I:

			if (attrs[i].value == NULL)
				stat = _DtCm_remove_reminder(
					_DtCM_OLD_ATTR_POPUP_REMINDER, appt);
			else if (attrs[i].value->item.reminder_value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				stat = _DtCm_add_reminder(
					_DtCM_OLD_ATTR_POPUP_REMINDER,
					attrs[i].value->item.reminder_value,
					appt);
			break;

		case CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I:

			if (_DtCm_rtype_to_interval4(
	    		    attrs[i].value->item.sint32_value,
			    &appt->period.period)) {
				if (check == B_TRUE)
					stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
				else
					appt->period.period = otherPeriod_4;
			}
			break;

		case CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I:

			if (attrs[i].value) {
				appt->ntimes =
					attrs[i].value->item.uint32_value;
				if (appt->ntimes == CSA_X_DT_DT_REPEAT_FOREVER)
					appt->ntimes = _DtCM_OLD_REPEAT_FOREVER;
			}
			break;

		case CSA_X_DT_ENTRY_ATTR_SHOWTIME_I:

			if (attrs[i].value)
				appt->tag->showtime =
					attrs[i].value->item.sint32_value;
			break;

		case CSA_ENTRY_ATTR_STATUS_I:

			if (_DtCm_status_to_apptstatus4(
			    attrs[i].value->item.uint32_value,
			    &appt->appt_status)) {
				if (check == B_TRUE)
					stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
				else
					appt->appt_status = active_4;
			}
			break;

		case CSA_ENTRY_ATTR_CLASSIFICATION_I:

			if (_DtCm_classification_to_privacy4(
			    attrs[i].value->item.uint32_value, &appt->privacy))
			{
				if (check == B_TRUE)
					stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
				else
					appt->privacy = public_4;
			}
			break;

		case CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I:

			if (attrs[i].value &&
			    attrs[i].value->item.date_time_value &&
			    *(attrs[i].value->item.date_time_value))
			{
				if (_csa_iso8601_to_tick(
				    attrs[i].value->item.date_time_value,
				    &appt->period.enddate))
					stat = CSA_E_INVALID_DATE_TIME;
			}
			break;

		case CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I:
			if (attrs[i].value)
				interval = attrs[i].value->item.uint32_value;
			break;

		case CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I:
			if (attrs[i].value) {
				set_wknum = B_TRUE;
				wknum = attrs[i].value->item.sint32_value;
			}
			break;
		}
	}

	if (stat == CSA_SUCCESS && enddateindex >= 0) {
		if (attrs[enddateindex].value == NULL ||
		    attrs[enddateindex].value->item.date_time_value == NULL) {
			appt->duration = 0;
		} else if (_csa_iso8601_to_tick(attrs[enddateindex].value->\
		    item.date_time_value, &endtick)) {
			stat = CSA_E_INVALID_DATE_TIME;
		} else {
			appt->duration = endtick - appt->appt_id.tick;
		}
	}

	if (stat == CSA_SUCCESS && (typeindex >= 0 || typeindex >= 0)) {
		if (typeindex >= 0) {
			if (stat = _DtCm_attrs_to_eventtype4(
			    attrs[typeindex].value,
			    (stypeindex >= 0 ? attrs[stypeindex].value : NULL),
			    appt->tag)) {
				if (check == B_FALSE) {
					/* this is the case where we are
					 * converting appt for clients
					 */
					appt->tag->tag = otherTag_4;
					stat = CSA_SUCCESS;
				}
			}
		} else if (stypeindex >= 0) {
			if (attrs[stypeindex].value &&
			    attrs[stypeindex].value->item.string_value)
			{
				if (strcmp(attrs[stypeindex].value->item.\
				    string_value, CSA_SUBTYPE_APPOINTMENT) == 0)
					appt->tag->tag = appointment_4;
				else if (strcmp(attrs[stypeindex].value->item.\
				    string_value, CSA_SUBTYPE_HOLIDAY) == 0)
					appt->tag->tag = holiday_4;
				else
					stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			} else
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
		}
	}

	if (stat == CSA_SUCCESS) {
		switch (appt->period.period) {
		case nthWeekday_4:
			if (set_wknum == B_TRUE)
				appt->period.nth = wknum;
			break;
		case everyNthDay_4:
		case everyNthWeek_4:
		case everyNthMonth_4:
			if (interval >= 0)
				appt->period.nth = interval;
			break;
		default:
			appt->period.nth = 0;
		}

		if (appt->period.period == single_4)
			appt->ntimes = 0;
	}
	return (stat);
}


/******************************************************************************
 * static functions used within the file
 ******************************************************************************/

/*
 * attributes are hashed
 * we are converting v4 events to v3 appts, so attributes are
 * assumed to be good and all mandatory ones are present
 */
static CSA_return_code
_AttrsToAbbrAppt(uint size, cms_attribute *attrs, Abb_Appt_4 *appt)
{
	CSA_return_code	stat = CSA_SUCCESS;
	char		*ptr;
	time_t		endtick;
	int		i, typeindex = 0, stypeindex = -1, enddateindex = -1;
	char		buf[BUFSIZ];

	for (i = 0; i < size && stat == CSA_SUCCESS; i++) {
		switch (attrs[i].name.num) {
		case CSA_ENTRY_ATTR_START_DATE_I:
			_csa_iso8601_to_tick(attrs[i].value->\
			    item.date_time_value, &appt->appt_id.tick);
			break;

		case CSA_ENTRY_ATTR_END_DATE_I:
			enddateindex = i;
			break;

		case CSA_ENTRY_ATTR_TYPE_I:
			typeindex = i;
			break;

		case CSA_ENTRY_ATTR_SUBTYPE_I:
			stypeindex = i;
			break;

		case CSA_X_DT_ENTRY_ATTR_SHOWTIME_I:
			appt->tag->showtime = attrs[i].value->item.sint32_value;
			break;

		case CSA_ENTRY_ATTR_SUMMARY_I:
			if (attrs[i].value && attrs[i].value->item.string_value)
			{
				free(appt->what);
				if ((appt->what = strdup(attrs[i].value->\
				    item.string_value)) == NULL)
					stat = CSA_E_INSUFFICIENT_MEMORY;
			}
			break;

		case CSA_ENTRY_ATTR_STATUS_I:
			if (_DtCm_status_to_apptstatus4(attrs[i].value->item.\
			    uint32_value, &appt->appt_status))
				appt->appt_status = active_4;
			break;

		case CSA_ENTRY_ATTR_CLASSIFICATION_I:
			if (_DtCm_classification_to_privacy4(attrs[i].value->\
			    item.uint32_value, &appt->privacy))
				appt->privacy = public_4;
			break;

		case CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I:
			if (_DtCm_rtype_to_interval4(attrs[i].value->\
			    item.sint32_value, &appt->period.period))
				appt->period.period = otherPeriod_4;
			break;

		case CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I:
			appt->period.nth = attrs[i].value->item.sint32_value;
			break;

		case CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I:
			appt->period.nth = attrs[i].value->item.uint32_value;
			break;

		case CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I:
			_csa_iso8601_to_tick(attrs[i].value->item.\
				date_time_value, &appt->period.enddate);
			break;
		}
	}

	if (stat != CSA_SUCCESS)
		return (stat);

	if (enddateindex >= 0) {
		_csa_iso8601_to_tick(attrs[enddateindex].value->item.\
			date_time_value, &endtick);
		appt->duration = endtick - appt->appt_id.tick;
	} else
		appt->duration = 0;

	if (_DtCm_attrs_to_eventtype4(attrs[typeindex].value,
	    (stypeindex >= 0 ? attrs[stypeindex].value : NULL), appt->tag))
		appt->tag->tag = otherTag_4;

	return (CSA_SUCCESS);
}

static Abb_Appt_4 *
_MakeAbbrAppt(void)
{
	Abb_Appt_4 *a;

	if ((a = (Abb_Appt_4 *)calloc(1, sizeof(Abb_Appt_4))) == NULL)
		return NULL;

	if ((a->tag = (Tag_4 *)calloc(1, sizeof(Tag_4))) == NULL) {
		free(a);
		return NULL;
	}

	a->tag->tag = appointment_4;
	a->tag->showtime =  B_TRUE;

	return(a);
}

static uint
_ConvertV5Access(uint v5rights, boolean_t strictmode)
{
	uint	access = 0;

	if (v5rights & CSA_OWNER_RIGHTS)
		return (access_read_4 | access_write_4 | access_delete_4);

	if (v5rights & (CSA_VIEW_PUBLIC_ENTRIES|CSA_VIEW_CONFIDENTIAL_ENTRIES|
	    CSA_VIEW_PRIVATE_ENTRIES))
		access = access_read_4;

	if ((strictmode == B_TRUE &&
	    (v5rights & CSA_INSERT_PUBLIC_ENTRIES) &&
	    (v5rights & CSA_INSERT_PRIVATE_ENTRIES) &&
	    (v5rights & CSA_INSERT_CONFIDENTIAL_ENTRIES)) ||
	    (strictmode == B_FALSE &&
	    (v5rights & (CSA_INSERT_PUBLIC_ENTRIES |
	    CSA_INSERT_PRIVATE_ENTRIES | CSA_INSERT_CONFIDENTIAL_ENTRIES))))
		access |= access_write_4;

	if ((strictmode == B_TRUE &&
	    (v5rights & CSA_CHANGE_PUBLIC_ENTRIES) &&
	    (v5rights & CSA_CHANGE_PRIVATE_ENTRIES) &&
	    (v5rights & CSA_CHANGE_CONFIDENTIAL_ENTRIES)) ||
	    (strictmode == B_FALSE &&
	    (v5rights & (CSA_CHANGE_PUBLIC_ENTRIES |
	    CSA_CHANGE_PRIVATE_ENTRIES | CSA_CHANGE_CONFIDENTIAL_ENTRIES))))
		access |= access_delete_4;

	return (access);
}

static uint
_ConvertV4Access(uint v4rights)
{
	uint	access = 0;

	if (v4rights & access_read_4)
		access = CSA_VIEW_PUBLIC_ENTRIES;

	if (v4rights & access_write_4)
		access |= CSA_INSERT_PUBLIC_ENTRIES;

	if (v4rights & (access_write_4 | access_delete_4))
		access |= CSA_CHANGE_PUBLIC_ENTRIES;

	return (access);
}

static char *
_ConvertReminderName(char *rname)
{
	if (!strcmp(rname, CSA_ENTRY_ATTR_AUDIO_REMINDER))
		return (strdup(_DtCM_OLD_ATTR_BEEP_REMINDER));
	else if (!strcmp(rname, CSA_ENTRY_ATTR_FLASHING_REMINDER))
		return (strdup(_DtCM_OLD_ATTR_FLASH_REMINDER));
	else if (!strcmp(rname, CSA_ENTRY_ATTR_MAIL_REMINDER))
		return (strdup(_DtCM_OLD_ATTR_MAIL_REMINDER));
	else if (!strcmp(rname, CSA_ENTRY_ATTR_POPUP_REMINDER))
		return (strdup(_DtCM_OLD_ATTR_POPUP_REMINDER));
	else
		return (strdup(rname));
}

static CSA_return_code
_Appt4ToCmsentryAttrs(
	char		*calname,
	Appt_4		*a4,
	uint		*num_attrs,
	cms_attribute	**attrs_r)
{
	CSA_return_code		stat = CSA_SUCCESS;
	cms_attribute		*attrs;
	cms_attribute		tmpattr;
	cms_attribute_value	tmpval;
	Attribute_4		*reminders = a4->attr;
	char			buf[BUFSIZ];
	CSA_opaque_data		opq;
	int			index, ntimes, count = 0;
	char			*ptr1, *ptr2;

	if (num_attrs == NULL || attrs_r == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((attrs = (cms_attribute *)calloc(1,
	    sizeof(cms_attribute) * (_DtCM_OLD_ENTRY_ATTR_SIZE+ 1))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if (a4->period.period != single_4 &&
	    a4->ntimes == _DtCM_OLD_REPEAT_FOREVER)
		ntimes = CSA_X_DT_DT_REPEAT_FOREVER;
	else
		ntimes = a4->ntimes;

	tmpattr.value = &tmpval;

	/* reference id */
	tmpattr.name.name = CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER;
	tmpattr.name.num = CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I;
	tmpval.type = CSA_VALUE_OPAQUE_DATA;
	tmpval.item.opaque_data_value = &opq;
	ptr1 = strchr(calname, '@');
	ptr2 = (ptr1 ? strchr(ptr1, '.') : NULL);
	if (ptr1) {
		sprintf(buf, "%ld:%s%s%s", a4->appt_id.key, calname,
			(ptr2 == NULL ? "." : ""),
			(ptr2 == NULL ? _DtCmGetLocalDomain(ptr1+1) : ""));
	} else {
		sprintf(buf, "%ld:%s@%s", a4->appt_id.key, calname,
			_DtCmGetHostAtDomain());
	}
	opq.size = strlen(buf);
	opq.data = (unsigned char *)buf;

	if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		free(attrs);
		return (stat);
	}

	/* start date */
	tmpattr.name.name = CSA_ENTRY_ATTR_START_DATE;
	tmpattr.name.num = CSA_ENTRY_ATTR_START_DATE_I;
	tmpval.type = CSA_VALUE_DATE_TIME;
	tmpval.item.date_time_value = buf;
	if ((stat = _csa_tick_to_iso8601(a4->appt_id.tick, buf) ?
	    CSA_E_INVALID_DATE_TIME : CSA_SUCCESS) ||
	    (stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		goto error;
	}

	/* type */
	tmpattr.name.name = CSA_ENTRY_ATTR_TYPE;
	tmpattr.name.num = CSA_ENTRY_ATTR_TYPE_I;
	tmpval.type = CSA_VALUE_UINT32;
	tmpval.item.uint32_value = _DtCm_eventtype4_to_type(a4->tag->tag);
	if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		goto error;
	}

	/* subtype */
	tmpattr.name.name = CSA_ENTRY_ATTR_SUBTYPE;
	tmpattr.name.num = CSA_ENTRY_ATTR_SUBTYPE_I;
	tmpval.type = CSA_VALUE_STRING;
	tmpval.item.string_value = _DtCm_eventtype4_to_subtype(a4->tag->tag);
	if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		goto error;
	}

	/* showtime */
	tmpattr.name.name = CSA_X_DT_ENTRY_ATTR_SHOWTIME;
	tmpattr.name.num = CSA_X_DT_ENTRY_ATTR_SHOWTIME_I;
	tmpval.type = CSA_VALUE_SINT32;
	tmpval.item.sint32_value = a4->tag->showtime;
	if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		goto error;
	}

	/* end date */
	tmpattr.name.name = CSA_ENTRY_ATTR_END_DATE;
	tmpattr.name.num = CSA_ENTRY_ATTR_END_DATE_I;
	tmpval.type = CSA_VALUE_DATE_TIME;
	tmpval.item.date_time_value = buf;
	if ((stat = _csa_tick_to_iso8601(a4->appt_id.tick + a4->duration,
	    buf) ? CSA_E_INVALID_DATE_TIME : CSA_SUCCESS) != CSA_SUCCESS ||
	    (stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		goto error;
	}

	/* summary */
	tmpattr.name.name = CSA_ENTRY_ATTR_SUMMARY;
	tmpattr.name.num = CSA_ENTRY_ATTR_SUMMARY_I;
	tmpval.type = CSA_VALUE_STRING;
	tmpval.item.string_value = a4->what;
	if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		goto error;
	}

	/* organizer */
	tmpattr.name.name = CSA_ENTRY_ATTR_ORGANIZER;
	tmpattr.name.num = CSA_ENTRY_ATTR_ORGANIZER_I;
	tmpval.type = CSA_VALUE_CALENDAR_USER;
	tmpval.item.calendar_user_value = a4->author;
	if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		goto error;
	}

	/* status */
	tmpattr.name.name = CSA_ENTRY_ATTR_STATUS;
	tmpattr.name.num = CSA_ENTRY_ATTR_STATUS_I;
	tmpval.type = CSA_VALUE_UINT32;
	tmpval.item.uint32_value = _DtCm_apptstatus4_to_status(a4->appt_status);
	if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		goto error;
	}

	/* classification */
	tmpattr.name.name = CSA_ENTRY_ATTR_CLASSIFICATION;
	tmpattr.name.num = CSA_ENTRY_ATTR_CLASSIFICATION_I;
	tmpval.type = CSA_VALUE_UINT32;
	tmpval.item.uint32_value = _DtCm_privacy4_to_classification(a4->privacy);
	if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		goto error;
	}

	/* repeat type */
	tmpattr.name.name = CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE;
	tmpattr.name.num = CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I;
	tmpval.type = CSA_VALUE_SINT32;
	tmpval.item.sint32_value = _DtCm_interval4_to_rtype(a4->period.period);
	if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr, B_TRUE))
	    != CSA_SUCCESS) {
		goto error;
	}

	/* repeat times */
	if (a4->period.period != single_4) {
		tmpattr.name.name = CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES;
		tmpattr.name.num = CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I;
		tmpval.type = CSA_VALUE_UINT32;
		tmpval.item.uint32_value = ntimes;
		if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr,
		    B_TRUE)) != CSA_SUCCESS) {
			goto error;
		}
	}

	/* occurrence number */
	if (a4->period.period == nthWeekday_4) {
		tmpattr.name.name = CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM;
		tmpattr.name.num = CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I;
		tmpval.type = CSA_VALUE_SINT32;
		tmpval.item.sint32_value = a4->period.nth;
		if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr,
		    B_TRUE)) != CSA_SUCCESS) {
			goto error;
		}
	}

	/* repeat interval */
	if (a4->period.period >= everyNthDay_4 &&
	    a4->period.period <= everyNthMonth_4) {
		tmpattr.name.name = CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL;
		tmpattr.name.num = CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I;
		tmpval.type = CSA_VALUE_UINT32;
		tmpval.item.uint32_value = a4->period.nth;
		if ((stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr,
		    B_TRUE)) != CSA_SUCCESS) {
			goto error;
		}
	}

	if (a4->period.enddate != 0) {
		tmpattr.name.name = CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE;
		tmpattr.name.num = CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I;
		tmpval.type = CSA_VALUE_DATE_TIME;
		tmpval.item.date_time_value = buf;
		if ((stat = _csa_tick_to_iso8601(a4->period.enddate, buf) ?
		    CSA_E_INVALID_DATE_TIME : CSA_SUCCESS) != CSA_SUCCESS ||
		    (stat = _DtCm_copy_cms_attribute(&attrs[count++], &tmpattr,
		    B_TRUE)) != CSA_SUCCESS) {
			goto error;
		}
	}

	while (reminders != NULL) {
		if ((attrs[count].name.num = _DtCm_old_reminder_name_to_index(
		    reminders->attr)) >= 0) {
			if ((attrs[count].name.name =
			    strdup(_DtCm_old_reminder_name_to_name(
			    reminders->attr))) == NULL) {
				count++;
				goto error;
			}
			if ((stat = _DtCmAttr4ToReminderValue(reminders,
			    &attrs[count++].value)) != CSA_SUCCESS) {
				count++;
				goto error;
			}

		}
		reminders = reminders->next;
	}

	*num_attrs = count;
	*attrs_r = attrs;
	return (CSA_SUCCESS);

error:
	_DtCm_free_cms_attributes(count--, attrs);
	free(attrs);
	return (stat);
}

