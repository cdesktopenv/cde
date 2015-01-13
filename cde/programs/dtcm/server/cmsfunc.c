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
/* $XConsortium: cmsfunc.c /main/4 1995/11/09 12:42:12 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>
#include <rpc/rpc.h>
#include <dirent.h>
#include "csa.h"
#include "cm.h"
#include "rtable4.h"
#include "attr.h"
#include "access.h"
#include "cmscalendar.h"
#include "updateattrs.h"
#include "cmsdata.h"
#include "cmsentry.h"
#include "match.h"
#include "rpcextras.h"
#include "v5ops.h"
#include "v4ops.h"
#include "iso8601.h"
#include "log.h"
#include "lutil.h"
#include "cm_tbl.i"
#include "callback.h"
#include "insert.h"
#include "update.h"
#include "delete.h"
#include "lookup.h"
#include "misc.h"
#include "convert4-5.h"
#include "convert5-4.h"
#include "appt4.h"
#include "cmsconvert.h"

extern int debug;

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/

static CSA_return_code _DtCmsCreateCallog(char *user, cms_create_args *args,
					_DtCmsCalendar **cal);
static CSA_return_code _ListCalendarNames(uint *num_names, char ***names);
static void * _grow_char_array(void *ptr, uint oldcount, uint newcount);
static void _free_char_array(uint num_elem, char **ptr);
static CSA_return_code _DtCmsGetOldCalAttrNames(_DtCmsCalendar *cal,
				uint *num_names_r, cms_attr_name **names_r);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

extern void *
cms_ping_5_svc(void *args, struct svc_req *svcrq)
{
	if (debug)
		fprintf(stderr, "cms_ping_5_svc called\n");

	return(NULL); /* for RPC reply */
}


extern cms_list_calendars_res *
cms_list_calendars_5_svc(void *dummy, struct svc_req *svcrq)
{
	static cms_list_calendars_res	res;

	if (debug)
		fprintf(stderr, "cms_list_calendars_5_svc called\n");

	if (res.num_names > 0) {
		_free_char_array(res.num_names, res.names);
		res.num_names = 0;
	}

	res.stat = _ListCalendarNames(&res.num_names, &res.names);

	return (&res);
}

extern  cms_open_res *
cms_open_calendar_5_svc(cms_open_args *args, struct svc_req *svcrq)
{
	static cms_open_res	res;
	static char		sversion[80];
	_DtCmsCalendar		*cal;
	char			*user;

	if (debug)
		fprintf(stderr, "cms_open_calendar_5_svc called\n");

	if (res.num_attrs > 0) {
		_DtCm_free_cms_attributes(res.num_attrs, res.attrs);
		free(res.attrs);
		res.num_attrs = 0;
	}

	/* check parameter */
	if (args->cal == NULL) {
		res.stat = CSA_E_INVALID_PARAMETER;
		return (&res);
	}

	if ((res.stat = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &res.user_access, &cal)) == CSA_SUCCESS) {

		res.svr_vers = TABLEVERS;

		if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {

			res.file_vers = cal->fversion;

		} else {
			/* old format data */
			res.file_vers = _DtCM_FIRST_EXTENSIBLE_DATA_VERSION - 1;
		}

		/* return attribute names */
		res.stat = _DtCmsGetAllCalAttrs(cal, &res.num_attrs,
					&res.attrs, B_FALSE);
	}

	if (res.stat == CSA_SUCCESS) {
		/* do callbacks */
		cal->rlist = _DtCmsDoOpenCalCallback(cal->rlist,
				cal->calendar, user, args->pid);
	}


	return (&res);
}

extern  CSA_return_code *
cms_create_calendar_5_svc(cms_create_args *args, struct svc_req *svcrq)
{
	static CSA_return_code	res;
	_DtCmsCalendar		*cal;
	char			*user;

	if (debug)
		fprintf(stderr, "cms_create_calendar_5_svc called\n");

	/* check parameter */
	if (args->cal == NULL) {
		res = CSA_E_INVALID_PARAMETER;
		return (&res);
	}

	/* need to check whether we know about the sender,
	 * if not, fail the request
	 */
	if ((res = _DtCmsGetClientInfo(svcrq, &user)) != CSA_SUCCESS) {
		return (&res);
	}

	if ((res = _DtCm_check_cal_cms_attributes(_DtCMS_VERSION4,
	    args->num_attrs, args->attrs, user, args->cal, B_TRUE, B_TRUE,
	    B_FALSE)) != CSA_SUCCESS)
		return (&res);

	if ((res = _DtCmsGetCalendarByName(args->cal, B_FALSE, &cal))
	    == CSA_SUCCESS && cal != NULL) {

		res = CSA_E_CALENDAR_EXISTS;

	} else {

		/* create callog file for new calendar */
		res = _DtCmsCreateCallog(user, args, &cal);
	}

	return (&res);
}

