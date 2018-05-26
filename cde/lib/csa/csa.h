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
/* $XConsortium: csa.h /main/1 1996/04/21 19:23:01 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CSA_H
#define _CSA_H

/* BEGIN CSA INTERFACE */

/* BASIC DATA TYPES */

#ifndef DIFFERENT_PLATFORM
typedef short		CSA_sint16;
typedef long		CSA_sint32;
typedef unsigned char	CSA_uint8;
typedef unsigned short	CSA_uint16;
typedef unsigned long	CSA_uint32;
typedef void		*CSA_buffer;
typedef CSA_uint32	CSA_entry_handle;
typedef CSA_uint32	CSA_session_handle;
typedef char		*CSA_string;
#endif

typedef CSA_string	CSA_attribute_reference;
typedef CSA_uint32	CSA_boolean;
typedef CSA_string	CSA_date_time;
typedef CSA_string	CSA_date_time_range;
typedef CSA_sint32	CSA_enum;
typedef CSA_uint32	CSA_flags;
typedef CSA_uint32	CSA_return_code;
typedef CSA_string	CSA_service_reference;
typedef CSA_string	CSA_time_duration;

#define CSA_FALSE	((CSA_boolean)0)
#define CSA_TRUE	((CSA_boolean)1)

/* DATA STRUCTURES */

/* EXTENSION */
typedef struct CSA_TAG_EXTENSION {
	CSA_uint32	item_code;
	CSA_uint32	item_data;
	CSA_buffer	item_reference;
	CSA_flags	extension_flags;
} CSA_extension;

/* EXTENSION FLAGS */
#define	CSA_EXT_REQUIRED	((CSA_flags)0x1)
#define	CSA_EXT_OUTPUT		((CSA_flags)0x2)
#define	CSA_EXT_LAST_ELEMENT	((CSA_flags)0x4)

/* CALENDAR USER */
typedef struct CSA_TAG_CALENDAR_USER{
	CSA_string	user_name;
	CSA_enum	user_type;
	CSA_string	calendar_address;
	CSA_extension	*calendar_user_extensions;
} CSA_calendar_user;

/* CALENDAR USER TYPE */
#define	CSA_USER_TYPE_INDIVIDUAL	((CSA_enum)0)
#define	CSA_USER_TYPE_GROUP		((CSA_enum)1)
#define	CSA_USER_TYPE_RESOURCE		((CSA_enum)2)

/* ACCESS LIST */
typedef struct CSA_TAG_ACESS_RIGHTS {
	CSA_calendar_user		*user;
	CSA_flags			rights;
	struct CSA_TAG_ACESS_RIGHTS	*next;
} CSA_access_rights, *CSA_access_list;

/* ACCESS RIGHT FLAGS */
#define	CSA_FREE_TIME_SEARCH		((CSA_flags)0x1)
#define	CSA_VIEW_PUBLIC_ENTRIES		((CSA_flags)0x2)
#define	CSA_VIEW_CONFIDENTIAL_ENTRIES	((CSA_flags)0x4)
#define	CSA_VIEW_PRIVATE_ENTRIES	((CSA_flags)0x8)
#define	CSA_INSERT_PUBLIC_ENTRIES	((CSA_flags)0x10)
#define	CSA_INSERT_CONFIDENTIAL_ENTRIES	((CSA_flags)0x20)
#define	CSA_INSERT_PRIVATE_ENTRIES	((CSA_flags)0x40)
#define	CSA_CHANGE_PUBLIC_ENTRIES	((CSA_flags)0x80)
#define	CSA_CHANGE_CONFIDENTIAL_ENTRIES	((CSA_flags)0x100)
#define	CSA_CHANGE_PRIVATE_ENTRIES	((CSA_flags)0x200)
#define	CSA_VIEW_CALENDAR_ATTRIBUTES	((CSA_flags)0x400)
#define	CSA_INSERT_CALENDAR_ATTRIBUTES	((CSA_flags)0x800)
#define	CSA_CHANGE_CALENDAR_ATTRIBUTES	((CSA_flags)0x1000)
#define	CSA_ORGANIZER_RIGHTS		((CSA_flags)0x2000)
#define	CSA_SPONSOR_RIGHTS		((CSA_flags)0x4000)
#define	CSA_OWNER_RIGHTS		((CSA_flags)0x8000)

/* access right flags for data version 1, 2, and 3 */
#define CSA_X_DT_BROWSE_ACCESS	((CSA_flags)0x1)
#define CSA_X_DT_INSERT_ACCESS	((CSA_flags)0x2)
#define CSA_X_DT_DELETE_ACCESS	((CSA_flags)0x4)

/* ATTENDEE LIST */
typedef struct CSA_TAG_ATTENDEE {
	CSA_calendar_user	attendee;
	CSA_enum		priority;
	CSA_enum		status;
	CSA_boolean		rsvp_requested;
	struct CSA_TAG_ATTENDEE	*next;
} CSA_attendee, *CSA_attendee_list;

/* ATTENDEE PRIORITIES */
#define	CSA_FOR_YOUR_INFORMATION	((CSA_enum)0)
#define	CSA_ATTENDANCE_REQUESTED	((CSA_enum)1)
#define	CSA_ATTENDANCE_REQUIRED		((CSA_enum)2)
#define	CSA_IMMEDIATE_RESPONSE		((CSA_enum)3)

/* DATE AND TIME LIST */
typedef struct CSA_TAG_DATE_TIME_ITEM {
	CSA_date_time			date_time;
	struct CSA_TAG_DATE_TIME_ITEM	*next;
} CSA_date_time_entry, *CSA_date_time_list;

/* OPAQUE DATA */
typedef struct CSA_TAG_OPAQUE_DATA {
	CSA_uint32	size;
	CSA_uint8	*data;
} CSA_opaque_data;

/* REMINDER */
typedef struct CSA_TAG_REMINDER {
	CSA_time_duration	lead_time;
	CSA_time_duration	snooze_time;
	CSA_uint32		repeat_count;
	CSA_opaque_data		reminder_data;
} CSA_reminder;

