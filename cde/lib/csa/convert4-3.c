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
/* $XConsortium: convert4-3.c /main/1 1996/04/21 19:22:43 drk $ */
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
#include "convert4-3.h"

/*
 * forward declaration of static functions
 */
static void free_excpt3(Except_3 *e);
static Abb_Appt_3 * abb4_to_abb3(Abb_Appt_4 *a4);
static void apptid4_to_apptid3(Apptid_4 *from, Apptid_3 *to);
static Reminder_3 * reminder4_to_reminder3(Reminder_4 *r4);
static Table_Res_Type_3 tablerestype4_to_tablerestype3(Table_Res_Type_4 t);
static void tablereslist4_to_tablereslist3(Table_Res_List_4 *from,
		Table_Res_List_3 *to);
static void period4_to_period3(Period_4 *p4, Period_3 *p3);
static void assign_tag4to3(Tag_4 *t4, Tag_3 *t3);
static void assign_status4to3(Appt_Status_4 *s4, Appt_Status_3 *s3);
static void assign_privacy4to3(Privacy_Level_4 *p4, Privacy_Level_3 *p3);
static Buffer_3 buffer4_to_buffer3(Buffer_4 b);
static Tag_3 * tag4_to_tag3(Tag_4 *t4);
static Attribute_3 * attr4_to_attr3(Attribute_4 *a4);
static Except_3 * except4_to_except3(Except_4 *e4);
static Table_Args_Type_3 argstag4_to_argstag3(Table_Args_Type_4 t);
static void args4_to_args3(Args_4 *from, Args_3 *to);
static Table_Args_3 * tableargs4_to_tableargs3(Table_Args_4 *a4);
static Registration_3 * reg4_to_reg3(Registration_4 *r4);
static void free_tag3(Tag_3 *t);

/**************** DATA TYPE (4->3) CONVERSION ROUTINES **************/

extern void
_DtCm_id4_to_id3(Id_4 *from, Id_3 *to)
{
        if ((from==NULL) || (to==NULL)) return;	
        to->tick = from->tick;
        to->key = from->key;
}

extern Uid_3 *
_DtCm_uid4_to_uid3(Uid_4 *ui4)
{
        Uid_3 *ui3, *head, *prev;

	prev = head = NULL;
	while (ui4 != NULL) {
		ui3 = (Uid_3 *)calloc(1, sizeof(Uid_3));
		_DtCm_id4_to_id3(&(ui4->appt_id), &(ui3->appt_id));
		ui3->next = NULL;

		if (head == NULL)
			head = ui3;
		else
			prev->next = ui3;
		prev = ui3;

		ui4 = ui4->next;
	}
	return(head);
}

extern Appt_3 *
_DtCm_appt4_to_appt3(Appt_4 *a4)
{
        Appt_3 *a3, *head, *prev;

	prev = head = NULL;
	while (a4 != NULL) {
		a3  = (Appt_3 *)calloc(1, sizeof(Appt_3));
		_DtCm_id4_to_id3(&(a4->appt_id), &(a3->appt_id));
		a3->tag = tag4_to_tag3(a4->tag);
		a3->duration = a4->duration;
		a3->ntimes = a4->ntimes;
		a3->what = buffer4_to_buffer3(a4->what);
		period4_to_period3(&(a4->period), &(a3->period));
		a3->author = buffer4_to_buffer3(a4->author);
		a3->client_data = buffer4_to_buffer3(a4->client_data);
		a3->exception = except4_to_except3(a4->exception);
		a3->attr = attr4_to_attr3(a4->attr);
		assign_status4to3(&a4->appt_status, &a3->appt_status);
		assign_privacy4to3(&a4->privacy, &a3->privacy);
		a3->next = NULL;

		if (head == NULL)
			head = a3;
		else
			prev->next = a3;
		prev = a3;

		a4 = a4->next;
	}
	return(head);
}

