/* $XConsortium: cm.x /main/1 1996/04/21 19:21:56 drk $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*
 * Calendar manager service RPC protocol.
 */

#if defined(RPC_HDR) || defined(RPC_XDR)

%
%#include "csa.h" 
%#include "connection.h"
#ifdef RPC_XDR
%#include "cmxdr.h"
#endif
%

%
%/*
% * cms_name - a user name or calendar name which is
% *		variable length character string
% */
typedef string	cms_name<>;

%
%/*
% * buffer - a variable length character string
% */
typedef string	buffer<>;

%
%/*
% * cms_access_entry - data structure for the access list attribute
% */
struct cms_access_entry {
	cms_name	user;
	u_int		rights;
	cms_access_entry *next;
};

%
%/*
% * cms_attribute_value - data structure for attribute value
% */
%struct cms_attribute_value {
%	int	type;
%	union {
%		CSA_boolean	boolean_value;
%		CSA_enum	enumerated_value;
%		CSA_flags	flags_value;
%		int		sint32_value;
%		uint		uint32_value;
%		char		*string_value;
%		char		*calendar_user_value;
%		char		*date_time_value;
%		char		*date_time_range_value;
%		char		*time_duration_value;
%		cms_access_entry *access_list_value;
%		CSA_date_time_entry *date_time_list_value;
%		CSA_reminder 	*reminder_value;
%		CSA_opaque_data *opaque_data_value;
%	} item;
%};
%typedef struct cms_attribute_value cms_attribute_value;
%

%
%/*
% * cms_key consists of the time and the unique
% * identifier of the entry.
% */
struct cms_key {
	time_t	time;
	long	id;
};

%
%/*
% * num contains the hashed number for the associated name assigned
% * by the server.  Client should not change its value.
% * If it's zero, name is not hashed yet.
% */
struct cms_attr_name {
	short		num;
	cms_name	name;
};

%
%/*
% *
% */
struct cms_attribute {
	cms_attr_name		name;
	cms_attribute_value	*value;
};

%
%
%/*
% * An entry is identified by an identifier unique within the calendar
% * and has a variable number of attributes associated with it.
% */
struct cms_entry {
	cms_key		key;
	u_int		num_attrs;
	cms_attribute	*attrs;
	struct cms_entry *next;
};

%
%
%/*
% * cms_enumerate_calendar_attr_res
% *
% * If stat is CSA_SUCCESS, names contains an array of names.
% */
struct cms_enumerate_calendar_attr_res { 
	CSA_return_code	stat;
	CSA_uint32	num_names;
	cms_attr_name	*names;
};

%
%
%/*
% * cms_list_calendars_res
% *
% * If stat is CSA_SUCCESS, names contains an array of names.
% */
struct cms_list_calendars_res { 
	CSA_return_code	stat;
	CSA_uint32	num_names;
	cms_name	*names;
};

%
%
%/*
% * cms_open_args - used by CMS_OPEN_CALENDAR
% */
struct cms_open_args {
	cms_name	cal;
	int		pid;
};

%
%
%/*
% * cms_open_res - returned by CMS_OPEN_CALENDAR
% *
% * If stat is DtCm_OK,
% *	svr_vers contains the version number of the server,
% *	file_vers contains the version number of the callog file,
% *	user_access is the calling user's access right to the calendar
% *	num_attrs indicates the size of the array pointed to by attrs,
% *	and attrs contains an array of calendar attributes.
% */
struct cms_open_res {
	CSA_return_code	stat;
	int		svr_vers;
	int		file_vers;
	u_int		user_access;
	CSA_uint32	num_attrs;
	cms_attribute	*attrs;
};

%
%
%/*
% * cms_create_args - used by CMS_CREATE_CALENDAR
% */
struct cms_create_args {
	cms_name	cal;
	buffer		char_set;
	int		pid;
	CSA_uint32	num_attrs;
	cms_attribute	*attrs;
};

%
%
%/*
% * cms_remove_args - used by CMS_REMOVE_CALENDAR
% */
struct cms_remove_args {
	cms_name	cal;
	int		pid;
};

%
%
%/*
% * cms_register_args - used by CMS_REGISTER and CMS_UNREGISTER
% */
struct cms_register_args {
	cms_name	cal;
	u_int		update_type;
	u_long		prognum;
	u_long		versnum;
	u_long		procnum;
	int		pid;
};

