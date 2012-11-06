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
/* $XConsortium: calendar.c /main/1 1996/04/21 19:21:47 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

/*****************************************************************************
 * Functions that manage the calendar data structures.
 *****************************************************************************/

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include "calendar.h"
#include "entry.h"
#include "cmsdata.h"
#include "agent_p.h"
#include "rpccalls.h"
#include "debug.h"
#include "attr.h"
#include "xtclient.h"
#include "misc.h"
#include "free.h"
#include "nametbl.h"
#include "lutil.h"

/* linked list of calendar structure representing opened calendars */
Calendar *_DtCm_active_cal_list = NULL;

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/
static CSA_return_code _get_owner_from_old_cal(Calendar *cal, char *buf);
static CSA_return_code _get_calendar_owner(Calendar *cal, uint num_names,
					char **names, CSA_attribute *attr);
static CSA_return_code _get_calendar_name(Calendar *cal, CSA_attribute *attr);
static CSA_return_code _get_product_identifier(Calendar *cal,
						CSA_attribute *attr);
static CSA_return_code _get_version_supported(Calendar *cal,
						CSA_attribute *attr);
static CSA_return_code _get_server_version(Calendar *cal, CSA_attribute *attr);
static CSA_return_code _get_data_version(Calendar *cal, CSA_attribute *attr);
static CSA_return_code _get_access_list(Calendar *cal, uint num_names,
					char ** names, CSA_attribute *attr);
static CSA_return_code _get_number_entries(Calendar *cal, uint num_names,
					char ** names, CSA_attribute *attr);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

/*
 * Allocate a calendar structure and initialize it with
 * the location, and name of the calendar.
 */
extern Calendar *
_DtCm_new_Calendar(const char *calendar)
{
	Calendar	*cal;
	int i;

	_DtCm_init_hash();

	if ((cal = (Calendar *)malloc(sizeof(Calendar))) == NULL)
		return(NULL);

	memset((void *)cal, 0, sizeof(Calendar));

	if ((cal->name = strdup(calendar)) == NULL) {
		free(cal);
		return(NULL);
	}

	cal->location = strchr(cal->name, '@');
	cal->location++;

	/* set up attribute array */
	if ((cal->attrs = (cms_attribute *)calloc(1,
	    sizeof(cms_attribute) * (_DtCm_cal_name_tbl->size + 1))) == NULL) {
		free(cal->name);
		free(cal);
		return(NULL);
	}

	for (i = 1; i <= _DtCm_cal_name_tbl->size; i++) {
		if ((cal->attrs[i].name.name =
		    strdup(_DtCm_cal_name_tbl->names[i])) == NULL) {
			/* clean up */
			cal->num_attrs = i - 1;
			_DtCm_free_Calendar(cal);
			return(NULL);
		} else
			cal->attrs[i].name.num = i;
	}
	cal->num_attrs = _DtCm_cal_name_tbl->size;

	if (_DtCm_active_cal_list == NULL) {
		_DtCm_active_cal_list = cal;
		cal->next = NULL;
		cal->prev = NULL;
	}
	else {
		cal->next = _DtCm_active_cal_list;
		_DtCm_active_cal_list->prev = cal;
		_DtCm_active_cal_list = cal;
	}

	cal->cal_tbl = _DtCm_cal_name_tbl;
	cal->entry_tbl = _DtCm_entry_name_tbl;

	cal->handle = (void *)cal;

	return(cal);
}

/*
 * After freeing up memory pointed to by cal,
 * put it in the free list.
 */
