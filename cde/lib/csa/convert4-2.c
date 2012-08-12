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
/* $XConsortium: convert4-2.c /main/1 1996/04/21 19:22:37 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <string.h>
#include <stdlib.h>
#include "rtable2.h"
#include "rtable4.h"
#include "convert4-2.h"

/*
 * forward declaration of static functions
 */
static void free_excpt2(Except_2 *e);
static Buffer_2 buffer4_to_buffer2(Buffer_4 b);
static Period_2 period4_to_period2(Period_4 *p);
static Tag_2 tag4_to_tag2(Tag_4 *t);
static Attribute_2 * attr4_to_attr2(Attribute_4 *a4);
static Except_2 * except4_to_except2(Except_4 *e4);
static Abb_Appt_2 * abb4_to_abb2(Abb_Appt_4 *a4);
static Reminder_2 * reminder4_to_reminder2(Reminder_4 *r4);
static Table_Res_Type_2 tablerestype4_to_tablerestype2(Table_Res_Type_4 t);
static void tablereslist4_to_tablereslist2(Table_Res_List_4 *from,
		Table_Res_List_2 *to);
static Table_Args_Type_2 argstag4_to_argstag2(Table_Args_Type_4 t);
static void args4_to_args2(Args_4 *from, Args_2 *to);
static Table_Args_2 * tableargs4_to_tableargs2(Table_Args_4 *a4);
static Registration_2 * reg4_to_reg2(Registration_4 *r4);

/**************** DATA TYPE (4->2) CONVERSION ROUTINES **************/

extern void
_DtCm_id4_to_id2(Id_4 *from, Id_2 *to)
{
        if ((from==NULL) || (to==NULL)) return;	
        to->tick = from->tick;
        to->key = from->key;
}

extern Uid_2 *
_DtCm_uid4_to_uid2(Uid_4 *ui4)
{
        Uid_2 *ui2, *head, *prev;

	prev = head = NULL;
	while (ui4 != NULL) {
		ui2 = (Uid_2 *)calloc(1, sizeof(Uid_2));
		_DtCm_id4_to_id2(&(ui4->appt_id), &(ui2->appt_id));
		ui2->next = NULL;

		if (head == NULL)
			head = ui2;
		else
			prev->next = ui2;
		prev = ui2;

		ui4 = ui4->next;
	}
	return(head);
}

extern Appt_2 *
_DtCm_appt4_to_appt2(Appt_4 *a4)
{
        Appt_2 *a2, *head, *prev;
	struct Attribute_4 *item;

	prev = head = NULL;
	while (a4 != NULL) {
		a2  = (Appt_2 *)calloc(1, sizeof(Appt_2));
		_DtCm_id4_to_id2(&(a4->appt_id), &(a2->appt_id));
		a2->tag = tag4_to_tag2(a4->tag);
		a2->duration = a4->duration;
		a2->ntimes = a4->ntimes;
		a2->what = buffer4_to_buffer2(a4->what);
		a2->script = "";
		a2->period = period4_to_period2(&(a4->period));
		a2->author = buffer4_to_buffer2(a4->author);
		a2->client_data = buffer4_to_buffer2(a4->client_data);
		a2->attr = attr4_to_attr2(a4->attr);

		/* Pick the mailto field out of the attribute list
		   client data field; put it back into the appt struct
		   proper.
		*/
		item = a4->attr;
		while(item!=NULL) {
			if (strcmp(item->attr, "ml")==0) {
				a2->mailto=buffer4_to_buffer2(item->clientdata);
				break;
			}
			item=item->next;
		}

		a2->exception = except4_to_except2(a4->exception);
		a2->next = NULL;

		if (head == NULL)
			head = a2;
		else
			prev->next = a2;
		prev = a2;

		a4 = a4->next;
	}
	return(head);
}

extern Access_Status_2
_DtCm_accstat4_to_accstat2(Access_Status_4 s)
{
	switch(s) {
	case access_ok_4:
		return(access_ok_2);
	case access_added_4:
		return(access_added_2);
	case access_removed_4:
		return(access_removed_2);
	case access_failed_4:
		return(access_failed_2);
	case access_exists_4:
		return(access_exists_2);
	case access_partial_4:
		return(access_partial_2);
	case access_other_4:
	default:
		return(access_other_2);
	}
}

