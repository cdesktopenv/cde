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
/* $XConsortium: convert3-4.c /main/1 1996/04/21 19:22:31 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <string.h>
#include <stdlib.h>
#include "rtable3.h"
#include "rtable4.h"
#include "convert3-4.h"

/*
 * forward declaration of static functions
 */
static void assign_period3to4(Period_3 *p3, Period_4 *p4);
static void assign_tag3to4(Tag_3 *t3, Tag_4 *t4);
static void assign_privacy3to4(Privacy_Level_3 *p3, Privacy_Level_4 *p4);
static void assign_status3to4(Appt_Status_3 *s3, Appt_Status_4 *s4);
static Buffer_4 buffer3_to_buffer4(Buffer_3 b);
static void period3_to_period4(Period_3 *p3, Period_4 *p4);
static Tag_4 * tag3_to_tag4(Tag_3 *t3);
static Attribute_4 * attr3_to_attr4(Attribute_3 *a3);
static Except_4 * except3_to_except4(Except_3 *e3);
static void id3_to_id4(Id_3 *from, Id_4 *to);
static Uid_4 * uid3_to_uid4(Uid_3 *ui3);
static Appt_4 * appt3_to_appt4(Appt_3 *a3);
static Abb_Appt_4 * abb3_to_abb4(Abb_Appt_3 *a3);
static void apptid3_to_apptid4(Apptid_3 *from, Apptid_4 *to);
static Reminder_4 * reminder3_to_reminder4(Reminder_3 *r3);
static Table_Res_Type_4 tablerestype3_to_tablerestype4(Table_Res_Type_3 t);
static void tablereslist3_to_tablereslist4(Table_Res_List_3 *from,
		Table_Res_List_4 *to);
static Access_Entry_4 * acclist3_to_acclist4(Access_Entry_3 *l3);
static Range_4 * range3_to_range4(Range_3 *r3);
static Keyrange_4 * keyrange3_to_keyrange4(Keyrange_3 *r3);
static Table_Args_Type_4 argstag3_to_argstag4(Table_Args_Type_3 t);
static void args3_to_args4(Args_3 *from, Args_4 *to);
static Uidopt_4 * uid3_to_uidopt(Uid_3 *uid3, Options_4 opt);

/**************** DATA TYPE (3->4) CONVERSION ROUTINES **************/

extern Access_Status_4
_DtCm_accstat3_to_accstat4(Access_Status_3 s)
{
        switch (s) {
        case access_ok_3:
                return(access_ok_4);
        case access_added_3:
                return(access_added_4);
        case access_removed_3:
                return(access_removed_4);
        case access_failed_3:
                return(access_failed_4);
        case access_exists_3:
                return(access_exists_4);
        case access_partial_3:
                return(access_partial_4);
        case access_other_3:
        default:
                return(access_other_4);
        }
}

extern Table_Res_4 *
_DtCm_tableres3_to_tableres4(Table_Res_3 *r3)
{
        Table_Res_4 *r4;
	
	if (r3==NULL) return((Table_Res_4 *)NULL);
	r4 = (Table_Res_4 *)calloc(1, sizeof(Table_Res_4));
        r4->status = _DtCm_accstat3_to_accstat4(r3->status);
        tablereslist3_to_tablereslist4(&(r3->res), &(r4->res));
        return(r4);
}

extern Access_Args_4 *
_DtCm_accargs3_to_accargs4(Access_Args_3 *a3)
{
	Access_Args_4 *a4;

	if (a3==NULL) return((Access_Args_4 *)NULL);
	a4 = (Access_Args_4 *)calloc(1, sizeof(Access_Args_4));
	a4->target = buffer3_to_buffer4(a3->target);
	a4->access_list = acclist3_to_acclist4(a3->access_list);
	return(a4);
}

