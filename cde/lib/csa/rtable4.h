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
/*******************************************************************************
**
**  rtable4.h
**
**  $TOG: rtable4.h /main/3 1999/10/14 17:48:06 mgreess $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
** (c) Copyright 1996 Digital Equipment Corporation.
** (c) Copyright 1996 Hewlett-Packard Company.
** (c) Copyright 1996 International Business Machines Corp.
** (c) Copyright 1993,1996 Sun Microsystems, Inc.
** (c) Copyright 1996 Novell, Inc. 
** (c) Copyright 1996 FUJITSU LIMITED.
** (c) Copyright 1996 Hitachi.
**
*******************************************************************************/
#ifndef _RTABLE4_H
#define _RTABLE4_H

#ifndef SunOS
#include <rpc/types.h>
#endif


#include <rpc/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "connection.h"

extern bool_t xdr_time_t();

typedef char *Buffer_4;

enum Interval_4 {
	single_4 = 0,
	daily_4 = 1,
	weekly_4 = 2,
	biweekly_4 = 3,
	monthly_4 = 4,
	yearly_4 = 5,
	nthWeekday_4 = 6,
	everyNthDay_4 = 7,
	everyNthWeek_4 = 8,
	everyNthMonth_4 = 9,
	otherPeriod_4 = 10,
	monThruFri_4 = 11,
	monWedFri_4 = 12,
	tueThur_4 = 13,
	daysOfWeek_4 = 14
};
typedef enum Interval_4 Interval_4;

struct Period_4 {
	Interval_4 period;
	int nth;
	time_t enddate;
};
typedef struct Period_4 Period_4;

enum Event_Type_4 {
	appointment_4 = 0,
	reminder_4 = 1,
	otherTag_4 = 2,
	holiday_4 = 3,
	toDo_4 = 4
};
typedef enum Event_Type_4 Event_Type_4;

enum Options_4 {
	do_all_4 = 0,
	do_one_4 = 1,
	do_forward_4 = 2
};
typedef enum Options_4 Options_4;

struct Tag_4 {
	Event_Type_4 tag;
	int showtime;
	struct Tag_4 *next;
};
typedef struct Tag_4 Tag_4;

enum Privacy_Level_4 {
	public_4 = 0,
	private_4 = 1,
	semiprivate_4 = 2
};
typedef enum Privacy_Level_4 Privacy_Level_4;

struct Attribute_4 {
	struct Attribute_4 *next;
	Buffer_4 attr;
	Buffer_4 value;
	Buffer_4 clientdata;
};
typedef struct Attribute_4 Attribute_4;

typedef Attribute_4 *Attr_4;

struct Except_4 {
	int ordinal;
	struct Except_4 *next;
};
typedef struct Except_4 Except_4;

typedef Except_4 *Exception_4;

struct Id_4 {
	time_t tick;
	long key;
};
typedef struct Id_4 Id_4;

struct Uid_4 {
	struct Id_4 appt_id;
	struct Uid_4 *next;
};
typedef struct Uid_4 Uid_4;

enum Appt_Status_4 {
	active_4 = 0,
	pendingAdd_4 = 1,
	pendingDelete_4 = 2,
	committed_4 = 3,
	cancelled_4 = 4,
	completed_4 = 5
};
typedef enum Appt_Status_4 Appt_Status_4;

struct Appt_4 {
	struct Id_4 appt_id;
	struct Tag_4 *tag;
	int duration;
	int ntimes;
	Buffer_4 what;
	struct Period_4 period;
	Buffer_4 author;
	Buffer_4 client_data;
	struct Except_4 *exception;
	struct Attribute_4 *attr;
	Appt_Status_4 appt_status;
	Privacy_Level_4 privacy;
	struct Appt_4 *next;
};
typedef struct Appt_4 Appt_4;

struct Abb_Appt_4 {
	struct Id_4 appt_id;
	struct Tag_4 *tag;
	Buffer_4 what;
	int duration;
	struct Period_4 period;
	struct Abb_Appt_4 *next;
	Appt_Status_4 appt_status;
	Privacy_Level_4 privacy;
};
typedef struct Abb_Appt_4 Abb_Appt_4;