/* ATTRIBUTE */
typedef struct CSA_TAG_ATTRIBUTE_ITEM {
	CSA_enum			type;
	union {
		CSA_boolean		boolean_value;
		CSA_enum		enumerated_value;
		CSA_flags		flags_value;
		CSA_sint32		sint32_value;
		CSA_uint32		uint32_value;
		CSA_string		string_value;
		CSA_calendar_user	*calendar_user_value;
		CSA_date_time		date_time_value;
		CSA_date_time_range	date_time_range_value;
		CSA_time_duration	time_duration_value;
		CSA_access_list		access_list_value;
		CSA_attendee_list	attendee_list_value;
		CSA_date_time_list	date_time_list_value;
		CSA_reminder		*reminder_value;
		CSA_opaque_data		*opaque_data_value;
	} item;
} CSA_attribute_value;

typedef struct CSA_TAG_ATTRIBUTE {
	CSA_string		name;
	CSA_attribute_value	*value;
	CSA_extension		*attribute_extensions;
} CSA_attribute;

/* ATTRIBUTE VALUE TYPE */
#define CSA_VALUE_BOOLEAN		((CSA_enum)0)
#define CSA_VALUE_ENUMERATED		((CSA_enum)1)
#define CSA_VALUE_FLAGS			((CSA_enum)2)
#define CSA_VALUE_SINT32		((CSA_enum)3)
#define	CSA_VALUE_UINT32		((CSA_enum)4)
#define	CSA_VALUE_STRING		((CSA_enum)5)
#define CSA_VALUE_CALENDAR_USER		((CSA_enum)6)
#define	CSA_VALUE_DATE_TIME		((CSA_enum)7)
#define	CSA_VALUE_DATE_TIME_RANGE	((CSA_enum)8)
#define CSA_VALUE_TIME_DURATION		((CSA_enum)9)
#define	CSA_VALUE_ACCESS_LIST		((CSA_enum)10)
#define	CSA_VALUE_ATTENDEE_LIST		((CSA_enum)11)
#define	CSA_VALUE_DATE_TIME_LIST	((CSA_enum)12)
#define	CSA_VALUE_REMINDER		((CSA_enum)13)
#define	CSA_VALUE_OPAQUE_DATA		((CSA_enum)14)

/* CALLBACK */

/* REASON FOR CALLBACK */
#define	CSA_CB_CALENDAR_LOGON			((CSA_flags)0x1)
#define	CSA_CB_CALENDAR_DELETED			((CSA_flags)0x2)
#define	CSA_CB_CALENDAR_ATTRIBUTE_UPDATED	((CSA_flags)0x4)
#define	CSA_CB_ENTRY_ADDED			((CSA_flags)0x8)
#define	CSA_CB_ENTRY_DELETED			((CSA_flags)0x10)
#define	CSA_CB_ENTRY_UPDATED			((CSA_flags)0x20)

/* CALL DATA FOR CSA_CB_CALENDAR_LOGON */
typedef struct CSA_TAG_LOGON_CB_DATA {
	CSA_calendar_user	*user;
} CSA_logon_callback_data;

/* CALL DATA FOR CSA_CB_CALENDAR_DELETED */
typedef struct CSA_TAG_CALENDAR_DELETED_CB_DATA {
	CSA_calendar_user	*user;
} CSA_calendar_deleted_callback_data;

/* CALL DATA FOR CSA_CB_CALENDAR_ATTRIBUTE_UPDATED */
typedef struct CSA_TAG_CALENDAR_ATTR_UPDATE_CB_DATA {
	CSA_calendar_user	*user;
	CSA_uint32		number_attributes;
	CSA_attribute_reference *attribute_names;
} CSA_calendar_attr_update_callback_data;

/* CALL DATA FOR CSA_CB_ENTRY_ADDED */
typedef struct CSA_TAG_ADD_ENTRY_CB_DATA {
	CSA_calendar_user	*user;
	CSA_opaque_data		added_entry_id;
} CSA_add_entry_callback_data;

/* CALL DATA FOR CSA_CB_ENTRY_DELETED */
typedef struct CSA_TAG_DELETE_ENTRY_CB_DATA {
	CSA_calendar_user	*user;
	CSA_opaque_data		deleted_entry_id;
	CSA_enum		scope;
	CSA_date_time		date_and_time;
} CSA_delete_entry_callback_data;

/* CALL DATA FOR CSA_CB_ENTRY_UPDATED */
typedef struct CSA_TAG_UPDATE_ENTRY_CB_DATA {
	CSA_calendar_user	*user;		/* who updated entry? */
	CSA_opaque_data		old_entry_id;	/* id of updated entry */
	CSA_opaque_data		new_entry_id;	/* new id after update */
	CSA_enum		scope;		/* scope of the update op */
	CSA_date_time		date_and_time;	/* time of first updated */
} CSA_update_entry_callback_data;

/* CALLBACK FUNCTION PROTOTYPE */
typedef void (*CSA_callback)(
	CSA_session_handle	session,
	CSA_flags		reason,
	CSA_buffer		call_data,
	CSA_buffer		client_data,
	CSA_extension		*callback_extensions);

/* FREE TIME */
typedef struct CSA_TAG_FREE_TIME {
	CSA_uint32		number_free_time_data;
	CSA_date_time_range	*free_time_data;
} CSA_free_time;

/* REMINDER REFERENCE */
typedef struct CSA_TAG_REMINDER_REFERENCE {
	CSA_entry_handle	entry;
	CSA_date_time		run_time;
	CSA_time_duration	snooze_time;
	CSA_uint32		repeat_count;
	CSA_attribute_reference	attribute_name;
} CSA_reminder_reference;

/* WORK SCHEDULE */
typedef struct CSA_TAG_WORK_SCHEDULE {
	CSA_date_time		schedule_begin_time;
	CSA_boolean		cyclic_definition_flag;
	CSA_date_time		cycle_end_time;
	CSA_date_time_list	*work_cycle;
} CSA_work_schedule;

/* CSA FUNCTIONS */

/* CROSS FUNCTION FLAGS */

/* CLASSIFICATION */
#define CSA_CLASS_PUBLIC	((CSA_enum)0)
#define CSA_CLASS_PRIVATE	((CSA_enum)1)
#define CSA_CLASS_CONFIDENTIAL	((CSA_enum)2)