extern Table_Args_4 *
_DtCm_tableargs3_to_tableargs4(Table_Args_3 *a3)
{
        Table_Args_4 *a4;

	if (a3==NULL) return((Table_Args_4 *)NULL);
	a4 = (Table_Args_4 *)calloc(1, sizeof(Table_Args_4));
        a4->target = buffer3_to_buffer4(a3->target);
        args3_to_args4(&(a3->args), &(a4->args));
	a4->pid = a3->pid;
        return(a4);
}

extern Table_Args_4 *
_DtCm_tabledelargs3_to_tabledelargs4(Table_Args_3 *a3, Options_4 opt)
{
	Table_Args_4 *a4;

	if (a3 == NULL)
		return((Table_Args_4 *)NULL);

	a4 = (Table_Args_4 *)calloc(1, sizeof(Table_Args_4));
	a4->target = buffer3_to_buffer4(a3->target);
	a4->pid = a3->pid;
	a4->args.tag = UIDOPT_4;
	a4->args.Args_4_u.uidopt = uid3_to_uidopt(a3->args.Args_3_u.key, opt);
	return(a4);
}

extern Registration_Status_4
_DtCm_regstat3_to_regstat4(Registration_Status_3 s)
{
	switch(s) {
	case registered_3:
		return(registered_4);
	case failed_3:
		return(failed_4);
	case deregistered_3:
		return(deregistered_4);
	case confused_3:
		return(confused_4);
	default:
		return(failed_4);
	}
}

extern Registration_4 *
_DtCm_reg3_to_reg4(Registration_3 *r3)
{
	Registration_4 *r4, *head, *prev;

	prev = head = NULL;
	while (r3 != NULL) {
		r4 = (Registration_4 *)calloc(1, sizeof(Registration_4));
		r4->target = buffer3_to_buffer4(r3->target);
		r4->prognum = r3->prognum;
		r4->versnum = r3->versnum;
		r4->procnum = r3->procnum;
		r4->next = NULL;
		r4->pid = r3->pid;

		if (head == NULL)
			head = r4;
		else
			prev->next = r4;
		prev = r4;

		r3 = r3->next;
	}
	return(head);
}

extern Table_Status_4
_DtCm_tablestat3_to_tablestat4(Table_Status_3 s)
{
	switch(s) {
	case ok_3:
		return(ok_4);
	case duplicate_3:
		return(duplicate_4);
	case badtable_3:
		return(badtable_4);
	case notable_3:
		return(notable_4);
	case denied_3:
		return(denied_4);
	case other_3:
	default:
		return(other_4);
	}
}

static void
assign_period3to4(Period_3 *p3, Period_4 *p4)
{
	switch (p3->period) {

	case single_3: p4->period = single_4;
			break;

	case daily_3: p4->period = daily_4;
			break;

	case weekly_3: p4->period = weekly_4;
			break;

	case biweekly_3: p4->period = biweekly_4;
			break;

	case monthly_3: p4->period = monthly_4;
			break;

	case yearly_3: p4->period = yearly_4;
			break;

	case nthWeekday_3: p4->period = nthWeekday_4;
			break;

	case everyNthDay_3: p4->period = everyNthDay_4;
			break;

	case everyNthWeek_3: p4->period = everyNthWeek_4;
			break;

	case everyNthMonth_3: p4->period = everyNthMonth_4;
			break;

	case otherPeriod_3: p4->period = otherPeriod_4;
			break;
	}
}

static void
assign_tag3to4(Tag_3 *t3, Tag_4 *t4)
{
	switch (t3->tag) {

	case appointment_3: t4->tag = appointment_4;
			break;

	case reminder_3: t4->tag = reminder_4;
			break;

	case otherTag_3: t4->tag = otherTag_4;
			break;

	case holiday_3: t4->tag = holiday_4;
			break;

	case toDo_3: t4->tag = toDo_4;
			break;
	}
}

static void
assign_privacy3to4(Privacy_Level_3 *p3, Privacy_Level_4 *p4)
{
	switch (*p3) {

	case public_3: *p4 = public_4;
			break;

	case private_3: *p4 = private_4;
			break;

	case semiprivate_3: *p4 = semiprivate_4;
			break;
	}
}

