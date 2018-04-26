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
**  rtable3.h
**
**  static char sccsid[] = "@(#)rtable3.h 1.1 94/09/05 Copyr 1991 Sun Microsystems, Inc.";
**
**  $TOG: rtable3.h /main/3 1999/10/14 17:47:54 mgreess $
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
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/
#ifndef _RTABLE3_H
#define _RTABLE3_H

#ifndef SunOS
#include <rpc/types.h>
#endif /* HPUX */


#include <rpc/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "connection.h"

extern bool_t xdr_time_t();

typedef char *Buffer_3;

enum Transaction_3 {
	add_3 = 0,
	cm_remove_3 = 1
};
typedef enum Transaction_3 Transaction_3;

enum Interval_3 {
	single_3 = 0,
	daily_3 = 1,
	weekly_3 = 2,
	biweekly_3 = 3,
	monthly_3 = 4,
	yearly_3 = 5,
	nthWeekday_3 = 6,
	everyNthDay_3 = 7,
	everyNthWeek_3 = 8,
	everyNthMonth_3 = 9,
	otherPeriod_3 = 10
};
typedef enum Interval_3 Interval_3;

struct Period_3 {
	Interval_3 period;
	int nth;
};
typedef struct Period_3 Period_3;

enum Event_Type_3 {
	appointment_3 = 0,
	reminder_3 = 1,
	otherTag_3 = 2,
	holiday_3 = 3,
	toDo_3 = 4
};
typedef enum Event_Type_3 Event_Type_3;

struct Tag_3 {
	Event_Type_3 tag;
	int showtime;
	struct Tag_3 *next;
};
typedef struct Tag_3 Tag_3;

enum Privacy_Level_3 {
	public_3 = 0,
	private_3 = 1,
	semiprivate_3 = 2
};
typedef enum Privacy_Level_3 Privacy_Level_3;

struct Attribute_3 {
	struct Attribute_3 *next;
	Buffer_3 attr;
	Buffer_3 value;
	Buffer_3 clientdata;
};
typedef struct Attribute_3 Attribute_3;

typedef Attribute_3 *Attr_3;

struct Except_3 {
	int ordinal;
	struct Except_3 *next;
};
typedef struct Except_3 Except_3;

typedef Except_3 *Exception_3;

struct Id_3 {
	long tick;
	long key;
};
typedef struct Id_3 Id_3;

struct Uid_3 {
	struct Id_3 appt_id;
	struct Uid_3 *next;
};
typedef struct Uid_3 Uid_3;

enum Appt_Status_3 {
	active_3 = 0,
	pendingAdd_3 = 1,
	pendingDelete_3 = 2,
	committed_3 = 3,
	cancelled_3 = 4,
	completed_3 = 5
};
typedef enum Appt_Status_3 Appt_Status_3;

struct Appt_3 {
	struct Id_3 appt_id;
	struct Tag_3 *tag;
	int duration;
	int ntimes;
	Buffer_3 what;
	struct Period_3 period;
	Buffer_3 author;
	Buffer_3 client_data;
	struct Except_3 *exception;
	struct Attribute_3 *attr;
	Appt_Status_3 appt_status;
	Privacy_Level_3 privacy;
	struct Appt_3 *next;
};
typedef struct Appt_3 Appt_3;

struct Abb_Appt_3 {
	struct Id_3 appt_id;
	struct Tag_3 *tag;
	Buffer_3 what;
	int duration;
	struct Period_3 period;
	struct Abb_Appt_3 *next;
	Appt_Status_3 appt_status;
	Privacy_Level_3 privacy;
};
typedef struct Abb_Appt_3 Abb_Appt_3;

struct Apptid_3 {
	struct Id_3 *oid;
	struct Appt_3 *new_appt;
};
typedef struct Apptid_3 Apptid_3;

struct Reminder_3 {
	struct Id_3 appt_id;
	long tick;
	Attribute_3 attr;
	struct Reminder_3 *next;
};
typedef struct Reminder_3 Reminder_3;

enum Table_Res_Type_3 {
	AP_3 = 0,
	RM_3 = 1,
	AB_3 = 2,
	ID_3 = 3
};
typedef enum Table_Res_Type_3 Table_Res_Type_3;