extern void
_DtCm_free_Calendar(Calendar *cal)
{
	_DtCmCallbackEntry	*cb, *ptr;

	/* remove from active list */
	if (_DtCm_active_cal_list == cal)
		_DtCm_active_cal_list = cal->next;

	if (cal->prev != NULL)
		cal->prev->next = cal->next;

	if (cal->next != NULL)
		cal->next->prev = cal->prev;

	/* free up resources used by it */
	if (cal->name) free(cal->name);
	if (cal->ehead) _DtCm_free_libentries((_DtCm_libentry *)cal->ehead);

	if (cal->cal_tbl != _DtCm_cal_name_tbl)
		_DtCm_free_name_table(cal->cal_tbl);

	if (cal->entry_tbl != _DtCm_entry_name_tbl)
		_DtCm_free_name_table(cal->entry_tbl);

	cb = cal->cb_list;
	while (cb) {
		ptr = cb->next;
		free(cb);
		cb = ptr;
	}

	/*
	 * no need to free attribute values now
	 * since the values are passed to client
	 * directly.
	 * need to free attribute values if they
	 * are cached later
	 */
	_DtCm_free_cms_attribute_values(cal->num_attrs, cal->attrs);
	free(cal->attrs);

	memset((void *)cal, '\0', sizeof(Calendar));

	/* if no calendar is open, destroy rpc mapping */
	if (_DtCm_active_cal_list == NULL)
		_DtCm_destroy_agent();
}

/*
 * Given the calendar handle, return the internal calendar data structure.
 */
extern Calendar *
_DtCm_get_Calendar(CSA_session_handle calhandle)
{
	Calendar *cal = (Calendar *)calhandle;

	if (cal == NULL || cal->handle != (void *)cal)
		return (NULL);
	else
		return ((Calendar *)calhandle);
}

/*
 * Add linked list of entry data structures to the calendar.
 */
extern uint
_DtCm_add_to_entry_list(Calendar *cal, caddr_t elist)
{
	int i;
	_DtCm_libentry *entries = (_DtCm_libentry *)elist;

	if (entries == NULL)
		return (0);

	if (cal->ehead == NULL)
		cal->ehead = elist;
	else {
		((_DtCm_libentry *)cal->etail)->next = entries;
		entries->prev = (_DtCm_libentry *)cal->etail;
	}

	/* find tail of given entry list */
	i = 1;
	entries->cal = cal;
	while (entries->next != NULL) {
		i++;
		entries->next->cal = cal;
		entries = entries->next;
	}

	cal->etail = (caddr_t)entries;

#ifdef CM_DEBUG
	_DtCm_count_entry_in_list(cal->ehead);
#endif

	return (i);
}

extern void
_DtCm_remove_from_entry_list(
	Calendar *cal,
	caddr_t head,
	caddr_t tail)
{
	_DtCm_libentry *ehead = (_DtCm_libentry *)head;
	_DtCm_libentry *etail = (_DtCm_libentry *)tail;

	if (ehead == NULL || tail == NULL)
		return;

	if (ehead == (_DtCm_libentry *)cal->ehead)
		cal->ehead = (caddr_t)etail->next;

	if (etail == (_DtCm_libentry *)cal->etail)
		cal->etail = (caddr_t)ehead->prev;

	/* remove from list */
	if (ehead->prev != NULL)
		ehead->prev->next = etail->next;

	if (etail->next != NULL)
		etail->next->prev = ehead->prev;

	etail->next = ehead->prev = NULL;

#ifdef CM_DEBUG
	_DtCm_count_entry_in_list(cal->ehead);
#endif
}

extern void
_DtCm_reset_cal_attrs(Calendar *cal)
{
	cal->got_attrs = B_FALSE;
}

/*
 * Assume good parameters.  Caller should check before calling this.
 */