extern void
_DtCm_free_appt3(Appt_3 *a)
{
	Appt_3 *ptr;

	while (a != NULL) {
		ptr = a->next;

		if (a->tag != NULL)
			free_tag3(a->tag);

		if (a->what != NULL)
			free(a->what);

		if (a->author != NULL)
			free(a->author);

		if (a->client_data != NULL)
			free(a->client_data);

		if (a->attr != NULL)
			_DtCm_free_attr3(a->attr);

		if (a->exception != NULL)
			free_excpt3(a->exception);

     		free(a);

		a = ptr;
	}
}

extern void
_DtCm_free_attr3(Attribute_3 *a)
{
	Attribute_3 *ptr;

	while (a != NULL) {
		ptr = a->next;
		if (a->attr != NULL)
			free(a->attr);
		if (a->value != NULL)
			free(a->value);
		if (a->clientdata != NULL)
			free(a->clientdata);
		free(a);

		a = ptr;
	}
}

static void
free_excpt3(Except_3 *e)
{
	Except_3 *ptr;

	while (e != NULL) {
		ptr = e->next;
		free(e);
		e = ptr;
	}
}

static Abb_Appt_3 *
abb4_to_abb3(Abb_Appt_4 *a4)
{
        Abb_Appt_3 *a3, *head, *prev;

	prev = head = NULL;
	while (a4 != NULL) {
		a3 = (Abb_Appt_3 *)calloc(1, sizeof(Abb_Appt_3));
		_DtCm_id4_to_id3(&(a4->appt_id), &(a3->appt_id));
		a3->tag = tag4_to_tag3(a4->tag);
		a3->what = buffer4_to_buffer3(a4->what);
		a3->duration = a4->duration;
		period4_to_period3(&(a4->period), &(a3->period));
		assign_status4to3(&a4->appt_status, &a3->appt_status);
		assign_privacy4to3(&a4->privacy, &a3->privacy);
		a3->next = NULL;

		if (head == NULL)
			head = a3;
		else
			prev->next = a3;
		prev = a3;

		a4 = a4->next;
	}
	return(head);
}

static void
apptid4_to_apptid3(Apptid_4 *from, Apptid_3 *to)
{
        if (from==NULL || to==NULL) return;
        _DtCm_id4_to_id3(from->oid, to->oid);
        to->new_appt = _DtCm_appt4_to_appt3(from->new_appt);
}

static Reminder_3 *
reminder4_to_reminder3(Reminder_4 *r4)
{
        Reminder_3 *r3, *head, *prev;
	Attribute_3 *attr3;

	prev = head = NULL;
	while (r4 != NULL) {
		r3 = (Reminder_3 *)calloc(1, sizeof(Reminder_3));
		_DtCm_id4_to_id3(&(r4->appt_id), &(r3->appt_id));
		r3->tick = r4->tick;
		attr3 = attr4_to_attr3(&(r4->attr));
		r3->attr = *attr3;
		free(attr3);
		r3->next = NULL;

		if (head == NULL)
			head = r3;
		else
			prev->next = r3;
		prev = r3;

		r4 = r4->next;
	}
	return(head);
}

static Table_Res_Type_3
tablerestype4_to_tablerestype3(Table_Res_Type_4 t)
{
	switch(t) {
	case AP_4:
		return(AP_3);
	case RM_4:
		return(RM_3);
	case AB_4:
		return(AB_3);
	case ID_4:
		return(ID_3);
	default:
		return(AP_3);
	}
}

static void
tablereslist4_to_tablereslist3(Table_Res_List_4 *from, Table_Res_List_3 *to)
{
        if (from==NULL || to==NULL) return;
        to->tag = tablerestype4_to_tablerestype3(from->tag);
        switch (from->tag) {
        case AP_4:
                to->Table_Res_List_3_u.a = _DtCm_appt4_to_appt3(
                        from->Table_Res_List_4_u.a);
                break;
        case RM_4:
                to->Table_Res_List_3_u.r = reminder4_to_reminder3(
                        from->Table_Res_List_4_u.r);
                break;
        case AB_4:
                to->Table_Res_List_3_u.b = abb4_to_abb3(
                        from->Table_Res_List_4_u.b);
                break;
        case ID_4:
                to->Table_Res_List_3_u.i = _DtCm_uid4_to_uid3(
                        from->Table_Res_List_4_u.i);
        default:
                return;
        }
}
 
