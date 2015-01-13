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
/* $XConsortium: cmscalendar.c /main/4 1995/11/09 12:40:39 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <time.h>
#ifdef SunOS
#include <sys/systeminfo.h>
#endif
#include "cmscalendar.h"
#include "access.h"
#include "attr.h"
#include "cm.h"
#include "rtable4.h"
#include "tree.h"
#include "list.h"
#include "log.h"
#include "appt4.h"		/* Internal appointment data structure */
#include "cmsdata.h"
#include "cmsentry.h"
#include "callback.h"
#include "garbage.h"
#include "repeat.h"
#include "v4ops.h"
#include "v5ops.h"
#include "insert.h"
#include "updateattrs.h"
#include "reminder.h"
#include "misc.h"
#include "convert4-5.h"
#include "lutil.h"

extern	int	debug;
extern	char	*pgname;

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/

static CSA_return_code get_file_owner(char *log, char **owner);
static char * get_calname(char *target);
static CSA_return_code init_cal_attrs(_DtCmsCalendar *cal);
static CSA_return_code _GetCalendarSize(_DtCmsCalendar *cal, int index,
					cms_attribute *attr);
static CSA_return_code _GetNumberEntries(_DtCmsCalendar *cal, int index,
					cms_attribute *attr);
static CSA_return_code _GetAccessList(_DtCmsCalendar *cal, int index,
					cms_attribute *attr);
static CSA_return_code _GetCalendarName(_DtCmsCalendar *cal, int index,
					cms_attribute *attr);
static CSA_return_code _GetCalendarOwner(_DtCmsCalendar *cal, int index,
					cms_attribute *attr);
static CSA_return_code _GetServerVersion(_DtCmsCalendar *cal, int index,
					cms_attribute *attr);
static CSA_return_code _GetDataVersion(_DtCmsCalendar *cal, int index,
					cms_attribute *attr);
static CSA_return_code _GetProductId(_DtCmsCalendar *cal, int index,
					cms_attribute *attr);
static CSA_return_code _GetSupportedVersion(_DtCmsCalendar *cal, int index,
					cms_attribute *attr);
static CSA_return_code _CopyCalendarAttr(_DtCmsCalendar *cal, int index,
					cms_attribute *attr);

/* global variable shared with parser.y */
_DtCmsCalendar *currentCalendar;

/* static variables used within the file */
static	_DtCmsCalendar	*calendar_list = NULL;

static _DtCmGetAttrFunc _GetAttrFuncPtrs[] =
	{	NULL,
		_GetAccessList,
		_GetCalendarName,
		_GetCalendarOwner,
		_GetCalendarSize,
		_CopyCalendarAttr,
		_CopyCalendarAttr,
		_CopyCalendarAttr,
		_CopyCalendarAttr,
		_GetNumberEntries,
		_GetProductId,
		_CopyCalendarAttr,
		_GetSupportedVersion,
		_CopyCalendarAttr,
		_GetServerVersion,
		_GetDataVersion
	};

/*****************************************************************************
 * extern functions
 *****************************************************************************/

extern _DtCmsCalendar *
_DtCmsMakeCalendar(char *owner, char *name)
{
	_DtCmsCalendar	*cal;

	if ((cal = (_DtCmsCalendar *)calloc(1, sizeof(_DtCmsCalendar)))
	    == NULL) {
		return (NULL);
	}

	cal->fversion = _DtCMS_VERSION4;

	if (init_cal_attrs(cal) != CSA_SUCCESS) {
		free(cal);
		return (NULL);
	}

	if (_DtCm_set_string_attrval(owner,
	    &cal->attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].value,
	    CSA_VALUE_CALENDAR_USER)) {
		_DtCmsFreeCalendar(cal);
		return (NULL);
	}
	if ((cal->owner = strdup(cal->attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].\
	    value->item.string_value)) == NULL) {
		_DtCmsFreeCalendar(cal);
		return (NULL);
	}

	if (_DtCm_set_string_attrval(name,
	    &cal->attrs[CSA_CAL_ATTR_CALENDAR_NAME_I].value, CSA_VALUE_STRING))
	{
		_DtCmsFreeCalendar(cal);
		return (NULL);
	}
	if ((cal->calendar = get_calname(name)) == NULL) {
		_DtCmsFreeCalendar(cal);
		return (NULL);
	}

	if ((cal->types = (int *)calloc(1, sizeof(int) *
	    (_DtCm_entry_name_tbl->size + 1))) == NULL) {
		_DtCmsFreeCalendar(cal);
		return (NULL);
	} else
		_DtCm_get_attribute_types(_DtCm_entry_name_tbl->size,
			cal->types);

	if (!(cal->tree = rb_create(_DtCmsGetEntryKey, _DtCmsCompareEntry))) {
		_DtCmsFreeCalendar(cal);
		return (NULL);
	}

	if (!(cal->list = hc_create(_DtCmsGetEntryKey, _DtCmsCompareRptEntry)))
	{
		_DtCmsFreeCalendar(cal);
		return (NULL);
	}

	cal->cal_tbl = _DtCm_cal_name_tbl;
	cal->entry_tbl = _DtCm_entry_name_tbl;
	cal->num_entry_attrs = _DtCm_entry_name_tbl->size;
	cal->getattrfuncs = _GetAttrFuncPtrs;

	return (cal);
}

