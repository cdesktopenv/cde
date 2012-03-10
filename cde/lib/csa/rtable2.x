/* $XConsortium: rtable2.x /main/1 1996/04/21 19:24:30 drk $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifdef RPC_HDR
%#include "connection.h"
%
#endif

typedef string Buffer_2<>;

enum Transaction_2 {
	add_2, cm_remove_2
};

enum Period_2 {
	single_2, daily_2, weekly_2, biweekly_2, monthly_2, yearly_2,
	nthWeekday_2, everyNthDay_2, everyNthWeek_2,
	everyNthMonth_2, otherPeriod_2
};

enum Tag_2{
	appointment_2, reminder_2, otherTag_2
};

struct Attribute_2 {
        struct Attribute_2 *next;
        Buffer_2          attr;
        Buffer_2          value;
};
typedef Attribute_2 *Attr_2;

struct Except_2 {
	int	ordinal;
	struct Except_2 *next;
};
typedef Except_2 *Exception_2;

struct Id_2 {
	time_t	tick;			/* user specified time stored as GMT */
	long	key;			/* system assigned id */
};

struct Uid_2 {
	struct Id_2	appt_id;
	struct Uid_2	*next;
};

struct Appt_2 {
	struct Id_2	appt_id;	/* appointment/repeater id */
	Tag_2	tag;		/* appointment or otherTag */
	int		duration;	/* appt duration in seconds */
	int		ntimes;		/* n repeat times (0 .. forever) */
	Buffer_2		what;		/* text of appointment */
	Buffer_2		mailto;		/* addressee of mail reminder */
	Buffer_2		script;		/* text of unix script for script reminder */
	Period_2		period;		/* periodicity of event: single default */
	Buffer_2		author;		/* user who inserted the appt */
	Buffer_2		client_data;	/* TBD. */
	struct Except_2	*exception;	/* list of exceptions to repeating events */
	struct Attribute_2 *attr;		/* list of reminder attributes */
	struct Appt_2	*next;		/* next appointment */
};

struct Abb_Appt_2 {
	struct Id_2	appt_id;
	Buffer_2		what;
	int		duration;
	Period_2		period;
	struct Abb_Appt_2	*next;
};

struct Apptid_2 {
	struct Id_2	*oid;		/* old appt key */
	struct Appt_2	*new_appt;	/* new appt */
};

struct Reminder_2 {
	struct Id_2	appt_id;	/* actual appt. key */
	long		tick;		/* the happening tick */
	Attribute_2	attr;		/* (attr, value) */
	struct Reminder_2	*next;
};


enum Table_Res_Type_2 {AP_2, RM_2, AB_2, ID_2};

union Table_Res_List_2 switch (Table_Res_Type_2 tag) { 
	case AP_2: 
		Appt_2 *a; 
	case RM_2:
		Reminder_2 *r;
	case AB_2:
		Abb_Appt_2 *b;
	case ID_2:
		Uid_2 *i;
	default: 
		void; 
};

enum Access_Status_2 {
	access_ok_2,
	access_added_2,
	access_removed_2,
	access_failed_2,
	access_exists_2,
	access_partial_2,
	access_other_2
};

struct Table_Res_2 {
	Access_Status_2	status;
	Table_Res_List_2	res;
};

%#define access_none_2	0x0	/* owner only */
%#define access_read_2	0x1
%#define access_write_2	0x2
%#define access_delete_2	0x4
%#define access_exec_2	0x8	/* execution permission is a hack! */
%#define VOIDPID	-1	/* pre-V3 tools do present pids */

struct Access_Entry_2 {
	Buffer_2 who; 
	int access_type;	/* Bit mask from access_read,write,delete */
	Access_Entry_2 *next;
};
	
struct Access_Args_2 {
	Buffer_2 target;
	Access_Entry_2 *access_list;
};

struct Range_2 {
	long key1;		/* lower bound tick */
	long key2;		/* upper bound tick */
	struct Range_2 *next;
};

enum Table_Args_Type_2 {TICK_2, APPTID_2, UID_2, APPT_2, RANGE_2};

union Args_2 switch (Table_Args_Type_2 tag) {
	case TICK_2:
		time_t		tick;
	case APPTID_2:
		Apptid_2	apptid;
	case UID_2:
		Uid_2		*key;
	case APPT_2:
		Appt_2		*appt;
	case RANGE_2:
		Range_2		*range;
};

struct Table_Args_2 {
	Buffer_2 target;
	Args_2 args;
};

struct Registration_2 {
	Buffer_2	target;
        u_long  prognum;
        u_long  versnum;
        u_long  procnum;
	struct	Registration_2 *next;
};

enum Table_Status_2 {ok_2, duplicate_2, badtable_2, notable_2, denied_2, other_2};
enum Registration_Status_2 {registered_2, failed_2, deregistered_2, confused_2};

program TABLEPROG {
	version TABLEVERS_2 {
		void rtable_ping(void) = 0;
		Table_Res_2 rtable_lookup(Table_Args_2) = 1;
		Table_Res_2 rtable_lookup_next_larger(Table_Args_2) = 2;
		Table_Res_2 rtable_lookup_next_smaller(Table_Args_2) = 3;
		Table_Res_2 rtable_lookup_range(Table_Args_2) = 4;
		Table_Res_2 rtable_abbreviated_lookup_range(Table_Args_2) = 5;
		Table_Res_2 rtable_insert(Table_Args_2) = 6;
		Table_Res_2 rtable_delete(Table_Args_2) = 7;
		Table_Res_2 rtable_delete_instance(Table_Args_2) = 8;
		Table_Res_2 rtable_change(Table_Args_2) = 9;
		Table_Res_2 rtable_change_instance(Table_Args_2) = 10;
		Table_Res_2 rtable_lookup_next_reminder(Table_Args_2) = 11;
		Table_Status_2 rtable_check(Table_Args_2) = 12;
		Table_Status_2 rtable_flush_table(Table_Args_2) = 13;
		int rtable_size(Table_Args_2) = 14;
		Registration_Status_2 register_callback(Registration_2) = 15;
		Registration_Status_2 deregister_callback(Registration_2) = 16;
		Access_Status_2 rtable_set_access(Access_Args_2) = 17;
		Access_Args_2 rtable_get_access(Access_Args_2) = 18;
	} = 2;
} = 100068; 

