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
/* $XConsortium: convert2-4.c /main/1 1996/04/21 19:22:26 drk $ */
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
#include "convert2-4.h"

/*
 * forward declaration of static functions
 */
static Buffer_4 buffer2_to_buffer4(Buffer_2 b);
static void period2_to_period4(Period_2 p2, Period_4 *p4);
static void tag2_to_tag4(Tag_2 t2, Tag_4 *t4);
static Attribute_4 * attr2_to_attr4(Attribute_2 *a2);
static Except_4 * except2_to_except4(Except_2 *e2);
static void id2_to_id4(Id_2 *from, Id_4 *to);
static Uid_4 * uid2_to_uid4(Uid_2 *ui2);
static Appt_4 * appt2_to_appt4(Appt_2 *a2);
static Abb_Appt_4 * abb2_to_abb4(Abb_Appt_2 *a2);
static void apptid2_to_apptid4(Apptid_2 *from, Apptid_4 *to);
static Reminder_4 * reminder2_to_reminder4(Reminder_2 *r2);
static Table_Res_Type_4 tablerestype2_to_tablerestype4(Table_Res_Type_2 t);
static void tablereslist2_to_tablereslist4(Table_Res_List_2 *from,
			Table_Res_List_4 *to);
static Access_Entry_4 * acclist2_to_acclist4(Access_Entry_2 *l2);
static Range_4 * range2_to_range4(Range_2 *r2);
static Table_Args_Type_4 argstag2_to_argstag4(Table_Args_Type_2 t);
static void args2_to_args4(Args_2 *from, Args_4 *to);
static Uidopt_4 * uid2_to_uidopt(Uid_2 *uid2, Options_4 opt);

/**************** DATA TYPE (2->4) CONVERSION ROUTINES **************/

extern Access_Status_4
_DtCm_accstat2_to_accstat4(Access_Status_2 s)
{
        switch (s) {
        case access_ok_2:
                return(access_ok_4);
        case access_added_2:
                return(access_added_4);
        case access_removed_2:
                return(access_removed_4);
        case access_failed_2:
                return(access_failed_4);
        case access_exists_2:
                return(access_exists_4);
        case access_partial_2:
                return(access_partial_4);
        case access_other_2:
        default:
                return(access_other_4);
        }
}

extern Table_Res_4 *
_DtCm_tableres2_to_tableres4(Table_Res_2 *r2)
{
        Table_Res_4 *r4;
	
	if (r2==NULL) return((Table_Res_4 *)NULL);
	r4 = (Table_Res_4 *)calloc(1, sizeof(Table_Res_4));
        r4->status = _DtCm_accstat2_to_accstat4(r2->status);
        tablereslist2_to_tablereslist4(&(r2->res), &(r4->res));
        return(r4);
}

extern Access_Args_4 *
_DtCm_accargs2_to_accargs4(Access_Args_2 *a2)
{
	Access_Args_4 *a4;

	if (a2==NULL) return((Access_Args_4 *)NULL);
	a4 = (Access_Args_4 *)calloc(1, sizeof(Access_Args_4));
	a4->target = buffer2_to_buffer4(a2->target);
	a4->access_list = acclist2_to_acclist4(a2->access_list);
	return(a4);
}

extern Table_Args_4 *
_DtCm_tableargs2_to_tableargs4(Table_Args_2 *a2)
{
        Table_Args_4 *a4;

	if (a2==NULL) return((Table_Args_4 *)NULL);
	a4 = (Table_Args_4 *)calloc(1, sizeof(Table_Args_4));
        a4->target = buffer2_to_buffer4(a2->target);
        args2_to_args4(&(a2->args), &(a4->args));
	a4->pid = VOIDPID;
        return(a4);
}

extern Table_Args_4 *
_DtCm_tabledelargs2_to_tabledelargs4(Table_Args_2 *a2, Options_4 opt)
{
	Table_Args_4 *a4;

	if (a2 == NULL)
		return((Table_Args_4 *)NULL);

	a4 = (Table_Args_4 *)calloc(1, sizeof(Table_Args_4));
	a4->target = buffer2_to_buffer4(a2->target);
	a4->pid = VOIDPID;
	a4->args.tag = UIDOPT_4;
	a4->args.Args_4_u.uidopt = uid2_to_uidopt(a2->args.Args_2_u.key, opt);
	return(a4);
}