extern void
_DtCmsPutInCalList(_DtCmsCalendar *cal)
{
	/* link with other calendar structures */
	cal->next = calendar_list;
	calendar_list = cal;
}

extern void
_DtCmsFreeCalendar(_DtCmsCalendar *cal)
{
	_DtCmsCalendar *clist, *prev;

	if (cal == NULL) return;

	for (clist = calendar_list, prev = NULL; clist != NULL;
	    clist = clist->next) {

		if (clist == cal) {
			if (prev == NULL)
				calendar_list = clist->next;
			else
				prev->next = clist->next;

			break;
		} else
			prev = clist;
	}

	if (cal->calendar)
		free(cal->calendar);

	if (cal->owner)
		free(cal->owner);

	if (cal->fversion == 1) {
		_DtCm_free_access_list4(GET_R_ACCESS ((cal)));
		_DtCm_free_access_list4(GET_W_ACCESS ((cal)));
		_DtCm_free_access_list4(GET_D_ACCESS ((cal)));
		_DtCm_free_access_list4(GET_X_ACCESS ((cal)));
		_DtCm_free_access_list4(cal->alist);
	} else {
		if (cal->cal_tbl != _DtCm_cal_name_tbl)
			_DtCm_free_name_table(cal->cal_tbl);

		if (cal->entry_tbl != _DtCm_entry_name_tbl)
			_DtCm_free_name_table(cal->entry_tbl);

		if (cal->types) free(cal->types);

		if (cal->num_attrs) {
			_DtCm_free_cms_attributes(cal->num_attrs + 1,
				cal->attrs);
			free(cal->attrs);
		}
	}

	if (cal->tree)
		rb_destroy(cal->tree, NULL);

	if (cal->list)
		hc_destroy(cal->list, NULL);
}

/*
 * Called when loading data from callog file.
 * Types of data to be stored in the tree and list structures
 * depends on the version of the callog file.
 */
