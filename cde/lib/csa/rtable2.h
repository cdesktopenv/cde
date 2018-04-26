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
**  rtable2.h
**
**  static char sccsid[] = "@(#)rtable2.h 1.1 94/09/05 Copyr 1991 Sun Microsystems, Inc.";
**
**  $TOG: rtable2.h /main/3 1999/10/14 17:47:41 mgreess $
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

#ifndef _RTABLE2_H
#define _RTABLE2_H

#ifndef SunOS
#include <rpc/types.h>
#endif /* HPUX */


#include <rpc/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "connection.h"

extern bool_t xdr_time_t();

typedef char *Buffer_2;

enum Transaction_2 {
	add_2 = 0,
	cm_remove_2 = 1
};
typedef enum Transaction_2 Transaction_2;

enum Period_2 {
	single_2 = 0,
	daily_2 = 1,
	weekly_2 = 2,
	biweekly_2 = 3,
	monthly_2 = 4,
	yearly_2 = 5,
	nthWeekday_2 = 6,
	everyNthDay_2 = 7,
	everyNthWeek_2 = 8,
	everyNthMonth_2 = 9,
	otherPeriod_2 = 10
};
typedef enum Period_2 Period_2;

enum Tag_2 {
	appointment_2 = 0,
	reminder_2 = 1,
	otherTag_2 = 2
};
typedef enum Tag_2 Tag_2;

struct Attribute_2 {
	struct Attribute_2 *next;
	Buffer_2 attr;
	Buffer_2 value;
};
typedef struct Attribute_2 Attribute_2;

typedef Attribute_2 *Attr_2;

struct Except_2 {
	int ordinal;
	struct Except_2 *next;
};
typedef struct Except_2 Except_2;

typedef Except_2 *Exception_2;

struct Id_2 {
	time_t tick;
	long key;
};
typedef struct Id_2 Id_2;

struct Uid_2 {
	struct Id_2 appt_id;
	struct Uid_2 *next;
};
typedef struct Uid_2 Uid_2;

struct Appt_2 {
	struct Id_2 appt_id;
	Tag_2 tag;
	int duration;
	int ntimes;
	Buffer_2 what;
	Buffer_2 mailto;
	Buffer_2 script;
	Period_2 period;
	Buffer_2 author;
	Buffer_2 client_data;
	struct Except_2 *exception;
	struct Attribute_2 *attr;
	struct Appt_2 *next;
};
typedef struct Appt_2 Appt_2;

struct Abb_Appt_2 {
	struct Id_2 appt_id;
	Buffer_2 what;
	int duration;
	Period_2 period;
	struct Abb_Appt_2 *next;
};
typedef struct Abb_Appt_2 Abb_Appt_2;

struct Apptid_2 {
	struct Id_2 *oid;
	struct Appt_2 *new_appt;
};
typedef struct Apptid_2 Apptid_2;

struct Reminder_2 {
	struct Id_2 appt_id;
	long tick;
	Attribute_2 attr;
	struct Reminder_2 *next;
};
typedef struct Reminder_2 Reminder_2;

enum Table_Res_Type_2 {
	AP_2 = 0,
	RM_2 = 1,
	AB_2 = 2,
	ID_2 = 3
};
typedef enum Table_Res_Type_2 Table_Res_Type_2;

struct Table_Res_List_2 {
	Table_Res_Type_2 tag;
	union {
		Appt_2 *a;
		Reminder_2 *r;
		Abb_Appt_2 *b;
		Uid_2 *i;
	} Table_Res_List_2_u;
};
typedef struct Table_Res_List_2 Table_Res_List_2;

enum Access_Status_2 {
	access_ok_2 = 0,
	access_added_2 = 1,
	access_removed_2 = 2,
	access_failed_2 = 3,
	access_exists_2 = 4,
	access_partial_2 = 5,
	access_other_2 = 6
};
typedef enum Access_Status_2 Access_Status_2;

struct Table_Res_2 {
	Access_Status_2 status;
	Table_Res_List_2 res;
};
typedef struct Table_Res_2 Table_Res_2;
#define access_none_2	0x0	/* owner only */
#define access_read_2	0x1
#define access_write_2	0x2
#define access_delete_2	0x4
#define access_exec_2	0x8	/* execution permission is a hack! */
#define VOIDPID	-1	/* pre-V3 tools do present pids */

struct Access_Entry_2 {
	Buffer_2 who;
	int access_type;
	struct Access_Entry_2 *next;
};
typedef struct Access_Entry_2 Access_Entry_2;

struct Access_Args_2 {
	Buffer_2 target;
	Access_Entry_2 *access_list;
};
typedef struct Access_Args_2 Access_Args_2;