extern Table_Res_2 *
_DtCm_tableres4_to_tableres2(Table_Res_4 *r4)
{
        Table_Res_2 *r2;

	if (r4==NULL) return((Table_Res_2 *)NULL);
	r2 = (Table_Res_2 *)calloc(1, sizeof(Table_Res_2));
        r2->status = _DtCm_accstat4_to_accstat2(r4->status);
        tablereslist4_to_tablereslist2(&(r4->res), &(r2->res));
        return(r2);
}

extern Access_Entry_2 *
_DtCm_acclist4_to_acclist2(Access_Entry_4 *l4)
{
        Access_Entry_2 *l2, *head, *prev;

	prev = head = NULL;
	while (l4 != NULL) {
		l2 = (Access_Entry_2 *)calloc(1, sizeof(Access_Entry_2));
		l2->who = buffer4_to_buffer2(l4->who);
		l2->access_type = l4->access_type;
		l2->next = NULL;

		if (head == NULL)
			head = l2;
		else
			prev->next = l2;
		prev = l2;

		l4 = l4->next;
	}
	return(head);
}
 
extern Access_Args_2 *
_DtCm_accargs4_to_accargs2(Access_Args_4 *a4)
{
        Access_Args_2 *a2;

	if (a4==NULL) return((Access_Args_2 *)NULL);
	a2 = (Access_Args_2 *)calloc(1, sizeof(Access_Args_2));
        a2->target = buffer4_to_buffer2(a4->target);
        a2->access_list = _DtCm_acclist4_to_acclist2(a4->access_list);
        return(a2);
}

extern Range_2 *
_DtCm_range4_to_range2(Range_4 *r4)
{
	Range_2 *r2, *head, *prev;

	prev = head = NULL;
	while (r4 != NULL) {
		r2 = (Range_2 *)calloc(1, sizeof(Range_2));
		r2->key1 = r4->key1;
		r2->key2 = r4->key2;
		r2->next = NULL;

		if (head == NULL)
			head = r2;
		else
			prev->next = r2;
		prev = r2;

		r4 = r4->next;
	}
	return(head);
}

extern Registration_Status_2 
_DtCm_regstat4_to_regstat2(Registration_Status_4 s)
{
	switch (s) {
	case registered_4:
		return(registered_2);
	case failed_4:
		return(failed_2);
	case deregistered_4:
		return(deregistered_2);
	case confused_4:
		return(confused_2);
	case reg_notable_4:
	default:
		return(failed_2);
	}
}

extern Table_Status_2
_DtCm_tablestat4_to_tablestat2(Table_Status_4 s)
{
	switch(s) {
	case ok_4:
		return(ok_2);
	case duplicate_4:
		return(duplicate_2);
	case badtable_4:
		return(badtable_2);
	case notable_4:
		return(notable_2);
	case denied_4:
		return(denied_2);
	case other_4:
	default:
		return(other_2);
	}
}

extern Uid_2 *
_DtCm_uidopt4_to_uid2(Uidopt_4 *uidopt)
{
        Uid_2 *uid2, *head, *prev;
 
	prev = head = NULL;
	while (uidopt != NULL) {
        	uid2 = (Uid_2 *)calloc(1, sizeof(Uid_2));
        	_DtCm_id4_to_id2(&(uidopt->appt_id), &(uid2->appt_id));
        	uid2->next = NULL;

		if (head == NULL)
			head = uid2;
		else
			prev->next = uid2;
		prev = uid2;

		uidopt = uidopt->next;
	}
	return(head);
}
 
extern void
_DtCm_free_attr2(Attribute_2 *a)
{
	Attribute_2 *ptr;

	while (a != NULL) {
		ptr = a->next;
		if (a->attr != NULL)
			free(a->attr);
		if (a->value != NULL)
			free(a->value);
		free(a);

		a = ptr;
	}
}