struct Table_Res_List_3 {
	Table_Res_Type_3 tag;
	union {
		Appt_3 *a;
		Reminder_3 *r;
		Abb_Appt_3 *b;
		Uid_3 *i;
	} Table_Res_List_3_u;
};
typedef struct Table_Res_List_3 Table_Res_List_3;

enum Access_Status_3 {
	access_ok_3 = 0,
	access_added_3 = 1,
	access_removed_3 = 2,
	access_failed_3 = 3,
	access_exists_3 = 4,
	access_partial_3 = 5,
	access_other_3 = 6
};
typedef enum Access_Status_3 Access_Status_3;

struct Table_Res_3 {
	Access_Status_3 status;
	Table_Res_List_3 res;
};
typedef struct Table_Res_3 Table_Res_3;
#define access_none_3   0x0     /* owner only */
#define access_read_3   0x1
#define access_write_3  0x2
#define access_delete_3 0x4
#define access_exec_3   0x8     /* execution permission is a hack! */

struct Access_Entry_3 {
	Buffer_3 who;
	int access_type;
	struct Access_Entry_3 *next;
};
typedef struct Access_Entry_3 Access_Entry_3;

struct Access_Args_3 {
	Buffer_3 target;
	Access_Entry_3 *access_list;
};
typedef struct Access_Args_3 Access_Args_3;

struct Range_3 {
	long key1;
	long key2;
	struct Range_3 *next;
};
typedef struct Range_3 Range_3;

struct Keyrange_3 {
	long key;
	long tick1;
	long tick2;
	struct Keyrange_3 *next;
};
typedef struct Keyrange_3 Keyrange_3;

enum Table_Args_Type_3 {
	TICK_3 = 0,
	APPTID_3 = 1,
	UID_3 = 2,
	APPT_3 = 3,
	RANGE_3 = 4,
	KEYRANGE_3 = 5
};
typedef enum Table_Args_Type_3 Table_Args_Type_3;

struct Args_3 {
	Table_Args_Type_3 tag;
	union {
		long tick;
		Apptid_3 apptid;
		Uid_3 *key;
		Appt_3 *appt;
		Range_3 *range;
		Keyrange_3 *keyrange;
	} Args_3_u;
};
typedef struct Args_3 Args_3;

struct Table_Args_3 {
	Buffer_3 target;
	Args_3 args;
	int pid;
};
typedef struct Table_Args_3 Table_Args_3;

struct Registration_3 {
	Buffer_3 target;
	u_long prognum;
	u_long versnum;
	u_long procnum;
	struct Registration_3 *next;
	int pid;
};
typedef struct Registration_3 Registration_3;

enum Table_Status_3 {
	ok_3 = 0,
	duplicate_3 = 1,
	badtable_3 = 2,
	notable_3 = 3,
	denied_3 = 4,
	other_3 = 5
};
typedef enum Table_Status_3 Table_Status_3;

enum Registration_Status_3 {
	registered_3 = 0,
	failed_3 = 1,
	deregistered_3 = 2,
	confused_3 = 3
};
typedef enum Registration_Status_3 Registration_Status_3;

#define	TABLEPROG ((unsigned long)(100068))
#define	TABLEVERS_3 ((unsigned long)(3))