/* STATUS */
#define	CSA_STATUS_ACCEPTED		((CSA_enum)0)
#define	CSA_STATUS_NEEDS_ACTION		((CSA_enum)1)
#define	CSA_STATUS_SENT			((CSA_enum)2)
#define	CSA_STATUS_TENTATIVE		((CSA_enum)3)
#define	CSA_STATUS_CONFIRMED		((CSA_enum)4)
#define	CSA_STATUS_REJECTED		((CSA_enum)5)
#define	CSA_STATUS_COMPLETED		((CSA_enum)6)
#define	CSA_STATUS_DELEGATED		((CSA_enum)7)

/* CDE specific status values */
#define	CSA_X_DT_STATUS_ACTIVE		((CSA_enum)0x900)
#define	CSA_X_DT_STATUS_DELETE_PENDING	((CSA_enum)0x901)
#define	CSA_X_DT_STATUS_ADD_PENDING	((CSA_enum)0x902)
#define	CSA_X_DT_STATUS_COMMITTED	((CSA_enum)0x903)
#define	CSA_X_DT_STATUS_CANCELLED	((CSA_enum)0x904)

/* SCOPE */
#define	CSA_SCOPE_ALL		((CSA_enum)0)
#define	CSA_SCOPE_ONE		((CSA_enum)1)
#define	CSA_SCOPE_FORWARD	((CSA_enum)2)

/* OPERATORS */
#define	CSA_MATCH_ANY				((CSA_enum)0)
#define	CSA_MATCH_EQUAL_TO			((CSA_enum)1)
#define	CSA_MATCH_NOT_EQUAL_TO			((CSA_enum)2)
#define	CSA_MATCH_GREATER_THAN			((CSA_enum)3)
#define	CSA_MATCH_LESS_THAN			((CSA_enum)4)
#define	CSA_MATCH_GREATER_THAN_OR_EQUAL_TO	((CSA_enum)5)
#define	CSA_MATCH_LESS_THAN_OR_EQUAL_TO		((CSA_enum)6)
#define	CSA_MATCH_CONTAIN			((CSA_enum)7)

/* FREE */
CSA_return_code
csa_free(
	CSA_buffer	memory
);

/* LIST CALENDARS */
CSA_return_code
csa_list_calendars(
	CSA_service_reference	calendar_service,
	CSA_uint32		*number_names,
	CSA_calendar_user	**calendar_names,
	CSA_extension		*list_calendars_extensions
);

/* LOGOFF */
CSA_return_code
csa_logoff(
	CSA_session_handle	session,
	CSA_extension		*logoff_extensions
);

/* LOGON */
CSA_return_code
csa_logon(
	CSA_service_reference	calendar_service,
	CSA_calendar_user	*user,
	CSA_string		password,
	CSA_string		character_set,
	CSA_string		required_csa_version,
	CSA_session_handle	*session,
	CSA_extension		*logon_extensions
);

/* LOOK UP */ /* NOT IMPLEMENTED IN THIS RELEASE */
CSA_return_code
csa_look_up(
	CSA_session_handle	session,
	CSA_calendar_user	*users,
	CSA_flags		look_up_flags,
	CSA_uint32		*number_users,
	CSA_calendar_user	**user_list,
	CSA_extension		*look_up_extensions
);

#define	CSA_LOOKUP_RESOLVE_PREFIX_SEARCH	((CSA_flags)0x1)
#define	CSA_LOOKUP_RESOLVE_IDENTITY		((CSA_flags)0x2)

/* QUERY CONFIGURATION */
CSA_return_code
csa_query_configuration(
	CSA_session_handle	session,
	CSA_enum		item,
	CSA_buffer		*reference,
	CSA_extension		*query_configuration_extensions
);

#define CSA_CONFIG_CHARACTER_SET	((CSA_enum)0)
#define CSA_CONFIG_LINE_TERM		((CSA_enum)1)
#define CSA_CONFIG_DEFAULT_SERVICE	((CSA_enum)2)
#define CSA_CONFIG_DEFAULT_USER		((CSA_enum)3)
#define	CSA_CONFIG_REQ_PASSWORD		((CSA_enum)4)
#define	CSA_CONFIG_REQ_SERVICE		((CSA_enum)5)
#define	CSA_CONFIG_REQ_USER		((CSA_enum)6)
#define	CSA_CONFIG_UI_AVAIL		((CSA_enum)7)
#define	CSA_CONFIG_VER_IMPLEM		((CSA_enum)8)
#define	CSA_CONFIG_VER_SPEC		((CSA_enum)9)

#define	CSA_LINE_TERM_CRLF		((CSA_enum)0)
#define	CSA_LINE_TERM_LF		((CSA_enum)1)
#define	CSA_LINE_TERM_CR		((CSA_enum)2)

#define	CSA_REQUIRED_NO			((CSA_enum)0)
#define	CSA_REQUIRED_OPT		((CSA_enum)1)
#define	CSA_REQUIRED_YES		((CSA_enum)2)

/* CHARACTER SE IDENTIFIERS */
#define	CSA_CHARSET_437		"-//XAPIA//CHARSET IBM 437//EN"
#define	CSA_CHARSET_850		"-//XAPIA//CHARSET IBM 850//EN"
#define	CSA_CHARSET_1252	"-//XAPIA//CHARSET Microsoft 1252//EN"
#define	CSA_CHARSET_ISTRING	"-//XAPIA//CHARSET Apple ISTRING//EN"
#define	CSA_CHARSET_UNICODE	"-//XAPIA//CHARSET UNICODE//EN"
#define	CSA_CHARSET_T61		"-//XAPIA//CHARSET TSS T61//EN"
#define	CSA_CHARSET_IA5		"-//XAPIA//CHARSET TSS IA5//EN"
#define	CSA_CHARSET_ISO_10646	"-//XAPIA//CHARSET ISO 10646//EN"
#define	CSA_CHARSET_ISO_646	"-//XAPIA//CHARSET ISO 646//EN"
#define	CSA_CHARSET_iso_8859_1	"-//XAPIA//CHARSET ISO 8859-1//EN"

/* RESTORE */ /* NOT IMPLEMENTED IN THIS RELEASE */
CSA_return_code
csa_restore(
	CSA_session_handle	session,
	CSA_string		archive_name,
	CSA_uint32		number_attributes,
	CSA_attribute		*attributes,
	CSA_enum		*operators,
	CSA_extension		*restore_extensions
);

