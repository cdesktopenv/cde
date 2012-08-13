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
/* $XConsortium: convert5-4.c /main/2 1996/10/07 15:57:11 drk $ */
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
#include "convert4-5.h"
#include "convert5-4.h"
#include "attr.h"
#include "appt4.h"
#include "iso8601.h"

/*
 * Routines to convert data types used in version 5 to version 4.
 */

/*****************************************************************************
 * extern functions
 *****************************************************************************/

/*
 * attributes are checked before calling this routine
 * so all attributes should be supported in v4
 */
extern CSA_return_code
_DtCm_cmsattrs_to_apptdata(uint size, cms_attribute *attrs, Appt_4 *appt)
{
	CSA_return_code	stat = CSA_SUCCESS;
	char		*string, *ptr;
	time_t		endtick;
	cms_attribute	*aptr;
	char		buf[BUFSIZ];

	/*
	 * this routine is called to convert valid v2 to v4 attributes
	 * so no checking of CSA_E_INVALID_ATTRIBUTE and
	 * CSA_E_UNSUPPORTED_ATTRIBUTE is necessary
	 */

	/* assuming all appt data is available */

	/* start time */
	if (_csa_iso8601_to_tick(
	    attrs[CSA_ENTRY_ATTR_START_DATE_I].value->item.date_time_value,
	    &appt->appt_id.tick))
		return (CSA_E_INVALID_DATE_TIME);

	/* duration */
	aptr = &attrs[CSA_ENTRY_ATTR_END_DATE_I];
	if (aptr->value && aptr->value->item.date_time_value &&
		*(aptr->value->item.date_time_value))
	{
		if (_csa_iso8601_to_tick(aptr->value->item.date_time_value,
		    &endtick))
			return (CSA_E_INVALID_DATE_TIME);
		else
			appt->duration = endtick - appt->appt_id.tick;
	}

	/* id */
	aptr = &attrs[CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I];
	if (aptr->value && aptr->value->item.opaque_data_value &&
		aptr->value->item.opaque_data_value->size > 0)
	{
		strncpy(buf, (char *)aptr->value->item.opaque_data_value->data,
			aptr->value->item.opaque_data_value->size);
		buf[aptr->value->item.opaque_data_value->size] = '\0';

		ptr = strchr(buf, ':');
		if (ptr != NULL)
			*ptr = '\0';

		appt->appt_id.key = atol(buf);

	} else
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);

	/* _DtCm_old_attr_type */
	if ((stat = _DtCm_attrs_to_eventtype4(
	    attrs[CSA_ENTRY_ATTR_TYPE_I].value,
	    attrs[CSA_ENTRY_ATTR_SUBTYPE_I].value, appt->tag)) != CSA_SUCCESS)
		return (stat);

	/* _DtCm_old_attr_what */
	aptr = &attrs[CSA_ENTRY_ATTR_SUMMARY_I];
	if (aptr->value->item.string_value) {
		if ((string = strdup(aptr->value->item.string_value)) == NULL)
			return (CSA_E_INSUFFICIENT_MEMORY);
		else {
			if (appt->what)
				free(appt->what);
			appt->what = string;
		}
	}

	/* _DtCm_old_attr_author */
	aptr = &attrs[CSA_ENTRY_ATTR_ORGANIZER_I];
	if (aptr->value && aptr->value->item.calendar_user_value) {
		if ((string = strdup(aptr->value->item.calendar_user_value))
		    == NULL)
			return (CSA_E_INSUFFICIENT_MEMORY);
		else {
			if (appt->author)
				free(appt->author);
			appt->author = string;
		}
	}

	/* _DtCm_old_attr_beep_reminder */
	aptr = &attrs[CSA_ENTRY_ATTR_AUDIO_REMINDER_I];
	if (aptr->value && aptr->value->item.reminder_value) {
		if ((stat = _DtCm_add_reminder(_DtCM_OLD_ATTR_BEEP_REMINDER,
		    aptr->value->item.reminder_value, appt)) != CSA_SUCCESS)
			return (stat);
	}

	/* _DtCm_old_attr_flash_reminder */
	aptr = &attrs[CSA_ENTRY_ATTR_FLASHING_REMINDER_I];
	if (aptr->value && aptr->value->item.reminder_value) {
		if ((stat = _DtCm_add_reminder(_DtCM_OLD_ATTR_FLASH_REMINDER,
		    aptr->value->item.reminder_value, appt)) != CSA_SUCCESS)
			return (stat);
	}

	/* _DtCm_old_attr_mail_reminder */
	aptr = &attrs[CSA_ENTRY_ATTR_MAIL_REMINDER_I];
	if (aptr->value && aptr->value->item.reminder_value) {
		if ((stat = _DtCm_add_reminder(_DtCM_OLD_ATTR_MAIL_REMINDER,
		    aptr->value->item.reminder_value, appt)) != CSA_SUCCESS)
			return (stat);
	}

	/* _DtCm_old_attr_popup_reminder */
	aptr = &attrs[CSA_ENTRY_ATTR_POPUP_REMINDER_I];
	if (aptr->value && aptr->value->item.reminder_value) {
		if ((stat = _DtCm_add_reminder(_DtCM_OLD_ATTR_POPUP_REMINDER,
		    aptr->value->item.reminder_value, appt)) != CSA_SUCCESS)
			return (stat);
	}

	/* _DtCm_old_attr_repeat_type */
	if (_DtCm_rtype_to_interval4(
	    attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I].value->item.sint32_value,
	    &appt->period.period))
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);

	/* _DtCm_old_attr_repeat_times */
	aptr = &attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I];
	if (aptr->value)
		appt->ntimes = aptr->value->item.uint32_value;

	if (appt->period.period != single_4 &&
	    appt->ntimes == CSA_X_DT_DT_REPEAT_FOREVER) {
		appt->ntimes = _DtCM_OLD_REPEAT_FOREVER;
	}

	/* _DtCm_old_attr_showtime */
	aptr = &attrs[CSA_X_DT_ENTRY_ATTR_SHOWTIME_I];
	if (aptr->value)
		appt->tag->showtime = aptr->value->item.sint32_value;

	/* _DtCm_old_attr_status */
	if (_DtCm_status_to_apptstatus4(
	    attrs[CSA_ENTRY_ATTR_STATUS_I].value->item.uint32_value,
	    &appt->appt_status))
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);

	/* _DtCm_old_attr_privacy */
	if (_DtCm_classification_to_privacy4(
	    attrs[CSA_ENTRY_ATTR_CLASSIFICATION_I].value->item.uint32_value,
	    &appt->privacy))
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);

	/* _DtCm_old_attr_end_date */
	aptr = &attrs[CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I];
	if (aptr->value && aptr->value->item.date_time_value &&
		*(aptr->value->item.date_time_value))
	{
		if (_csa_iso8601_to_tick(aptr->value->item.date_time_value,
		    &appt->period.enddate))
			return (CSA_E_INVALID_DATE_TIME);
	}

	switch (appt->period.period) {
	case nthWeekday_4:
		/* _DtCm_old_attr_repeat_nth_weeknum */
		aptr = &attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I];
		if (aptr->value)
			appt->period.nth = aptr->value->item.sint32_value;
		break;
	case everyNthDay_4:
	case everyNthWeek_4:
	case everyNthMonth_4:
		/* _DtCm_old_attr_repeat_nth_interval */
		aptr = &attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I];
		if (aptr->value)
			appt->period.nth = aptr->value->item.uint32_value;
		break;
	default:
		appt->period.nth = 0;
	}

	return (stat);
}