extern Registration_Status_4
_DtCm_regstat2_to_regstat4(Registration_Status_2 s)
{
	switch(s) {
	case registered_2:
		return(registered_4);
	case failed_2:
		return(failed_4);
	case deregistered_2:
		return(deregistered_4);
	case confused_2:
		return(confused_4);
	default:
		return(failed_4);
	}
}

extern Registration_4 *
_DtCm_reg2_to_reg4(Registration_2 *r2)
{
	Registration_4 *r4, *head, *prev;

	prev = head = NULL;
	while (r2 != NULL) {
		r4 = (Registration_4 *)calloc(1, sizeof(Registration_4));
		r4->target = buffer2_to_buffer4(r2->target);
		r4->prognum = r2->prognum;
		r4->versnum = r2->versnum;
		r4->procnum = r2->procnum;
		r4->next = NULL;
		r4->pid = VOIDPID;

		if (head == NULL)
			head = r4;
		else
			prev->next = r4;
		prev = r4;

		r2 = r2->next;
	}
	return(head);
}

extern Table_Status_4
_DtCm_tablestat2_to_tablestat4(Table_Status_2 s)
{
	switch(s) {
	case ok_2:
		return(ok_4);
	case duplicate_2:
		return(duplicate_4);
	case badtable_2:
		return(badtable_4);
	case notable_2:
		return(notable_4);
	case denied_2:
		return(denied_4);
	case other_2:
	default:
		return(other_4);
	}
}

static Buffer_4
buffer2_to_buffer4(Buffer_2 b)
{
	Buffer_4 copy;
	if (b!=NULL)
		copy = strdup(b);
	else
		copy = calloc(1,1);
        return(copy);
}

static void
period2_to_period4(Period_2 p2, Period_4 *p4)
{
	if (p4==NULL) return;
        switch (p2) {
        case single_2:
		p4->period = single_4;
		p4->nth = 0;
		break;
        case daily_2:
		p4->period = daily_4;
		p4->nth = 0;
		break;
        case weekly_2:
		p4->period = weekly_4;
		p4->nth = 0;
		break;
	case biweekly_2:
		p4->period = biweekly_4;
		p4->nth = 0;
		break;
	case monthly_2:
		p4->period = monthly_4;
		p4->nth = 0;
		break;
        case yearly_2:
		p4->period = yearly_4;
		p4->nth = 0;
		break;
        case nthWeekday_2:
		p4->period = nthWeekday_4;
		p4->nth = 0;
		break;
        case everyNthDay_2:
		p4->period = everyNthDay_4;
		p4->nth = 0;
		break;
        case everyNthWeek_2:
		p4->period = everyNthWeek_4;
		p4->nth = 0;
		break;
        case everyNthMonth_2:
		p4->period = everyNthMonth_4;
		p4->nth = 0;
		break;
        case otherPeriod_2:
		p4->period = otherPeriod_4;
		p4->nth = 0;
		break;
        default:
		p4->period = single_4;
		p4->nth = 0;
		break;
        }
}

static void
tag2_to_tag4(Tag_2 t2, Tag_4 *t4)
{
	if (t4==NULL) return;
	switch(t2) {
	case appointment_2:
		t4->tag = appointment_4;
		t4->showtime = B_TRUE;
		t4->next = NULL;
		break;
	case reminder_2:
		t4->tag = reminder_4;
		t4->showtime = B_FALSE;
		t4->next = NULL;
		break;
	case otherTag_2:
		t4->tag = otherTag_4;
		t4->showtime = B_FALSE;
		t4->next = NULL;
		break;
	default:
		t4->tag = appointment_4;	
		t4->showtime = B_TRUE;
		t4->next = NULL;
		break;
	}
}

static Attribute_4 *
attr2_to_attr4(Attribute_2 *a2)
{
        Attribute_4 *a4, *head, *prev;
 
	prev = head = NULL;
	while (a2 != NULL) {
		a4 = (Attribute_4 *)calloc(1, sizeof(Attribute_4));
		a4->next = NULL;
		a4->attr = strdup(a2->attr);
		a4->value = strdup(a2->value);
		a4->clientdata = NULL;

		if (head == NULL)
			head = a4;
		else
			prev->next = a4;
		prev = a4;

		a2 = a2->next;
	}
	return(head);
}

static Except_4 *
except2_to_except4(Except_2 *e2)
{
	Except_4 *e4, *head, *prev;

	prev = head = NULL;
	while (e2 != NULL) {
		e4  = (Except_4 *)calloc(1, sizeof(Except_4));
		e4->ordinal = e2->ordinal;
		e4->next=NULL;

		if (head == NULL)
			head = e4;
		else
			prev->next = e4;
		prev = e4;

		e2 = e2->next;
	}
	return(head);
}