/* SAVE */ /* NOT IMPLEMENTED IN THIS RELEASE */
CSA_return_code
csa_save(
	CSA_session_handle	session,
	CSA_string		archive_name,
	CSA_uint32		number_attributes,
	CSA_attribute		*attributes,
	CSA_enum		*operators,
	CSA_boolean		delete_entry,
	CSA_extension		*save_extensions
);

/* ADD CALENDAR */
CSA_return_code
csa_add_calendar(
	CSA_session_handle	session,
	CSA_calendar_user	*user,
	CSA_uint32		number_attributes,
	CSA_attribute		*calendar_attributes,
	CSA_extension		*add_calendar_extensions
);

/* DELETE CALENDAR */
CSA_return_code
csa_delete_calendar(
	CSA_session_handle	session,
	CSA_extension		*delete_calendar_extensions
);

/* LIST CALENDAR ATTRIBUTES */
CSA_return_code
csa_list_calendar_attributes(
	CSA_session_handle	session,
	CSA_uint32		*number_names,
	CSA_attribute_reference	**calendar_attributes_names,
	CSA_extension		*list_calendar_attributes_extensions
);

/* READ CALENDAR ATTRIBUTES */
CSA_return_code
csa_read_calendar_attributes(
	CSA_session_handle	session,
	CSA_uint32		number_names,
	CSA_attribute_reference	*attribte_names,
	CSA_uint32		*number_attributes,
	CSA_attribute		**calendar_attributes,
	CSA_extension		*read_calendar_attributes_extensions
);

/* REGISTER CALLBACK FUNCTION */
CSA_return_code
csa_register_callback(
	CSA_session_handle	session,
	CSA_flags		reason,
	CSA_callback		callback,
	CSA_buffer		client_data,
	CSA_extension		*register_callback_extensions
);

/* UNREGISTER CALLBACK FUNCTION */
CSA_return_code
csa_unregister_callback(
	CSA_session_handle	session,
	CSA_flags		reason,
	CSA_callback		callback,
	CSA_buffer		client_data,
	CSA_extension		*unregister_callback_extensions
);

/* CALL CALLBACK FUNCTION */
CSA_return_code
csa_call_callbacks(
	CSA_session_handle	session,
	CSA_flags		reason,
	CSA_extension		*call_callbacks_extensions
);

/* UDPATE CALENDAR ATTRIBUTES */
CSA_return_code
csa_update_calendar_attributes(
	CSA_session_handle	session,
	CSA_uint32		number_attributes,
	CSA_attribute		*calendar_attributes,
	CSA_extension		*update_calendar_attributes_extensions
);

/* Add ENTRY */
CSA_return_code
csa_add_entry(
	CSA_session_handle	session,
	CSA_uint32		number_attributes,
	CSA_attribute		*entry_attributes,
	CSA_entry_handle	*entry,
	CSA_extension		*add_entry_extensions
);

/* DELETE ENTRY */
CSA_return_code
csa_delete_entry(
	CSA_session_handle	session,
	CSA_entry_handle	entry,
	CSA_enum		delete_scope,
	CSA_extension		*delete_entry_extensions
);

/* FREE TIME SEARCH */ /* NOT IMPLEMENTED IN THIS RELEASE */
CSA_return_code
csa_free_time_search(
	CSA_session_handle	session,
	CSA_date_time_range	date_time_range,
	CSA_time_duration	time_duration,
	CSA_uint32		number_users,
	CSA_calendar_user	*calendar_users,
	CSA_free_time		**free_time,
	CSA_extension		*free_time_search_extensions
);

/* LIST ENTRIES */
CSA_return_code
csa_list_entries(
	CSA_session_handle	session,
	CSA_uint32		number_attributes,
	CSA_attribute		*entry_attributes,
	CSA_enum		*list_operators,
	CSA_uint32		*number_entries,
	CSA_entry_handle	**entries,
	CSA_extension		*list_entries_extensions
);

/* LIST ENTRY ATTRIBUTES */
CSA_return_code
csa_list_entry_attributes(
	CSA_session_handle	session,
	CSA_entry_handle	entry,
	CSA_uint32		*number_names,
	CSA_attribute_reference	**entry_attribute_names,
	CSA_extension		*list_entry_attributes_extensions
);

/* LIST ENTRY SEQUENCE */
CSA_return_code
csa_list_entry_sequence(
	CSA_session_handle	session,
	CSA_entry_handle	entry,
	CSA_date_time_range	time_range,
	CSA_uint32		*number_entries,
	CSA_entry_handle	**entry_list,
	CSA_extension		*list_entry_sequence_extensions
);

/* READ ENTRY ATTRIBUTES */
CSA_return_code
csa_read_entry_attributes(
	CSA_session_handle	session,
	CSA_entry_handle	entry,
	CSA_uint32		number_names,
	CSA_attribute_reference	*attribute_names,
	CSA_uint32		*number_attributes,
	CSA_attribute		**entry_attributes,
	CSA_extension		*read_entry_attributes_extensions
);

/* READ NEXT REMINDERS */
CSA_return_code
csa_read_next_reminder(
	CSA_session_handle	session,
	CSA_uint32		number_names,
	CSA_attribute_reference	*reminder_names,
	CSA_date_time		given_time,
	CSA_uint32		*number_reminders,
	CSA_reminder_reference	**reminder_references,
	CSA_extension		*read_next_reminder_extensions
);

/* UPDATE ENTRY ATTRIBUTES */
CSA_return_code
csa_update_entry_attributes(
	CSA_session_handle	session,
	CSA_entry_handle	entry,
	CSA_enum		update_scope,
	CSA_boolean		update_propagation,
	CSA_uint32		number_attributes,
	CSA_attribute		*entry_attributes,
	CSA_entry_handle	*new_entry,
	CSA_extension		*update_entry_attributes_extensions
);

/* STRING BASED FUNCTIONS */