extern CSA_return_code
_DtCmsSetFileVersion(_DtCmsCalendar *cal, int version)
{
	cal->fversion = version;

	if (version == 1) {
		if (!(cal->tree = rb_create(_DtCmsGetApptKey,
		    _DtCmsCompareAppt))) {
			return (CSA_E_INSUFFICIENT_MEMORY);
		}

		if (!(cal->list = hc_create(_DtCmsGetApptKey,
		    _DtCmsCompareRptAppt))) {
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
	} else {
		if (!(cal->tree = rb_create(_DtCmsGetEntryKey,
		    _DtCmsCompareEntry))) {
			return (CSA_E_INSUFFICIENT_MEMORY);
		}

		if (!(cal->list = hc_create(_DtCmsGetEntryKey,
		    _DtCmsCompareRptEntry))) {
			return (CSA_E_INSUFFICIENT_MEMORY);
		}

		if (init_cal_attrs(cal) != CSA_SUCCESS)
			return (CSA_E_INSUFFICIENT_MEMORY);
	
		if ((cal->types = (int *)calloc(1, sizeof(int) *
		    (_DtCm_entry_name_tbl->size + 1))) == NULL) {
			_DtCmsFreeCalendar(cal);
			return (0);
		} else
			_DtCm_get_attribute_types(_DtCm_entry_name_tbl->size,
				cal->types);

	}

	cal->cal_tbl = _DtCm_cal_name_tbl;
	cal->entry_tbl = _DtCm_entry_name_tbl;

	return (CSA_SUCCESS);
}

/* return value:
 * 0		- file loaded successfully
 * FILEERROR	- file does not exist
 * CERROR	- something goes wrong
 */
extern CSA_return_code
start_log(_DtCmsCalendar *cal, char *filename)
{
	int	fd;
	FILE	*f;
	char	firstline[80], *numptr;
	int	version;
	struct stat info;
	struct passwd *pw;
	extern void setinput(FILE *);
	extern int yyyparse();
	extern int yyywrap(FILE *);

	if (cal == NULL || filename == NULL)
		return(CSA_E_FAILURE);

	if ((f = fopen(filename, "r")) == NULL) {
		if (errno == ENOENT)
			return(CSA_E_CALENDAR_NOT_EXIST);
		else
			return(CSA_X_DT_E_BACKING_STORE_PROBLEM);
	}

	setinput(f);
	currentCalendar = cal;
	if (yyyparse() == -1) {
		fprintf(stderr, "%s: unable to parse %s\n", pgname, filename);
		yyywrap(f);
		return(CSA_X_DT_E_BACKING_STORE_PROBLEM);
	}
	yyywrap(f);

	return(CSA_SUCCESS);
}

/*
 * Load calendar data into memory from callog file.
 */
extern CSA_return_code
_DtCmsLoadCalendar(char *target, _DtCmsCalendar **infoptr)
{
	CSA_return_code	stat;
	char		*calname;
	char		*log;
	_DtCmsCalendar	*cal = NULL;

	if (target == NULL || infoptr == NULL)
		return (CSA_E_FAILURE);

	*infoptr = NULL;

	if ((calname = get_calname(target)) == NULL)
		return(CSA_E_INSUFFICIENT_MEMORY);

	if ((log = _DtCmsGetLogFN(calname)) == NULL)
	{
		free(calname);
		return(CSA_E_INSUFFICIENT_MEMORY);
	}

	if ((cal = (_DtCmsCalendar *)calloc(1, sizeof(_DtCmsCalendar)))
	    == NULL) {
		stat = CSA_E_INSUFFICIENT_MEMORY;
		goto ERROR;
	}

	if (!(cal->calendar = (char *)strdup(calname))) {
		stat = CSA_E_INSUFFICIENT_MEMORY;
		goto ERROR;
	}

	/* load data from file */
	if ((stat = start_log(cal, log)) != CSA_SUCCESS) {
		goto ERROR;
	}

	/*
	 * get file owner after the file is loaded since file
	 * ownership might be fixed in start_log()
	 */
	if (cal->fversion == _DtCMS_VERSION1) {
		if ((stat = get_file_owner(calname, &cal->owner))
		    != CSA_SUCCESS)
			goto ERROR;

		cal->alist = _DtCmsCalendarAccessList(cal);
	} else {
		if (cal->attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].value)
			cal->owner = strdup(cal->\
					attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].\
					value->item.calendar_user_value);
		else {
			stat = CSA_X_DT_E_BACKING_STORE_PROBLEM;
	    		fprintf(stderr, "%s: %s\n", pgname,
				"calendar owner attribute is missing from callog file");
			goto ERROR;
		}

		if (cal->attrs[CSA_CAL_ATTR_PRODUCT_IDENTIFIER_I].value == NULL)
		{
			if ((stat = _DtCm_set_string_attrval(
			    _DtCM_PRODUCT_IDENTIFIER, &cal->\
			    attrs[CSA_CAL_ATTR_PRODUCT_IDENTIFIER_I].value,
			    CSA_VALUE_STRING)) != CSA_SUCCESS) {
				goto ERROR;
			}
		}

		if (cal->attrs[CSA_CAL_ATTR_VERSION_I].value == NULL) {
			if ((stat = _DtCm_set_string_attrval(
			    _DtCM_SPEC_VERSION_SUPPORTED,
			    &cal->attrs[CSA_CAL_ATTR_VERSION_I].value,
			    CSA_VALUE_STRING)) != CSA_SUCCESS) {
				goto ERROR;
			}
		}

		cal->num_entry_attrs = cal->entry_tbl->size;
	}

	cal->getattrfuncs = _GetAttrFuncPtrs;

	/* link with other calendar structures */
	cal->next = calendar_list;
	calendar_list = cal;

	free(log);
	free(calname);

	*infoptr = cal;
	return (CSA_SUCCESS);
ERROR:
	free(calname);
	free(log);

	_DtCmsFreeCalendar(cal);
	return (stat);
}

extern char *
_DtCmsGetCalendarOwner(char *target)
{
	char *name;
	_DtCmsCalendar *clist = calendar_list;

	if ((name = get_calname(target)) == NULL)
		return(NULL);

	while (clist != NULL) {
		if (strcmp(name, clist->calendar) == 0) {
			free(name);
			return(clist->owner);
		}
		clist = clist->next;
	}
	free(name);
	return(NULL);
}

extern CSA_return_code
_DtCmsGetCalendarByName(char *target, boolean_t load, _DtCmsCalendar **cal)
{
	char	*name;
	_DtCmsCalendar *clist = calendar_list;

	if ((name = get_calname(target)) == NULL)
		return (0);

	while (clist != NULL) {
		if (strcmp (name, clist->calendar) == 0) {
			free (name);
			*cal = clist;
			return (CSA_SUCCESS);
		}
		clist = clist->next;
	}
	free (name);

	if (load == B_TRUE)
		return (_DtCmsLoadCalendar(target, cal));
	else {
		*cal = NULL;
		return (CSA_SUCCESS);
	}
}