struct Apptid_4 {
	struct Id_4 *oid;
	struct Appt_4 *new_appt;
	Options_4 option;
};
typedef struct Apptid_4 Apptid_4;

struct Reminder_4 {
	struct Id_4 appt_id;
	time_t tick;
	Attribute_4 attr;
	struct Reminder_4 *next;
};
typedef struct Reminder_4 Reminder_4;

enum Table_Res_Type_4 {
	AP_4 = 0,
	RM_4 = 1,
	AB_4 = 2,
	ID_4 = 3
};
typedef enum Table_Res_Type_4 Table_Res_Type_4;

struct Table_Res_List_4 {
	Table_Res_Type_4 tag;
	union {
		Appt_4 *a;
		Reminder_4 *r;
		Abb_Appt_4 *b;
		Uid_4 *i;
	} Table_Res_List_4_u;
};
typedef struct Table_Res_List_4 Table_Res_List_4;

enum Access_Status_4 {
	access_ok_4 = 0,
	access_added_4 = 1,
	access_removed_4 = 2,
	access_failed_4 = 3,
	access_exists_4 = 4,
	access_partial_4 = 5,
	access_other_4 = 6,
	access_notable_4 = 7,
	access_notsupported_4 = 8,
	access_incomplete_4 = 9
};
typedef enum Access_Status_4 Access_Status_4;

struct Table_Res_4 {
	Access_Status_4 status;
	Table_Res_List_4 res;
};
typedef struct Table_Res_4 Table_Res_4;
#define access_none_4   0x0     /* owner only */
#define access_read_4   0x1
#define access_write_4  0x2
#define access_delete_4 0x4
#define access_exec_4   0x8     /* execution permission is a hack! */
#define WORLD "world"	/* special user */

struct Access_Entry_4 {
	Buffer_4 who;
	int access_type;
	struct Access_Entry_4 *next;
};
typedef struct Access_Entry_4 Access_Entry_4;

struct Access_Args_4 {
	Buffer_4 target;
	Access_Entry_4 *access_list;
};
typedef struct Access_Args_4 Access_Args_4;

struct Range_4 {
	long key1;
	long key2;
	struct Range_4 *next;
};
typedef struct Range_4 Range_4;

struct Keyrange_4 {
	long key;
	time_t tick1;
	time_t tick2;
	struct Keyrange_4 *next;
};
typedef struct Keyrange_4 Keyrange_4;

struct Uidopt_4 {
	struct Id_4 appt_id;
	Options_4 option;
	struct Uidopt_4 *next;
};
typedef struct Uidopt_4 Uidopt_4;

enum Table_Args_Type_4 {
	TICK_4 = 0,
	APPTID_4 = 1,
	UID_4 = 2,
	APPT_4 = 3,
	RANGE_4 = 4,
	KEYRANGE_4 = 5,
	UIDOPT_4 = 6
};
typedef enum Table_Args_Type_4 Table_Args_Type_4;

struct Args_4 {
	Table_Args_Type_4 tag;
	union {
		time_t tick;
		Apptid_4 apptid;
		Uid_4 *key;
		Appt_4 *appt;
		Range_4 *range;
		Keyrange_4 *keyrange;
		Uidopt_4 *uidopt;
	} Args_4_u;
};
typedef struct Args_4 Args_4;

struct Table_Args_4 {
	Buffer_4 target;
	Args_4 args;
	int pid;
};
typedef struct Table_Args_4 Table_Args_4;

struct Registration_4 {
	Buffer_4 target;
	u_long prognum;
	u_long versnum;
	u_long procnum;
	struct Registration_4 *next;
	int pid;
};
typedef struct Registration_4 Registration_4;

struct Table_Op_Args_4 {
	Buffer_4 target;
	Buffer_4 new_target;
};
typedef struct Table_Op_Args_4 Table_Op_Args_4;

enum Table_Status_4 {
	ok_4 = 0,
	duplicate_4 = 1,
	badtable_4 = 2,
	notable_4 = 3,
	denied_4 = 4,
	other_4 = 5,
	tbl_not_owner_4 = 6,
	tbl_exist_4 = 7,
	tbl_notsupported_4 = 8
};
typedef enum Table_Status_4 Table_Status_4;