static void
id2_to_id4(Id_2 *from, Id_4 *to)
{
	if ((from==NULL) || (to==NULL)) return;
        to->tick = from->tick;
        to->key = from->key;
}

static Uid_4 *
uid2_to_uid4(Uid_2 *ui2)
{
        Uid_4 *ui4, *head, *prev;
 
	prev = head = NULL;
	while (ui2 != NULL) {
        	ui4 = (Uid_4 *)calloc(1, sizeof(Uid_4));
        	id2_to_id4(&(ui2->appt_id), &(ui4->appt_id));
        	ui4->next = NULL;

		if (head == NULL)
			head = ui4;
		else
			prev->next = ui4;
		prev = ui4;

		ui2 = ui2->next;
	}
	return(head);
}
 
static Appt_4 *
appt2_to_appt4(Appt_2 *a2)
{
        Appt_4 *a4, *head, *prev;

	prev = head = NULL;
	while (a2 != NULL) {
		a4  = (Appt_4 *)calloc(1, sizeof(Appt_4));
		a4->tag = (Tag_4 *)calloc(1, sizeof(Tag_4));
		id2_to_id4(&(a2->appt_id), &(a4->appt_id));
		tag2_to_tag4(a2->tag, a4->tag);
		a4->duration = a2->duration;
		a4->ntimes = a2->ntimes;
		a4->what = buffer2_to_buffer4(a2->what);
		period2_to_period4(a2->period, &(a4->period));
		a4->author = buffer2_to_buffer4(a2->author);
		a4->client_data = buffer2_to_buffer4(a2->client_data);
		a4->attr = attr2_to_attr4(a2->attr);

		/* mailto is being removed from the appt struct proper,
		   and held instead as client data on the "ml" attribute */
		if (a2->mailto != NULL) {
			struct Attribute_4 *item = a4->attr;
			while(item!=NULL) {
				if(strcmp(item->attr, "ml")==0) {
			  	item->clientdata=buffer2_to_buffer4(a2->mailto);
			  	break;
				}
				item=item->next;
			}
		}

		a4->exception = except2_to_except4(a2->exception);
		a4->appt_status = active_4;
		a4->privacy = public_4;
		a4->next = NULL;

		if (head == NULL)
			head = a4;
		else
			prev->next = a4;
		prev = a4;

		a2 = a2->next;
	}
	return(head);
}

static Abb_Appt_4 *
abb2_to_abb4(Abb_Appt_2 *a2)
{
        Abb_Appt_4 *a4, *head, *prev;
 
	prev = head = NULL;
	while (a2 != NULL) {
		a4 = (Abb_Appt_4 *)calloc(1, sizeof(Abb_Appt_4));
		a4->tag = (Tag_4 *)calloc(1, sizeof(Tag_4));
		id2_to_id4(&(a2->appt_id), &(a4->appt_id));
		a4->tag->tag = appointment_4;
		a4->tag->showtime = B_TRUE;
		a4->tag->next = NULL;
		a4->what = buffer2_to_buffer4(a2->what);
		a4->duration = a2->duration;
		period2_to_period4(a2->period, &(a4->period));
		a4->appt_status = active_4;
		a4->privacy = public_4;
		a4->next = NULL;

		if (head == NULL)
			head = a4;
		else
			prev->next = a4;
		prev = a4;

		a2 = a2->next;
	}
	return(head);
}

static void
apptid2_to_apptid4(Apptid_2 *from, Apptid_4 *to)
{
        if (from==NULL || to==NULL) return;
        id2_to_id4(from->oid, to->oid);
        to->new_appt = appt2_to_appt4(from->new_appt);
	/* do_all is the default, the caller needs to
	 * modify it to the appropriate value
	 */
	to->option = do_all_4;
}

static Reminder_4 *
reminder2_to_reminder4(Reminder_2 *r2)
{
        Reminder_4 *r4, *head, *prev;
	Attribute_4 *attr4;

	prev = head = NULL;
	while (r2 != NULL) {
		r4 = (Reminder_4 *)calloc(1, sizeof(Reminder_4));
		id2_to_id4(&(r2->appt_id), &(r4->appt_id));
		r4->tick = r2->tick;
		attr4 = attr2_to_attr4(&(r2->attr));
		r4->attr = *attr4;
		free(attr4);
		r4->next = NULL;

		if (head == NULL)
			head = r4;
		else
			prev->next = r4;
		prev = r4;

		r2 = r2->next;
	}
	return(head);
}
      