extern void
_DtCm_free_appt2(Appt_2 *a)
{
	Appt_2 *ptr;

	while (a != NULL) {
		ptr = a->next;

		if (a->what != NULL)
			free(a->what);

		if (a->mailto != NULL)
			free(a->mailto);

		if (a->script != NULL)
			free(a->script);

		if (a->author != NULL)
			free(a->author);

		if (a->client_data != NULL)
			free(a->client_data);

		if (a->attr != NULL)
			_DtCm_free_attr2(a->attr);

		if (a->exception != NULL)
			free_excpt2(a->exception);

     		free(a);

		a = ptr;
	}
}

static void
free_excpt2(Except_2 *e)
{
	Except_2 *ptr;

	while (e != NULL) {
		ptr = e->next;
		free(e);
		e = ptr;
	}
}

static Buffer_2
buffer4_to_buffer2(Buffer_4 b)
{
	Buffer_2 copy;
	if (b!=NULL)
		copy = strdup(b);
	else
		copy = calloc(1, 1);
	return(copy);
}

/*
 * Repeating event types beyond "yearly" are mapped to "single"
 * because the old front end does not recognize any other types
 * Worse yet it uses Period_2 to index into an array which
 * contains strings up to "yearly", any period types beyond that
 * would index beyond the array and cause the front end to dump core.
 */
static Period_2
period4_to_period2(Period_4 *p)
{
	if (p==NULL) return(otherPeriod_2);
        switch (p->period) {
        case single_4:
                return(single_2);
        case daily_4:
                return(daily_2);
        case weekly_4:
                return(weekly_2);
	case biweekly_4:
		return(biweekly_2);
	case monthly_4:
		return(monthly_2);
        case yearly_4:
                return(yearly_2);
        default:
                return(single_2);
        }
}

static Tag_2
tag4_to_tag2(Tag_4 *t)
{
	if (t==NULL) return(otherTag_2);
	switch(t->tag) {
	case appointment_4:
		return(appointment_2);
	case reminder_4:
		return(reminder_2);
	case otherTag_4:
	case holiday_4:
	case toDo_4:
	default:
		return(otherTag_2);
	}
}

static Attribute_2 *
attr4_to_attr2(Attribute_4 *a4)
{
        Attribute_2 *a2, *head, *prev;
 
	prev = head = NULL;
	while (a4 != NULL) {
		a2 = (Attribute_2 *)calloc(1, sizeof(Attribute_2));
		a2->next = NULL;
		a2->attr = strdup(a4->attr);
		a2->value = strdup(a4->value);

		if (head == NULL)
			head = a2;
		else
			prev->next = a2;
		prev = a2;

		a4 = a4->next;
	}
	return(head);
}

static Except_2 *
except4_to_except2(Except_4 *e4)
{
        Except_2 *e2, *head, *prev;

	prev = head = NULL;
	while (e4 != NULL) {
		e2 = (Except_2 *)calloc(1, sizeof(Except_2));
		e2->ordinal = e4->ordinal;
		e2->next=NULL;

		if (head == NULL)
			head = e2;
		else
			prev->next = e2;
		prev = e2;

		e4 = e4->next;
	}
	return(head);
}
 
static Abb_Appt_2 *
abb4_to_abb2(Abb_Appt_4 *a4)
{
        Abb_Appt_2 *a2, *head, *prev;

	prev = head = NULL;
	while (a4 != NULL) {
		a2 = (Abb_Appt_2 *)calloc(1, sizeof(Abb_Appt_2));
		_DtCm_id4_to_id2(&(a4->appt_id), &(a2->appt_id));
		a2->what = buffer4_to_buffer2(a4->what);
		a2->duration = a4->duration;
		a2->period = period4_to_period2(&(a4->period));
		a2->next = NULL;

		if (head == NULL)
			head = a2;
		else
			prev->next = a2;
		prev = a2;

		a4 = a4->next;
	}
	return(head);
}

static void
apptid4_to_apptid2(Apptid_4 *from, Apptid_2 *to)
{
        if (from==NULL || to==NULL) return;
        _DtCm_id4_to_id2(from->oid, to->oid);
        to->new_appt = _DtCm_appt4_to_appt2(from->new_appt);
}