enum Registration_Status_4 {
	registered_4 = 0,
	failed_4 = 1,
	deregistered_4 = 2,
	confused_4 = 3,
	reg_notable_4 = 4
};
typedef enum Registration_Status_4 Registration_Status_4;

/*
 * rtable_delete and rtable_change take over the functionality of
 * rtable_delete_instance and rtable_change_instance repectively.
 * rtable_delete_instance and rtable_change_instance are now dummy
 * routines exist for backward compatibility purpose and return
 * access_notsupported.
 */

#define	TABLEPROG ((unsigned long)(100068))
#define	TABLEVERS_4 ((unsigned long)(4))

#if defined(__STDC__) || defined(__cplusplus)
#define	rtable_ping ((unsigned long)(0))
extern  void * _DtCm_rtable_ping_4(void *, _DtCm_Connection *);
extern  void * _DtCm_rtable_ping_4_svc(void *, struct svc_req *);
#define	rtable_lookup ((unsigned long)(1))
extern  Table_Res_4 * _DtCm_rtable_lookup_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_lookup_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_lookup_next_larger ((unsigned long)(2))
extern  Table_Res_4 * _DtCm_rtable_lookup_next_larger_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_lookup_next_larger_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_lookup_next_smaller ((unsigned long)(3))
extern  Table_Res_4 * _DtCm_rtable_lookup_next_smaller_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_lookup_next_smaller_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_lookup_range ((unsigned long)(4))
extern  Table_Res_4 * _DtCm_rtable_lookup_range_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_lookup_range_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_abbreviated_lookup_range ((unsigned long)(5))
extern  Table_Res_4 * _DtCm_rtable_abbreviated_lookup_range_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_abbreviated_lookup_range_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_insert ((unsigned long)(6))
extern  Table_Res_4 * _DtCm_rtable_insert_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_insert_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_delete ((unsigned long)(7))
extern  Table_Res_4 * _DtCm_rtable_delete_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_delete_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_delete_instance ((unsigned long)(8))
extern  Table_Res_4 * _DtCm_rtable_delete_instance_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_delete_instance_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_change ((unsigned long)(9))
extern  Table_Res_4 * _DtCm_rtable_change_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_change_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_change_instance ((unsigned long)(10))
extern  Table_Res_4 * _DtCm_rtable_change_instance_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_change_instance_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_lookup_next_reminder ((unsigned long)(11))
extern  Table_Res_4 * _DtCm_rtable_lookup_next_reminder_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_lookup_next_reminder_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_check ((unsigned long)(12))
extern  Table_Status_4 * _DtCm_rtable_check_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Status_4 * _DtCm_rtable_check_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_flush_table ((unsigned long)(13))
extern  Table_Status_4 * _DtCm_rtable_flush_table_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Status_4 * _DtCm_rtable_flush_table_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_size ((unsigned long)(14))
extern  int * _DtCm_rtable_size_4(Table_Args_4 *, _DtCm_Connection *);
extern  int * _DtCm_rtable_size_4_svc(Table_Args_4 *, struct svc_req *);
#define	register_callback ((unsigned long)(15))
extern  Registration_Status_4 * _DtCm_register_callback_4(Registration_4 *, _DtCm_Connection *);
extern  Registration_Status_4 * _DtCm_register_callback_4_svc(Registration_4 *, struct svc_req *);
#define	deregister_callback ((unsigned long)(16))
extern  Registration_Status_4 * _DtCm_deregister_callback_4(Registration_4 *, _DtCm_Connection *);
extern  Registration_Status_4 * _DtCm_deregister_callback_4_svc(Registration_4 *, struct svc_req *);
#define	rtable_set_access ((unsigned long)(17))
extern  Access_Status_4 * _DtCm_rtable_set_access_4(Access_Args_4 *, _DtCm_Connection *);
extern  Access_Status_4 * _DtCm_rtable_set_access_4_svc(Access_Args_4 *, struct svc_req *);
#define	rtable_get_access ((unsigned long)(18))
extern  Access_Args_4 * _DtCm_rtable_get_access_4(Access_Args_4 *, _DtCm_Connection *);
extern  Access_Args_4 * _DtCm_rtable_get_access_4_svc(Access_Args_4 *, struct svc_req *);
#define	rtable_abbreviated_lookup_key_range ((unsigned long)(19))
extern  Table_Res_4 * _DtCm_rtable_abbreviated_lookup_key_range_4(Table_Args_4 *, _DtCm_Connection *);
extern  Table_Res_4 * _DtCm_rtable_abbreviated_lookup_key_range_4_svc(Table_Args_4 *, struct svc_req *);
#define	rtable_gmtoff ((unsigned long)(20))
extern  long * _DtCm_rtable_gmtoff_4(void *, _DtCm_Connection *);
extern  long * _DtCm_rtable_gmtoff_4_svc(void *, struct svc_req *);
#define	rtable_create ((unsigned long)(21))
extern  Table_Status_4 * _DtCm_rtable_create_4(Table_Op_Args_4 *, _DtCm_Connection *);
extern  Table_Status_4 * _DtCm_rtable_create_4_svc(Table_Op_Args_4 *, struct svc_req *);
#define	rtable_remove ((unsigned long)(22))
extern  Table_Status_4 * _DtCm_rtable_remove_4(Table_Op_Args_4 *, _DtCm_Connection *);
extern  Table_Status_4 * _DtCm_rtable_remove_4_svc(Table_Op_Args_4 *, struct svc_req *);
#define	rtable_rename ((unsigned long)(23))
extern  Table_Status_4 * _DtCm_rtable_rename_4(Table_Op_Args_4 *, _DtCm_Connection *);
extern  Table_Status_4 * _DtCm_rtable_rename_4_svc(Table_Op_Args_4 *, struct svc_req *);
extern int tableprog_4_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define	rtable_ping ((unsigned long)(0))
extern  void * _DtCm_rtable_ping_4();
extern  void * _DtCm_rtable_ping_4_svc();
#define	rtable_lookup ((unsigned long)(1))
extern  Table_Res_4 * _DtCm_rtable_lookup_4();
extern  Table_Res_4 * _DtCm_rtable_lookup_4_svc();
#define	rtable_lookup_next_larger ((unsigned long)(2))
extern  Table_Res_4 * _DtCm_rtable_lookup_next_larger_4();
extern  Table_Res_4 * _DtCm_rtable_lookup_next_larger_4_svc();
#define	rtable_lookup_next_smaller ((unsigned long)(3))
extern  Table_Res_4 * _DtCm_rtable_lookup_next_smaller_4();
extern  Table_Res_4 * _DtCm_rtable_lookup_next_smaller_4_svc();
#define	rtable_lookup_range ((unsigned long)(4))
extern  Table_Res_4 * _DtCm_rtable_lookup_range_4();
extern  Table_Res_4 * _DtCm_rtable_lookup_range_4_svc();
#define	rtable_abbreviated_lookup_range ((unsigned long)(5))
extern  Table_Res_4 * _DtCm_rtable_abbreviated_lookup_range_4();
extern  Table_Res_4 * _DtCm_rtable_abbreviated_lookup_range_4_svc();
#define	rtable_insert ((unsigned long)(6))
extern  Table_Res_4 * _DtCm_rtable_insert_4();
extern  Table_Res_4 * _DtCm_rtable_insert_4_svc();
#define	rtable_delete ((unsigned long)(7))
extern  Table_Res_4 * _DtCm_rtable_delete_4();
extern  Table_Res_4 * _DtCm_rtable_delete_4_svc();
#define	rtable_delete_instance ((unsigned long)(8))
extern  Table_Res_4 * _DtCm_rtable_delete_instance_4();
extern  Table_Res_4 * _DtCm_rtable_delete_instance_4_svc();
#define	rtable_change ((unsigned long)(9))
extern  Table_Res_4 * _DtCm_rtable_change_4();
extern  Table_Res_4 * _DtCm_rtable_change_4_svc();
#define	rtable_change_instance ((unsigned long)(10))
extern  Table_Res_4 * _DtCm_rtable_change_instance_4();
extern  Table_Res_4 * _DtCm_rtable_change_instance_4_svc();
#define	rtable_lookup_next_reminder ((unsigned long)(11))
extern  Table_Res_4 * _DtCm_rtable_lookup_next_reminder_4();
extern  Table_Res_4 * _DtCm_rtable_lookup_next_reminder_4_svc();
#define	rtable_check ((unsigned long)(12))
extern  Table_Status_4 * _DtCm_rtable_check_4();
extern  Table_Status_4 * _DtCm_rtable_check_4_svc();
#define	rtable_flush_table ((unsigned long)(13))
extern  Table_Status_4 * _DtCm_rtable_flush_table_4();
extern  Table_Status_4 * _DtCm_rtable_flush_table_4_svc();
#define	rtable_size ((unsigned long)(14))
extern  int * _DtCm_rtable_size_4();
extern  int * _DtCm_rtable_size_4_svc();
#define	register_callback ((unsigned long)(15))
extern  Registration_Status_4 * _DtCm_register_callback_4();
extern  Registration_Status_4 * _DtCm_register_callback_4_svc();
#define	deregister_callback ((unsigned long)(16))
extern  Registration_Status_4 * _DtCm_deregister_callback_4();
extern  Registration_Status_4 * _DtCm_deregister_callback_4_svc();
#define	rtable_set_access ((unsigned long)(17))
extern  Access_Status_4 * _DtCm_rtable_set_access_4();
extern  Access_Status_4 * _DtCm_rtable_set_access_4_svc();
#define	rtable_get_access ((unsigned long)(18))
extern  Access_Args_4 * _DtCm_rtable_get_access_4();
extern  Access_Args_4 * _DtCm_rtable_get_access_4_svc();
#define	rtable_abbreviated_lookup_key_range ((unsigned long)(19))
extern  Table_Res_4 * _DtCm_rtable_abbreviated_lookup_key_range_4();
extern  Table_Res_4 * _DtCm_rtable_abbreviated_lookup_key_range_4_svc();
#define	rtable_gmtoff ((unsigned long)(20))
extern  long * _DtCm_rtable_gmtoff_4();
extern  long * _DtCm_rtable_gmtoff_4_svc();
#define	rtable_create ((unsigned long)(21))
extern  Table_Status_4 * _DtCm_rtable_create_4();
extern  Table_Status_4 * _DtCm_rtable_create_4_svc();
#define	rtable_remove ((unsigned long)(22))
extern  Table_Status_4 * _DtCm_rtable_remove_4();
extern  Table_Status_4 * _DtCm_rtable_remove_4_svc();
#define	rtable_rename ((unsigned long)(23))
extern  Table_Status_4 * _DtCm_rtable_rename_4();
extern  Table_Status_4 * _DtCm_rtable_rename_4_svc();
extern int tableprog_4_freeresult();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t _DtCm_xdr_Buffer_4(XDR *, Buffer_4*);
extern  bool_t _DtCm_xdr_Interval_4(XDR *, Interval_4*);
extern  bool_t _DtCm_xdr_Period_4(XDR *, Period_4*);
extern  bool_t _DtCm_xdr_Event_Type_4(XDR *, Event_Type_4*);
extern  bool_t _DtCm_xdr_Options_4(XDR *, Options_4*);
extern  bool_t _DtCm_xdr_Tag_4(XDR *, Tag_4*);
extern  bool_t _DtCm_xdr_Privacy_Level_4(XDR *, Privacy_Level_4*);
extern  bool_t _DtCm_xdr_Attribute_4(XDR *, Attribute_4*);
extern  bool_t _DtCm_xdr_Attr_4(XDR *, Attr_4*);
extern  bool_t _DtCm_xdr_Except_4(XDR *, Except_4*);
extern  bool_t _DtCm_xdr_Exception_4(XDR *, Exception_4*);
extern  bool_t _DtCm_xdr_Id_4(XDR *, Id_4*);
extern  bool_t _DtCm_xdr_Uid_4(XDR *, Uid_4*);
extern  bool_t _DtCm_xdr_Appt_Status_4(XDR *, Appt_Status_4*);
extern  bool_t _DtCm_xdr_Appt_4(XDR *, Appt_4*);
extern  bool_t _DtCm_xdr_Abb_Appt_4(XDR *, Abb_Appt_4*);
extern  bool_t _DtCm_xdr_Apptid_4(XDR *, Apptid_4*);
extern  bool_t _DtCm_xdr_Reminder_4(XDR *, Reminder_4*);
extern  bool_t _DtCm_xdr_Table_Res_Type_4(XDR *, Table_Res_Type_4*);
extern  bool_t _DtCm_xdr_Table_Res_List_4(XDR *, Table_Res_List_4*);
extern  bool_t _DtCm_xdr_Access_Status_4(XDR *, Access_Status_4*);
extern  bool_t _DtCm_xdr_Table_Res_4(XDR *, Table_Res_4*);
extern  bool_t _DtCm_xdr_Access_Entry_4(XDR *, Access_Entry_4*);
extern  bool_t _DtCm_xdr_Access_Args_4(XDR *, Access_Args_4*);
extern  bool_t _DtCm_xdr_Range_4(XDR *, Range_4*);
extern  bool_t _DtCm_xdr_Keyrange_4(XDR *, Keyrange_4*);
extern  bool_t _DtCm_xdr_Uidopt_4(XDR *, Uidopt_4*);
extern  bool_t _DtCm_xdr_Table_Args_Type_4(XDR *, Table_Args_Type_4*);
extern  bool_t _DtCm_xdr_Args_4(XDR *, Args_4*);
extern  bool_t _DtCm_xdr_Table_Args_4(XDR *, Table_Args_4*);
extern  bool_t _DtCm_xdr_Registration_4(XDR *, Registration_4*);
extern  bool_t _DtCm_xdr_Table_Op_Args_4(XDR *, Table_Op_Args_4*);
extern  bool_t _DtCm_xdr_Table_Status_4(XDR *, Table_Status_4*);
extern  bool_t _DtCm_xdr_Registration_Status_4(XDR *, Registration_Status_4*);