struct Range_2 {
	long key1;
	long key2;
	struct Range_2 *next;
};
typedef struct Range_2 Range_2;

enum Table_Args_Type_2 {
	TICK_2 = 0,
	APPTID_2 = 1,
	UID_2 = 2,
	APPT_2 = 3,
	RANGE_2 = 4
};
typedef enum Table_Args_Type_2 Table_Args_Type_2;

struct Args_2 {
	Table_Args_Type_2 tag;
	union {
		long tick;
		Apptid_2 apptid;
		Uid_2 *key;
		Appt_2 *appt;
		Range_2 *range;
	} Args_2_u;
};
typedef struct Args_2 Args_2;

struct Table_Args_2 {
	Buffer_2 target;
	Args_2 args;
};
typedef struct Table_Args_2 Table_Args_2;

struct Registration_2 {
	Buffer_2 target;
	u_long prognum;
	u_long versnum;
	u_long procnum;
	struct Registration_2 *next;
};
typedef struct Registration_2 Registration_2;

enum Table_Status_2 {
	ok_2 = 0,
	duplicate_2 = 1,
	badtable_2 = 2,
	notable_2 = 3,
	denied_2 = 4,
	other_2 = 5
};
typedef enum Table_Status_2 Table_Status_2;

enum Registration_Status_2 {
	registered_2 = 0,
	failed_2 = 1,
	deregistered_2 = 2,
	confused_2 = 3
};
typedef enum Registration_Status_2 Registration_Status_2;

#define	TABLEPROG ((unsigned long)(100068))
#define	TABLEVERS_2 ((unsigned long)(2))