/*
 * For parser only.
 */
extern CSA_return_code
_DtCmsInsertAppt_for_parser(_DtCmsCalendar *cal, Appt_4 *appt)
{
	return (_DtCmsInsertAppt(cal, appt));
}

/*
 * For parser only.
 */
extern CSA_return_code
_DtCmsInsertEntry4Parser(_DtCmsCalendar *cal, cms_entry *entry)
{
	cms_entry	*eptr;
	CSA_return_code stat;

	if ((stat = _DtCmsMakeHashedEntry(cal, entry->num_attrs,
	    entry->attrs, &eptr)) != CSA_SUCCESS)
		return (stat);

	eptr->key = entry->key;

	stat = _DtCmsInsertEntry(cal, eptr);

	_DtCm_free_cms_entry(eptr);

	return(stat);
}

extern void
_DtCmsGenerateKey(_DtCmsCalendar *cal, long *key)
{
	if (*key == 0) {
		*key = ++(cal->lastkey);
	} else if (*key > cal->lastkey)
		cal->lastkey = *key;
}

extern CSA_return_code
_DtCmsEnumerateUp(_DtCmsCalendar *cal, _DtCmsEnumerateProc doit)
{
	if (cal == NULL || doit == NULL)
		return (CSA_E_INVALID_PARAMETER);
	if (rb_enumerate_up (APPT_TREE(cal), doit) != rb_ok ||
	    hc_enumerate_up (REPT_LIST(cal), doit) != rb_ok)
		return (CSA_E_FAILURE);
	else
		return (CSA_SUCCESS);
}

extern void
_DtCmsEnumerateDown(_DtCmsCalendar *cal, _DtCmsEnumerateProc doit)
{
	if (cal == NULL || doit == NULL)
		return;
	rb_enumerate_down (APPT_TREE(cal), doit);
	hc_enumerate_down (REPT_LIST(cal), doit);
}

/*
 * For parser only.
 */
extern void
_DtCmsSetAccess4Parser(
	_DtCmsCalendar *cal,
	Access_Entry_4 *list,
	int type)
{
	if (type == access_read_4)
		SET_R_ACCESS(cal, list);
	else if (type == access_write_4)
		SET_W_ACCESS(cal, list);
	else if (type == access_delete_4)
		SET_D_ACCESS(cal, list);
	else if (type == access_exec_4)
		SET_X_ACCESS(cal, list);
}

extern void
_DtCmsSetCalendarAttrs4Parser(
	_DtCmsCalendar *cal,
	int len,
	cms_attribute *attrs)
{
	cms_attribute_value *val;

	/* we don't want to free the value part of the access list
	 * attribute even it may be an empty list
	 */
	val = cal->attrs[CSA_CAL_ATTR_ACCESS_LIST_I].value;
	cal->attrs[CSA_CAL_ATTR_ACCESS_LIST_I].value = NULL;
	_DtCm_free_cms_attribute_values(cal->num_attrs, cal->attrs);
	cal->attrs[CSA_CAL_ATTR_ACCESS_LIST_I].value = val;

	(void)_DtCmUpdateAttributes(len, attrs, &cal->num_attrs, &cal->attrs,
		&cal->cal_tbl, B_TRUE, NULL, B_FALSE);
}

extern CSA_return_code
_DtCmsRbToCsaStat(Rb_Status rb_stat)
{
	switch (rb_stat) {
	case rb_ok:
		return (CSA_SUCCESS);
	case rb_duplicate:
	case rb_badtable:
	case rb_notable:
	case rb_failed:
	case rb_other:
		return (CSA_E_FAILURE);
	}
}

extern void
garbage_collect()
{
	unsigned remain;
	_DtCmsCalendar *clist = calendar_list;

	while (clist != NULL) {

		/* deregister stale client */
		clist->rlist = _DtCmsCheckRegistrationList(clist->rlist);

		if (clist->remq)
			_DtCmsUpdateReminders(clist->remq);

		if (clist->modified == B_TRUE) {
			if (debug)
				fprintf(stderr, "rpc.csmd: %s%s\n",
					"do garbage collection on ",
					clist->calendar);
			_DtCmsCollectOne(clist);
			clist->modified = B_FALSE;
		}

		clist = clist->next;
	}

	if ((remain = (unsigned) alarm((unsigned) 0)) == 0)
		alarm ((unsigned) (3600L * 24L));
	else
		alarm (remain);
}