extern  CSA_return_code *
cms_remove_calendar_5_svc(cms_remove_args *args, struct svc_req *svcrq)
{
	static CSA_return_code	res;
	_DtCmsCalendar		*cal;
	char			*user;
	uint			access;

	if (debug)
		fprintf(stderr, "cms_remove_calendar_5_svc called\n");

	if ((res = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	if (!(access & CSA_OWNER_RIGHTS)) {

		res = CSA_E_NO_AUTHORITY;

	} else {

		/* move callog.name file to calrm.name */
		res = _DtCmsRemoveLog(cal->calendar, user);

		if (res != CSA_SUCCESS)
			return (&res);

		/* do callbacks */
		cal->rlist = _DtCmsDoRemoveCalCallback(cal->rlist,
				cal->calendar, user, args->pid);

		/* free up internal structures */
		_DtCmsFreeCalendar(cal);
	} 

	return (&res);
}

extern  CSA_return_code *
cms_register_5_svc(cms_register_args *args, struct svc_req *svcrq)
{
	static CSA_return_code	res;
	char			*user;
	_DtCmsCalendar		*cal;
	_DtCmsRegistrationInfo	*rinfo;

	if (debug)
		fprintf(stderr, "cms_register_5_svc called\n");

	if (args->cal == NULL ||
	    args->update_type >= (CSA_CB_ENTRY_UPDATED << 1)) {
		res = CSA_E_INVALID_PARAMETER;
		return (&res);
	}

	if ((res = _DtCmsGetClientInfo(svcrq, &user)) != CSA_SUCCESS)
		return (&res);

	if ((res = _DtCmsGetCalendarByName(args->cal, B_TRUE, &cal))
	    != CSA_SUCCESS)
		return (&res);

	if ((rinfo = _DtCmsGetRegistration(&(cal->rlist), user, args->prognum,
	    args->versnum, args->procnum, args->pid)) == NULL) {
		/* this client has not registered */

		if ((rinfo = _DtCmsMakeRegistrationInfo(user,
		    args->update_type, args->prognum, args->versnum,
		    args->procnum, args->pid)) == NULL) {
			res = CSA_E_INSUFFICIENT_MEMORY;
			return (&res);
		}

		/* put in the calendar's registration list */
		rinfo->next = cal->rlist;
		cal->rlist = rinfo;
	} else {
		if (debug) {
			fprintf(stderr, "%s registered on %s, old types = %d\n",
				user, args->cal, rinfo->types);
		}

		/* add new type to the registration */
		rinfo->types = rinfo->types | args->update_type;
	}

	if (debug) {
		fprintf(stderr, "%s registered on %s, types = %d\n",
			user, args->cal, rinfo->types);
	}

	res = CSA_SUCCESS;
	return (&res);
}

extern  CSA_return_code *
cms_unregister_5_svc(cms_register_args *args, struct svc_req *svcrq)
{
	static CSA_return_code	res;
	char			*user;
	_DtCmsCalendar		*cal;
	_DtCmsRegistrationInfo	*rinfo;

	if (debug)
		fprintf(stderr, "cms_unregister_5_svc called\n");

	if (args->cal == NULL ||
	    args->update_type >= (CSA_CB_ENTRY_UPDATED << 1)) {
		res = CSA_E_INVALID_PARAMETER;
		return (&res);
	}

	if ((res = _DtCmsGetClientInfo(svcrq, &user)) != CSA_SUCCESS)
		return (&res);

	if ((res = _DtCmsGetCalendarByName(args->cal, B_FALSE, &cal))
	    != CSA_SUCCESS)
		return (&res);

	if (cal == NULL || (rinfo = _DtCmsGetRegistration(&(cal->rlist), user,
	    args->prognum, args->versnum, args->procnum, args->pid)) == NULL) {
		res = CSA_E_CALLBACK_NOT_REGISTERED;
		return (&res);
	}

	/* update registration info */
	if (debug) {
		fprintf(stderr, "%s registered on %s, old types = %d\n",
			user, args->cal, rinfo->types);
	}

	/* registered bits are cleared, unregistered bits are ignored */
	rinfo->types = (rinfo->types | args->update_type) ^ args->update_type;

	if (debug) {
		fprintf(stderr, "%s unregistered types %d on %s, new types = %d\n",
			user, args->update_type, args->cal, rinfo->types);
	}

	if (rinfo->types == 0) {
		cal->rlist = _DtCmsRemoveRegistration(cal->rlist, rinfo);
	}

	res = CSA_SUCCESS;
	return (&res);
}

extern  cms_enumerate_calendar_attr_res *
cms_enumerate_calendar_attr_5_svc(buffer *args, struct svc_req *svcrq)
{
	static cms_enumerate_calendar_attr_res	res;
	_DtCmsCalendar		*cal;
	char			*user;
	uint			access;


	if (debug)
		fprintf(stderr, "cms_enumerate_calendar_attr_5_svc called\n");

	if (res.num_names > 0) {
		_DtCmsFreeCmsAttrNames(res.num_names, res.names);
		res.num_names = 0;
	}

	if ((res.stat = _DtCmsV5LoadAndCheckAccess(svcrq, *args, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		if (!_DTCMS_HAS_VIEW_CALENDAR_ATTR_ACCESS(access)) {
			res.stat = CSA_E_NO_AUTHORITY;
			return (&res);
		}

		res.stat = _DtCmsGetCalAttrNames(cal, &res.num_names,
				&res.names);
	} else {
		res.stat = _DtCmsGetOldCalAttrNames(cal, &res.num_names,
				&res.names);
	}

	return (&res);
}

extern  cms_get_cal_attr_res *
cms_get_calendar_attr_5_svc(cms_get_cal_attr_args *args, struct svc_req *svcrq)
{
	static cms_get_cal_attr_res	res;
	_DtCmsCalendar		*cal;
	char			*user;
	uint			access;

	if (debug)
		fprintf(stderr, "cms_get_calendar_attr_5_svc called\n");

	if (res.num_attrs > 0) {
		_DtCm_free_cms_attributes(res.num_attrs, res.attrs);
		free(res.attrs);
		res.num_attrs = 0;
	}

	if ((res.stat = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !_DTCMS_HAS_VIEW_CALENDAR_ATTR_ACCESS(access)) {
		res.stat = CSA_E_NO_AUTHORITY;
		return (&res);
	}

	if (args->num_names > 0)
		res.stat = _DtCmsGetCalAttrsByName(cal, args->num_names,
				args->names, &res.num_attrs, &res.attrs);
	else
		res.stat = _DtCmsGetAllCalAttrs(cal, &res.num_attrs,
				&res.attrs, B_TRUE);

	return (&res);
}

extern  CSA_return_code *
cms_set_calendar_attr_5_svc(cms_set_cal_attr_args *args, struct svc_req *svcrq)
{
	static CSA_return_code	res;
	_DtCmsCalendar		*cal;
	char			*user;
	uint			access;

	if (debug)
		fprintf(stderr, "cms_set_calendar_attr_5_svc called\n");

	if ((res = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	if ((cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !(access & CSA_OWNER_RIGHTS)) ||
	    (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !(access & (CSA_OWNER_RIGHTS | CSA_INSERT_CALENDAR_ATTRIBUTES |
	    CSA_CHANGE_CALENDAR_ATTRIBUTES)))) {
		res = CSA_E_NO_AUTHORITY;
		return (&res);
	}

	/* check argument */
	if (args->cal == NULL || args->num_attrs == 0) {
		res = CSA_E_INVALID_PARAMETER;
		return (&res);
	}

	/* check validity of attribute values */
	if ((res = _DtCm_check_cal_cms_attributes(
	    (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION ?
	    cal->fversion : _DtCM_FIRST_EXTENSIBLE_DATA_VERSION - 1),
	    args->num_attrs, args->attrs, NULL, NULL, B_TRUE, B_FALSE, B_TRUE))
	    != CSA_SUCCESS)
		return (&res);

	if (cal->fversion >=_DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		if ((res = _DtCmsUpdateCalAttributesAndLog(cal, args->num_attrs,
		    args->attrs, access)) != CSA_SUCCESS)
		{
			return (&res);
		}
	} else {
		int		i;
		Access_Entry_4	*v4list;

		/* for old format file, the only settable calendar attribute
		 * is access list
		 */
		for (i = args->num_attrs - 1; i >= 0; i--) {
			if (args->attrs[i].name.name)
				break;
		}

		if (args->attrs[i].value == NULL ||
		    args->attrs[i].value->item.access_list_value == NULL) {
			res = _DtCmsSetV4AccessListAndLog(cal, NULL);
		} else {
			if ((res = _DtCmsCmsAccessToV4Access(
			    args->attrs[i].value->item.access_list_value,
			    &v4list)) == CSA_SUCCESS)
				res = _DtCmsSetV4AccessListAndLog(cal, v4list);
		}

		if (res != CSA_SUCCESS)
			return (&res);
	}

	/* do callback */
	cal->rlist = _DtCmsDoUpdateCalAttrsCallback(cal->rlist, cal->calendar,
			user, args->num_attrs, args->attrs, args->pid);

	/* reply */
	res = CSA_SUCCESS;

	return (&res);
}

extern  cms_archive_res *
cms_archive_5_svc(cms_archive_args *args, struct svc_req *svcrq)
{
	static cms_archive_res res;

	if (debug)
		fprintf(stderr, "cms_archive_5_svc called\n");

	res.stat = CSA_E_NOT_SUPPORTED;
	return (&res);
}

extern  CSA_return_code *
cms_restore_5_svc(cms_restore_args *args, struct svc_req *svcrq)
{
	static CSA_return_code res;

	if (debug)
		fprintf(stderr, "cms_restore_5_svc called\n");

	res = CSA_E_NOT_SUPPORTED;
	return (&res);
}

extern  cms_reminder_res *
cms_lookup_reminder_5_svc(cms_reminder_args *args, struct svc_req *svcrq)
{
	static cms_reminder_res res;
	_DtCmsCalendar		*cal;
	char			*user;
	uint			access;

	if (debug)
		fprintf(stderr, "cms_lookup_reminder_5_svc called\n");

	if (res.rems != NULL) {
		_DtCmsFreeReminderRef(res.rems);
		res.rems = NULL;
	}

	if ((res.stat = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	if ((access & CSA_OWNER_RIGHTS) == 0) {
		res.stat = CSA_E_NO_AUTHORITY;
		return (&res);
	}

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		res.stat = _DtCmsLookupReminder(cal->remq, args->tick,
				args->num_names, args->names, &res.rems);
	} else {
		Reminder_4	*v4rem;
		_DtCmsEntryId	*ids;

		if ((res.stat = _DtCmsGetV4Reminders(cal, time(0), &v4rem,
		    &ids)) == CSA_SUCCESS) {
			res.stat = _DtCmsV4ReminderToReminderRef(cal->calendar,
					v4rem, ids, &res.rems);
			_DtCm_free_reminder4(v4rem);
			_DtCmsFreeEntryIds(ids);
		}
	}

	return (&res);
}

extern  cms_entries_res *
cms_lookup_entries_5_svc(cms_lookup_entries_args *args, struct svc_req *svcrq)
{
	static cms_entries_res	res;
	_DtCmsCalendar		*cal;
	char			*user;
	uint			access;
	time_t			start1, start2, end1, end2;
	long                    id = 0;
	boolean_t		no_match;
	boolean_t		no_start_time_range, no_end_time_range;
	CSA_uint32		hnum;
	cms_attribute		*hattrs;
	CSA_enum		*hops;

	if (debug)
		fprintf(stderr, "cms_lookup_entries_5_svc called\n");

	if (res.entries) {
		_DtCm_free_cms_entries(res.entries);
		res.entries = NULL;
	}

	if ((res.stat = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !_DTCMS_HAS_VIEW_ACCESS(access)) {
		res.stat = CSA_E_NO_AUTHORITY;
		return (&res);
	}

	/* check operator */
	if ((res.stat = _DtCm_check_operator(args->num_attrs, NULL,
	    args->attrs, args->ops)) != CSA_SUCCESS)
		return (&res);

	/* get time range */
	if ((res.stat = _DtCmHashCriteria(
	    cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION ?
	    _DtCm_entry_name_tbl : cal->entry_tbl, args->num_attrs,
	    NULL, args->attrs, args->ops, &no_match, &no_start_time_range,
	    &no_end_time_range, &start1, &start2, &end1, &end2, &id,
	    &hnum, &hattrs, &hops)) == CSA_E_INVALID_ATTRIBUTE) {

		/* attribute not defined in this calendar specified,
		 * return NULL list
		 */
		res.stat = CSA_SUCCESS;
		return (&res);

	} else if (res.stat != CSA_SUCCESS || no_match == B_TRUE)
		return (&res);

	/* do lookup */
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		if (id > 0)
			res.stat = _DtCmsLookupEntriesById(cal, user, access,
					no_start_time_range, no_end_time_range,
					start1, start2, end1, end2, id, hnum,
					hattrs, hops, &res.entries);
		else
			res.stat = _DtCmsLookupEntries(cal, user, access,
					start1, start2, no_end_time_range,
					end1, end2, hnum, hattrs, hops,
					&res.entries);
	} else {
		Range_4	prange;
		Appt_4	*appt = NULL;;

		if (id > 0) {
			res.stat = _DtCmsLookupKeyrangeV4(cal, user, access,
					no_start_time_range, no_end_time_range,
					start1, start2, end1, end2, id,
					_DtCm_match_one_appt, hnum, hattrs,
					hops, &appt, NULL);
		} else {
			prange.key1 = start1;
			prange.key2 = start2;
			prange.next = NULL;
			res.stat = _DtCmsLookupRangeV4(cal, user, access,
					&prange, no_end_time_range, end1, end2,
					_DtCm_match_one_appt, hnum,
					hattrs, hops, &appt, NULL);
		}

		if (res.stat == CSA_SUCCESS && appt) {
			res.stat = _DtCmsAppt4ToCmsentriesForClient(args->cal,
					appt, &res.entries);
			_DtCm_free_appt4(appt);
		}
	}

	_DtCmFreeHashedArrays(hnum, hattrs, hops);

	return (&res);
}

extern  cms_entries_res *
cms_enumerate_sequence_5_svc(cms_enumerate_args *args, struct svc_req *svcrq)
{
	static cms_entries_res	res;
	_DtCmsCalendar		*cal;
	char			*user;
	uint			access;

	if (debug)
		fprintf(stderr, "cms_enumerate_sequence_5_svc called\n");

	if (res.entries) {
		_DtCm_free_cms_entries(res.entries);
		res.entries = NULL;
	}

	if ((res.stat = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !_DTCMS_HAS_VIEW_ACCESS(access)) {
		res.stat = CSA_E_NO_AUTHORITY;
		return (&res);
	}

	/* do lookup */
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		res.stat = _DtCmsEnumerateSequenceById(cal, user, access,
				B_FALSE, B_TRUE, args->start, args->end,
				0, 0, args->id, 0, NULL, NULL, &res.entries);
	} else {
		Range_4 prange;
		Appt_4 *appt = NULL;
		res.stat = _DtCmsLookupKeyrangeV4(cal, user, access,
				B_FALSE, B_TRUE, args->start, args->end,
				0, 0, args->id, NULL, 0, NULL, NULL, &appt,
				NULL);

		if (res.stat == CSA_SUCCESS && appt) {
			res.stat = _DtCmsAppt4ToCmsentriesForClient(args->cal,
					appt, &res.entries);
			_DtCm_free_appt4(appt);
		}
	}

	return (&res);
}

extern  cms_get_entry_attr_res *
cms_get_entry_attr_5_svc(cms_get_entry_attr_args *args, struct svc_req *svcrq)
{
	static cms_get_entry_attr_res	res;
	_DtCmsCalendar		*cal;
	char			*user;
	uint			access;

	if (debug)
		fprintf(stderr, "cms_get_entry_attr_5_svc called\n");

	if (res.entries) {
		_DtCmsFreeEntryAttrResItem(res.entries);
		res.entries = NULL;
	}

	if ((res.stat = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !_DTCMS_HAS_VIEW_ACCESS(access)) {
		res.stat = CSA_E_NO_AUTHORITY;
		return (&res);
	}

	/* check operator */
	if (args->num_keys == 0) {
		res.stat = CSA_E_INVALID_PARAMETER;
		return (&res);
	}

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		res.stat = _DtCmsLookupEntriesByKey(cal, user, access,
			args->num_keys, args->keys, args->num_names,
			args->names, &res.entries);
	} else {
		res.stat = CSA_E_NOT_SUPPORTED;
	}

	return (&res);
}

extern  cms_entry_res *
cms_insert_entry_5_svc(cms_insert_args *args, struct svc_req *svcrq)
{
	static cms_entry_res	res;
	_DtCmsCalendar		*cal;
	cms_entry		*entry;
	cms_key			key;
	char			*user;
	uint			access, needaccess;
	Appt_4			*appt;

	if (debug)
		fprintf(stderr, "cms_insert_entry_5_svc called\n");

	if (res.entry != NULL) {
		res.entry->num_attrs--;
		_DtCm_free_cms_entry(res.entry);
		res.entry = NULL;
	}

	if ((res.stat = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	if ((cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !_DTCMS_HAS_INSERT_ACCESS(access)) ||
	    (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !_DTCMS_HAS_V4_WRITE_ACCESS(access))) {
		res.stat = CSA_E_NO_AUTHORITY;
		return (&res);
	}

	/* check argument */
	if (args->cal == NULL || args->num_attrs == 0) {
		res.stat = CSA_E_INVALID_PARAMETER;
		return (&res);
	}

	/* check validity of attribute values */
	if ((res.stat = _DtCm_check_entry_cms_attributes(
	    (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION ?
	    cal->fversion : _DtCM_FIRST_EXTENSIBLE_DATA_VERSION - 1),
	    args->num_attrs, args->attrs, CSA_CB_ENTRY_ADDED, B_TRUE))
	    != CSA_SUCCESS)
		return (&res);

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		if ((res.stat = _DtCmsMakeHashedEntry(cal, args->num_attrs,
		    args->attrs, &entry)) != CSA_SUCCESS)
			return (&res);

		if ((res.stat = _DtCmsCheckInitialAttributes(entry))
		    != CSA_SUCCESS) {
			_DtCm_free_cms_entry(entry);
			return (&res);
		}

		/* check access rights */
		needaccess = _DtCmsClassToInsertAccess(entry);
		if ((access & (CSA_OWNER_RIGHTS | needaccess)) == 0) {
			_DtCm_free_cms_entry(entry);
			res.stat = CSA_E_NO_AUTHORITY;
			return (&res);
		}

		/* set organizer */
		if ((res.stat = _DtCm_set_string_attrval(user,
		    &entry->attrs[CSA_ENTRY_ATTR_ORGANIZER_I].value,
		    CSA_VALUE_CALENDAR_USER)) != CSA_SUCCESS) {
			_DtCm_free_cms_entry(entry);
			return (&res);
		}

		/* insert entry and log it */
		if ((res.stat = _DtCmsInsertEntryAndLog(cal, entry))
		    != CSA_SUCCESS) {
			_DtCm_free_cms_entry(entry);
			return (&res);
		}

		key = entry->key;
	} else {
		if ((appt = _DtCm_make_appt4(B_TRUE)) == NULL) {
			res.stat = CSA_E_INSUFFICIENT_MEMORY;
			return (&res);
		}
 
		if ((res.stat = _DtCmsAttrsToAppt4(args->num_attrs,
		    args->attrs, appt, B_TRUE)) != CSA_SUCCESS) {
			_DtCm_free_appt4(appt);
			return (&res);
		}

		if (appt->author) free(appt->author);
		if ((appt->author = strdup(user)) == NULL) {
			_DtCm_free_appt4(appt);
			return (&res);
		}

		/*
		 * calculate the correct start day,
		 */
		_DtCms_adjust_appt_startdate(appt);

		if ((res.stat = _DtCmsInsertApptAndLog(cal, appt))
		    != CSA_SUCCESS) {
			_DtCm_free_appt4(appt);
			return (&res);
		}

		key.id = appt->appt_id.key;
		key.time = appt->appt_id.tick;
	}

	if (res.stat == CSA_SUCCESS)
		cal->modified = B_TRUE;
	else
		return (&res);

	/* do callback */
	cal->rlist = _DtCmsDoV1CbForV4Data(cal->rlist, user, args->pid,
			&key, NULL);

	cal->rlist = _DtCmsDoInsertEntryCallback(cal->rlist, cal->calendar,
			user, key.id, args->pid);

	/* reply */
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION ||
	    (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    (res.stat = _DtCmsAppt4ToCmsentriesForClient(cal->calendar, appt,
	    &entry)) == CSA_SUCCESS)) {

		res.stat = _DtCmsGetCmsEntryForClient(entry, &res.entry, B_FALSE);

		_DtCm_free_cms_entry(entry);
	}

	return (&res);
}

extern  cms_entry_res *
cms_update_entry_5_svc(cms_update_args *args, struct svc_req *svcrq)
{
	static cms_entry_res	res;
	_DtCmsCalendar		*cal;
	cms_entry		*newentry;
	caddr_t			event;
	char			*user;
	uint			access, needaccess;
	Appt_4			*appt = NULL;

	if (debug)
		fprintf(stderr, "cms_update_entry_5_svc called\n");

	if (res.entry != NULL) {
		res.entry->num_attrs--;
		_DtCm_free_cms_entry(res.entry);
		res.entry = NULL;
	}

	if ((res.stat = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	if ((cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !_DTCMS_HAS_CHANGE_ACCESS(access)) ||
	    (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !_DTCMS_HAS_V4_WRITE_ACCESS(access))) {
		res.stat = CSA_E_NO_AUTHORITY;
		return (&res);
	}

	/* check argument */
	if (args->cal == NULL || args->entry.id <= 0 || args->num_attrs == 0) {
		res.stat = CSA_E_INVALID_PARAMETER;
		return (&res);
	}

	if (args->scope < CSA_SCOPE_ALL || args->scope > CSA_SCOPE_FORWARD) {
		res.stat = CSA_E_INVALID_ENUM;
		return (&res);
	}

	/* check validity of attribute values */
	if ((res.stat = _DtCm_check_entry_cms_attributes(
	    (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION ?
	    cal->fversion : _DtCM_FIRST_EXTENSIBLE_DATA_VERSION - 1),
	    args->num_attrs, args->attrs, CSA_CB_ENTRY_UPDATED, B_TRUE))
	    != CSA_SUCCESS)
		return (&res);

	/* get event from one-time event tree */
	event = (caddr_t)rb_lookup(cal->tree, (caddr_t)&args->entry);

	/* update entry and log it */
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		if (event != NULL || args->scope == CSA_SCOPE_ALL) {
			res.stat = _DtCmsUpdateEntry(cal, user, access,
					&args->entry, args->num_attrs,
					args->attrs, NULL, &newentry);
		} else {
			res.stat = _DtCmsUpdateInstances(cal, user, access,
					&args->entry, args->scope,
					args->num_attrs, args->attrs, NULL,
					&newentry);
		}
	} else {
		Options_4	opt;

		if (event != NULL || args->scope == CSA_SCOPE_ALL)
			opt = do_all_4;
		else
			opt = (args->scope == CSA_SCOPE_ONE) ?
				do_one_4 : do_forward_4;

		if (event == NULL && (event = hc_lookup(cal->list,
		    (caddr_t)&args->entry)) == NULL) {

			res.stat = CSA_X_DT_E_ENTRY_NOT_FOUND;

		} else if ((appt = _DtCm_copy_one_appt4((Appt_4 *)event))
		    == NULL) {

			res.stat = CSA_E_INSUFFICIENT_MEMORY; 

		} else {
			/* get rid of exceptions */
			_DtCm_free_excpt4(appt->exception);
			appt->exception = NULL;

			if ((res.stat = _DtCmsAttrsToAppt4(args->num_attrs,
			    args->attrs, appt, B_TRUE)) == CSA_SUCCESS) {

				if (opt == do_all_4)
					res.stat = _DtCmsChangeAll(cal, user,
						access, (Id_4 *)&args->entry,
						appt, NULL);
				else
					res.stat = _DtCmsChangeSome(cal, user,
						access, (Id_4 *)&args->entry,
						appt, opt, NULL);

			}
		}
	}

	if (res.stat == CSA_SUCCESS)
		cal->modified = B_TRUE;
	else
		return (&res);

	/* do callback */
	cal->rlist = _DtCmsDoV1CbForV4Data(cal->rlist, user, args->pid,
			&args->entry,
			(appt ? (cms_key *)&appt->appt_id : &newentry->key));

	cal->rlist = _DtCmsDoUpdateEntryCallback(cal->rlist, cal->calendar,
			user, (appt ? appt->appt_id.key : newentry->key.id),
			args->entry.id, args->scope, args->entry.time,
			args->pid);

	/* reply */
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION ||
	    (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    (res.stat = _DtCmsAppt4ToCmsentriesForClient(cal->calendar, appt,
	    &newentry)) == CSA_SUCCESS)) {

		res.stat = _DtCmsGetCmsEntryForClient(newentry, &res.entry, B_FALSE);

		_DtCm_free_cms_entry(newentry);
	}

	return (&res);
}

extern  CSA_return_code *
cms_delete_entry_5_svc(cms_delete_args *args, struct svc_req *svcrq)
{
	static CSA_return_code	res;
	_DtCmsCalendar		*cal;
	caddr_t			event;
	char			*user;
	uint			access, needaccess;

	if (debug)
		fprintf(stderr, "cms_delete_entry_5_svc called\n");

	if ((res = _DtCmsV5LoadAndCheckAccess(svcrq, args->cal, &user,
	    &access, &cal)) != CSA_SUCCESS)
		return (&res);

	/* for v3 data, authority check is done in the routines doing the
	 * deletion since we may need to check the organizer
	 */
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
	    !_DTCMS_HAS_CHANGE_ACCESS(access)) {
		res = CSA_E_NO_AUTHORITY;
		return (&res);
	}

	/* check argument */
	if (args->cal == NULL || args->entry.id <= 0) {
		res = CSA_E_INVALID_PARAMETER;
		return (&res);
	}

	if (args->scope < CSA_SCOPE_ALL || args->scope > CSA_SCOPE_FORWARD) {
		res = CSA_E_INVALID_ENUM;
		return (&res);
	}


	/* get event from one-time event tree */
	event = (caddr_t)rb_lookup(cal->tree, (caddr_t)&args->entry);

	/* delete entry and log it */
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		if (event != NULL || args->scope == CSA_SCOPE_ALL) {
			res = _DtCmsDeleteEntryAndLog(cal, user, access,
				&args->entry, NULL);
		} else {
			res = _DtCmsDeleteInstancesAndLog(cal, user, access,
				&args->entry, args->scope, NULL, NULL);
		}
	} else {
		if (event != NULL || args->scope == CSA_SCOPE_ALL) {
			res = _DtCmsDeleteApptAndLog(cal, user, access,
				(Id_4 *)&args->entry, NULL);
		} else {
			res = _DtCmsDeleteApptInstancesAndLog(cal, user, access,
				(Id_4 *)&args->entry,
				((args->scope == CSA_SCOPE_ONE) ? do_one_4 :
				do_forward_4), NULL, NULL);
		}
	}

	if (res == CSA_SUCCESS)
		cal->modified = B_TRUE;
	else
		return (&res);

	/* do callback */
	cal->rlist = _DtCmsDoV1CbForV4Data(cal->rlist, user, args->pid,
			&args->entry, NULL);

	cal->rlist = _DtCmsDoDeleteEntryCallback(cal->rlist, cal->calendar,
			user, args->entry.id, args->scope, args->entry.time,
			args->pid);


	return (&res);
}

void initfunctable(program_handle ph)
{
        ph->program_num = TABLEPROG;
        ph->prog[TABLEVERS].vers = &tableprog_5_table[0];
        ph->prog[TABLEVERS].nproc = sizeof(tableprog_5_table)/sizeof(tableprog_5_table[0]);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

static CSA_return_code
_DtCmsCreateCallog(char *user, cms_create_args *args, _DtCmsCalendar **newcal)
{
	CSA_return_code	stat;
	_DtCmsCalendar	*cal;
	int		i, index;
	char		datestr[80];
	char		*calname;
	char		*log;
	char		*username;
	cms_attribute_value val;
	cms_access_entry aentry;
	int		nidx = 0, oidx = 0;
	char		*name, *owner;

	/*
	 * if calendar name is a user name, make sure that
	 * it's the same as the sender.
	 */
	calname = _DtCmGetPrefix(args->cal, '@');
	username = _DtCmGetPrefix(user, '@');

	if (_DtCmIsUserName(calname) && strcmp(calname, username)) {
		free(calname);
		free(username);
		return (CSA_E_NO_AUTHORITY);
	}
	log = _DtCmsGetLogFN(calname);
	free(calname);
	free(username);

	/* create internal calendar data structure */
	if ((cal = _DtCmsMakeCalendar(user, args->cal)) == NULL) {
		free(log);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	/* fill in information */
	_csa_tick_to_iso8601(time(0), datestr);

	if ((stat = _DtCm_set_string_attrval(datestr,
	    &cal->attrs[CSA_CAL_ATTR_DATE_CREATED_I].value,
	    CSA_VALUE_DATE_TIME)) != CSA_SUCCESS) {
		_DtCmsFreeCalendar(cal);
		free(log);
		return (stat);
	}

	/* initialize access list to be "WORLD", VIEW_PUBLIC */
	aentry.user = WORLD;
	aentry.rights = CSA_VIEW_PUBLIC_ENTRIES;
	aentry.next = NULL;
	val.item.access_list_value = &aentry;
	val.type = CSA_VALUE_ACCESS_LIST;

	if ((stat = _DtCmUpdateAccessListAttrVal(&val,
	    &cal->attrs[CSA_CAL_ATTR_ACCESS_LIST_I].value)) != CSA_SUCCESS) {
		_DtCmsFreeCalendar(cal);
		free(log);
		return (stat);
	}

	/* set product identifier */
	if ((stat = _DtCm_set_string_attrval(_DtCM_PRODUCT_IDENTIFIER,
	    &cal->attrs[CSA_CAL_ATTR_PRODUCT_IDENTIFIER_I].value,
	    CSA_VALUE_STRING)) != CSA_SUCCESS) {
		_DtCmsFreeCalendar(cal);
		free(log);
		return (stat);
	}

	/* set CSA version */
	if ((stat = _DtCm_set_string_attrval(_DtCM_SPEC_VERSION_SUPPORTED,
	    &cal->attrs[CSA_CAL_ATTR_VERSION_I].value, CSA_VALUE_STRING))
	    != CSA_SUCCESS) {
		_DtCmsFreeCalendar(cal);
		free(log);
		return (stat);
	}

	/* we dont use the values specified by client */
	for (i = 0; i < args->num_attrs; i++) {
		if (strcmp(args->attrs[i].name.name,
		    CSA_CAL_ATTR_CALENDAR_NAME) == 0) {
			nidx = i;
			name = args->attrs[i].name.name;
			args->attrs[i].name.name = NULL;
		} else if (strcmp(args->attrs[i].name.name,
		    CSA_CAL_ATTR_CALENDAR_OWNER) == 0) {
			oidx = i;
			owner = args->attrs[i].name.name;
			args->attrs[i].name.name = NULL;
		}
	}

	/* initialize calendar attribute with info provided by caller */
	if ((stat = _DtCmUpdateAttributes(args->num_attrs, args->attrs,
	    &cal->num_attrs, &cal->attrs, &cal->cal_tbl, B_TRUE,
	    NULL, B_FALSE)) != CSA_SUCCESS) {
		_DtCmsFreeCalendar(cal);
		free(log);
		return (stat);
	}

	if (nidx) args->attrs[nidx].name.name = name;
	if (oidx) args->attrs[oidx].name.name = owner;

	/* use passed in char set if client does not supply one */
	if (cal->attrs[CSA_CAL_ATTR_CHARACTER_SET_I].value == NULL &&
	     args->char_set && *args->char_set != '\0') {
		if ((stat = _DtCm_set_string_attrval(args->char_set,
		    &cal->attrs[CSA_CAL_ATTR_CHARACTER_SET_I].value,
		    CSA_VALUE_STRING)) != CSA_SUCCESS) {
			_DtCmsFreeCalendar(cal);
			free(log);
			return (stat);
		}
	}

	/* create file */
	if ((stat = _DtCmsCreateLogV2(user, log)) != CSA_SUCCESS) {
		_DtCmsFreeCalendar(cal);
		free(log);
		return (stat);
	}

	/* dump file */
	if ((stat = _DtCmsAppendCalAttrsByFN(log, cal->num_attrs, cal->attrs))
	    != CSA_SUCCESS) {
		free(log);
		unlink(log);
		_DtCmsFreeCalendar(cal);
		return (stat);
	}
	free(log);

	_DtCmsPutInCalList(cal);

	*newcal = cal;

	return (stat);
}

#define	_NAME_INCREMENT	10

static CSA_return_code
_ListCalendarNames(uint *num_names, char ***names)
{
	uint	num = 0, count = 0;
	char	**names_r = NULL;
	DIR	*dirp;
	struct	dirent *dp;

	if ((dirp = opendir(".")) == NULL)
		return (CSA_E_FAILURE);

#ifdef SunOS
	if ((dp = (struct dirent *)malloc(sizeof(struct dirent) + FILENAME_MAX))
	    == NULL) {
		closedir(dirp);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	while (dp = readdir_r(dirp, dp)) {
#else
	while (dp = readdir(dirp)) {
#endif
		if (strncmp(dp->d_name, "callog.", strlen("callog.")) == 0) {
			if (count == num) {
				count += _NAME_INCREMENT;
				if ((names_r = (char **)_grow_char_array(
				    names_r, sizeof(char *) * count,
				    sizeof(char *) * (count + _NAME_INCREMENT)))
				    == NULL) {
					if (num > 0) {
						_free_char_array(
							num, names_r);
					}
					closedir(dirp);
					return (CSA_E_INSUFFICIENT_MEMORY);
				}
			}
			if ((names_r[num++] = strdup(dp->d_name)) == NULL) {
				_free_char_array(num, names_r);
				closedir(dirp);
				return (CSA_E_INSUFFICIENT_MEMORY);
			}
		}
	}

	if (num > 0) {
		*num_names = num;
		*names = names_r;
	} else if (count > 0)
		free(names_r);

	return (CSA_SUCCESS);
}

static void *
_grow_char_array(void *ptr, uint oldcount, uint newcount)
{
	void *nptr;

	if (nptr = realloc(ptr, newcount)) {
		memset((void *)((char *)nptr + oldcount), 0,
			newcount - oldcount);
		return (nptr);
	} else
		return (NULL);
}

static void
_free_char_array(uint num_elem, char **ptr)
{
	int i;

	if (num_elem == 0)
		return;

	for (i = 0; i < num_elem; i++) {
		if (ptr[i])
			free(ptr[i]);
		else
			break;
	}

	free(ptr);
}

/*
 * This routine assumes that the attributes are hashed already
 */
static CSA_return_code
_DtCmsGetOldCalAttrNames(
	_DtCmsCalendar	*cal,
	uint		*num_names_r,
	cms_attr_name	**names_r)
{
	CSA_return_code	stat = CSA_SUCCESS;
	cms_attr_name	*names;
	uint		i, j;

	if ((names = (cms_attr_name *)calloc(1,
	    sizeof(cms_attr_name)*_DtCM_OLD_CAL_ATTR_SIZE)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	for (i = 1, j = 0; i <= _DtCM_DEFINED_CAL_ATTR_SIZE; i++) {
		if ((_CSA_cal_attr_info[i].fst_vers > 0 &&
		    _CSA_cal_attr_info[i].fst_vers <= cal->fversion)
		    || i == CSA_CAL_ATTR_CALENDAR_SIZE_I) {
			if ((names[j].name =
			    strdup(_CSA_calendar_attribute_names[i])) == NULL)
			{
				_DtCmsFreeCmsAttrNames(j, names);
				return (CSA_E_INSUFFICIENT_MEMORY);
			} else {
				names[j].num = i;
				j++;
			}
		}
	}

	*num_names_r = j;
	*names_r = names;

	return (CSA_SUCCESS);
}