#if defined(__STDC__) || defined(__cplusplus)
#define	rtable_ping ((unsigned long)(0))
extern  void * _DtCm_rtable_ping_2(void *, _DtCm_Connection *);
extern  void * _DtCm_rtable_ping_2_svc(void *, struct svc_req *);
#define	rtable_lookup ((unsigned long)(1))
extern  Table_Res_2 * _DtCm_rtable_lookup_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_lookup_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_lookup_next_larger ((unsigned long)(2))
extern  Table_Res_2 * _DtCm_rtable_lookup_next_larger_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_lookup_next_larger_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_lookup_next_smaller ((unsigned long)(3))
extern  Table_Res_2 * _DtCm_rtable_lookup_next_smaller_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_lookup_next_smaller_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_lookup_range ((unsigned long)(4))
extern  Table_Res_2 * _DtCm_rtable_lookup_range_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_lookup_range_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_abbreviated_lookup_range ((unsigned long)(5))
extern  Table_Res_2 * _DtCm_rtable_abbreviated_lookup_range_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_abbreviated_lookup_range_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_insert ((unsigned long)(6))
extern  Table_Res_2 * _DtCm_rtable_insert_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_insert_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_delete ((unsigned long)(7))
extern  Table_Res_2 * _DtCm_rtable_delete_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_delete_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_delete_instance ((unsigned long)(8))
extern  Table_Res_2 * _DtCm_rtable_delete_instance_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_delete_instance_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_change ((unsigned long)(9))
extern  Table_Res_2 * _DtCm_rtable_change_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_change_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_change_instance ((unsigned long)(10))
extern  Table_Res_2 * _DtCm_rtable_change_instance_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_change_instance_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_lookup_next_reminder ((unsigned long)(11))
extern  Table_Res_2 * _DtCm_rtable_lookup_next_reminder_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Res_2 * _DtCm_rtable_lookup_next_reminder_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_check ((unsigned long)(12))
extern  Table_Status_2 * _DtCm_rtable_check_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Status_2 * _DtCm_rtable_check_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_flush_table ((unsigned long)(13))
extern  Table_Status_2 * _DtCm_rtable_flush_table_2(Table_Args_2 *, _DtCm_Connection *);
extern  Table_Status_2 * _DtCm_rtable_flush_table_2_svc(Table_Args_2 *, struct svc_req *);
#define	rtable_size ((unsigned long)(14))
extern  int * _DtCm_rtable_size_2(Table_Args_2 *, _DtCm_Connection *);
extern  int * _DtCm_rtable_size_2_svc(Table_Args_2 *, struct svc_req *);
#define	register_callback ((unsigned long)(15))
extern  Registration_Status_2 * _DtCm_register_callback_2(Registration_2 *, _DtCm_Connection *);
extern  Registration_Status_2 * _DtCm_register_callback_2_svc(Registration_2 *, struct svc_req *);
#define	deregister_callback ((unsigned long)(16))
extern  Registration_Status_2 * _DtCm_deregister_callback_2(Registration_2 *, _DtCm_Connection *);
extern  Registration_Status_2 * _DtCm_deregister_callback_2_svc(Registration_2 *, struct svc_req *);
#define	rtable_set_access ((unsigned long)(17))
extern  Access_Status_2 * _DtCm_rtable_set_access_2(Access_Args_2 *, _DtCm_Connection *);
extern  Access_Status_2 * _DtCm_rtable_set_access_2_svc(Access_Args_2 *, struct svc_req *);
#define	rtable_get_access ((unsigned long)(18))
extern  Access_Args_2 * _DtCm_rtable_get_access_2(Access_Args_2 *, _DtCm_Connection *);
extern  Access_Args_2 * _DtCm_rtable_get_access_2_svc(Access_Args_2 *, struct svc_req *);
extern int tableprog_2_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define	rtable_ping ((unsigned long)(0))
extern  void * _DtCm_rtable_ping_2();
extern  void * _DtCm_rtable_ping_2_svc();
#define	rtable_lookup ((unsigned long)(1))
extern  Table_Res_2 * _DtCm_rtable_lookup_2();
extern  Table_Res_2 * _DtCm_rtable_lookup_2_svc();
#define	rtable_lookup_next_larger ((unsigned long)(2))
extern  Table_Res_2 * _DtCm_rtable_lookup_next_larger_2();
extern  Table_Res_2 * _DtCm_rtable_lookup_next_larger_2_svc();
#define	rtable_lookup_next_smaller ((unsigned long)(3))
extern  Table_Res_2 * _DtCm_rtable_lookup_next_smaller_2();
extern  Table_Res_2 * _DtCm_rtable_lookup_next_smaller_2_svc();
#define	rtable_lookup_range ((unsigned long)(4))
extern  Table_Res_2 * _DtCm_rtable_lookup_range_2();
extern  Table_Res_2 * _DtCm_rtable_lookup_range_2_svc();
#define	rtable_abbreviated_lookup_range ((unsigned long)(5))
extern  Table_Res_2 * _DtCm_rtable_abbreviated_lookup_range_2();
extern  Table_Res_2 * _DtCm_rtable_abbreviated_lookup_range_2_svc();
#define	rtable_insert ((unsigned long)(6))
extern  Table_Res_2 * _DtCm_rtable_insert_2();
extern  Table_Res_2 * _DtCm_rtable_insert_2_svc();
#define	rtable_delete ((unsigned long)(7))
extern  Table_Res_2 * _DtCm_rtable_delete_2();
extern  Table_Res_2 * _DtCm_rtable_delete_2_svc();
#define	rtable_delete_instance ((unsigned long)(8))
extern  Table_Res_2 * _DtCm_rtable_delete_instance_2();
extern  Table_Res_2 * _DtCm_rtable_delete_instance_2_svc();
#define	rtable_change ((unsigned long)(9))
extern  Table_Res_2 * _DtCm_rtable_change_2();
extern  Table_Res_2 * _DtCm_rtable_change_2_svc();
#define	rtable_change_instance ((unsigned long)(10))
extern  Table_Res_2 * _DtCm_rtable_change_instance_2();
extern  Table_Res_2 * _DtCm_rtable_change_instance_2_svc();
#define	rtable_lookup_next_reminder ((unsigned long)(11))
extern  Table_Res_2 * _DtCm_rtable_lookup_next_reminder_2();
extern  Table_Res_2 * _DtCm_rtable_lookup_next_reminder_2_svc();
#define	rtable_check ((unsigned long)(12))
extern  Table_Status_2 * _DtCm_rtable_check_2();
extern  Table_Status_2 * _DtCm_rtable_check_2_svc();
#define	rtable_flush_table ((unsigned long)(13))
extern  Table_Status_2 * _DtCm_rtable_flush_table_2();
extern  Table_Status_2 * _DtCm_rtable_flush_table_2_svc();
#define	rtable_size ((unsigned long)(14))
extern  int * _DtCm_rtable_size_2();
extern  int * _DtCm_rtable_size_2_svc();
#define	register_callback ((unsigned long)(15))
extern  Registration_Status_2 * _DtCm_register_callback_2();
extern  Registration_Status_2 * _DtCm_register_callback_2_svc();
#define	deregister_callback ((unsigned long)(16))
extern  Registration_Status_2 * _DtCm_deregister_callback_2();
extern  Registration_Status_2 * _DtCm_deregister_callback_2_svc();
#define	rtable_set_access ((unsigned long)(17))
extern  Access_Status_2 * _DtCm_rtable_set_access_2();
extern  Access_Status_2 * _DtCm_rtable_set_access_2_svc();
#define	rtable_get_access ((unsigned long)(18))
extern  Access_Args_2 * _DtCm_rtable_get_access_2();
extern  Access_Args_2 * _DtCm_rtable_get_access_2_svc();
extern int tableprog_2_freeresult();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t _DtCm_xdr_Buffer_2(XDR *, Buffer_2*);
extern  bool_t _DtCm_xdr_Transaction_2(XDR *, Transaction_2*);
extern  bool_t _DtCm_xdr_Period_2(XDR *, Period_2*);
extern  bool_t _DtCm_xdr_Tag_2(XDR *, Tag_2*);
extern  bool_t _DtCm_xdr_Attribute_2(XDR *, Attribute_2*);
extern  bool_t _DtCm_xdr_Attr_2(XDR *, Attr_2*);
extern  bool_t _DtCm_xdr_Except_2(XDR *, Except_2*);
extern  bool_t _DtCm_xdr_Exception_2(XDR *, Exception_2*);
extern  bool_t _DtCm_xdr_Id_2(XDR *, Id_2*);
extern  bool_t _DtCm_xdr_Uid_2(XDR *, Uid_2*);
extern  bool_t _DtCm_xdr_Appt_2(XDR *, Appt_2*);
extern  bool_t _DtCm_xdr_Abb_Appt_2(XDR *, Abb_Appt_2*);
extern  bool_t _DtCm_xdr_Apptid_2(XDR *, Apptid_2*);
extern  bool_t _DtCm_xdr_Reminder_2(XDR *, Reminder_2*);
extern  bool_t _DtCm_xdr_Table_Res_Type_2(XDR *, Table_Res_Type_2*);
extern  bool_t _DtCm_xdr_Table_Res_List_2(XDR *, Table_Res_List_2*);
extern  bool_t _DtCm_xdr_Access_Status_2(XDR *, Access_Status_2*);
extern  bool_t _DtCm_xdr_Table_Res_2(XDR *, Table_Res_2*);
extern  bool_t _DtCm_xdr_Access_Entry_2(XDR *, Access_Entry_2*);
extern  bool_t _DtCm_xdr_Access_Args_2(XDR *, Access_Args_2*);
extern  bool_t _DtCm_xdr_Range_2(XDR *, Range_2*);
extern  bool_t _DtCm_xdr_Table_Args_Type_2(XDR *, Table_Args_Type_2*);
extern  bool_t _DtCm_xdr_Args_2(XDR *, Args_2*);
extern  bool_t _DtCm_xdr_Table_Args_2(XDR *, Table_Args_2*);
extern  bool_t _DtCm_xdr_Registration_2(XDR *, Registration_2*);
extern  bool_t _DtCm_xdr_Table_Status_2(XDR *, Table_Status_2*);
extern  bool_t _DtCm_xdr_Registration_Status_2(XDR *, Registration_Status_2*);