%
%
%/*
% * cms_get_cal_attr_res - used by CMS_GET_CALENDAR_ATTR
% *
% * If stat is CSA_SUCCESS, attrs contains an array of attributes.
% */
struct cms_get_cal_attr_res { 
	CSA_return_code	stat;
	CSA_uint32	num_attrs;
	cms_attribute	*attrs;
};

%
%
%/*
% * cms_get_cal_attr_args - used by CMS_GET_CALENDAR_ATTR
% */
struct cms_get_cal_attr_args {
	cms_name	cal;
	CSA_uint32	num_names;
	cms_attr_name	*names;
};

%
%
%/*
% * cms_set_cal_attr_args - used by CMS_SET_CALENDAR_ATTR
% */
struct cms_set_cal_attr_args {
	cms_name	cal;
	int		pid;
	CSA_uint32	num_attrs;
	cms_attribute	*attrs;
};

%
%
%/*
% * cms_archive_res - used by CMS_ARCHIVE
% *
% * If stat is CSA_SUCCESS, data contains the data to be archived
% */
struct cms_archive_res {
	CSA_return_code	stat;
	buffer		data;
};

%
%
%/*
% * cms_archive_args - used by CMS_ARCHIVE
% */
struct cms_archive_args {
	cms_name	cal;
	bool		delete;
	buffer		char_set;
	CSA_uint32	num_attrs;
	cms_attribute	*attrs;
	int		*ops;
};

%
%
%/*
% * cms_restore_args - used by CMS_RESTORE
% */
struct cms_restore_args {
	cms_name	cal;
	buffer		data;
	buffer		char_set;
	CSA_uint32	num_attrs;
	cms_attribute	*attrs;
	int		*ops;
};

%
%
%/*
% * cms_reminder
% */
struct cms_reminder_ref {
	buffer	reminder_name;
	buffer	entryid;
	cms_key	key;
	long	runtime;
	struct cms_reminder_ref *next;
};

%
%
%/*
% * cms_reminder_res - used by CMS_LOOKUP_REMINDER
% *
% * If stat is CSA_SUCCESS, rems contains an array of reminders.
% */
struct cms_reminder_res {
	CSA_return_code		stat;
	cms_reminder_ref	*rems;
};

%
%
%/*
% * cms_reminder_args - used by CMS_LOOKUP_REMINDER
% */
struct cms_reminder_args {
	cms_name	cal;
	long		tick;
	CSA_uint32	num_names;
	cms_attr_name	*names;
};

%
%
%/*
% * cms_entries_res
% *
% * If stat is CSA_SUCCESS, entries contains an array of entries.
% */
struct cms_entries_res { 
	CSA_return_code	stat;
	cms_entry	*entries;
};

%
%
%/*
% * cms_lookup_entries_args - used by CMS_LOOKUP_ENTRIES
% */
struct cms_lookup_entries_args {
	cms_name	cal;
	buffer		char_set;
	CSA_uint32 	num_attrs;
	cms_attribute	*attrs;
	int		*ops;
};

%
%
%/*
% * cms_enumerate_args - used by CMS_ENUERATE_SEQUENCE
% */
struct cms_enumerate_args {
	cms_name	cal;
	long		id;
	long		start;
	long		end;
};

%
%/*
% * If stat is CSA_SUCCESS, attrs contains an array of
% * attributes.
% */
struct cms_get_entry_attr_res_item {
	CSA_return_code	stat;
	cms_key		key;
	CSA_uint32	num_attrs;
	cms_attribute	*attrs;
	struct cms_get_entry_attr_res_item *next;
};

%
%
%/*
% * cms_get_entry_attr_res - used by CMS_GET_ENTRY_ATTR
% *
% * If stat is CSA_SUCCESS, entries contains an array of
% * cms_get_entry_attr_res_item structures.
% */
struct cms_get_entry_attr_res { 
	CSA_return_code	stat;
	cms_get_entry_attr_res_item	*entries;
};

%
%
%/*
% * cms_get_entry_attr_args - used by CMS_GET_ENTRY_ATTR
% */
struct cms_get_entry_attr_args {
	cms_name	cal;
	u_int		num_keys;
	cms_key		*keys;
	CSA_uint32	num_names;
	cms_attr_name	*names;
};