static Reminder_2 *
reminder4_to_reminder2(Reminder_4 *r4)
{
        Reminder_2 *r2, *head, *prev;
	Attribute_2 *attr2;

	prev = head = NULL;
	while (r4 != NULL) {
		r2 = (Reminder_2 *)calloc(1, sizeof(Reminder_2));
		_DtCm_id4_to_id2(&(r4->appt_id), &(r2->appt_id));
		r2->tick = r4->tick;
		attr2 = attr4_to_attr2(&(r4->attr));
		r2->attr = *attr2;
		free(attr2);
		r2->next = NULL;

		if (head == NULL)
			head = r2;
		else
			prev->next = r2;
		prev = r2;

		r4 = r4->next;
	}
	return(head);
}

static Table_Res_Type_2
tablerestype4_to_tablerestype2(Table_Res_Type_4 t)
{
	switch(t) {
	case AP_4:
		return(AP_2);
	case RM_4:
		return(RM_2);
	case AB_4:
		return(AB_2);
	case ID_4:
		return(ID_2);
	default:
		return(AP_2);
	}
}

static void
tablereslist4_to_tablereslist2(Table_Res_List_4 *from, Table_Res_List_2 *to)
{
        if (from==NULL || to==NULL) return;
        to->tag = tablerestype4_to_tablerestype2(from->tag);
        switch (from->tag) {
        case AP_4:
                to->Table_Res_List_2_u.a = _DtCm_appt4_to_appt2(
                        from->Table_Res_List_4_u.a);
                break;
        case RM_4:
                to->Table_Res_List_2_u.r = reminder4_to_reminder2(
                        from->Table_Res_List_4_u.r);
                break;
        case AB_4:
                to->Table_Res_List_2_u.b = abb4_to_abb2(
                        from->Table_Res_List_4_u.b);
                break;
        case ID_4:
                to->Table_Res_List_2_u.i = _DtCm_uid4_to_uid2(
                        from->Table_Res_List_4_u.i);
        default:
                return;
        }
}
 
static Table_Args_Type_2
argstag4_to_argstag2(Table_Args_Type_4 t)
{
        switch(t) {
        case TICK_4:
                return(TICK_2);
        case APPTID_4:
                return(APPTID_2);
        case UID_4:
                return(UID_2);
        case APPT_4:
                return(APPT_2);
        case RANGE_4:
                return(RANGE_2);
        default:
                return(TICK_2);
        }
}

static void
args4_to_args2(Args_4 *from, Args_2 *to)
{
	if (from==NULL || to==NULL) return;
	to->tag = argstag4_to_argstag2(from->tag);
	switch(from->tag) {
	case TICK_4:
		to->Args_2_u.tick = from->Args_4_u.tick;
		break;
	case APPTID_4:
		to->Args_2_u.apptid.oid = (Id_2 *)calloc(1, sizeof(Id_2));
		apptid4_to_apptid2(
			&(from->Args_4_u.apptid),
			&(to->Args_2_u.apptid));
		break;
	case UID_4:
		to->Args_2_u.key = _DtCm_uid4_to_uid2(from->Args_4_u.key);
		break;
	case APPT_4:
		to->Args_2_u.appt = _DtCm_appt4_to_appt2(from->Args_4_u.appt);
		break;
	case RANGE_4:
		to->Args_2_u.range = _DtCm_range4_to_range2(from->Args_4_u.range);
		break;
	default:
		break;
	}
}

static Table_Args_2 *
tableargs4_to_tableargs2(Table_Args_4 *a4)
{
	Table_Args_2 *a2;
	
	if (a4==NULL) return((Table_Args_2 *)NULL);
	a2 = (Table_Args_2 *)calloc(1, sizeof(Table_Args_2));
	a2->target = buffer4_to_buffer2(a4->target);
	args4_to_args2(&(a4->args), &(a2->args));
	return(a2);
}

static Registration_2 *
reg4_to_reg2(Registration_4 *r4)
{
        Registration_2 *r2, *head, *prev;

	prev = head = NULL;
	while (r4 != NULL) {
		r2 = (Registration_2 *)calloc(1, sizeof(Registration_2));
		r2->target = buffer4_to_buffer2(r4->target);
		r2->prognum = r4->prognum;
		r2->versnum = r4->versnum;
		r2->procnum = r4->procnum;
		r2->next = NULL;

		if (head == NULL)
			head = r2;
		else
			prev->next = r2;
		prev = r2;

		r4 = r4->next;
	}
	return(head);
}