#else /* K&R C */
extern bool_t _DtCm_xdr_Buffer_2();
extern bool_t _DtCm_xdr_Transaction_2();
extern bool_t _DtCm_xdr_Period_2();
extern bool_t _DtCm_xdr_Tag_2();
extern bool_t _DtCm_xdr_Attribute_2();
extern bool_t _DtCm_xdr_Attr_2();
extern bool_t _DtCm_xdr_Except_2();
extern bool_t _DtCm_xdr_Exception_2();
extern bool_t _DtCm_xdr_Id_2();
extern bool_t _DtCm_xdr_Uid_2();
extern bool_t _DtCm_xdr_Appt_2();
extern bool_t _DtCm_xdr_Abb_Appt_2();
extern bool_t _DtCm_xdr_Apptid_2();
extern bool_t _DtCm_xdr_Reminder_2();
extern bool_t _DtCm_xdr_Table_Res_Type_2();
extern bool_t _DtCm_xdr_Table_Res_List_2();
extern bool_t _DtCm_xdr_Access_Status_2();
extern bool_t _DtCm_xdr_Table_Res_2();
extern bool_t _DtCm_xdr_Access_Entry_2();
extern bool_t _DtCm_xdr_Access_Args_2();
extern bool_t _DtCm_xdr_Range_2();
extern bool_t _DtCm_xdr_Table_Args_Type_2();
extern bool_t _DtCm_xdr_Args_2();
extern bool_t _DtCm_xdr_Table_Args_2();
extern bool_t _DtCm_xdr_Registration_2();
extern bool_t _DtCm_xdr_Table_Status_2();
extern bool_t _DtCm_xdr_Registration_Status_2();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#ifdef HPUX
#undef hpux
#define hpux
#endif
#endif