static void
assign_status3to4(Appt_Status_3 *s3, Appt_Status_4 *s4)
{
	switch (*s3) {

	case active_3: *s4 = active_4;
			break;

	case pendingAdd_3: *s4 = pendingAdd_4;
			break;

	case pendingDelete_3: *s4 = pendingDelete_4;
			break;

	case committed_3: *s4 = committed_4;
			break;

	case cancelled_3: *s4 = cancelled_4;
			break;

	case completed_3: *s4 = completed_4;
			break;
	}
}

static Buffer_4
buffer3_to_buffer4(Buffer_3 b)
{
	Buffer_4 copy;
	if (b!=NULL)
		copy = strdup(b);
	else
		copy = calloc(1, 1);
        return(copy);
}

static void
period3_to_period4(Period_3 *p3, Period_4 *p4)
{
	if (p3 == NULL || p4==NULL) return;
	assign_period3to4(p3, p4);
	p4->nth = p3->nth;
}

static Tag_4 *
tag3_to_tag4(Tag_3 *t3)
{
	Tag_4 *t4, *head, *prev;

	prev = head = NULL;
	while (t3 != NULL) {
		t4 = (Tag_4 *)calloc(1, sizeof(Tag_4));
		assign_tag3to4(t3, t4);
		t4->showtime = t3->showtime;
		t4->next = NULL;

		if (head == NULL)
			head = t4;
		else
			prev->next = t4;
		prev = t4;

		t3 = t3->next;
	}
	return(head);
}

static Attribute_4 *
attr3_to_attr4(Attribute_3 *a3)
{
        Attribute_4 *a4, *head, *prev;
 
	prev = head = NULL;
	while (a3 != NULL) {
		a4 = (Attribute_4 *)calloc(1, sizeof(Attribute_4));
		a4->next = NULL;
		a4->attr = buffer3_to_buffer4(a3->attr);
		a4->value = buffer3_to_buffer4(a3->value);
		a4->clientdata = buffer3_to_buffer4(a3->clientdata);

		if (head == NULL)
			head = a4;
		else
			prev->next = a4;
		prev = a4;

		a3 = a3->next;
	}
	return(head);
}

static Except_4 *
except3_to_except4(Except_3 *e3)
{
	Except_4 *e4, *head, *prev;

	prev = head = NULL;
	while (e3 != NULL) {
		e4  = (Except_4 *)calloc(1, sizeof(Except_4));
		e4->ordinal = e3->ordinal;
		e4->next=NULL;

		if (head == NULL)
			head = e4;
		else
			prev->next = e4;
		prev = e4;

		e3 = e3->next;
	}
	return(head);
}

static void
id3_to_id4(Id_3 *from, Id_4 *to)
{
	if ((from==NULL) || (to==NULL)) return;
        to->tick = from->tick;
        to->key = from->key;
}

static Uid_4 *
uid3_to_uid4(Uid_3 *ui3)
{
        Uid_4 *ui4, *head, *prev;
 
	prev = head = NULL;
	while (ui3 != NULL) {
        	ui4 = (Uid_4 *)calloc(1, sizeof(Uid_4));
        	id3_to_id4(&(ui3->appt_id), &(ui4->appt_id));
        	ui4->next = NULL;

		if (head == NULL)
			head = ui4;
		else
			prev->next = ui4;
		prev = ui4;

		ui3 = ui3->next;
	}
	return(head);
}
 