extern CSA_return_code
_DtCm_list_old_cal_attr_names(
	Calendar *cal,
	CSA_uint32 *num_names_r,
	char **names_r[])
{
	CSA_return_code	stat;
	char		**names, buf[BUFSIZ];
	int		i, j;

	if ((names = _DtCm_alloc_character_pointers(_DtCM_OLD_CAL_ATTR_SIZE))
	    == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	/* find out whether we know the owner of the calendar */
	if ((stat = _get_owner_from_old_cal(cal, buf)) != CSA_SUCCESS)
		return (stat);

	for (i = 1, j = 0; i <= _DtCM_DEFINED_CAL_ATTR_SIZE; i++) {
		if (_CSA_cal_attr_info[i].fst_vers > 0 &&
		    _CSA_cal_attr_info[i].fst_vers <= cal->file_version) {
			if (i == CSA_CAL_ATTR_CALENDAR_OWNER_I && *buf == '\0')
				continue;

			if ((names[j] =
			    strdup(_CSA_calendar_attribute_names[i])) == NULL)
			{
				_DtCm_free(names);
				return (CSA_E_INSUFFICIENT_MEMORY);
			} else
				j++;
		}
	}

	*names_r = names;
	*num_names_r = j;

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCm_get_all_cal_attrs(
	Calendar *cal,
	CSA_uint32 *num_attrs,
	CSA_attribute **attrs)
{
	CSA_return_code	stat;
	int		i, j;
	CSA_attribute	*attrs_r;

	if (num_attrs == NULL || attrs == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if (cal->rpc_version >= _DtCM_FIRST_EXTENSIBLE_SERVER_VERSION) {
		if ((stat = _DtCm_rpc_get_cal_attrs(cal, 0, 0, NULL))
		    != CSA_SUCCESS)
			return (stat);

		if ((attrs_r = _DtCm_alloc_attributes(cal->num_attrs)) == NULL)
			return (CSA_E_INSUFFICIENT_MEMORY);

		/* get attributes */
		for (i = 1, j = 0; i <= cal->num_attrs; i++) {
			if (cal->attrs[i].value != NULL) {

				if ((stat = _DtCm_cms2csa_attribute(
				    cal->attrs[i], &attrs_r[j])) != CSA_SUCCESS)
				{
					_DtCm_free(attrs_r);
					return (stat);
				} else
					j++;
			}
		}

		*num_attrs = j;
		*attrs = attrs_r;

		return (CSA_SUCCESS);
	} else
		return (_DtCm_get_cal_attrs_by_name(cal,
			_DtCM_DEFINED_CAL_ATTR_SIZE + 1,
			_CSA_calendar_attribute_names, num_attrs, attrs));
}

/*
 * If it's not found, the attribute value is set to NULL.
 */
extern CSA_return_code
_DtCm_get_cal_attrs_by_name(
	Calendar *cal,
	CSA_uint32      num_names,
	CSA_attribute_reference *names,
	CSA_uint32      *num_attrs,
	CSA_attribute **attrs)
{
	CSA_return_code	stat = CSA_SUCCESS;
	CSA_attribute *attrs_r;
	int i, j, index;

	if (num_attrs == 0 || attrs == NULL)
		return (CSA_E_INVALID_PARAMETER);


	if ((attrs_r = _DtCm_alloc_attributes(num_names)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	for (i = 0, j = 0; i < num_names; i++) {
		if (names[i] == NULL)
			continue;

		index = _DtCm_get_index_from_table(cal->cal_tbl, names[i]);

		switch (index) {
		case CSA_X_DT_CAL_ATTR_SERVER_VERSION_I:

			stat = _get_server_version(cal, &attrs_r[j]);
			break;

		case CSA_X_DT_CAL_ATTR_DATA_VERSION_I:

			stat = _get_data_version(cal, &attrs_r[j]);
			break;

		case CSA_CAL_ATTR_ACCESS_LIST_I:

			stat = _get_access_list(cal, num_names, names,
				&attrs_r[j]);
			break;

		case CSA_CAL_ATTR_NUMBER_ENTRIES_I:

			stat = _get_number_entries(cal, num_names,
				names, &attrs_r[j]);
			break;

		case CSA_CAL_ATTR_CALENDAR_NAME_I:

			stat = _get_calendar_name(cal, &attrs_r[j]);
			break;

		case CSA_CAL_ATTR_CALENDAR_OWNER_I:

			stat = _get_calendar_owner(cal, num_names, names,
				&attrs_r[j]);
			break;

		case CSA_CAL_ATTR_PRODUCT_IDENTIFIER_I:

			stat = _get_product_identifier(cal, &attrs_r[j]);
			break;

		case CSA_CAL_ATTR_VERSION_I:

			stat = _get_version_supported(cal, &attrs_r[j]);
			break;

		default:
			if (cal->rpc_version >=
			    _DtCM_FIRST_EXTENSIBLE_SERVER_VERSION)
			{
				if (cal->file_version <
				    _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
				    index == -1)
					break;

				if (cal->got_attrs == B_FALSE &&
				    (stat = _DtCm_rpc_get_cal_attrs(cal, 0,
				    num_names, names)) == CSA_SUCCESS) {
					/* there might by new attributes */
					if (index == -1 &&
					    (index = _DtCm_get_index_from_table(
					    cal->cal_tbl, names[i])) == -1)
						break;
				}

				if (stat == CSA_SUCCESS &&
				    cal->attrs[index].value)
					stat = _DtCm_cms2csa_attribute(
						cal->attrs[index], &attrs_r[j]);
			}
			break;
		}

		if (stat == CSA_SUCCESS) {
			if (attrs_r[j].value != NULL)
				j++;
		} else {
			_DtCm_free(attrs_r);
			return (stat);
		}
	}

	*num_attrs = j;

	if (j > 0) {
		*attrs = attrs_r;
	} else {
		*attrs = NULL;
		_DtCm_free(attrs_r);
	}

	return (CSA_SUCCESS);
}

/*
 * Debugging function - count the number of entries in the list and print it out
 */
extern void
_DtCm_count_entry_in_list(caddr_t elist)
{
	int i;
	_DtCm_libentry *list;

	for (i = 0, list = (_DtCm_libentry *)elist;
	    list != NULL;
	    i++, list = list->next)
		;

	fprintf(stderr, "number of entries = %d\n", i);
}

/******************************************************************************
 * static functions used within in the file
 ******************************************************************************/

/*
 * owner should point to a buffer big enough to hold the owner name
 * We test whether the calendar name is a user name, if so, the
 * owner will be the same as the calendar name.
 * Otherwise, we don't know the owner.
 */
static CSA_return_code
_get_owner_from_old_cal(Calendar *cal, char *owner)
{
	char		*calname;

	if ((calname = _DtCmGetPrefix(cal->name, '@')) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if (_DtCmIsUserName(calname) == B_TRUE)
		strcpy(owner, calname);
	else
		*owner = '\0';

	free(calname);
	return (CSA_SUCCESS);
}

static CSA_return_code
_get_calendar_owner(
	Calendar	*cal,
	uint		num_names,
	char		**names,
	CSA_attribute	*attr)
{
	CSA_return_code		stat;
	CSA_attribute_value	*val;
	char			*owner, buf[BUFSIZ];

	if (cal->rpc_version >= _DtCM_FIRST_EXTENSIBLE_SERVER_VERSION) {
		if (cal->got_attrs == B_FALSE) {
			if ((stat = _DtCm_rpc_get_cal_attrs(cal, 0, num_names,
			    names)) != CSA_SUCCESS)
				return (stat);
		}
		owner = cal->attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].value->\
			item.string_value;
	} else {
		if ((stat = _get_owner_from_old_cal(cal, buf)) != CSA_SUCCESS)
			return (stat);
		else if (*buf == '\0')
			return (CSA_SUCCESS);
		else
			owner = buf;
	}

	if (attr->name = strdup(CSA_CAL_ATTR_CALENDAR_OWNER)) {
		if ((val = (CSA_attribute_value *)calloc(1,
		    sizeof(CSA_attribute_value))) == NULL) {
			free(attr->name);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}

		val->type = CSA_VALUE_CALENDAR_USER;

		if ((val->item.calendar_user_value = (CSA_calendar_user *)
		    calloc(1, sizeof(CSA_calendar_user))) == NULL) {
			free(val);
			free(attr->name);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}

		if (val->item.calendar_user_value->user_name = strdup(owner)) {
			attr->value = val;
			return (CSA_SUCCESS);
		} else {
			free(val->item.calendar_user_value);
			free(val);
			free(attr->name);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
	} else
		return (CSA_E_INSUFFICIENT_MEMORY);
}

static CSA_return_code
_get_calendar_name(Calendar *cal, CSA_attribute *attr)
{
	if (attr->name = strdup(CSA_CAL_ATTR_CALENDAR_NAME))
		return (_DtCm_set_csa_string_attrval(cal->name, &attr->value,
			CSA_VALUE_STRING));
	else
		return (CSA_E_INSUFFICIENT_MEMORY);
}

static CSA_return_code
_get_product_identifier(Calendar *cal, CSA_attribute *attr)
{
	if (attr->name = strdup(CSA_CAL_ATTR_PRODUCT_IDENTIFIER))
		return (_DtCm_set_csa_string_attrval(_DtCM_PRODUCT_IDENTIFIER,
			&attr->value, CSA_VALUE_STRING));
	else
		return (CSA_E_INSUFFICIENT_MEMORY);
}

static CSA_return_code
_get_version_supported(Calendar *cal, CSA_attribute *attr)
{
	if (attr->name = strdup(CSA_CAL_ATTR_VERSION))
		return (_DtCm_set_csa_string_attrval(
			_DtCM_SPEC_VERSION_SUPPORTED, &attr->value,
			CSA_VALUE_STRING));
	else
		return (CSA_E_INSUFFICIENT_MEMORY);
}

static CSA_return_code
_get_server_version(Calendar *cal, CSA_attribute *attr)
{
	if (attr->name = strdup(CSA_X_DT_CAL_ATTR_SERVER_VERSION))
		return (_DtCm_set_csa_uint32_attrval(cal->rpc_version,
			&attr->value));
	else
		return (CSA_E_INSUFFICIENT_MEMORY);
}

static CSA_return_code
_get_data_version(Calendar *cal, CSA_attribute *attr)
{
	if (attr->name = strdup(CSA_X_DT_CAL_ATTR_DATA_VERSION))
		return (_DtCm_set_csa_uint32_attrval(cal->file_version,
			&attr->value));
	else
		return (CSA_E_INSUFFICIENT_MEMORY);
}

static CSA_return_code
_get_access_list(
	Calendar	*cal,
	uint		num_names,
	char		**names,
	CSA_attribute	*attr)
{
	CSA_return_code stat = CSA_SUCCESS;

	if (cal->rpc_version < _DtCM_FIRST_EXTENSIBLE_SERVER_VERSION)
		stat = _DtCm_rpc_get_cal_attrs(cal, CSA_CAL_ATTR_ACCESS_LIST_I,
			0, NULL);
	else if (cal->got_attrs == B_FALSE)
		stat = _DtCm_rpc_get_cal_attrs(cal, 0, num_names, names);

	if (stat == CSA_SUCCESS)
		stat = _DtCm_cms2csa_attribute(
			cal->attrs[CSA_CAL_ATTR_ACCESS_LIST_I], attr);

	return (stat);
}

static CSA_return_code
_get_number_entries(
	Calendar	*cal,
	uint		num_names,
	char		**names,
	CSA_attribute	*attr)
{
	CSA_return_code stat = CSA_SUCCESS;

	if (cal->rpc_version < _DtCM_FIRST_EXTENSIBLE_SERVER_VERSION)
		stat = _DtCm_rpc_get_cal_attrs(cal,
			CSA_CAL_ATTR_NUMBER_ENTRIES_I, 0, NULL);
	else if (cal->got_attrs == B_FALSE)
		stat = _DtCm_rpc_get_cal_attrs(cal, 0, num_names, names);

	if (stat == CSA_SUCCESS)
		stat = _DtCm_cms2csa_attribute(
			cal->attrs[CSA_CAL_ATTR_NUMBER_ENTRIES_I], attr);

	return (stat);
}