static Table_Res_Type_4
tablerestype2_to_tablerestype4(Table_Res_Type_2 t)
{
        switch(t) {
        case AP_2:
                return(AP_4);
        case RM_2:
                return(RM_4);
        case AB_2:
                return(AB_4);
        case ID_2:
                return(ID_4);
        default:
                return(AP_4);
        }
}

static void
tablereslist2_to_tablereslist4(Table_Res_List_2 *from, Table_Res_List_4 *to)
{
        if (from==NULL || to==NULL) return;
        to->tag = tablerestype2_to_tablerestype4(from->tag);
        switch (from->tag) {
        case AP_2:
                to->Table_Res_List_4_u.a = appt2_to_appt4(
                        from->Table_Res_List_2_u.a);
                break;
        case RM_2:
                to->Table_Res_List_4_u.r = reminder2_to_reminder4(
                        from->Table_Res_List_2_u.r);
                break;
        case AB_2:
                to->Table_Res_List_4_u.b = abb2_to_abb4(
                        from->Table_Res_List_2_u.b);
                break;
        case ID_2:
                to->Table_Res_List_4_u.i = uid2_to_uid4(
                        from->Table_Res_List_2_u.i);
		break;
        default:
                return;
        }
}

static Access_Entry_4 *
acclist2_to_acclist4(Access_Entry_2 *l2)
{
	Access_Entry_4 *l4, *head, *prev;

	prev = head = NULL;
	while (l2 != NULL) {
		l4 = (Access_Entry_4 *)calloc(1, sizeof(Access_Entry_4));
		l4->who = buffer2_to_buffer4(l2->who);
		l4->access_type = l2->access_type;
		l4->next = NULL;

		if (head == NULL)
			head = l4;
		else
			prev->next = l4;
		prev = l4;

		l2 = l2->next;
	}
	return(head);
}

static Range_4 *
range2_to_range4(Range_2 *r2)
{
        Range_4 *r4, *head, *prev;
 
	prev = head = NULL;
	while (r2 != NULL) {
		r4 = (Range_4 *)calloc(1, sizeof(Range_4));
		r4->key1 = r2->key1;
		r4->key2 = r2->key2;
		r4->next = NULL;

		if (head == NULL)
			head = r4;
		else
			prev->next = r4;
		prev = r4;

		r2 = r2->next;
	}
	return(head);
}

static Table_Args_Type_4
argstag2_to_argstag4(Table_Args_Type_2 t)
{
	switch(t) {
	case TICK_2:
		return(TICK_4);
	case APPTID_2:
		return(APPTID_4);
	case UID_2:
		return(UID_4);
	case APPT_2:
		return(APPT_4);
	case RANGE_2:
		return(RANGE_4);
	default:
		return(TICK_4);
	}
}

static void
args2_to_args4(Args_2 *from, Args_4 *to)
{
        if (from==NULL || to==NULL) return;
        to->tag = argstag2_to_argstag4(from->tag);
        switch(from->tag) {
        case TICK_2:
                to->Args_4_u.tick = from->Args_2_u.tick;
                break;
        case APPTID_2:
		to->Args_4_u.apptid.oid = (Id_4 *)calloc(1, sizeof(Id_4));
                apptid2_to_apptid4(
                        &(from->Args_2_u.apptid),
                        &(to->Args_4_u.apptid));
                break;
        case UID_2:
                to->Args_4_u.key = uid2_to_uid4(from->Args_2_u.key);
                break;
        case APPT_2:
                to->Args_4_u.appt = appt2_to_appt4(from->Args_2_u.appt);
                break;
        case RANGE_2:
                to->Args_4_u.range = range2_to_range4(from->Args_2_u.range);
                break;
        default:
                break;
        }
}

static Uidopt_4 *
uid2_to_uidopt(Uid_2 *uid2, Options_4 opt)
{
        Uidopt_4 *uidopt, *head, *prev;
 
	prev = head = NULL;
	while (uid2 != NULL) {
        	uidopt = (Uidopt_4 *)calloc(1, sizeof(Uidopt_4));
        	id2_to_id4(&(uid2->appt_id), &(uidopt->appt_id));
		uidopt->option = opt;
        	uidopt->next = NULL;

		if (head == NULL)
			head = uidopt;
		else
			prev->next = uidopt;
		prev = uidopt;

		uid2 = uid2->next;
	}
	return(head);
}
 