%
%
%/*
% * cms_entry_res
% *
% * If stat is CSA_SUCCESS, entry points to an entry.
% */
struct cms_entry_res {
	CSA_return_code	stat;
	cms_entry	*entry;
};

%
%
%/*
% * cms_insert_args - used by CMS_INSERT_ENTRY
% */
struct cms_insert_args {
	cms_name	cal;
	int		pid;
	CSA_uint32	num_attrs;
	cms_attribute	*attrs;
};

%
%
%/*
% * cms_update_args - used by CMS_UPDATE_ENTRY
% */
struct cms_update_args {
	cms_name	cal;
	int		pid;
	cms_key		entry;
	int		scope;
	CSA_uint32	num_attrs;
	cms_attribute	*attrs;
};

%
%
%/*
% * cms_delete_args - used by CMS_DELETE_ENTRY
% */
struct cms_delete_args {
	cms_name	cal;
	int		pid;
	cms_key		entry;
	int		scope;
};

#endif /* RPC_HDR */

program TABLEPROG {

	/* RPC Language description of the calendar protocol */

	version TABLEVERS {
		void
		CMS_PING(void) = 0;

		/*
		 * List all calendars supported by the server.
		 */
		cms_list_calendars_res
		CMS_LIST_CALENDARS(void) = 1;

		/*
		 * Check existence of the calendar.
		 * Names and tags of all calendar attribute
		 * will be returned.
		 */
		cms_open_res
		CMS_OPEN_CALENDAR(cms_open_args) = 2;

		/*
		 * Create a calendar and initialize it with
		 * the given attributes.
		 */
		CSA_return_code
		CMS_CREATE_CALENDAR(cms_create_args) = 3;

		/*
		 * Remove the calendar.
		 */
		CSA_return_code
		CMS_REMOVE_CALENDAR(cms_remove_args) = 4;

		/*
		 * Register client for callbacks.
		 */
		CSA_return_code
		CMS_REGISTER(cms_register_args) = 5;

		/*
		 * Unregister the client.
		 */
		CSA_return_code
		CMS_UNREGISTER(cms_register_args) = 6;

		/*
		 * Enumerate all calendar attributes.
		 */
		cms_enumerate_calendar_attr_res
		CMS_ENUMERATE_CALENDAR_ATTR(cms_name) = 7;

		/*
		 * Get calendar attribute values.
		 */
		cms_get_cal_attr_res
		CMS_GET_CALENDAR_ATTR(cms_get_cal_attr_args) = 8;

		/*
		 * Set calendar attribute values.
		 */
		CSA_return_code
		CMS_SET_CALENDAR_ATTR(cms_set_cal_attr_args) = 9;

		/*
		 * Get data to be archived.
		 */
		cms_archive_res
		CMS_ARCHIVE(cms_archive_args) = 10;

		/*
		 * Restore data to the calendar.
		 */
		CSA_return_code
		CMS_RESTORE(cms_restore_args) = 11;

		/*
		 * Lookup reminders relative to the given tick.
		 */
		cms_reminder_res
		CMS_LOOKUP_REMINDER(cms_reminder_args) = 12;

		/*
		 * Lookup entries that match the given criteria.
		 * Attribute values of predefined attributes are returned.
		 * For other attributes, only the names are
		 * returned but not the values.
		 */
		cms_entries_res
		CMS_LOOKUP_ENTRIES(cms_lookup_entries_args) = 13;

		/*
		 * Lookup instances of a repeating entry
		 * The attribute list specify the entry
		 * id and a time range.
		 * If the entry repeats indefinitely,
		 * the time range is mandatory.
		 */
		cms_entries_res
		CMS_ENUMERATE_SEQUENCE(cms_enumerate_args) = 14;

		/*
		 * Get entry attribute values.
		 */
		cms_get_entry_attr_res
		CMS_GET_ENTRY_ATTR(cms_get_entry_attr_args) = 15;

		/*
		 * Insert an entry.
		 */
		cms_entry_res
		CMS_INSERT_ENTRY(cms_insert_args) = 16;

		/*
		 * Update an entry.
		 */
		cms_entry_res
		CMS_UPDATE_ENTRY(cms_update_args) = 17;

		/*
		 * Delete an entry.
		 */
		CSA_return_code
		CMS_DELETE_ENTRY(cms_delete_args) = 18;

	} = 5;
} = 100068; 