/*
 * ** need to check whether value is valid
 */
extern CSA_return_code
_DtCm_attrs_to_apptdata(uint size, CSA_attribute *attrs, Appt_4 *appt)
{
	CSA_return_code	stat = CSA_SUCCESS;
	_DtCm_old_attrs	oldattr;
	char		*string, *ptr;
	int		interval = 0, week_num = 0, i;
	boolean_t	set_interval = B_FALSE, set_weeknum = B_FALSE;
	time_t		endtick = -1;
	int		tindex = 0, stindex = 0;
	CSA_attribute_value	otype;

	for (i = 0; i < size && stat == CSA_SUCCESS; i++) {

		if (attrs[i].name == NULL)
			continue;

		if ((stat = _DtCm_get_old_attr_by_name(attrs[i].name, &oldattr))
		    != CSA_SUCCESS)
			break;

		switch (oldattr) {
		case _DtCm_old_attr_id:
			stat = CSA_E_READONLY;
			break;

		case _DtCm_old_attr_time:
			if (attrs[i].value == NULL ||
			    attrs[i].value->item.date_time_value == NULL ||
			    _csa_iso8601_to_tick(
			    attrs[i].value->item.date_time_value,
			    &appt->appt_id.tick))
				stat = CSA_E_INVALID_DATE_TIME;
			break;
		case _DtCm_old_attr_type:
			if (attrs[i].value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				tindex = i;
			break;
		case _DtCm_old_attr_type2:
			stindex = i;
			break;
		case _DtCm_old_attr_duration:
			if (attrs[i].value == NULL)
				appt->duration = 0;
			else if (attrs[i].value->item.date_time_value
			    == NULL || _csa_iso8601_to_tick(
			    attrs[i].value->item.date_time_value,
			    &endtick))
				stat = CSA_E_INVALID_DATE_TIME;
			break;
		case _DtCm_old_attr_what:
			if (attrs[i].value && attrs[i].value->item.string_value)
			{
				if ((string = strdup(
				    attrs[i].value->item.string_value))
				    == NULL)
					stat = CSA_E_INSUFFICIENT_MEMORY;
				else {
					if (appt->what)
						free(appt->what);
					appt->what = string;
				}
			}
			break;
		case _DtCm_old_attr_author:
			stat = CSA_E_READONLY;
			break;

		case _DtCm_old_attr_beep_reminder:
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

		case _DtCm_old_attr_flash_reminder:
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

		case _DtCm_old_attr_mail_reminder:
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

		case _DtCm_old_attr_popup_reminder:
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

		case _DtCm_old_attr_repeat_type:
			if (attrs[i].value == NULL)
				appt->period.period = single_4;
			else if (_DtCm_rtype_to_interval4(
			    attrs[i].value->item.sint32_value,
			    &appt->period.period))
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			break;
		case _DtCm_old_attr_repeat_times:
			if (attrs[i].value == NULL)
				appt->ntimes = 1;
			else 
				appt->ntimes =
					attrs[i].value->item.uint32_value;
			break;
		case _DtCm_old_attr_showtime:
			if (attrs[i].value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				appt->tag->showtime =
					attrs[i].value->item.sint32_value;
			break;
		case _DtCm_old_attr_status:
			if (attrs[i].value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else if (_DtCm_status_to_apptstatus4(
			    attrs[i].value->item.sint32_value,
			    &appt->appt_status))
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			break;
		case _DtCm_old_attr_privacy:
			if (attrs[i].value == NULL)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else if (_DtCm_classification_to_privacy4(
			    attrs[i].value->item.sint32_value,
			    &appt->privacy))
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			break;
		case _DtCm_old_attr_repeat_nth_interval:
			if (attrs[i].value != NULL) {
				set_interval = B_TRUE;
				interval = attrs[i].value->item.uint32_value;
			}
			break;
		case _DtCm_old_attr_repeat_nth_weeknum:
			if (attrs[i].value != NULL) {
				set_weeknum = B_TRUE;
				week_num = attrs[i].value->item.sint32_value;
			}
			break;
		case _DtCm_old_attr_end_date:
			if (attrs[i].value == NULL)
				appt->period.enddate = 0;
			else if (attrs[i].value->item.date_time_value == NULL ||
			    _csa_iso8601_to_tick(
			    attrs[i].value->item.date_time_value,
			    &appt->period.enddate))
				stat = CSA_E_INVALID_DATE_TIME;
			break;
		default:
			stat = CSA_E_INVALID_PARAMETER;
		}
	}

	if (stat == CSA_SUCCESS) {

		if (tindex || stindex) {
			if (tindex == 0) {
				otype.type = CSA_VALUE_SINT32;
				switch (appt->tag->tag) {
				case appointment_4:
				case holiday_4:
					otype.item.sint32_value =
						CSA_TYPE_EVENT;
					break;
				case toDo_4:
					otype.item.sint32_value = CSA_TYPE_TODO;
					break;
				case reminder_4:
				case otherTag_4:
					otype.item.sint32_value =
						CSA_X_DT_TYPE_OTHER; 
				}
			}

			stat = _DtCm_attrs_to_eventtype4(
				(cms_attribute_value *)
				(tindex ? attrs[tindex].value : &otype),
				(cms_attribute_value *)
				(stindex ? attrs[stindex].value : NULL),
				appt->tag);
		}

		if (stat == CSA_SUCCESS) {
			if (endtick >= 0) {
				appt->duration = endtick - appt->appt_id.tick;
				/* make sure duration is positive */
				if (appt->duration < 0)
					stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			}

			if (appt->period.period != single_4 &&
	    		    appt->ntimes == CSA_X_DT_DT_REPEAT_FOREVER) {
				appt->ntimes = _DtCM_OLD_REPEAT_FOREVER;
			}

			switch (appt->period.period) {
			case nthWeekday_4:
				if (set_weeknum == B_TRUE)
					appt->period.nth = week_num;
				break;
			case everyNthDay_4:
			case everyNthWeek_4:
			case everyNthMonth_4:
				if (set_interval == B_TRUE)
					appt->period.nth = interval;
				break;
			default:
				appt->period.nth = 0;
			}
		}
	}

	return (stat);
}

extern CSA_return_code
_DtCm_cms_entry_to_appt4(cms_entry *entry, Appt_4 **appt4_r)
{
	CSA_return_code	stat;
	Appt_4		*appt;

	if (entry == NULL || appt4_r == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((appt = _DtCm_make_appt4(B_TRUE)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if ((stat = _DtCm_cmsattrs_to_apptdata(entry->num_attrs, entry->attrs,
	    appt)) != CSA_SUCCESS) {
		_DtCm_free_appt4(appt);
		return (stat);
	} else {
		*appt4_r = appt;
		return (CSA_SUCCESS);
	}
}

extern CSA_return_code
_DtCm_attrs_to_appt4(uint num_attrs, CSA_attribute *attrs, Appt_4 **appt4_r)
{
	CSA_return_code	stat;
	Appt_4		*appt;

	if (appt4_r == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((appt = _DtCm_make_appt4(B_TRUE)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if ((stat = _DtCm_attrs_to_apptdata(num_attrs, attrs, appt)) != CSA_SUCCESS) {
		_DtCm_free_appt4(appt);
		return (stat);
	} else {
		*appt4_r = appt;
		return (CSA_SUCCESS);
	}
}

extern CSA_return_code
_DtCm_scope_to_options4(CSA_enum scope, Options_4 *opt)
{
	if (opt == NULL)
		return (CSA_E_INVALID_PARAMETER);

	switch (scope) {
	case CSA_SCOPE_ONE:
		*opt = do_one_4;
		break;
	case CSA_SCOPE_ALL:
		*opt = do_all_4;
		break;
	case CSA_SCOPE_FORWARD:
		*opt = do_forward_4;
		break;
	default:
		return (CSA_E_INVALID_ENUM);
	}

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCm_csaaccesslist_toaccessentry4(CSA_access_list alist, Access_Entry_4 **a4)
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

		if (alist->user == NULL || alist->user->user_name == NULL) {
			stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			break;
		}

		if ((to->who = strdup(alist->user->user_name)) == NULL) {
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

extern int
_DtCm_rtype_to_interval4(int val, Interval_4 *period)
{
	switch (val) {
	case CSA_X_DT_REPEAT_ONETIME:
		*period = single_4;
		break;
	case CSA_X_DT_REPEAT_WEEKLY:
		*period = weekly_4;
		break;
	case CSA_X_DT_REPEAT_DAILY:
		*period = daily_4;
		break;
	case CSA_X_DT_REPEAT_BIWEEKLY:
		*period = biweekly_4;
		break;
	case CSA_X_DT_REPEAT_MONTHLY_BY_DATE:
		*period = monthly_4;
		break;
	case CSA_X_DT_REPEAT_YEARLY:
		*period = yearly_4;
		break;
	case CSA_X_DT_REPEAT_MONTHLY_BY_WEEKDAY:
		*period = nthWeekday_4;
		break;
	case CSA_X_DT_REPEAT_EVERY_NDAY:
		*period = everyNthDay_4;
		break;
	case CSA_X_DT_REPEAT_EVERY_NWEEK:
		*period = everyNthWeek_4;
		break;
	case CSA_X_DT_REPEAT_EVERY_NMONTH:
		*period = everyNthMonth_4;
		break;
	case CSA_X_DT_REPEAT_MON_TO_FRI:
		*period = monThruFri_4;
		break;
	case CSA_X_DT_REPEAT_MONWEDFRI:
		*period = monWedFri_4;
		break;
	case CSA_X_DT_REPEAT_TUETHUR:
		*period = tueThur_4;
		break;
	case CSA_X_DT_REPEAT_WEEKDAYCOMBO:
		*period = daysOfWeek_4;
		break;
	case CSA_X_DT_REPEAT_OTHER:
		*period = otherPeriod_4;
		break;
	default:
		return (-1);
	}
	return (0);
}

extern int
_DtCm_status_to_apptstatus4(int val, Appt_Status_4 *astat)
{
	switch (val) {
	case CSA_X_DT_STATUS_ACTIVE:
		*astat = active_4;
		break;
	case CSA_STATUS_COMPLETED:
		*astat = completed_4;
		break;
	case CSA_X_DT_STATUS_CANCELLED:
		*astat = cancelled_4;
		break;
	case CSA_X_DT_STATUS_COMMITTED:
		*astat = committed_4;
		break;
	case CSA_X_DT_STATUS_ADD_PENDING:
		*astat = pendingAdd_4;
		break;
	case CSA_X_DT_STATUS_DELETE_PENDING:
		*astat = pendingDelete_4;
		break;
	default:
		return (-1);
	}

	return (0);
}

extern int
_DtCm_classification_to_privacy4(int val, Privacy_Level_4 *privacy)
{
	switch (val) {
	case CSA_CLASS_PUBLIC:
		*privacy = public_4;
		break;
	case CSA_CLASS_CONFIDENTIAL:
		*privacy = semiprivate_4;
		break;
	case CSA_CLASS_PRIVATE:
		*privacy = private_4;
		break;
	default:
		return (-1);
	}

	return (0);
}

extern CSA_return_code
_DtCm_attrs_to_eventtype4(
	cms_attribute_value	*type,
	cms_attribute_value	*stype,
	Tag_4			*tag)
{
	CSA_return_code	stat = CSA_SUCCESS;

	if (type) {
		switch (type->item.uint32_value) {
		case CSA_TYPE_EVENT:

			if (stype && stype->item.string_value) {
				if (strcmp(stype->item.string_value,
				    CSA_SUBTYPE_APPOINTMENT) == 0)
					tag->tag = appointment_4;
				else if (strcmp(stype->item.string_value,
				    CSA_SUBTYPE_HOLIDAY) == 0)
					tag->tag = holiday_4;
				else
					stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			} else
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			break;

		case CSA_TYPE_TODO:

			if (stype)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				tag->tag = toDo_4;
			break;

		case CSA_X_DT_TYPE_OTHER:

			if (stype)
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				tag->tag = otherTag_4;
			break;

		default:
			stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
		}
	} else
		stat = CSA_E_INVALID_ATTRIBUTE_VALUE;

	return (stat);
}

/*
 * Get rid of the given reminder in the appt.
 */
extern CSA_return_code
_DtCm_remove_reminder(char *rem, Appt_4 *appt)
{
	Attribute_4	*attr, *prev;

	for (attr = prev = appt->attr; attr != NULL; attr = attr->next) { 
		if (strcmp(attr->attr, rem) == 0) {
			if (prev == attr)
				appt->attr = attr->next;
			else
				prev->next = attr->next;

			attr->next = NULL;
			_DtCm_free_attr4(attr);
		} else
			prev = attr;
	}

	return (CSA_SUCCESS);
}

/*
 * If the corresponding attribute exists already, replace it.
 */
extern CSA_return_code
_DtCm_add_reminder(char *rem, CSA_reminder * val, Appt_4 *appt)
{
	Attribute_4	*newattr, *attrs, *prev = NULL;
	char		adv[80];
	char		*str;
	time_t		ltime = 0;

/*
	if ((val->snooze_time && val->snooze_time[0] != NULL)
	    || val->repeat_count > 1)
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);
*/

	if ((newattr = calloc(1, sizeof(Attribute_4))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if ((newattr->attr = strdup(rem)) == NULL) {
		free(newattr);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	/* convert lead_time to advance value */
	if (val->lead_time)
		_csa_iso8601_to_duration(val->lead_time, &ltime);
	sprintf(adv, "%d", ltime);

	if ((newattr->value = strdup(adv)) == NULL) {
		_DtCm_free_attr4(newattr);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	if (val->reminder_data.size > 0) {
		if ((newattr->clientdata = malloc(val->reminder_data.size + 1))
		    == NULL) {
			_DtCm_free_attr4(newattr);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
		memcpy(newattr->clientdata, val->reminder_data.data,
			val->reminder_data.size);
		newattr->clientdata[val->reminder_data.size] = '\0';

	} else if ((newattr->clientdata = calloc(1,1)) == NULL) {
		_DtCm_free_attr4(newattr);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	for (attrs = appt->attr; attrs != NULL; attrs = attrs->next) {
		if (strcmp(attrs->attr, rem) == 0) {

			newattr->next = attrs->next;
			if (prev == NULL)
				appt->attr = newattr;
			else
				prev->next = newattr;

			attrs->next = NULL;
			_DtCm_free_attr4(attrs);

			break;
		} else
			prev = attrs;
	}

	if (attrs == NULL) {
		newattr->next = appt->attr;
		appt->attr = newattr;
	}

	return (CSA_SUCCESS);
}

extern int
_DtCmAccessRightToV4AccessType(unsigned int access)
{
	int	newaccess = access_none_4;

	if (access < 0 ||
	    access & ~(CSA_X_DT_BROWSE_ACCESS | CSA_X_DT_INSERT_ACCESS |
	    CSA_X_DT_DELETE_ACCESS))
		return (-1);

	if (access & CSA_X_DT_BROWSE_ACCESS)
		newaccess |= access_read_4;
	if (access & CSA_X_DT_INSERT_ACCESS)
		newaccess |= access_write_4;
	if (access & CSA_X_DT_DELETE_ACCESS)
		newaccess |= access_delete_4;

	return (newaccess);
}

