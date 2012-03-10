/* $XConsortium: rtable4.x /main/1 1996/04/21 19:24:43 drk $ */
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

typedef string Buffer_4<>;

enum Interval_4 {
	single_4, daily_4, weekly_4, biweekly_4, monthly_4, yearly_4,
	nthWeekday_4, everyNthDay_4, everyNthWeek_4,
	everyNthMonth_4, otherPeriod_4,
	monThruFri_4, monWedFri_4, tueThur_4, daysOfWeek_4
};

struct Period_4 {
	Interval_4 period;
	int	nth;
	time_t	enddate;
};

enum Event_Type_4 {
	appointment_4, reminder_4, otherTag_4, holiday_4, toDo_4
};

enum Options_4 {
	do_all_4, do_one_4, do_forward_4
};

struct Tag_4 {
	Event_Type_4 tag;
	int showtime;		/* Advisory for formatting time */
	struct Tag_4 *next;
};

enum Privacy_Level_4 {
	public_4, private_4, semiprivate_4
};

struct Attribute_4 {
        struct Attribute_4 *next;
        Buffer_4          attr;
        Buffer_4          value;
	Buffer_4	clientdata;
};
typedef Attribute_4 *Attr_4;

struct Except_4 {
	int	ordinal;
	struct Except_4 *next;
};
typedef Except_4 *Exception_4;

struct Id_4 {
	time_t	tick;			/* user specified time stored as GMT */
	long	key;			/* system assigned id */
};

struct Uid_4 {
	struct Id_4	appt_id;
	struct Uid_4	*next;
};

enum Appt_Status_4 {
	active_4, pendingAdd_4, pendingDelete_4, committed_4,
	cancelled_4, completed_4
};

struct Appt_4 {
	struct Id_4	appt_id;	/* appointment/repeater id */
	struct Tag_4	*tag;		/* event type & advisory time display */
	int		duration;	/* appt duration in seconds */
	int		ntimes;		/* n repeat times (0 .. forever) */
	Buffer_4	what;		/* text of appointment */
	struct Period_4	period;		/* periodicity of event: single default */
	Buffer_4	author;		/* user who inserted the appt */
	Buffer_4	client_data;	/* TBD. */
	struct Except_4	*exception;	/* list of exceptions to repeating events */
	struct Attribute_4 *attr;		/* list of reminder attributes */
	Appt_Status_4	appt_status;
	Privacy_Level_4	privacy;
	struct Appt_4	*next;		/* next appointment */
};

struct Abb_Appt_4 {
	struct Id_4	appt_id;
	struct Tag_4	*tag;
	Buffer_4		what;
	int		duration;
	struct Period_4	period;
	struct Abb_Appt_4	*next;
	Appt_Status_4	appt_status;
	Privacy_Level_4	privacy;
};

struct Apptid_4 {
	struct Id_4	*oid;		/* old appt key */
	struct Appt_4	*new_appt;	/* new appt */
	Options_4	option;
};

struct Reminder_4 {
	struct Id_4	appt_id;	/* actual appt. key */
	time_t		tick;		/* the happening tick */
	Attribute_4	attr;		/* (attr, value) */
	struct Reminder_4	*next;
};

enum Table_Res_Type_4 {AP_4, RM_4, AB_4, ID_4};

union Table_Res_List_4 switch (Table_Res_Type_4 tag) { 
	case AP_4: 
		Appt_4 *a; 
	case RM_4:
		Reminder_4 *r;
	case AB_4:
		Abb_Appt_4 *b;
	case ID_4:
		Uid_4 *i;
	default: 
		void; 
};

enum Access_Status_4 {
	access_ok_4,
	access_added_4,
	access_removed_4,
	access_failed_4,
	access_exists_4,
	access_partial_4,
	access_other_4,
	access_notable_4,
	access_notsupported_4,
	access_incomplete_4
};

struct Table_Res_4 {
	Access_Status_4	status;
	Table_Res_List_4	res;
};

%#define access_none_4   0x0     /* owner only */
%#define access_read_4   0x1
%#define access_write_4  0x2
%#define access_delete_4 0x4
%#define access_exec_4   0x8     /* execution permission is a hack! */
%#define WORLD "world"	/* special user */