extern void
debug_switch()
{
	_DtCmsCalendar *clist = calendar_list;
	Access_Entry_4 *l;

	debug = !debug;
	fprintf (stderr, "Debug Mode is %s\n", debug ? "ON" : "OFF");

	if (debug) {
		while (clist != NULL) {
			_DtCmsPrintReminderListV4(clist->rm_queue);

			if ((l = _DtCmsCalendarAccessList(clist)) != NULL) {
				_DtCmsShowAccessList(l);
				_DtCm_free_access_list4(l);
			}
			clist = clist->next;
		}
	}
}

extern CSA_return_code
_DtCmsGetCalAttrsByName(
	_DtCmsCalendar	*cal,
	uint		num_names,
	cms_attr_name	*names,
	uint		*num_attrs_r,
	cms_attribute	**attrs_r)
{
	CSA_return_code	stat = CSA_SUCCESS;
	int		i, j, index;
	cms_attribute	*attrs;

	if ((attrs = calloc(1, sizeof(cms_attribute)*num_names)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	/* get attributes */
	for (i = 0, j = 0; i < num_names; i++) {
		if (names[i].name == NULL)
			continue;

		if (names[i].num <= 0)
			index = names[i].num = _DtCm_get_index_from_table(
					cal->cal_tbl, names[i].name);

		if (index > 0) {
			if (index <= _DtCM_DEFINED_CAL_ATTR_SIZE) {
				if (_CSA_cal_attr_info[index].fst_vers
				    <= cal->fversion
				    || index == CSA_CAL_ATTR_CALENDAR_SIZE_I) {
					stat = cal->getattrfuncs[index](cal,
						index, &attrs[j]);
				}
			} else if (cal->attrs && cal->attrs[index].value) {
				stat = _DtCm_copy_cms_attribute(&attrs[j],
					&cal->attrs[index], B_TRUE);
			}

			if (stat != CSA_SUCCESS) {
				_DtCm_free_cms_attributes(j, attrs);
				free(attrs);
				return (stat);
			} else if (attrs[j].name.name)
				j++;
		}
	}

	if (j > 0) {
		*num_attrs_r = j;
		*attrs_r = attrs;
	} else {
		*num_attrs_r = 0;
		*attrs_r = NULL;
		free(attrs);
	}

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCmsGetAllCalAttrs(
	_DtCmsCalendar	*cal,
	uint		*num_attrs_r,
	cms_attribute	**attrs_r,
	boolean_t	returnall)
{
	CSA_return_code	stat = CSA_SUCCESS;
	cms_attribute	*attrs, *cattrs = cal->attrs;
	int		i, j, num_cal_attrs = cal->num_attrs;

	num_cal_attrs = cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION ?
			cal->num_attrs : _DtCM_OLD_CAL_ATTR_SIZE;

	if ((attrs = calloc(1, sizeof(cms_attribute)*num_cal_attrs)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		/* first element is not used */
		for (i = 1, j = 0; i <= num_cal_attrs; i++) {
			if (i <= _DtCM_DEFINED_CAL_ATTR_SIZE) {
				if ((stat = cal->getattrfuncs[i](cal, i,
				    &attrs[j])) != CSA_SUCCESS) {
					_DtCm_free_cms_attributes(j, attrs);
					free(attrs);
					return (stat);
				} else if (attrs[j].name.name)
					j++;
			} else if (cattrs[i].value) {
				if (returnall) {
					stat = _DtCm_copy_cms_attribute(
						&attrs[j], &cal->attrs[i],
						B_TRUE);
				} else {
					/* just return the attribute name */
					if (attrs[j].name.name = strdup(
					    cattrs[i].name.name)) {
						attrs[j].name.num =
							cattrs[i].name.num;
					} else
						stat =CSA_E_INSUFFICIENT_MEMORY;
				}

				if (stat == CSA_SUCCESS)
					j++;
				else {
					_DtCm_free_cms_attributes(j, attrs);
					free(attrs);
					return (stat);
				}
			}
		}
	} else {
		for (i = 1, j = 0; i <= _DtCM_DEFINED_CAL_ATTR_SIZE; i++) {
			if (_CSA_cal_attr_info[i].fst_vers <= cal->fversion ||
			    i == CSA_CAL_ATTR_CALENDAR_SIZE_I)
			{
				if ((stat = cal->getattrfuncs[i](cal, i,
				    &attrs[j])) != CSA_SUCCESS) {
					_DtCm_free_cms_attributes(j, attrs);
					free(attrs);
					return (stat);
				} else if (attrs[j].name.name)
					j++;
			}
		}
	}

	if (j > 0) {
		*num_attrs_r = j;
		*attrs_r = attrs;
	} else {
		*num_attrs_r = 0;
		*attrs_r = NULL;
		free(attrs);
	}

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCmsGetCalAttrNames(
	_DtCmsCalendar	*cal,
	uint		*num_names_r,
	cms_attr_name	**names_r)
{
	CSA_return_code	stat = CSA_SUCCESS;
	cms_attribute	*attrs = cal->attrs;
	cms_attr_name	*names;
	uint		i, j, num_attrs = cal->num_attrs;

	if ((names = calloc(1, sizeof(cms_attr_name)*num_attrs)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	/* first element is not used */
	for (i = 1, j = 0; i <= num_attrs; i++) {
		if (attrs[i].value || (i == CSA_CAL_ATTR_CALENDAR_SIZE_I ||
		    i == CSA_CAL_ATTR_NUMBER_ENTRIES_I ||
		    i == CSA_X_DT_CAL_ATTR_SERVER_VERSION_I ||
		    i == CSA_X_DT_CAL_ATTR_DATA_VERSION_I ||
		    i == CSA_CAL_ATTR_ACCESS_LIST_I))
		{
			if (names[j].name = strdup(attrs[i].name.name)) {
				names[j].num = attrs[i].name.num;
				j++;
			} else {
				_DtCmsFreeCmsAttrNames(j, names);
				return (CSA_E_INSUFFICIENT_MEMORY);
			}
		}
	}

	if (j > 0) {
		*num_names_r = j;
		*names_r = names;
	} else {
		*num_names_r = 0;
		*names_r = NULL;
		free(names);
	}

	return (CSA_SUCCESS);
}

extern void
_DtCmsFreeCmsAttrNames(uint num, cms_attr_name *names)
{
	uint	i;

	for (i = 0; i < num; i++)
		if (names[i].name) free(names[i].name);

	free(names);
}

extern CSA_return_code
_DtCmsUpdateCalAttributesAndLog(
	_DtCmsCalendar	*cal,
	uint		numsrc,
	cms_attribute	*srcattrs,
	uint		access)
{
	CSA_return_code stat = CSA_SUCCESS;
	uint		i, oldnum = 0;
	cms_attribute	*oldattrs = NULL;
	char		*log;

	/* we didn't use _DtCmUpdateAttributes because we need
	 * to check access rights here
	 */

	/* copy original attributes for rollback if update fails */
	if (cal->attrs && (stat = _DtCm_copy_cms_attributes(cal->num_attrs,
	    cal->attrs, &oldnum, &oldattrs)) != CSA_SUCCESS)
		return (stat);

	for (i = 0; i < numsrc && stat == CSA_SUCCESS; i++) {
		if (srcattrs[i].name.name == NULL)
			continue;

		if (srcattrs[i].name.num <= 0)
			srcattrs[i].name.num = _DtCm_get_index_from_table(
						cal->cal_tbl,
						srcattrs[i].name.name);

		if (srcattrs[i].name.num < 0) {
			if (!_DTCMS_HAS_INSERT_CALENDAR_ATTR_ACCESS(access))
				stat = CSA_E_NO_AUTHORITY;
			else {
				if ((stat = _DtCmExtendNameTable(
				    srcattrs[i].name.name, 0, 0,
				    _DtCm_cal_name_tbl,
				    _DtCM_DEFINED_CAL_ATTR_SIZE,
				    _CSA_calendar_attribute_names,
				    &cal->cal_tbl, NULL)) == CSA_SUCCESS) {
					srcattrs[i].name.num =
						cal->cal_tbl->size;
					stat = _DtCmGrowAttrArray(
						&cal->num_attrs, &cal->attrs,
						&srcattrs[i]);
				}
			}

		} else if (!_DTCMS_HAS_CHANGE_CALENDAR_ATTR_ACCESS(access)) {

			stat = CSA_E_NO_AUTHORITY;
		} else {
			cms_attribute_value val;

			if (srcattrs[i].name.num == CSA_CAL_ATTR_ACCESS_LIST_I
			    && srcattrs[i].value == NULL) {
				val.type = CSA_VALUE_ACCESS_LIST;
				val.item.access_list_value = NULL;
				srcattrs[i].value = &val;
			}
			stat = _DtCmUpdateAttribute(&srcattrs[i],
				&cal->attrs[srcattrs[i].name.num]);
		}
	}

	if (stat == CSA_SUCCESS) {
		log = _DtCmsGetLogFN(cal->calendar);

		/* dump file */
		stat = _DtCmsAppendCalAttrsByFN(log, cal->num_attrs,
			cal->attrs);
		free(log);
	}

	if (stat == CSA_SUCCESS) {
		cal->modified = B_TRUE;

		_DtCm_free_cms_attributes(oldnum + 1, oldattrs);
		free(oldattrs);
	} else {
		_DtCm_free_cms_attributes(cal->num_attrs+1, cal->attrs);
		free(cal->attrs);
		cal->num_attrs = oldnum;
		cal->attrs = oldattrs;
	}

	return (stat);
}

extern CSA_return_code
_DtCmsV5TransactLog(_DtCmsCalendar *cal, cms_entry *e, _DtCmsLogOps op)
{
	CSA_return_code	stat;
	char		*log;

	if ((log = _DtCmsGetLogFN(cal->calendar)) == NULL)
		 return (CSA_E_INSUFFICIENT_MEMORY);

	if (cal->hashed == B_FALSE ||
	    cal->num_entry_attrs < cal->entry_tbl->size) {
		if ((stat = _DtCmsAppendHTableByFN(log, cal->entry_tbl->size,
		    cal->entry_tbl->names, cal->types)) != CSA_SUCCESS) {
			free(log);
			return (stat);
		} else {
			cal->hashed = B_TRUE;
			cal->num_entry_attrs = cal->entry_tbl->size;
		}
	}
	stat = _DtCmsAppendEntryByFN(log, e, op);
	free(log);
	return(stat);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

/*
 * return user name of file owner
 * If the calendar name is the same as a user account name, the
 * calendar name is return; otherwise, the owner name is derived from
 * the owner of the calendar file
 */
static CSA_return_code
get_file_owner(char *calname, char **owner)
{
	struct stat info;
	struct passwd *pw;
	char buf[BUFSIZ];
	char *log;
	int res;

	if (calname == NULL || owner == NULL)
		return (CSA_E_FAILURE);

	*owner = NULL;

	if (_DtCmIsUserName(calname) == B_TRUE) {
		strcpy(buf, calname);
	} else {
		if ((log = _DtCmsGetLogFN(calname)) == NULL)
			return (CSA_E_INSUFFICIENT_MEMORY);

		res = stat(log, &info);
		free(log);

		if (res == 0) {

			if (pw = getpwuid(info.st_uid))
				strcpy(buf, pw->pw_name);
			else
				return (CSA_E_FAILURE);
		} else
			return(CSA_X_DT_E_BACKING_STORE_PROBLEM);
	}

	if (((*owner) = (char *)strdup(buf)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);
	else
		return (CSA_SUCCESS);
}

static char *
get_calname(char *target)
{
	char *ptr, *name;

	if (target == NULL)
		return (NULL);

	ptr = strchr(target, '@');
	if (ptr == NULL) {
		return (strdup(target));
	} else {
		*ptr = '\0';
		name = strdup(target);
		*ptr = '@';
		return (name);
	}
}

static CSA_return_code
init_cal_attrs(_DtCmsCalendar *cal)
{
	int i;

	/* initialize the calendar with predefined attribute names */
	if ((cal->attrs = (cms_attribute *)calloc(1,
	    sizeof(cms_attribute)*(_DtCm_cal_name_tbl->size + 1))) == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	for (i = 1; i <= _DtCm_cal_name_tbl->size; i++) {
		if ((cal->attrs[i].name.name =
		    strdup(_DtCm_cal_name_tbl->names[i])) == NULL) {
			_DtCm_free_cms_attributes(i, cal->attrs);
			free(cal->attrs);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}

		cal->attrs[i].name.num = i;
	}
	cal->num_attrs = _DtCm_cal_name_tbl->size;

	return (CSA_SUCCESS);
}

static CSA_return_code
_CopyCalendarAttr(_DtCmsCalendar *cal, int index, cms_attribute *attr)
{
	cms_attribute		tmp;
	cms_attribute_value	tmpval;

	if (cal->attrs && cal->attrs[index].value)
		return (_DtCm_copy_cms_attribute(attr,
				&cal->attrs[index], B_TRUE));
	else
		return (CSA_SUCCESS);
}

static CSA_return_code
_GetCalendarSize(_DtCmsCalendar *cal, int index, cms_attribute *attr)
{
	CSA_return_code	stat;
	int		size;

	if ((stat = _DtCmsGetFileSize(cal->calendar, &size)) == CSA_SUCCESS) {
		attr->name.num = CSA_CAL_ATTR_CALENDAR_SIZE_I;
		if (attr->name.name = strdup(CSA_CAL_ATTR_CALENDAR_SIZE))
			return (_DtCm_set_uint32_attrval((uint)size,
				&attr->value));
		else
			return (CSA_E_INSUFFICIENT_MEMORY);
	}
	return (stat);
}

static CSA_return_code
_GetNumberEntries(_DtCmsCalendar *cal, int index, cms_attribute *attr)
{
	uint	size;

	size = rb_size(cal->tree) + hc_size(cal->list);

	attr->name.num = CSA_CAL_ATTR_NUMBER_ENTRIES_I;
	if (attr->name.name = strdup(CSA_CAL_ATTR_NUMBER_ENTRIES))
		return (_DtCm_set_uint32_attrval(size, &attr->value));
	else
		return (CSA_E_INSUFFICIENT_MEMORY);
}

static CSA_return_code
_GetAccessList(_DtCmsCalendar *cal, int index, cms_attribute *attr)
{
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		return (_DtCm_copy_cms_attribute(attr,
			&cal->attrs[CSA_CAL_ATTR_ACCESS_LIST_I], B_TRUE));
	} else {
		Access_Entry_4	*alist;
		CSA_return_code	stat;
		cms_attribute	tmpattr;
		cms_attribute_value tmpval;

		alist = _DtCmsCalendarAccessList(cal);
		if ((stat = _DtCm_accessentry4_to_cmsaccesslist(alist,
		    &tmpval.item.access_list_value)) == CSA_SUCCESS) {
			tmpattr.name.num = CSA_CAL_ATTR_ACCESS_LIST_I;
			tmpattr.name.name = CSA_CAL_ATTR_ACCESS_LIST;
			tmpattr.value = &tmpval;
			tmpval.type = CSA_VALUE_ACCESS_LIST;

			return (_DtCm_copy_cms_attribute(attr, &tmpattr,
				B_TRUE));
		} else
			return (stat);
	}
}

static CSA_return_code
_GetCalendarName(_DtCmsCalendar *cal, int index, cms_attribute *attr)
{
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		return (_DtCm_copy_cms_attribute(attr,
			&cal->attrs[CSA_CAL_ATTR_CALENDAR_NAME_I], B_TRUE));
	} else {
		attr->name.num = CSA_CAL_ATTR_CALENDAR_NAME_I;
		if (attr->name.name = strdup(CSA_CAL_ATTR_CALENDAR_NAME)) {
			return (_DtCm_set_string_attrval(cal->calendar,
				&attr->value, CSA_VALUE_STRING));
		} else {
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
	}
}

static CSA_return_code
_GetCalendarOwner(_DtCmsCalendar *cal, int index, cms_attribute *attr)
{
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		return (_DtCm_copy_cms_attribute(attr,
			&cal->attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I], B_TRUE));
	} else {
		attr->name.num = CSA_CAL_ATTR_CALENDAR_OWNER_I;
		if (attr->name.name = strdup(CSA_CAL_ATTR_CALENDAR_OWNER)) {
			return (_DtCm_set_user_attrval(cal->owner,
				&attr->value));
		} else {
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
	}
}

static CSA_return_code
_GetServerVersion(_DtCmsCalendar *cal, int index, cms_attribute *attr)
{
	attr->name.num = CSA_X_DT_CAL_ATTR_SERVER_VERSION_I;
	if (attr->name.name = strdup(CSA_X_DT_CAL_ATTR_SERVER_VERSION))
		return (_DtCm_set_uint32_attrval((uint)TABLEVERS, &attr->value));
	else
		return (CSA_E_INSUFFICIENT_MEMORY);
}

static CSA_return_code
_GetDataVersion(_DtCmsCalendar *cal, int index, cms_attribute *attr)
{
	uint	version;

	version = cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION ?
			cal->fversion : _DtCM_FIRST_EXTENSIBLE_DATA_VERSION - 1;

	attr->name.num = CSA_X_DT_CAL_ATTR_DATA_VERSION_I;
	if (attr->name.name = strdup(CSA_X_DT_CAL_ATTR_DATA_VERSION))
		return (_DtCm_set_uint32_attrval(version, &attr->value));
	else
		return (CSA_E_INSUFFICIENT_MEMORY);
}

static CSA_return_code
_GetProductId(_DtCmsCalendar *cal, int index, cms_attribute *attr)
{
	attr->name.num = CSA_CAL_ATTR_PRODUCT_IDENTIFIER_I;
	if (attr->name.name = strdup(CSA_CAL_ATTR_PRODUCT_IDENTIFIER)) {
		return (_DtCm_set_string_attrval(_DtCM_PRODUCT_IDENTIFIER,
			&attr->value, CSA_VALUE_STRING));
	} else {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}
}

static CSA_return_code
_GetSupportedVersion(_DtCmsCalendar *cal, int index, cms_attribute *attr)
{
	attr->name.num = CSA_CAL_ATTR_VERSION_I;
	if (attr->name.name = strdup(CSA_CAL_ATTR_VERSION)) {
		return (_DtCm_set_string_attrval(_DtCM_SPEC_VERSION_SUPPORTED,
			&attr->value, CSA_VALUE_STRING));
	} else {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}
}