static Appt_4 *
appt3_to_appt4(Appt_3 *a3)
{
        Appt_4 *a4, *head, *prev;

	prev = head = NULL;
	while (a3 != NULL) {
		a4  = (Appt_4 *)calloc(1, sizeof(Appt_4));
		id3_to_id4(&(a3->appt_id), &(a4->appt_id));
		a4->tag = tag3_to_tag4(a3->tag);
		a4->duration = a3->duration;
		a4->ntimes = a3->ntimes;
		a4->what = buffer3_to_buffer4(a3->what);
		period3_to_period4(&(a3->period), &(a4->period));
		a4->author = buffer3_to_buffer4(a3->author);
		a4->client_data = buffer3_to_buffer4(a3->client_data);
		a4->exception = except3_to_except4(a3->exception);
		a4->attr = attr3_to_attr4(a3->attr);
		assign_status3to4(&a3->appt_status, &a4->appt_status);
		assign_privacy3to4(&a3->privacy, &a4->privacy);
		a4->next = NULL;

		if (head == NULL)
			head = a4;
		else
			prev->next = a4;
		prev = a4;

		a3 = a3->next;
	}
	return(head);
}

static Abb_Appt_4 *
abb3_to_abb4(Abb_Appt_3 *a3)
{
        Abb_Appt_4 *a4, *head, *prev;
 
	prev = head = NULL;
	while (a3 != NULL) {
		a4 = (Abb_Appt_4 *)calloc(1, sizeof(Abb_Appt_4));
		id3_to_id4(&(a3->appt_id), &(a4->appt_id));
		a4->tag = tag3_to_tag4(a3->tag);
		a4->what = buffer3_to_buffer4(a3->what);
		a4->duration = a3->duration;
		period3_to_period4(&(a3->period), &(a4->period));
		assign_status3to4(&a3->appt_status, &a4->appt_status);
		assign_privacy3to4(&a3->privacy, &a4->privacy);

		a4->next = NULL;

		if (head == NULL)
			head = a4;
		else
			prev->next = a4;
		prev = a4;

		a3 = a3->next;
	}
	return(head);
}

static void
apptid3_to_apptid4(Apptid_3 *from, Apptid_4 *to)
{
        if (from==NULL || to==NULL) return;
        id3_to_id4(from->oid, to->oid);
        to->new_appt = appt3_to_appt4(from->new_appt);
	/* do_all is the default, the caller needs to
	 * modify it to the appropriate value
	 */
	to->option = do_all_4;
}

static Reminder_4 *
reminder3_to_reminder4(Reminder_3 *r3)
{
        Reminder_4 *r4, *head, *prev;
	Attribute_4 *attr4;

	prev = head = NULL;
	while (r3 != NULL) {
		r4 = (Reminder_4 *)calloc(1, sizeof(Reminder_4));
		id3_to_id4(&(r3->appt_id), &(r4->appt_id));
		r4->tick = r3->tick;
		attr4 = attr3_to_attr4(&(r3->attr));
		r4->attr = *attr4;
		free(attr4);
		r4->next = NULL;

		if (head == NULL)
			head = r4;
		else
			prev->next = r4;
		prev = r4;

		r3 = r3->next;
	}
	return(head);
}
      
static Table_Res_Type_4
tablerestype3_to_tablerestype4(Table_Res_Type_3 t)
{
        switch(t) {
        case AP_3:
                return(AP_4);
        case RM_3:
                return(RM_4);
        case AB_3:
                return(AB_4);
        case ID_3:
                return(ID_4);
        default:
                return(AP_4);
        }
}

static void
tablereslist3_to_tablereslist4(Table_Res_List_3 *from, Table_Res_List_4 *to)
{
        if (from==NULL || to==NULL) return;
        to->tag = tablerestype3_to_tablerestype4(from->tag);
        switch (from->tag) {
        case AP_3:
                to->Table_Res_List_4_u.a = appt3_to_appt4(
                        from->Table_Res_List_3_u.a);
                break;
        case RM_3:
                to->Table_Res_List_4_u.r = reminder3_to_reminder4(
                        from->Table_Res_List_3_u.r);
                break;
        case AB_3:
                to->Table_Res_List_4_u.b = abb3_to_abb4(
                        from->Table_Res_List_3_u.b);
                break;
        case ID_3:
                to->Table_Res_List_4_u.i = uid3_to_uid4(
                        from->Table_Res_List_3_u.i);
		break;
        default:
                return;
        }
}