/* ADD EVENT */ /* NOT IMPLEMENTED IN THIS RELEASE */
CSA_return_code
csa_add_event(
	CSA_service_reference	calendar_service,
	CSA_string		calendar_address,
	CSA_string		logon_user,
	CSA_string		logon_password,
	CSA_string		attendee,
	CSA_enum		attendee_priority,
	CSA_enum		attendee_status,
	CSA_boolean		attendee_rsvp_requested,
	CSA_date_time		start_date,
	CSA_date_time		end_date,
	CSA_string		organizer,
	CSA_string		sponsor,
	CSA_string		summary,
	CSA_string		description,
	CSA_string		recurrence_rule,
	CSA_string		exception_rule,
	CSA_string		subtype,
	CSA_enum		classification,
	CSA_string		delimiters,
	CSA_string		add_event_extensions
);

/* ADD TODO */ /* NOT IMPLEMENTED IN THIS RELEASE */
CSA_return_code
csa_add_todo(
	CSA_service_reference	calendar_service,
	CSA_string		calendar_address,
	CSA_string		logon_user,
	CSA_string		logon_password,
	CSA_enum		attendee_priority,
	CSA_enum		attendee_status,
	CSA_boolean		attendee_rsvp_requested,
	CSA_date_time		start_date,
	CSA_date_time		due_date,
	CSA_uint32		priority,
	CSA_string		summary,
	CSA_string		description,
	CSA_enum		classification,
	CSA_string		delimiters,
	CSA_string		add_todo_extensions
);

/* ADD MEMO */ /* NOT IMPLEMENTED IN THIS RELEASE */
CSA_return_code
csa_add_memo(
	CSA_service_reference	calendar_service,
	CSA_string		calendar_address,
	CSA_string		logon_user,
	CSA_string		logon_password,
	CSA_date_time		start_date,
	CSA_string		summary,
	CSA_string		delimiters,
	CSA_string		add_memo_extensions
);

/* RETURN ERROR FLAGS */
#define	CSA_ERROR_RSV_MASK	((CSA_return_code)0x0000FFFF)
#define	CSA_ERROR_IMPL_MASK	((CSA_return_code)0xFFFF0000)

/* macro to get XAPIA CSA return code */
#define CSA_X_DT_CSA_RETURN_CODE(c) \
	 ((CSA_return_code)((c) & CSA_ERROR_RSV_MASK))

/* RETURN CODES */
#define	CSA_SUCCESS				((CSA_return_code)0)

#define	CSA_E_AMBIGUOUS_USER			((CSA_return_code)1)
#define CSA_E_CALENDAR_EXISTS 			((CSA_return_code)2)
#define CSA_E_CALENDAR_NOT_EXIST		((CSA_return_code)3)
#define CSA_E_CALLBACK_NOT_REGISTERED		((CSA_return_code)4)
#define	CSA_E_DISK_FULL				((CSA_return_code)5)
#define	CSA_E_FAILURE				((CSA_return_code)6)
#define	CSA_E_FILE_EXIST			((CSA_return_code)7)
#define	CSA_E_FILE_NOT_EXIST			((CSA_return_code)8)
#define	CSA_E_INSUFFICIENT_MEMORY		((CSA_return_code)9)
#define	CSA_E_INVALID_ATTRIBUTE			((CSA_return_code)10)
#define	CSA_E_INVALID_ATTRIBUTE_VALUE		((CSA_return_code)11)
#define	CSA_E_INVALID_CALENDAR_SERVICE		((CSA_return_code)12)
#define	CSA_E_INVALID_CONFIGURATION		((CSA_return_code)13)
#define	CSA_E_INVALID_DATA_EXT			((CSA_return_code)14)
#define	CSA_E_INVALID_DATE_TIME			((CSA_return_code)15)
#define	CSA_E_INVALID_ENTRY_HANDLE		((CSA_return_code)16)
#define	CSA_E_INVALID_ENUM			((CSA_return_code)17)
#define	CSA_E_INVALID_FILE_NAME			((CSA_return_code)18)
#define	CSA_E_INVALID_FLAG			((CSA_return_code)19)
#define	CSA_E_INVALID_FUNCTION_EXT		((CSA_return_code)20)
#define	CSA_E_INVALID_MEMORY			((CSA_return_code)21)
#define	CSA_E_INVALID_PARAMETER			((CSA_return_code)22)
#define	CSA_E_INVALID_PASSWORD			((CSA_return_code)23)
#define	CSA_E_INVALID_RULE			((CSA_return_code)24)
#define	CSA_E_INVALID_SESSION_HANDLE		((CSA_return_code)25)
#define	CSA_E_INVALID_USER			((CSA_return_code)26)
#define	CSA_E_NO_AUTHORITY			((CSA_return_code)27)
#define	CSA_E_NOT_SUPPORTED			((CSA_return_code)28)
#define	CSA_E_PASSWORD_REQUIRED			((CSA_return_code)29)
#define	CSA_E_READONLY				((CSA_return_code)30)
#define	CSA_E_SERVICE_UNAVAILABLE		((CSA_return_code)31)
#define	CSA_E_TEXT_TOO_LARGE			((CSA_return_code)32)
#define	CSA_E_TOO_MANY_USERS			((CSA_return_code)33)
#define	CSA_E_UNABLE_TO_OPEN_FILE		((CSA_return_code)34)
#define	CSA_E_UNSUPPORTED_ATTRIBUTE		((CSA_return_code)35)
#define	CSA_E_UNSUPPORTED_CHARACTER_SET		((CSA_return_code)36)
#define	CSA_E_UNSUPPORTED_DATA_EXT		((CSA_return_code)37)
#define	CSA_E_UNSUPPORTED_ENUM			((CSA_return_code)38)
#define	CSA_E_UNSUPPORTED_FLAG			((CSA_return_code)39)
#define	CSA_E_UNSUPPORTED_FUNCTION_EXT		((CSA_return_code)40)
#define	CSA_E_UNSUPPORTED_PARAMETER		((CSA_return_code)41)
#define	CSA_E_UNSUPPORTED_VERSION		((CSA_return_code)42)
#define	CSA_E_USER_NOT_FOUND			((CSA_return_code)43)
#define	CSA_E_TIME_ONLY				((CSA_return_code)44)

#define CSA_X_DT_E_BACKING_STORE_PROBLEM 	((CSA_return_code)0x900001F)
#define CSA_X_DT_E_ENTRY_NOT_FOUND 		((CSA_return_code)0x9010010)
#define CSA_X_DT_E_INVALID_SERVER_LOCATION 	((CSA_return_code)0x902000C)
#define CSA_X_DT_E_SERVER_TIMEOUT 		((CSA_return_code)0x903001F)
#define CSA_X_DT_E_SERVICE_NOT_REGISTERED 	((CSA_return_code)0x904001F)