extern Access_Status_3
_DtCm_accstat4_to_accstat3(Access_Status_4 s)
{
	switch(s) {
	case access_ok_4:
		return(access_ok_3);
	case access_added_4:
		return(access_added_3);
	case access_removed_4:
		return(access_removed_3);
	case access_failed_4:
		return(access_failed_3);
	case access_exists_4:
		return(access_exists_3);
	case access_partial_4:
		return(access_partial_3);
	case access_other_4:
	default:
		return(access_other_3);
	}
}

extern Table_Res_3 *
_DtCm_tableres4_to_tableres3(Table_Res_4 *r4)
{
        Table_Res_3 *r3;

	if (r4==NULL) return((Table_Res_3 *)NULL);
	r3 = (Table_Res_3 *)calloc(1, sizeof(Table_Res_3));
        r3->status = _DtCm_accstat4_to_accstat3(r4->status);
        tablereslist4_to_tablereslist3(&(r4->res), &(r3->res));
        return(r3);
}

extern Access_Entry_3 *
_DtCm_acclist4_to_acclist3(Access_Entry_4 *l4)
{
        Access_Entry_3 *l3, *head, *prev;

	prev = head = NULL;
	while (l4 != NULL) {
		l3 = (Access_Entry_3 *)calloc(1, sizeof(Access_Entry_3));
		l3->who = buffer4_to_buffer3(l4->who);
		l3->access_type = l4->access_type;
		l3->next = NULL;

		if (head == NULL)
			head = l3;
		else
			prev->next = l3;
		prev = l3;

		l4 = l4->next;
	}
	return(head);
}
 
extern Access_Args_3 *
_DtCm_accargs4_to_accargs3(Access_Args_4 *a4)
{
        Access_Args_3 *a3;

	if (a4==NULL) return((Access_Args_3 *)NULL);
	a3 = (Access_Args_3 *)calloc(1, sizeof(Access_Args_3));
        a3->target = buffer4_to_buffer3(a4->target);
        a3->access_list = _DtCm_acclist4_to_acclist3(a4->access_list);
        return(a3);
}

extern Range_3 *
_DtCm_range4_to_range3(Range_4 *r4)
{
	Range_3 *r3, *head, *prev;

	prev = head = NULL;
	while (r4 != NULL) {
		r3 = (Range_3 *)calloc(1, sizeof(Range_3));
		r3->key1 = r4->key1;
		r3->key2 = r4->key2;
		r3->next = NULL;

		if (head == NULL)
			head = r3;
		else
			prev->next = r3;
		prev = r3;

		r4 = r4->next;
	}
	return(head);
}

extern Keyrange_3 *
_DtCm_keyrange4_to_keyrange3(Keyrange_4 *r4)
{
        Keyrange_3 *r3, *head, *prev;
 
	prev = head = NULL;
	while (r4 != NULL) {
		r3 = (Keyrange_3 *)calloc(1, sizeof(Keyrange_3));
		r3->key = r4->key;
		r3->tick1 = r4->tick1;
		r3->tick2 = r4->tick2;
		r3->next = NULL;

		if (head == NULL)
			head = r3;
		else
			prev->next = r3;
		prev = r3;

		r4 = r4->next;
	}
	return(head);
}

extern Registration_Status_3 
_DtCm_regstat4_to_regstat3(Registration_Status_4 s)
{
	switch (s) {
	case registered_4:
		return(registered_3);
	case failed_4:
		return(failed_3);
	case deregistered_4:
		return(deregistered_3);
	case confused_4:
		return(confused_3);
	case reg_notable_4:
	default:
		return(failed_3);
	}
}

extern Table_Status_3
_DtCm_tablestat4_to_tablestat3(Table_Status_4 s)
{
	switch(s) {
	case ok_4:
		return(ok_3);
	case duplicate_4:
		return(duplicate_3);
	case badtable_4:
		return(badtable_3);
	case notable_4:
		return(notable_3);
	case denied_4:
		return(denied_3);
	case other_4:
	default:
		return(other_3);
	}
}