static Access_Entry_4 *
acclist3_to_acclist4(Access_Entry_3 *l3)
{
	Access_Entry_4 *l4, *head, *prev;

	prev = head = NULL;
	while (l3 != NULL) {
		l4 = (Access_Entry_4 *)calloc(1, sizeof(Access_Entry_4));
		l4->who = buffer3_to_buffer4(l3->who);
		l4->access_type = l3->access_type;
		l4->next = NULL;

		if (head == NULL)
			head = l4;
		else
			prev->next = l4;
		prev = l4;

		l3 = l3->next;
	}
	return(head);
}

static Range_4 *
range3_to_range4(Range_3 *r3)
{
        Range_4 *r4, *head, *prev;
 
	prev = head = NULL;
	while (r3 != NULL) {
		r4 = (Range_4 *)calloc(1, sizeof(Range_4));
		r4->key1 = r3->key1;
		r4->key2 = r3->key2;
		r4->next = NULL;

		if (head == NULL)
			head = r4;
		else
			prev->next = r4;
		prev = r4;

		r3 = r3->next;
	}
	return(head);
}

static Keyrange_4 *
keyrange3_to_keyrange4(Keyrange_3 *r3)
{
        Keyrange_4 *r4, *head, *prev;
 
	prev = head = NULL;
	while (r3 != NULL) {
		r4 = (Keyrange_4 *)calloc(1, sizeof(Keyrange_4));
		r4->key = r3->key;
		r4->tick1 = r3->tick1;
		r4->tick2 = r3->tick2;
		r4->next = NULL;

		if (head == NULL)
			head = r4;
		else
			prev->next = r4;
		prev = r4;

		r3 = r3->next;
	}
	return(head);
}

static Table_Args_Type_4
argstag3_to_argstag4(Table_Args_Type_3 t)
{
	switch(t) {
	case TICK_3:
		return(TICK_4);
	case APPTID_3:
		return(APPTID_4);
	case UID_3:
		return(UID_4);
	case APPT_3:
		return(APPT_4);
	case RANGE_3:
		return(RANGE_4);
	case KEYRANGE_3:
		return(KEYRANGE_4);
	default:
		return(TICK_4);
	}
}

static void
args3_to_args4(Args_3 *from, Args_4 *to)
{
        if (from==NULL || to==NULL) return;
        to->tag = argstag3_to_argstag4(from->tag);
        switch(from->tag) {
        case TICK_3:
                to->Args_4_u.tick = from->Args_3_u.tick;
                break;
        case APPTID_3:
		to->Args_4_u.apptid.oid = (Id_4 *)calloc(1, sizeof(Id_4));
                apptid3_to_apptid4(
			&(from->Args_3_u.apptid),
                        &(to->Args_4_u.apptid));
                break;
        case UID_3:
                to->Args_4_u.key = uid3_to_uid4(from->Args_3_u.key);
                break;
        case APPT_3:
                to->Args_4_u.appt = appt3_to_appt4(from->Args_3_u.appt);
                break;
        case RANGE_3:
                to->Args_4_u.range = range3_to_range4(from->Args_3_u.range);
                break;
	case KEYRANGE_3:
		to->Args_4_u.keyrange = keyrange3_to_keyrange4(
			from->Args_3_u.keyrange);
        default:
                break;
        }
}

static Uidopt_4 *
uid3_to_uidopt(Uid_3 *uid3, Options_4 opt)
{
        Uidopt_4 *uidopt, *head, *prev;
 
	prev = head = NULL;
	while (uid3 != NULL) {
        	uidopt = (Uidopt_4 *)calloc(1, sizeof(Uidopt_4));
        	id3_to_id4(&(uid3->appt_id), &(uidopt->appt_id));
		uidopt->option = opt;
        	uidopt->next = NULL;

		if (head == NULL)
			head = uidopt;
		else
			prev->next = uidopt;
		prev = uidopt;

		uid3 = uid3->next;
	}
	return(head);
}
 