/* CALENDAR ATTRIBUTES */

extern char *_CSA_calendar_attribute_names[];

/* CALENDAR ATTRIBUTE NAMES */

#define CSA_CAL_ATTR_ACCESS_LIST_I	1
#define CSA_CAL_ATTR_ACCESS_LIST \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_ACCESS_LIST_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Access List//EN" */

#define CSA_CAL_ATTR_CALENDAR_NAME_I	2
#define CSA_CAL_ATTR_CALENDAR_NAME \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_CALENDAR_NAME_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Calendar Name//EN" */

#define CSA_CAL_ATTR_CALENDAR_OWNER_I	3
#define CSA_CAL_ATTR_CALENDAR_OWNER \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_CALENDAR_OWNER_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Calendar Owner//EN" */

#define CSA_CAL_ATTR_CALENDAR_SIZE_I	4
#define CSA_CAL_ATTR_CALENDAR_SIZE \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_CALENDAR_SIZE_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Calendar Size//EN" */

#define CSA_CAL_ATTR_CHARACTER_SET_I	5
#define CSA_CAL_ATTR_CHARACTER_SET \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_CHARACTER_SET_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Character Set//EN" */

#define CSA_CAL_ATTR_COUNTRY_I		6
#define CSA_CAL_ATTR_COUNTRY \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_COUNTRY_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Country//EN" */

#define CSA_CAL_ATTR_DATE_CREATED_I	7
#define CSA_CAL_ATTR_DATE_CREATED \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_DATE_CREATED_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Date Created//EN" */

#define CSA_CAL_ATTR_LANGUAGE_I		8
#define CSA_CAL_ATTR_LANGUAGE \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_LANGUAGE_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Language//EN" */

#define CSA_CAL_ATTR_NUMBER_ENTRIES_I	9
#define CSA_CAL_ATTR_NUMBER_ENTRIES \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_NUMBER_ENTRIES_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Number Entries//EN" */

#define CSA_CAL_ATTR_PRODUCT_IDENTIFIER_I	10
#define CSA_CAL_ATTR_PRODUCT_IDENTIFIER \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_PRODUCT_IDENTIFIER_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Product Identifier//EN" */

#define CSA_CAL_ATTR_TIME_ZONE_I	11
#define CSA_CAL_ATTR_TIME_ZONE \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_TIME_ZONE_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Time Zone//EN" */

#define CSA_CAL_ATTR_VERSION_I		12
#define CSA_CAL_ATTR_VERSION \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_VERSION_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Version//EN" */

#define CSA_CAL_ATTR_WORK_SCHEDULE_I	13
#define CSA_CAL_ATTR_WORK_SCHEDULE \
	 _CSA_calendar_attribute_names[CSA_CAL_ATTR_WORK_SCHEDULE_I]
  /* "-//XAPIA/CSA/CALATTR//NONSGML Work Schedule//EN" */

#define CSA_X_DT_CAL_ATTR_SERVER_VERSION_I	14
#define CSA_X_DT_CAL_ATTR_SERVER_VERSION \
	 _CSA_calendar_attribute_names[CSA_X_DT_CAL_ATTR_SERVER_VERSION_I]
  /* "-//CDE_XAPIA_PRIVATE/CSA/CALATTR//NONSGML Server Version//EN" */

#define CSA_X_DT_CAL_ATTR_DATA_VERSION_I	15
#define CSA_X_DT_CAL_ATTR_DATA_VERSION \
	 _CSA_calendar_attribute_names[CSA_X_DT_CAL_ATTR_DATA_VERSION_I]
  /* "-//CDE_XAPIA_PRIVATE/CSA/CALATTR//NONSGML Data Version//EN" */

#define CSA_X_DT_CAL_ATTR_CAL_DELIMITER_I	16
#define CSA_X_DT_CAL_ATTR_CAL_DELIMITER \
	 _CSA_calendar_attribute_names[CSA_X_DT_CAL_ATTR_CAL_DELIMITER_I]
  /* "-//CDE_XAPIA_PRIVATE/CSA/CALATTR//NONSGML Calendar Delimiter//EN" */


/* ENTRY ATTRIBUTES */

extern char *_CSA_entry_attribute_names[];

/* ENTRY ATTRIBUTES NAMES */

#define CSA_ENTRY_ATTR_ATTENDEE_LIST_I		1
#define CSA_ENTRY_ATTR_ATTENDEE_LIST \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_ATTENDEE_LIST_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Attendee List//EN" */

#define CSA_ENTRY_ATTR_AUDIO_REMINDER_I 	2
#define CSA_ENTRY_ATTR_AUDIO_REMINDER \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_AUDIO_REMINDER_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Audio Reminder//EN" */

#define CSA_ENTRY_ATTR_CLASSIFICATION_I		3
#define CSA_ENTRY_ATTR_CLASSIFICATION \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_CLASSIFICATION_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Classification//EN" */

#define CSA_ENTRY_ATTR_DATE_COMPLETED_I		4
#define CSA_ENTRY_ATTR_DATE_COMPLETED \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_DATE_COMPLETED_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Date Completed//EN" */

#define CSA_ENTRY_ATTR_DATE_CREATED_I		5
#define CSA_ENTRY_ATTR_DATE_CREATED \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_DATE_CREATED_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Date Created//EN" */

#define CSA_ENTRY_ATTR_DESCRIPTION_I		6
#define CSA_ENTRY_ATTR_DESCRIPTION \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_DESCRIPTION_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Description//EN" */

#define CSA_ENTRY_ATTR_DUE_DATE_I		7
#define CSA_ENTRY_ATTR_DUE_DATE \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_DUE_DATE_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Due Date//EN" */

#define CSA_ENTRY_ATTR_END_DATE_I		8
#define CSA_ENTRY_ATTR_END_DATE \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_END_DATE_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML End Date//EN" */

#define CSA_ENTRY_ATTR_EXCEPTION_DATES_I	9
#define CSA_ENTRY_ATTR_EXCEPTION_DATES \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_EXCEPTION_DATES_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Exception Dates//EN" */