#if defined(__STDC__) || defined(__cplusplus)
#define	rtable_ping ((unsigned long)(0))
extern  void * _DtCm_rtable_ping_3(void *, _DtCm_Connection *);
extern  void * _DtCm_rtable_ping_3_svc(void *, struct svc_req *);
#define	rtable_lookup ((unsigned long)(1))
extern  Table_Res_3 * _DtCm_rtable_lookup_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_lookup_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_lookup_next_larger ((unsigned long)(2))
extern  Table_Res_3 * _DtCm_rtable_lookup_next_larger_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_lookup_next_larger_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_lookup_next_smaller ((unsigned long)(3))
extern  Table_Res_3 * _DtCm_rtable_lookup_next_smaller_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_lookup_next_smaller_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_lookup_range ((unsigned long)(4))
extern  Table_Res_3 * _DtCm_rtable_lookup_range_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_lookup_range_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_abbreviated_lookup_range ((unsigned long)(5))
extern  Table_Res_3 * _DtCm_rtable_abbreviated_lookup_range_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_abbreviated_lookup_range_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_insert ((unsigned long)(6))
extern  Table_Res_3 * _DtCm_rtable_insert_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_insert_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_delete ((unsigned long)(7))
extern  Table_Res_3 * _DtCm_rtable_delete_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_delete_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_delete_instance ((unsigned long)(8))
extern  Table_Res_3 * _DtCm_rtable_delete_instance_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_delete_instance_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_change ((unsigned long)(9))
extern  Table_Res_3 * _DtCm_rtable_change_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_change_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_change_instance ((unsigned long)(10))
extern  Table_Res_3 * _DtCm_rtable_change_instance_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_change_instance_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_lookup_next_reminder ((unsigned long)(11))
extern  Table_Res_3 * _DtCm_rtable_lookup_next_reminder_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_lookup_next_reminder_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_check ((unsigned long)(12))
extern  Table_Status_3 * _DtCm_rtable_check_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Status_3 * _DtCm_rtable_check_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_flush_table ((unsigned long)(13))
extern  Table_Status_3 * _DtCm_rtable_flush_table_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Status_3 * _DtCm_rtable_flush_table_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_size ((unsigned long)(14))
extern  int * _DtCm_rtable_size_3(Table_Args_3 *, _DtCm_Connection *);
extern  int * _DtCm_rtable_size_3_svc(Table_Args_3 *, struct svc_req *);
#define	register_callback ((unsigned long)(15))
extern  Registration_Status_3 * _DtCm_register_callback_3(Registration_3 *, _DtCm_Connection *);
extern  Registration_Status_3 * _DtCm_register_callback_3_svc(Registration_3 *, struct svc_req *);
#define	deregister_callback ((unsigned long)(16))
extern  Registration_Status_3 * _DtCm_deregister_callback_3(Registration_3 *, _DtCm_Connection *);
extern  Registration_Status_3 * _DtCm_deregister_callback_3_svc(Registration_3 *, struct svc_req *);
#define	rtable_set_access ((unsigned long)(17))
extern  Access_Status_3 * _DtCm_rtable_set_access_3(Access_Args_3 *, _DtCm_Connection *);
extern  Access_Status_3 * _DtCm_rtable_set_access_3_svc(Access_Args_3 *, struct svc_req *);
#define	rtable_get_access ((unsigned long)(18))
extern  Access_Args_3 * _DtCm_rtable_get_access_3(Access_Args_3 *, _DtCm_Connection *);
extern  Access_Args_3 * _DtCm_rtable_get_access_3_svc(Access_Args_3 *, struct svc_req *);
#define	rtable_abbreviated_lookup_key_range ((unsigned long)(19))
extern  Table_Res_3 * _DtCm_rtable_abbreviated_lookup_key_range_3(Table_Args_3 *, _DtCm_Connection *);
extern  Table_Res_3 * _DtCm_rtable_abbreviated_lookup_key_range_3_svc(Table_Args_3 *, struct svc_req *);
#define	rtable_gmtoff ((unsigned long)(20))
extern  long * _DtCm_rtable_gmtoff_3(void *, _DtCm_Connection *);
extern  long * _DtCm_rtable_gmtoff_3_svc(void *, struct svc_req *);
extern int tableprog_3_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define	rtable_ping ((unsigned long)(0))
extern  void * _DtCm_rtable_ping_3();
extern  void * _DtCm_rtable_ping_3_svc();
#define	rtable_lookup ((unsigned long)(1))
extern  Table_Res_3 * _DtCm_rtable_lookup_3();
extern  Table_Res_3 * _DtCm_rtable_lookup_3_svc();
#define	rtable_lookup_next_larger ((unsigned long)(2))
extern  Table_Res_3 * _DtCm_rtable_lookup_next_larger_3();
extern  Table_Res_3 * _DtCm_rtable_lookup_next_larger_3_svc();
#define	rtable_lookup_next_smaller ((unsigned long)(3))
extern  Table_Res_3 * _DtCm_rtable_lookup_next_smaller_3();
extern  Table_Res_3 * _DtCm_rtable_lookup_next_smaller_3_svc();
#define	rtable_lookup_range ((unsigned long)(4))
extern  Table_Res_3 * _DtCm_rtable_lookup_range_3();
extern  Table_Res_3 * _DtCm_rtable_lookup_range_3_svc();
#define	rtable_abbreviated_lookup_range ((unsigned long)(5))
extern  Table_Res_3 * _DtCm_rtable_abbreviated_lookup_range_3();
extern  Table_Res_3 * _DtCm_rtable_abbreviated_lookup_range_3_svc();
#define	rtable_insert ((unsigned long)(6))
extern  Table_Res_3 * _DtCm_rtable_insert_3();
extern  Table_Res_3 * _DtCm_rtable_insert_3_svc();
#define	rtable_delete ((unsigned long)(7))
extern  Table_Res_3 * _DtCm_rtable_delete_3();
extern  Table_Res_3 * _DtCm_rtable_delete_3_svc();
#define	rtable_delete_instance ((unsigned long)(8))
extern  Table_Res_3 * _DtCm_rtable_delete_instance_3();
extern  Table_Res_3 * _DtCm_rtable_delete_instance_3_svc();
#define	rtable_change ((unsigned long)(9))
extern  Table_Res_3 * _DtCm_rtable_change_3();
extern  Table_Res_3 * _DtCm_rtable_change_3_svc();
#define	rtable_change_instance ((unsigned long)(10))
extern  Table_Res_3 * _DtCm_rtable_change_instance_3();
extern  Table_Res_3 * _DtCm_rtable_change_instance_3_svc();
#define	rtable_lookup_next_reminder ((unsigned long)(11))
extern  Table_Res_3 * _DtCm_rtable_lookup_next_reminder_3();
extern  Table_Res_3 * _DtCm_rtable_lookup_next_reminder_3_svc();
#define	rtable_check ((unsigned long)(12))
extern  Table_Status_3 * _DtCm_rtable_check_3();
extern  Table_Status_3 * _DtCm_rtable_check_3_svc();
#define	rtable_flush_table ((unsigned long)(13))
extern  Table_Status_3 * _DtCm_rtable_flush_table_3();
extern  Table_Status_3 * _DtCm_rtable_flush_table_3_svc();
#define	rtable_size ((unsigned long)(14))
extern  int * _DtCm_rtable_size_3();
extern  int * _DtCm_rtable_size_3_svc();
#define	register_callback ((unsigned long)(15))
extern  Registration_Status_3 * _DtCm_register_callback_3();
extern  Registration_Status_3 * _DtCm_register_callback_3_svc();
#define	deregister_callback ((unsigned long)(16))
extern  Registration_Status_3 * _DtCm_deregister_callback_3();
extern  Registration_Status_3 * _DtCm_deregister_callback_3_svc();
#define	rtable_set_access ((unsigned long)(17))
extern  Access_Status_3 * _DtCm_rtable_set_access_3();
extern  Access_Status_3 * _DtCm_rtable_set_access_3_svc();
#define	rtable_get_access ((unsigned long)(18))
extern  Access_Args_3 * _DtCm_rtable_get_access_3();
extern  Access_Args_3 * _DtCm_rtable_get_access_3_svc();
#define	rtable_abbreviated_lookup_key_range ((unsigned long)(19))
extern  Table_Res_3 * _DtCm_rtable_abbreviated_lookup_key_range_3();
extern  Table_Res_3 * _DtCm_rtable_abbreviated_lookup_key_range_3_svc();
#define	rtable_gmtoff ((unsigned long)(20))
extern  long * _DtCm_rtable_gmtoff_3();
extern  long * _DtCm_rtable_gmtoff_3_svc();
extern int tableprog_3_freeresult();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t _DtCm_xdr_Buffer_3(XDR *, Buffer_3*);
extern  bool_t _DtCm_xdr_Transaction_3(XDR *, Transaction_3*);
extern  bool_t _DtCm_xdr_Interval_3(XDR *, Interval_3*);
extern  bool_t _DtCm_xdr_Period_3(XDR *, Period_3*);
extern  bool_t _DtCm_xdr_Event_Type_3(XDR *, Event_Type_3*);
extern  bool_t _DtCm_xdr_Tag_3(XDR *, Tag_3*);
extern  bool_t _DtCm_xdr_Privacy_Level_3(XDR *, Privacy_Level_3*);
extern  bool_t _DtCm_xdr_Attribute_3(XDR *, Attribute_3*);
extern  bool_t _DtCm_xdr_Attr_3(XDR *, Attr_3*);
extern  bool_t _DtCm_xdr_Except_3(XDR *, Except_3*);
extern  bool_t _DtCm_xdr_Exception_3(XDR *, Exception_3*);
extern  bool_t _DtCm_xdr_Id_3(XDR *, Id_3*);
extern  bool_t _DtCm_xdr_Uid_3(XDR *, Uid_3*);
extern  bool_t _DtCm_xdr_Appt_Status_3(XDR *, Appt_Status_3*);
extern  bool_t _DtCm_xdr_Appt_3(XDR *, Appt_3*);
extern  bool_t _DtCm_xdr_Abb_Appt_3(XDR *, Abb_Appt_3*);
extern  bool_t _DtCm_xdr_Apptid_3(XDR *, Apptid_3*);
extern  bool_t _DtCm_xdr_Reminder_3(XDR *, Reminder_3*);
extern  bool_t _DtCm_xdr_Table_Res_Type_3(XDR *, Table_Res_Type_3*);
extern  bool_t _DtCm_xdr_Table_Res_List_3(XDR *, Table_Res_List_3*);
extern  bool_t _DtCm_xdr_Access_Status_3(XDR *, Access_Status_3*);
extern  bool_t _DtCm_xdr_Table_Res_3(XDR *, Table_Res_3*);
extern  bool_t _DtCm_xdr_Access_Entry_3(XDR *, Access_Entry_3*);
extern  bool_t _DtCm_xdr_Access_Args_3(XDR *, Access_Args_3*);
extern  bool_t _DtCm_xdr_Range_3(XDR *, Range_3*);
extern  bool_t _DtCm_xdr_Keyrange_3(XDR *, Keyrange_3*);
extern  bool_t _DtCm_xdr_Table_Args_Type_3(XDR *, Table_Args_Type_3*);
extern  bool_t _DtCm_xdr_Args_3(XDR *, Args_3*);
extern  bool_t _DtCm_xdr_Table_Args_3(XDR *, Table_Args_3*);
extern  bool_t _DtCm_xdr_Registration_3(XDR *, Registration_3*);
extern  bool_t _DtCm_xdr_Table_Status_3(XDR *, Table_Status_3*);
extern  bool_t _DtCm_xdr_Registration_Status_3(XDR *, Registration_Status_3*);