struct Access_Entry_4 {
	Buffer_4 who; 
	int access_type;	/* Bit mask from access_read,write,delete */
	Access_Entry_4 *next;
};
	
struct Access_Args_4 {
	Buffer_4 target;
	Access_Entry_4 *access_list;
};

struct Range_4 {
	long key1;		/* lower bound tick */
	long key2;		/* upper bound tick */
	struct Range_4 *next;
};

struct Keyrange_4 {
	long key;		/* key of appt */
	time_t tick1;		/* lower bound tick */
	time_t tick2;		/* upper bound tick */
	struct Keyrange_4 *next;
};

struct Uidopt_4 {
	struct Id_4	appt_id;
	Options_4		option;
	struct Uidopt_4	*next;
};

enum Table_Args_Type_4 {TICK_4, APPTID_4, UID_4, APPT_4, RANGE_4,
	KEYRANGE_4, UIDOPT_4};

union Args_4 switch (Table_Args_Type_4 tag) {
	case TICK_4:
		time_t		tick;
	case APPTID_4:
		Apptid_4	apptid;
	case UID_4:
		Uid_4		*key;
	case APPT_4:
		Appt_4		*appt;
	case RANGE_4:
		Range_4		*range;
	case KEYRANGE_4:
		Keyrange_4	*keyrange;
	case UIDOPT_4:
		Uidopt_4	*uidopt;
};

struct Table_Args_4 {
	Buffer_4 target;
	Args_4 args;
	int pid;
};

struct Registration_4 {
	Buffer_4	target;
        u_long  prognum;
        u_long  versnum;
        u_long  procnum;
	struct	Registration_4 *next;
	int	pid;
};

struct Table_Op_Args_4 {
	Buffer_4 target;
	Buffer_4 new_target;
};

enum Table_Status_4 {ok_4, duplicate_4, badtable_4, notable_4, denied_4,
	other_4, tbl_not_owner_4, tbl_exist_4, tbl_notsupported_4};
enum Registration_Status_4 {registered_4, failed_4, deregistered_4,
	confused_4, reg_notable_4};

%
%/*
% * rtable_delete and rtable_change take over the functionality of
% * rtable_delete_instance and rtable_change_instance repectively.
% * rtable_delete_instance and rtable_change_instance are now dummy
% * routines exist for backward compatibility purpose and return
% * access_notsupported.
% */
program TABLEPROG {
	version TABLEVERS_4 {
		void rtable_ping(void)=0;
		Table_Res_4 rtable_lookup(Table_Args_4) = 1;
		Table_Res_4 rtable_lookup_next_larger(Table_Args_4) = 2;
		Table_Res_4 rtable_lookup_next_smaller(Table_Args_4) = 3;
		Table_Res_4 rtable_lookup_range(Table_Args_4) = 4;
		Table_Res_4 rtable_abbreviated_lookup_range(Table_Args_4) = 5;
		Table_Res_4 rtable_insert(Table_Args_4) = 6;
		Table_Res_4 rtable_delete(Table_Args_4) = 7;
		Table_Res_4 rtable_delete_instance(Table_Args_4) = 8;
		Table_Res_4 rtable_change(Table_Args_4) = 9;
		Table_Res_4 rtable_change_instance(Table_Args_4) = 10;
		Table_Res_4 rtable_lookup_next_reminder(Table_Args_4) = 11;
		Table_Status_4 rtable_check(Table_Args_4) = 12;
		Table_Status_4 rtable_flush_table(Table_Args_4) = 13;
		int rtable_size(Table_Args_4) = 14;
		Registration_Status_4 register_callback(Registration_4) = 15;
		Registration_Status_4 deregister_callback(Registration_4) = 16;
		Access_Status_4 rtable_set_access(Access_Args_4) = 17;
		Access_Args_4 rtable_get_access(Access_Args_4) = 18;
		Table_Res_4 rtable_abbreviated_lookup_key_range(Table_Args_4) = 19;
		long rtable_gmtoff(void) = 20;
		Table_Status_4 rtable_create(Table_Op_Args_4) = 21;
		Table_Status_4 rtable_remove(Table_Op_Args_4) = 22;
		Table_Status_4 rtable_rename(Table_Op_Args_4) = 23;
	} = 4;
} = 100068; 