#define CSA_ENTRY_ATTR_EXCEPTION_RULE_I		10
#define CSA_ENTRY_ATTR_EXCEPTION_RULE \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_EXCEPTION_RULE_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Exception Rule//EN" */

#define CSA_ENTRY_ATTR_FLASHING_REMINDER_I	11
#define CSA_ENTRY_ATTR_FLASHING_REMINDER \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_FLASHING_REMINDER_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Flashing Reminder//EN" */

#define CSA_ENTRY_ATTR_LAST_UPDATE_I		12
#define CSA_ENTRY_ATTR_LAST_UPDATE \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_LAST_UPDATE_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Last Update//EN" */

#define CSA_ENTRY_ATTR_MAIL_REMINDER_I		13
#define CSA_ENTRY_ATTR_MAIL_REMINDER \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_MAIL_REMINDER_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Mail Reminder//EN" */

#define CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I	14
#define CSA_ENTRY_ATTR_NUMBER_RECURRENCES \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Number Recurrences//EN" */

#define CSA_ENTRY_ATTR_ORGANIZER_I		15
#define CSA_ENTRY_ATTR_ORGANIZER \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_ORGANIZER_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Organizer//EN" */

#define CSA_ENTRY_ATTR_POPUP_REMINDER_I		16
#define CSA_ENTRY_ATTR_POPUP_REMINDER \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_POPUP_REMINDER_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Popup Reminder//EN" */

#define CSA_ENTRY_ATTR_PRIORITY_I		17
#define CSA_ENTRY_ATTR_PRIORITY \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_PRIORITY_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Priority//EN" */

#define CSA_ENTRY_ATTR_RECURRENCE_RULE_I	18
#define CSA_ENTRY_ATTR_RECURRENCE_RULE \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_RECURRENCE_RULE_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Recurrence Rule//EN" */

#define CSA_ENTRY_ATTR_RECURRING_DATES_I	19
#define CSA_ENTRY_ATTR_RECURRING_DATES \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_RECURRING_DATES_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Recurring Dates//EN" */

#define CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I	20
#define CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Reference Identifier//EN" */

#define CSA_ENTRY_ATTR_SEQUENCE_NUMBER_I	21
#define CSA_ENTRY_ATTR_SEQUENCE_NUMBER \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_SEQUENCE_NUMBER_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Sequence Number//EN" */

#define CSA_ENTRY_ATTR_SPONSOR_I		22
#define CSA_ENTRY_ATTR_SPONSOR \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_SPONSOR_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Sponsor//EN" */

#define CSA_ENTRY_ATTR_START_DATE_I		23
#define CSA_ENTRY_ATTR_START_DATE \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_START_DATE_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Start Date//EN" */

#define CSA_ENTRY_ATTR_STATUS_I			24
#define CSA_ENTRY_ATTR_STATUS \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_STATUS_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Status//EN" */

#define CSA_ENTRY_ATTR_SUBTYPE_I		25
#define CSA_ENTRY_ATTR_SUBTYPE \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_SUBTYPE_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Subtype//EN" */

#define CSA_ENTRY_ATTR_SUMMARY_I		26
#define CSA_ENTRY_ATTR_SUMMARY \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_SUMMARY_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Summary//EN" */

#define CSA_ENTRY_ATTR_TIME_TRANSPARENCY_I	27
#define CSA_ENTRY_ATTR_TIME_TRANSPARENCY \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_TIME_TRANSPARENCY_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Time Transparency//EN" */

#define CSA_ENTRY_ATTR_TYPE_I			28
#define CSA_ENTRY_ATTR_TYPE \
	 _CSA_entry_attribute_names[CSA_ENTRY_ATTR_TYPE_I]
  /* "-//XAPIA/CSA/ENTRYATTR//NONSGML Type//EN" */

/* cde defined entry attributes */
#define CSA_X_DT_ENTRY_ATTR_SHOWTIME_I			29
#define CSA_X_DT_ENTRY_ATTR_SHOWTIME \
	 _CSA_entry_attribute_names[CSA_X_DT_ENTRY_ATTR_SHOWTIME_I]
  /* "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Show Time//EN" */

#define CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I		30
#define	CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE \
	 _CSA_entry_attribute_names[CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I]
  /* "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Type//EN" */

#define CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I		31
#define	CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES \
	 _CSA_entry_attribute_names[CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I]
  /* "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Times//EN" */

#define CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I		32
#define CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL \
	 _CSA_entry_attribute_names[CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I]
  /* "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Interval//EN" */

#define CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I	33
#define CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM \
	 _CSA_entry_attribute_names[CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I]
  /*"-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Occurrence Number//EN"*/

#define CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I		34
#define	CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE \
	 _CSA_entry_attribute_names[CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I]
  /* "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Sequence End Date//EN" */

#define CSA_X_DT_ENTRY_ATTR_ENTRY_DELIMITER_I		35
#define CSA_X_DT_ENTRY_ATTR_ENTRY_DELIMITER \
	 _CSA_entry_attribute_names[CSA_X_DT_ENTRY_ATTR_ENTRY_DELIMITER_I]
  /* "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Entry Delimiter//EN" */

/*
 * values for entry attribute CSA_ENTRY_ATTR_TYPE
 */

#define	CSA_TYPE_EVENT		((CSA_enum)0)
#define	CSA_TYPE_TODO		((CSA_enum)1)
#define	CSA_TYPE_MEMO		((CSA_enum)2)
#define CSA_X_DT_TYPE_OTHER	((CSA_enum)900)

/*
 * values for entry attribute CSA_ENTRY_ATTR_SUBTYPE
 */

extern char *_CSA_entry_subtype_values[];

#define	CSA_SUBTYPE_APPOINTMENT_I	0
#define	CSA_SUBTYPE_APPOINTMENT	\
	 _CSA_entry_subtype_values[CSA_SUBTYPE_APPOINTMENT_I]
  /* "-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Appointment//EN" */

#define	CSA_SUBTYPE_CLASS_I		1
#define	CSA_SUBTYPE_CLASS \
	 _CSA_entry_subtype_values[CSA_SUBTYPE_CLASS_I]
  /* "-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Class//EN" */

#define	CSA_SUBTYPE_HOLIDAY_I		2
#define	CSA_SUBTYPE_HOLIDAY \
	 _CSA_entry_subtype_values[CSA_SUBTYPE_HOLIDAY_I]
  /* "-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Holiday//EN" */