extern Uid_3 *
_DtCm_uidopt4_to_uid3(Uidopt_4 *uidopt)
{
        Uid_3 *uid3, *head, *prev;
 
	prev = head = NULL;
	while (uidopt != NULL) {
        	uid3 = (Uid_3 *)calloc(1, sizeof(Uid_3));
        	_DtCm_id4_to_id3(&(uidopt->appt_id), &(uid3->appt_id));
        	uid3->next = NULL;

		if (head == NULL)
			head = uid3;
		else
			prev->next = uid3;
		prev = uid3;

		uidopt = uidopt->next;
	}
	return(head);
}
 
/*
 * Repeating event types beyond "yearly" are mapped to "single"
 * because the old front end does not recognize any other types
 * Worse yet it uses Interval to index into an array which
 * contains strings up to "yearly", any period types beyond that
 * would index beyond the array and cause the front end to dump core.
 */
static void
period4_to_period3(Period_4 *p4, Period_3 *p3)
{
	if (p3 == NULL || p4 == NULL) return;

        switch (p4->period) {
 
        case single_4: 	p3->period = single_3;
			p3->nth = p4->nth;
                        break;
 
        case daily_4: 	p3->period = daily_3;
			p3->nth = p4->nth;
                        break;
 
        case weekly_4: 	p3->period = weekly_3;
			p3->nth = p4->nth;
                        break;
 
        case biweekly_4: p3->period = biweekly_3;
			p3->nth = p4->nth;
                        break;
 
        case monthly_4: p3->period = monthly_3;
			p3->nth = p4->nth;
                        break;
 
        case yearly_4: 	p3->period = yearly_3;
			p3->nth = p4->nth;
                        break;

	default: p3->period = single_3;
		 p3->nth = 0;
			break;
        }
}

 
static void
assign_tag4to3(Tag_4 *t4, Tag_3 *t3)
{
        switch (t4->tag) {
 
        case appointment_4: t3->tag = appointment_3;
                        break;
 
        case reminder_4: t3->tag = reminder_3;
                        break;
 
        case otherTag_4: t3->tag = otherTag_3;
                        break;
 
        case holiday_4: t3->tag = holiday_3;
                        break;
 
        case toDo_4: t3->tag = toDo_3;
                        break;
        }
}

 
static void
assign_status4to3(Appt_Status_4 *s4, Appt_Status_3 *s3)
{
        switch (*s4) {
 
        case active_4: *s3 = active_3;
                        break;
 
        case pendingAdd_4: *s3 = pendingAdd_3;
                        break;
 
        case pendingDelete_4: *s3 = pendingDelete_3;
                        break;
 
        case committed_4: *s3 = committed_3;
                        break;
 
        case cancelled_4: *s3 = cancelled_3;
                        break;
 
        case completed_4: *s3 = completed_3;
                        break;
        }
}

 
static void
assign_privacy4to3(Privacy_Level_4 *p4, Privacy_Level_3 *p3)
{
        switch (*p4) {
 
        case public_4: *p3 = public_3;
                        break;
 
        case private_4: *p3 = private_3;
                        break;
 
        case semiprivate_4: *p3 = semiprivate_3;
                        break;
        }
}




static Buffer_3
buffer4_to_buffer3(Buffer_4 b)
{
	Buffer_3 copy;
	if (b!=NULL)
		copy = strdup(b);
	else
		copy = calloc(1, 1);
	return(copy);
}

static Tag_3 *
tag4_to_tag3(Tag_4 *t4)
{
	Tag_3 *t3, *head, *prev;

	prev = head = NULL;
	while (t4 != NULL) {
		t3 = (Tag_3 *)calloc(1, sizeof(Tag_3));
		assign_tag4to3(t4, t3);
		t3->showtime = t4->showtime;
		t3->next = NULL;

		if (head == NULL)
			head = t3;
		else
			prev->next = t3;
		prev = t3;

		t4 = t4->next;
	}
	return(head);
}