#else /* K&R C */
extern bool_t _DtCm_xdr_Buffer_3();
extern bool_t _DtCm_xdr_Transaction_3();
extern bool_t _DtCm_xdr_Interval_3();
extern bool_t _DtCm_xdr_Period_3();
extern bool_t _DtCm_xdr_Event_Type_3();
extern bool_t _DtCm_xdr_Tag_3();
extern bool_t _DtCm_xdr_Privacy_Level_3();
extern bool_t _DtCm_xdr_Attribute_3();
extern bool_t _DtCm_xdr_Attr_3();
extern bool_t _DtCm_xdr_Except_3();
extern bool_t _DtCm_xdr_Exception_3();
extern bool_t _DtCm_xdr_Id_3();
extern bool_t _DtCm_xdr_Uid_3();
extern bool_t _DtCm_xdr_Appt_Status_3();
extern bool_t _DtCm_xdr_Appt_3();
extern bool_t _DtCm_xdr_Abb_Appt_3();
extern bool_t _DtCm_xdr_Apptid_3();
extern bool_t _DtCm_xdr_Reminder_3();
extern bool_t _DtCm_xdr_Table_Res_Type_3();
extern bool_t _DtCm_xdr_Table_Res_List_3();
extern bool_t _DtCm_xdr_Access_Status_3();
extern bool_t _DtCm_xdr_Table_Res_3();
extern bool_t _DtCm_xdr_Access_Entry_3();
extern bool_t _DtCm_xdr_Access_Args_3();
extern bool_t _DtCm_xdr_Range_3();
extern bool_t _DtCm_xdr_Keyrange_3();
extern bool_t _DtCm_xdr_Table_Args_Type_3();
extern bool_t _DtCm_xdr_Args_3();
extern bool_t _DtCm_xdr_Table_Args_3();
extern bool_t _DtCm_xdr_Registration_3();
extern bool_t _DtCm_xdr_Table_Status_3();
extern bool_t _DtCm_xdr_Registration_Status_3();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#ifdef HPUX
#undef hpux
#define hpux
#endif
#endif