#define CSA_SUBTYPE_MEETING_I		3
#define CSA_SUBTYPE_MEETING \
	 _CSA_entry_subtype_values[CSA_SUBTYPE_MEETING_I]
  /* "-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Meeting//EN" */

#define	CSA_SUBTYPE_MISCELLANEOUS_I	4
#define	CSA_SUBTYPE_MISCELLANEOUS \
	 _CSA_entry_subtype_values[CSA_SUBTYPE_MISCELLANEOUS_I]
  /* "-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Miscellaneous//EN" */

#define	CSA_SUBTYPE_PHONE_CALL_I	5
#define	CSA_SUBTYPE_PHONE_CALL \
	 _CSA_entry_subtype_values[CSA_SUBTYPE_PHONE_CALL_I]
  /* "-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Phone Call//EN" */

#define	CSA_SUBTYPE_SICK_DAY_I		6
#define	CSA_SUBTYPE_SICK_DAY \
	 _CSA_entry_subtype_values[CSA_SUBTYPE_SICK_DAY_I]
  /* "-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Sick Day//EN" */

#define	CSA_SUBTYPE_SPECIAL_OCCASION_I	7
#define	CSA_SUBTYPE_SPECIAL_OCCASION \
	 _CSA_entry_subtype_values[CSA_SUBTYPE_SPECIAL_OCCASION_I]
  /* "-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Special Occasion//EN" */

#define	CSA_SUBTYPE_TRAVEL_I		8
#define	CSA_SUBTYPE_TRAVEL \
	 _CSA_entry_subtype_values[CSA_SUBTYPE_TRAVEL_I]
  /* "-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Travel//EN" */

#define	CSA_SUBTYPE_VACATION_I		9
#define	CSA_SUBTYPE_VACATION \
	 _CSA_entry_subtype_values[CSA_SUBTYPE_VACATION_I]
  /* "-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Vacation//EN" */

/*
 * special value for CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES
 */

#define	CSA_X_DT_DT_REPEAT_FOREVER	0

/*
 * Values for entry attribute CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE
 */

#define	CSA_X_DT_REPEAT_ONETIME			((CSA_enum)0)
#define	CSA_X_DT_REPEAT_DAILY			((CSA_enum)1)
#define	CSA_X_DT_REPEAT_WEEKLY			((CSA_enum)2)
#define	CSA_X_DT_REPEAT_BIWEEKLY		((CSA_enum)3)
#define	CSA_X_DT_REPEAT_MONTHLY_BY_WEEKDAY	((CSA_enum)4)
#define	CSA_X_DT_REPEAT_MONTHLY_BY_DATE		((CSA_enum)5)
#define	CSA_X_DT_REPEAT_YEARLY			((CSA_enum)6)
#define	CSA_X_DT_REPEAT_EVERY_NDAY		((CSA_enum)7)
#define	CSA_X_DT_REPEAT_EVERY_NWEEK		((CSA_enum)8)
#define	CSA_X_DT_REPEAT_EVERY_NMONTH		((CSA_enum)9)
#define	CSA_X_DT_REPEAT_MON_TO_FRI		((CSA_enum)10)
#define	CSA_X_DT_REPEAT_MONWEDFRI		((CSA_enum)11)
#define	CSA_X_DT_REPEAT_TUETHUR			((CSA_enum)12)
#define	CSA_X_DT_REPEAT_WEEKDAYCOMBO		((CSA_enum)13)
#define	CSA_X_DT_REPEAT_OTHER			((CSA_enum)14)
#define	CSA_X_DT_REPEAT_OTHER_WEEKLY		((CSA_enum)15)
#define	CSA_X_DT_REPEAT_OTHER_MONTHLY		((CSA_enum)16)
#define	CSA_X_DT_REPEAT_OTHER_YEARLY		((CSA_enum)17)

/* COMMON EXTENSIONS DECLARATIONS */

/* EXTENSION SET ID */

/* Common Extension Set */
#define	CSA_XS_COM			((CSA_uint32)0)

/* Bilateral Extension Set */
#define	CSA_XS_BLT			((CSA_uint32)256)

/* FUNCTION EXTENSIONS */

/* Query for extension support in implementation */

#define	CSA_X_COM_SUPPORT_EXT		((CSA_uint32)1)

typedef struct CSA_TAG_XCOM {
	CSA_uint32	item_code;
	CSA_flags	flags;
} CSA_X_COM_support;

#define	CSA_X_COM_SUPPORTED		((CSA_flags)0x1)
#define	CSA_X_COM_NOT_SUPPORTED		((CSA_flags)0x2)
#define	CSA_X_COM_DATA_EXT_SUPPORTED	((CSA_flags)0x4)
#define	CSA_X_COM_FUNC_EXT_SUPPORTED	((CSA_flags)0x8)
#define	CSA_X_COM_SUP_EXCLUDE		((CSA_flags)0x10)

/* UI_ID EXTENSION */

#define CSA_X_UI_ID_EXT			((CSA_uint32)2)

/* Extension Flags */
#define CSA_X_LOGON_UI_ALLOWED		((CSA_flags)0x1)
#define CSA_X_ERROR_UI_ALLOWED		((CSA_flags)0x2)
#define CSA_X_LOOKUP_RESOLVE_UI		((CSA_flags)0x4)
#define CSA_X_LOOKUP_DETAILS_UI		((CSA_flags)0x8)
#define CSA_X_LOOKUP_ADDRESSING_UI	((CSA_flags)0x10)
#define CSA_X_ADD_DEFINE_ENTRY_UI	((CSA_flags)0x20)

/* Extension Return Codes */
#define	CSA_X_E_INVALID_UI_ID		((CSA_return_code)1025)
#define	CSA_X_E_LOGON_FAILURE		((CSA_return_code)1026)
#define	CSA_X_E_USER_CANCEL		((CSA_return_code)1027)

/* XT APPLICATION CONTEXT EXTENSION */

#define CSA_X_XT_APP_CONTEXT_EXT	((CSA_uint32)3)

/* Get User access extension */
#define	CSA_XS_DT			((CSA_uint32)900)
#define	CSA_X_DT_GET_USER_ACCESS_EXT	((CSA_uint32)901)

#endif