static Attribute_3 *
attr4_to_attr3(Attribute_4 *a4)
{
        Attribute_3 *a3, *head, *prev;
 
	prev = head = NULL;
	while (a4 != NULL) {
		a3 = (Attribute_3 *)calloc(1, sizeof(Attribute_3));
		a3->next = NULL;
		a3->attr = buffer4_to_buffer3(a4->attr);
		a3->value = buffer4_to_buffer3(a4->value);
		a3->clientdata = buffer4_to_buffer3(a4->clientdata);

		if (head == NULL)
			head = a3;
		else
			prev->next = a3;
		prev = a3;

		a4 = a4->next;
	}
	return(head);
}

static Except_3 *
except4_to_except3(Except_4 *e4)
{
        Except_3 *e3, *head, *prev;

	prev = head = NULL;
	while (e4 != NULL) {
		e3 = (Except_3 *)calloc(1, sizeof(Except_3));
		e3->ordinal = e4->ordinal;
		e3->next=NULL;

		if (head == NULL)
			head = e3;
		else
			prev->next = e3;
		prev = e3;

		e4 = e4->next;
	}
	return(head);
}
 
static Table_Args_Type_3
argstag4_to_argstag3(Table_Args_Type_4 t)
{
        switch(t) {
        case TICK_4:
                return(TICK_3);
        case APPTID_4:
                return(APPTID_3);
        case UID_4:
                return(UID_3);
        case APPT_4:
                return(APPT_3);
        case RANGE_4:
                return(RANGE_3);
	case KEYRANGE_4:
		return(KEYRANGE_3);
        default:
                return(TICK_3);
        }
}

static void
args4_to_args3(Args_4 *from, Args_3 *to)
{
	if (from==NULL || to==NULL) return;
	to->tag = argstag4_to_argstag3(from->tag);
	switch(from->tag) {
	case TICK_4:
		to->Args_3_u.tick = from->Args_4_u.tick;
		break;
	case APPTID_4:
		to->Args_3_u.apptid.oid = (Id_3 *)calloc(1, sizeof(Id_3));
		apptid4_to_apptid3(
			&(from->Args_4_u.apptid),
			&(to->Args_3_u.apptid));
		break;
	case UID_4:
		to->Args_3_u.key = _DtCm_uid4_to_uid3(from->Args_4_u.key);
		break;
	case APPT_4:
		to->Args_3_u.appt = _DtCm_appt4_to_appt3(from->Args_4_u.appt);
		break;
	case RANGE_4:
		to->Args_3_u.range = _DtCm_range4_to_range3(from->Args_4_u.range);
		break;
	case KEYRANGE_4:
		to->Args_3_u.keyrange = _DtCm_keyrange4_to_keyrange3(
			from->Args_4_u.keyrange);
	default:
		break;
	}
}

static Table_Args_3 *
tableargs4_to_tableargs3(Table_Args_4 *a4)
{
	Table_Args_3 *a3;
	
	if (a4==NULL) return((Table_Args_3 *)NULL);
	a3 = (Table_Args_3 *)calloc(1, sizeof(Table_Args_3));
	a3->target = buffer4_to_buffer3(a4->target);
	args4_to_args3(&(a4->args), &(a3->args));
	a3->pid = a4->pid;
	return(a3);
}

static Registration_3 *
reg4_to_reg3(Registration_4 *r4)
{
        Registration_3 *r3, *head, *prev;

	prev = head = NULL;
	while (r4 != NULL) {
		r3 = (Registration_3 *)calloc(1, sizeof(Registration_3));
		r3->target = buffer4_to_buffer3(r4->target);
		r3->prognum = r4->prognum;
		r3->versnum = r4->versnum;
		r3->procnum = r4->procnum;
		r3->next = NULL;
		r3->pid = r4->pid;

		if (head == NULL)
			head = r3;
		else
			prev->next = r3;
		prev = r3;

		r4 = r4->next;
	}
	return(head);
}

static void
free_tag3(Tag_3 *t)
{
	Tag_3 *ptr;

	while (t != NULL) {
		ptr = t->next;
		free(t);
		t = ptr;
	}
}

