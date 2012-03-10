/* $XConsortium: rtable3.x /main/1 1996/04/21 19:24:37 drk $ */
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

typedef string Buffer_3<>;

enum Transaction_3 {
	add_3, cm_remove_3
};

enum Interval_3 {
	single_3, daily_3, weekly_3, biweekly_3, monthly_3, yearly_3,
	nthWeekday_3, everyNthDay_3, everyNthWeek_3,
	everyNthMonth_3, otherPeriod_3
};

struct Period_3 {
	Interval_3 period;
	int	nth;
};

enum Event_Type_3 {
	appointment_3, reminder_3, otherTag_3, holiday_3, toDo_3
};

struct Tag_3 {
	Event_Type_3 tag;
	int showtime;		/* Advisory for formatting time */
	struct Tag_3 *next;
};

enum Privacy_Level_3 {
	public_3, private_3, semiprivate_3
};

struct Attribute_3 {
        struct Attribute_3 *next;
        Buffer_3          attr;
        Buffer_3          value;
	Buffer_3		clientdata;
};
typedef Attribute_3 *Attr_3;

struct Except_3 {
	int	ordinal;
	struct Except_3 *next;
};
typedef Except_3 *Exception_3;

struct Id_3 {
	time_t	tick;			/* user specified time stored as GMT */
	long	key;			/* system assigned id */
};

struct Uid_3 {
	struct Id_3	appt_id;
	struct Uid_3	*next;
};

enum Appt_Status_3 {
	active_3, pendingAdd_3, pendingDelete_3, committed_3,
	cancelled_3, completed_3
};

struct Appt_3 {
	struct Id_3	appt_id;	/* appointment/repeater id */
	struct Tag_3	*tag;		/* event type & advisory time display */
	int		duration;	/* appt duration in seconds */
	int		ntimes;		/* n repeat times (0 .. forever) */
	Buffer_3	what;		/* text of appointment */
	struct Period_3	period;		/* periodicity of event: single default */
	Buffer_3	author;		/* user who inserted the appt */
	Buffer_3		client_data;	/* TBD. */
	struct Except_3	*exception;	/* list of exceptions to repeating events */
	struct Attribute_3 *attr;		/* list of reminder attributes */
	Appt_Status_3	appt_status;
	Privacy_Level_3	privacy;
	struct Appt_3	*next;		/* next appointment */
};

struct Abb_Appt_3 {
	struct Id_3	appt_id;
	struct Tag_3	*tag;
	Buffer_3		what;
	int		duration;
	struct Period_3	period;
	struct Abb_Appt_3	*next;
	Appt_Status_3	appt_status;
	Privacy_Level_3	privacy;
};

struct Apptid_3 {
	struct Id_3	*oid;		/* old appt key */
	struct Appt_3	*new_appt;	/* new appt */
};

struct Reminder_3 {
	struct Id_3	appt_id;	/* actual appt. key */
	time_t		tick;		/* the happening tick */
	Attribute_3	attr;		/* (attr, value) */
	struct Reminder_3	*next;
};

enum Table_Res_Type_3 {AP_3, RM_3, AB_3, ID_3};

union Table_Res_List_3 switch (Table_Res_Type_3 tag) { 
	case AP_3: 
		Appt_3 *a; 
	case RM_3:
		Reminder_3 *r;
	case AB_3:
		Abb_Appt_3 *b;
	case ID_3:
		Uid_3 *i;
	default: 
		void; 
};

enum Access_Status_3 {
	access_ok_3,
	access_added_3,
	access_removed_3,
	access_failed_3,
	access_exists_3,
	access_partial_3,
	access_other_3
};

struct Table_Res_3 {
	Access_Status_3	status;
	Table_Res_List_3	res;
};

%#define access_none_3   0x0     /* owner only */
%#define access_read_3   0x1
%#define access_write_3  0x2
%#define access_delete_3 0x4
%#define access_exec_3   0x8     /* execution permission is a hack! */

struct Access_Entry_3 {
	Buffer_3 who; 
	int access_type;	/* Bit mask from access_read,write,delete */
	Access_Entry_3 *next;
};
	
struct Access_Args_3 {
	Buffer_3 target;
	Access_Entry_3 *access_list;
};

struct Range_3 {
	long key1;		/* lower bound tick */
	long key2;		/* upper bound tick */
	struct Range_3 *next;
};

struct Keyrange_3 {
	long key;		/* key of appt */
	time_t tick1;		/* lower bound tick */
	time_t tick2;		/* upper bound tick */
	struct Keyrange_3 *next;
};

enum Table_Args_Type_3 {TICK_3, APPTID_3, UID_3, APPT_3, RANGE_3, KEYRANGE_3};

union Args_3 switch (Table_Args_Type_3 tag) {
	case TICK_3:
		time_t		tick;
	case APPTID_3:
		Apptid_3	apptid;
	case UID_3:
		Uid_3		*key;
	case APPT_3:
		Appt_3		*appt;
	case RANGE_3:
		Range_3		*range;
	case KEYRANGE_3:
		Keyrange_3	*keyrange;
};

struct Table_Args_3 {
	Buffer_3 target;
	Args_3 args;
	int pid;
};

struct Registration_3 {
	Buffer_3	target;
        u_long  prognum;
        u_long  versnum;
        u_long  procnum;
	struct	Registration_3 *next;
	int	pid;
};

enum Table_Status_3 {ok_3, duplicate_3, badtable_3, notable_3, denied_3, other_3};
enum Registration_Status_3 {registered_3, failed_3, deregistered_3, confused_3};

program TABLEPROG {
	version TABLEVERS_3 {
		void rtable_ping(void)=0;
		Table_Res_3 rtable_lookup(Table_Args_3) = 1;
		Table_Res_3 rtable_lookup_next_larger(Table_Args_3) = 2;
		Table_Res_3 rtable_lookup_next_smaller(Table_Args_3) = 3;
		Table_Res_3 rtable_lookup_range(Table_Args_3) = 4;
		Table_Res_3 rtable_abbreviated_lookup_range(Table_Args_3) = 5;
		Table_Res_3 rtable_insert(Table_Args_3) = 6;
		Table_Res_3 rtable_delete(Table_Args_3) = 7;
		Table_Res_3 rtable_delete_instance(Table_Args_3) = 8;
		Table_Res_3 rtable_change(Table_Args_3) = 9;
		Table_Res_3 rtable_change_instance(Table_Args_3) = 10;
		Table_Res_3 rtable_lookup_next_reminder(Table_Args_3) = 11;
		Table_Status_3 rtable_check(Table_Args_3) = 12;
		Table_Status_3 rtable_flush_table(Table_Args_3) = 13;
		int rtable_size(Table_Args_3) = 14;
		Registration_Status_3 register_callback(Registration_3) = 15;
		Registration_Status_3 deregister_callback(Registration_3) = 16;
		Access_Status_3 rtable_set_access(Access_Args_3) = 17;
		Access_Args_3 rtable_get_access(Access_Args_3) = 18;
		Table_Res_3 rtable_abbreviated_lookup_key_range(Table_Args_3) = 19;
		long rtable_gmtoff(void) = 20;
	} = 3;
} = 100068; 