#else /* K&R C */
extern bool_t _DtCm_xdr_Buffer_4();
extern bool_t _DtCm_xdr_Interval_4();
extern bool_t _DtCm_xdr_Period_4();
extern bool_t _DtCm_xdr_Event_Type_4();
extern bool_t _DtCm_xdr_Options_4();
extern bool_t _DtCm_xdr_Tag_4();
extern bool_t _DtCm_xdr_Privacy_Level_4();
extern bool_t _DtCm_xdr_Attribute_4();
extern bool_t _DtCm_xdr_Attr_4();
extern bool_t _DtCm_xdr_Except_4();
extern bool_t _DtCm_xdr_Exception_4();
extern bool_t _DtCm_xdr_Id_4();
extern bool_t _DtCm_xdr_Uid_4();
extern bool_t _DtCm_xdr_Appt_Status_4();
extern bool_t _DtCm_xdr_Appt_4();
extern bool_t _DtCm_xdr_Abb_Appt_4();
extern bool_t _DtCm_xdr_Apptid_4();
extern bool_t _DtCm_xdr_Reminder_4();
extern bool_t _DtCm_xdr_Table_Res_Type_4();
extern bool_t _DtCm_xdr_Table_Res_List_4();
extern bool_t _DtCm_xdr_Access_Status_4();
extern bool_t _DtCm_xdr_Table_Res_4();
extern bool_t _DtCm_xdr_Access_Entry_4();
extern bool_t _DtCm_xdr_Access_Args_4();
extern bool_t _DtCm_xdr_Range_4();
extern bool_t _DtCm_xdr_Keyrange_4();
extern bool_t _DtCm_xdr_Uidopt_4();
extern bool_t _DtCm_xdr_Table_Args_Type_4();
extern bool_t _DtCm_xdr_Args_4();
extern bool_t _DtCm_xdr_Table_Args_4();
extern bool_t _DtCm_xdr_Registration_4();
extern bool_t _DtCm_xdr_Table_Op_Args_4();
extern bool_t _DtCm_xdr_Table_Status_4();
extern bool_t _DtCm_xdr_Registration_Status_4();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#ifdef HPUX
#undef hpux
#define hpux
#endif
#endif

