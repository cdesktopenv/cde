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
/* $XConsortium: reminder.c /main/4 1995/11/09 12:48:20 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cmscalendar.h"
#include "reminder.h"		
#include "appt4.h"
#include "repeat.h"
#include "v4ops.h"
#include "v5ops.h"
#include "rerule.h"
#include "reutil.h"
#include "iso8601.h"

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/

static _DtCmsRemInfo *_BuildReminder4Entry(
			cms_entry	*entry,
			int		aindex,
			List_node	*lnode,
			time_t		cutoff,
			_DtCmsRemInfo	**active);

static void _InsertReminder(
			_DtCmsRemInfo	**head,
			_DtCmsRemInfo	*rem);

static CSA_return_code _GetNextReminders(
			_DtCmsRemQueue 		*remq,
			time_t			tick,
			cms_reminder_ref	**rems);

static CSA_return_code _GetNextRemindersFromQ(
			_DtCmsRemQueue		*remq,
			int			qindex,
			time_t			tick,
			cms_reminder_ref	**rems);

static cms_reminder_ref *_GetReminderRefFromInfo(
			_DtCmsRemInfo	*rem,
			time_t		starttime,
			time_t		runtime);

static void _RemoveReminderFromQ(
			_DtCmsRemQueue	*remq,
			int		qindex,
			cms_entry	*entry,
			List_node	*lnode,
			time_t		starttime,
			boolean_t	delfwd);

static _DtCmsRemInfo *_RemoveReminderFromList(
			_DtCmsRemInfo	**remq,
			cms_entry	*entry,
			time_t		starttime,
			boolean_t	delfwd);

static long _GetNextActiveTick(
			cms_entry	*entry,
			time_t		target,
			time_t		lasttick,
			RepeatEvent	*re);

static void _UpdateReminderQ(_DtCmsRemQueue *remq, int qindex);

static CSA_return_code _GetNextRemindersFromList(
			_DtCmsRemInfo		*rlist,
			time_t			giventime,
			cms_reminder_ref	**rf_r);

static void _DtCmsAddReminder4EntryToQ(
			_DtCmsRemQueue	*remq,
			cms_entry	*entry,
			int		aindex,
			List_node	*lnode);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

extern void
_DtCmsAddReminderV4(Rm_que **qhead, Rm_que *p_reminder)
{
	Rm_que	*p_prev;
	Rm_que	*p_node;

	if (p_reminder == NULL)
		return;

	p_prev = NULL;
	p_node = *qhead;
	while (p_node != NULL)
	{
		if (p_reminder->remind_at < p_node->remind_at)
			break;
		p_prev = p_node;
		p_node = p_node->next;
	}

	if (p_prev == NULL) {
		p_reminder->next = *qhead;
		*qhead = p_reminder;
	} else {
		p_reminder->next = p_prev->next;
		p_prev->next = p_reminder;
	}
}

extern Rm_que *
_DtCmsRemoveReminderV4(Rm_que **qhead, Rm_que *p_prev, Rm_que *p_curr)
{
	if (p_prev == NULL)
		*qhead = p_curr->next;
	else
		p_prev->next = p_curr->next;
	return (p_curr->next);
}

extern Rm_que *
build_reminder(
	time_t	current_time,
	Appt_4	*p_appt,
	Attr_4	p_attr,
	time_t	start_tick,
	u_int	start_ord)
{
	int	ntimes;
	Period_4 period;
	Rm_que	*p_reminder = NULL;

	/* Ignore the expired or unqualified reminder. */
	p_reminder = NULL;
	if (is_appointment(p_appt)) {
		/* The event is not expired yet, build the reminder */
		if (start_tick >= current_time)
		{
			if ((p_reminder = (Rm_que *)calloc(1, sizeof(Rm_que)))
			   == NULL)
				return (NULL);

			p_reminder->remind_ord = 0;
		}
	} else {
		period = p_appt->period;
		ntimes = _DtCms_get_ninstance_v4(p_appt);
		while (start_ord <= ntimes) {
			/* Event is not expired */
			if (start_tick >= current_time) {
				/* Event is not cancelled */
				if (!_DtCms_marked_4_cancellation (p_appt, start_ord))
				{
					if ((p_reminder = (Rm_que *)calloc(1,
					    sizeof(Rm_que))) == NULL)
						return (NULL);

					p_reminder->remind_ord = start_ord;
					break;
				}
			}
			/* Event is expired, advance to next event */
			start_tick = _DtCms_next_tick_v4 (start_tick, period);
			start_ord++;
		}
	}

	if (p_reminder != NULL) {
		p_reminder->remind_at = start_tick;
		p_reminder->appt = p_appt;
		p_reminder->attr = p_attr;
	}

	return (p_reminder);
}

extern Reminder_4 *
_DtCmsGetReminderInfoV4(Rm_que	*original)
{
	Reminder_4 *copy;

	if (original == NULL)
		return (NULL);
	
	if ((copy = calloc(1, sizeof(Reminder_4))) != NULL) {
		copy->tick = original->remind_at;
		copy->next = NULL;

		if ((copy->attr.attr = strdup(original->attr->attr)) == NULL) {
			free(copy);
			return (NULL);
		}

		if ((copy->attr.value = strdup(original->attr->value)) == NULL)
		{
			_DtCm_free_reminder4(copy);
			return (NULL);
		}

		if ((copy->attr.clientdata = strdup(original->attr->clientdata))
		    == NULL) {
			_DtCm_free_reminder4(copy);
			return (NULL);
		}

		copy->attr.next = NULL;

		copy->appt_id.tick = copy->tick + atol (copy->attr.value);
		copy->appt_id.key = original->appt->appt_id.key;
	}

	return (copy);
}

extern void
_DtCmsPrintReminderListV4(Rm_que *qhead)
{
	Rm_que	*p_node = qhead;
	char	*temp=NULL;

	if (qhead == NULL)
		return;

	fprintf (stderr, "--- Active Reminder Queue ---\n");
	while (p_node != NULL) {

		if (temp = strchr(p_node->appt->what, '\n'))
			*temp = '\0';

		fprintf(stderr, "%s (%d) %s: %s\n", ctime(&p_node->remind_at),
			p_node->remind_ord, p_node->attr->attr,
			p_node->appt->what);

		if (temp)
			*temp = '\n';

		p_node = p_node->next;
	}
}

/*
 * Obsolete all reminders (iff ord == 0) whose parent appointment matches a
 * given appointment.  If ord != 0, then obsolete all active reminders whose
 * serving ordinal matches ord in additional to the matching of its parent
 * appointment.  The reminder of the next available instance will be put on the
 * reminder queue.
 */
extern void
_DtCmsObsoleteReminderV4(
	Rm_que **qhead, 
	Appt_4 *p_appt, 
	int ord, 
	boolean_t delforward)
{
	Rm_que *p_prev;
	Rm_que *p_next;
	Rm_que *p_node;
	Rm_que *p_hdr = NULL;

	p_prev = NULL;
	p_node = *qhead;
	while (p_node != NULL) {

		if ((p_node->appt != p_appt) ||
		    ((ord != 0) && (ord != p_node->remind_ord)) ||
		    ((ord != 0) && delforward && p_node->remind_ord < ord)) {

			p_next = p_node->next;

		} else {
			/* Found the obsolete reminder. */
			p_next = _DtCmsRemoveReminderV4 (qhead,p_prev,p_node);

			if (ord == 0)
				free (p_node);
			else {
				/* Chain the obsolete reminders together to
				 * re-calculate the new reminders.
				 */
				p_node->next = p_hdr;
				p_hdr = p_node;
			}

			p_node = p_prev;
		}

		p_prev = p_node;
		p_node = p_next;
	}

	/* Build the reminders of the next instance from obsoleted reminders.
	 * Note, we can't put this code in the above 'while'-loop because it
	 * may confuse the loop.
	 */
	while (p_hdr != NULL) {

		p_next = p_hdr->next;
		p_node = build_reminder(p_hdr->remind_at+1,
					p_hdr->appt, p_hdr->attr,
					p_hdr->remind_at, ord);
		_DtCmsAddReminderV4 (qhead, p_node);
		free (p_hdr);
		p_hdr = p_next;
	}
}

#define	_DtCms_NUM_REMINDERS	4

extern void
_DtCmsAddReminders4Entry(
	_DtCmsRemQueue	**remq,
	cms_entry	*entry,
	List_node	*lnode)
{
	_DtCmsRemQueue	*queue;
	int		i;

	if (*remq == NULL) {
		if ((queue = (_DtCmsRemQueue *)calloc(1,
		    sizeof(_DtCmsRemQueue))) == NULL)
			return;

		/* initialize queues for the cde defined reminders */
		if ((queue->aindex = (int *)malloc(sizeof(int) *
		    _DtCms_NUM_REMINDERS)) == NULL) {
			free(queue);
			return;
		}

		if ((queue->names = (char **)malloc(sizeof(char *) *
		    _DtCms_NUM_REMINDERS)) == NULL) {
			free(queue->aindex);
			free(queue);
			return;
		}

		if ((queue->active = (_DtCmsRemInfo **)calloc(1, 
		    sizeof(_DtCmsRemInfo) * _DtCms_NUM_REMINDERS)) == NULL) {
			free(queue->names);
			free(queue->aindex);
			free(queue);
			return;
		}

		if ((queue->oldhead = (_DtCmsRemInfo **)calloc(1, 
		    sizeof(_DtCmsRemInfo) * _DtCms_NUM_REMINDERS)) == NULL) {
			free(queue->active);
			free(queue->names);
			free(queue->aindex);
			free(queue);
			return;
		}

		queue->num_queues = _DtCms_NUM_REMINDERS;
		queue->aindex[0] = CSA_ENTRY_ATTR_AUDIO_REMINDER_I;
		queue->names[0] = CSA_ENTRY_ATTR_AUDIO_REMINDER;
		queue->aindex[1] = CSA_ENTRY_ATTR_FLASHING_REMINDER_I;
		queue->names[1] = CSA_ENTRY_ATTR_FLASHING_REMINDER;
		queue->aindex[2] = CSA_ENTRY_ATTR_MAIL_REMINDER_I;
		queue->names[2] = CSA_ENTRY_ATTR_MAIL_REMINDER;
		queue->aindex[3] = CSA_ENTRY_ATTR_POPUP_REMINDER_I;
		queue->names[3] = CSA_ENTRY_ATTR_POPUP_REMINDER;

		/* set cutoff to be half an hour earlier than now
		 * to compensate time difference between machines
		 */
		queue->cutoff = time(0) - 60 * 30;

		*remq = queue;
	}

	/* Add the qualified reminder attrs to the reminder queue */
	for (i = 1; i <= entry->num_attrs; i++) {

		if (entry->attrs[i].value == NULL ||
		    entry->attrs[i].value->type != CSA_VALUE_REMINDER) 
			continue;

		_DtCmsAddReminder4EntryToQ(*remq, entry, i, lnode);
	}
}

extern void
_DtCmsObsoleteReminder4Entry(
	_DtCmsRemQueue	*remq,
	cms_entry	*entry,
	List_node	*lnode,
	time_t		starttime,
	boolean_t	delfwd)
{
	int	i;

	for (i = 0; i < remq->num_queues; i++) {
		if (entry->attrs[remq->aindex[i]].value)
			_RemoveReminderFromQ(remq, i, entry, lnode,
				starttime, delfwd);
	}
}

extern CSA_return_code
_DtCmsLookupReminder(
	_DtCmsRemQueue		*remq,
	time_t			tick,
	uint			num_names,
	cms_attr_name		*names,
	cms_reminder_ref	**rems)
{
	int		i, j;
	CSA_return_code	stat;

	*rems = NULL;

	if (remq == NULL)
		return (CSA_SUCCESS);

	if (num_names == 0)
		return (_GetNextReminders(remq, tick, rems));

	for (i = 0; i < num_names; i++) {
		for (j = 0; j < remq->num_queues; j++) {
			if (strcmp(names[i].name, remq->names[j]) == 0) {
				names[i].num = remq->aindex[j];

				if ((stat = _GetNextRemindersFromQ(remq, i,
				    tick, rems)) != CSA_SUCCESS) {
					if (*rems)
						_DtCmsFreeReminderRef(*rems);
					return (stat);
				}
			}
		}
	}

	return (CSA_SUCCESS);
}

extern void
_DtCmsFreeReminderRef(cms_reminder_ref *rems)
{
	cms_reminder_ref	*next;

	while (rems != NULL) {
		next = rems->next;

		if (rems->reminder_name)
			free(rems->reminder_name);
		if (rems->entryid)
			free(rems->entryid);

		free(rems);
		rems = next;
	}
}

extern void
_DtCmsUpdateReminders(_DtCmsRemQueue *remq)
{
	int	i;

	remq->cutoff = time(0) - 60*30;

	for (i = 0; i < remq->num_queues; i++)
		_UpdateReminderQ(remq, i);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

static _DtCmsRemInfo *
_BuildReminder4Entry(
	cms_entry	*entry,
	int		aindex,
	List_node	*lnode,
	time_t		cutoff,
	_DtCmsRemInfo	**active)
{
	_DtCmsRemInfo	*rptr, *rptr2;
	time_t		lead;
	time_t		tick;
	RepeatEventState *restate;

	if (active) *active = NULL;

	if ((rptr = (_DtCmsRemInfo *)calloc(1, sizeof(_DtCmsRemInfo))) == NULL)
		return (NULL);

	rptr->lnode = lnode;
	rptr->isentry = B_TRUE;
	rptr->data.e = entry;
	rptr->rem.i = entry->attrs[aindex].name.num;

	_csa_iso8601_to_duration(entry->attrs[aindex].value->item.\
			reminder_value->lead_time, &lead);

	if (lnode == NULL) {
		rptr->starttime = entry->key.time;
		rptr->runtime = entry->key.time - lead;
	} else {
		if (lnode->lasttick == 0) {
			lnode->lasttick = LastTick(entry->key.time, lnode->re);
			lnode->duration = _DtCmsGetDuration(entry);
		}
		rptr->lasttick = lnode->lasttick;

		/* calculate first tick */
		tick = _GetNextActiveTick(entry, entry->key.time,
			lnode->lasttick, lnode->re);

		rptr->starttime = tick;
		rptr->runtime = tick - lead;

		/* need to calculate the active tick if
		 * 1. runtime of first tick is before cutoff time,
		 * 2. runtime of last tick is after cutoff time, and
		 * 3. there's reminders for instances after the cutoff time
		 */
		if (active && rptr->runtime < cutoff &&
		    (lnode->lasttick - lead >= cutoff) &&
		    (tick = _GetNextActiveTick(entry, cutoff + lead,
		    lnode->lasttick, lnode->re)) > 0)
		{
			if ((rptr2 = (_DtCmsRemInfo *)calloc(1,
			    sizeof(_DtCmsRemInfo))) == NULL) {
				free(rptr);
				return (NULL);
			}

			rptr2->lnode = lnode;
			rptr2->isentry = B_TRUE;
			rptr2->data.e = entry;
			rptr2->rem.i = entry->attrs[aindex].name.num;
			rptr2->lasttick = lnode->lasttick;
			rptr2->starttime = tick;
			rptr2->runtime = tick - lead; 
			*active = rptr2;
		}
	}

	return (rptr);
}

static void
_InsertReminder(
	_DtCmsRemInfo	**head,
	_DtCmsRemInfo	*rem)
{
	_DtCmsRemInfo	*rptr, *prev;

	for (rptr = *head, prev = NULL; rptr != NULL;
	    prev = rptr, rptr = rptr->next) {
		if (rem->runtime < rptr->runtime)
			break;
	}

	if (rptr == NULL) {
		if (*head == NULL) {
			*head = rem;
		} else
			prev->next = rem;
	} else {
		rem->next = rptr;

		if (prev == NULL)
			*head = rem;
		else
			prev->next = rem;
	}
}

static CSA_return_code
_GetNextReminders(_DtCmsRemQueue *remq, time_t tick, cms_reminder_ref **rems)
{
	CSA_return_code		stat;
	cms_reminder_ref	*rptr, *head, *tail;
	int			i;

	for (i = 0, head = NULL; i < remq->num_queues; i++) {
		rptr = NULL;

		if ((stat = _GetNextRemindersFromQ(remq, i, tick, &rptr))
		    != CSA_SUCCESS) {
			if (head)
				_DtCmsFreeReminderRef(head);
			return (stat);
		}

		if (rptr == NULL)
			continue;

		if (head == NULL)
			head = tail = rptr;
		else if (rptr->runtime == head->runtime) {
			/* combine the list */
			for (; tail->next != NULL; tail = tail->next);

			tail->next = rptr;
			tail = rptr;
		} else if (rptr->runtime < head->runtime) {
			_DtCmsFreeReminderRef(head);
			head = tail = rptr;
		} else
			_DtCmsFreeReminderRef(rptr);
	}

	if (head) {
		*rems = head;
	}

	return (CSA_SUCCESS);
}

static CSA_return_code
_GetNextRemindersFromQ(
	_DtCmsRemQueue		*remq,
	int			qindex,
	time_t			tick,
	cms_reminder_ref	**rems)
{
	CSA_return_code		stat;
	cms_reminder_ref	*rem = *rems;

	if (tick >= remq->cutoff)
		return (_GetNextRemindersFromList(remq->active[qindex], tick,
			rems));
	else {
		if ((stat = _GetNextRemindersFromList(remq->oldhead[qindex],
		    tick, rems)) == CSA_SUCCESS && rem == *rems)
			return (_GetNextRemindersFromList(remq->active[qindex],
				tick, rems));
		else
			return (stat);
	}
}

static cms_reminder_ref *
_GetReminderRefFromInfo(_DtCmsRemInfo *rem, time_t starttime, time_t runtime)
{
	cms_reminder_ref	*rptr;
	cms_entry		*entry = rem->data.e;
	int			size;

	if ((rptr = (cms_reminder_ref *)calloc(1, sizeof(cms_reminder_ref)))
	    == NULL)
		return (NULL);

	if ((rptr->reminder_name = strdup(entry->attrs[rem->rem.i].name.name))
	    == NULL) {
		free(rptr);
		return (NULL);
	}

	size = entry->attrs[CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I].value->item.\
		opaque_data_value->size;

	if ((rptr->entryid = malloc(size)) == NULL) {
		free(rptr->reminder_name);
		free(rptr);
		return (NULL);
	} else
		strncpy(rptr->entryid, (char *)entry->attrs\
			[CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I].value->item.\
			opaque_data_value->data, size);

	rptr->key.id = entry->key.id;
	rptr->key.time = starttime ? starttime : rem->starttime;

	rptr->runtime = runtime ? runtime : rem->runtime;

	return (rptr);
}

static void
_RemoveReminderFromQ(
	_DtCmsRemQueue	*remq,
	int		qindex,
	cms_entry	*entry,
	List_node	*lnode,	/* zero for one time entries */
	time_t		starttime,
	boolean_t	delfwd)
{
	_DtCmsRemInfo	*rptr, *rptr1 = NULL, *rptr2 = NULL;
	time_t		lead;
	time_t		tick;
	boolean_t	do_old, do_new;
	RepeatEventState *restate;

	_csa_iso8601_to_duration(entry->attrs[remq->aindex[qindex]].value->\
		item.reminder_value->lead_time, &lead);

	if (lnode == 0) {
		if (entry->key.time - lead < remq->cutoff)
			_RemoveReminderFromList(&remq->oldhead[qindex], entry,
				starttime, delfwd);
		else
			_RemoveReminderFromList(&remq->active[qindex], entry,
				starttime, delfwd);
	} else {
		tick = ClosestTick(entry->key.time, entry->key.time, lnode->re,
			&restate);
		free(restate);
		if (do_old = (tick - lead < remq->cutoff))
			rptr1 = _RemoveReminderFromList(&remq->oldhead[qindex],
				entry, starttime, delfwd);

		if (do_new = (lnode->lasttick - lead >= remq->cutoff))
			rptr2 = _RemoveReminderFromList(&remq->active[qindex],
				entry, starttime, delfwd);

		if (rptr = rptr1 ? rptr1 : rptr2) {
			if (do_old && do_new) {
				/* need to clean up the other queue
				 * since add reminder will add to both
				 * queue
				 */
				if (rptr1 == NULL)
					_RemoveReminderFromList(
						&remq->oldhead[qindex], entry,
						0, B_FALSE);
				else
					_RemoveReminderFromList(
						&remq->active[qindex], entry,
						0, B_FALSE);
			}

			_DtCmsAddReminder4EntryToQ(remq, entry,
				remq->aindex[qindex], rptr->lnode);

			if (rptr1) free(rptr1);
			if (rptr2) free(rptr2);
		}
	}
}

static _DtCmsRemInfo *
_RemoveReminderFromList(
	_DtCmsRemInfo	**qhead,
	cms_entry	*entry,
	time_t		starttime,
	boolean_t	delfwd)
{
	_DtCmsRemInfo	*rptr, *prev;

	/* find reminder in list */
	for (rptr = *qhead, prev = NULL; rptr != NULL;
	    prev = rptr, rptr = rptr->next) {
		if (rptr->data.e != entry ||
		    (starttime > 0 && !delfwd && rptr->starttime != starttime)||
		    (starttime > 0 && delfwd && rptr->starttime < starttime))
		{
			continue;
		}

		if (prev == NULL)
			*qhead = rptr->next;
		else
			prev->next = rptr->next;

		if (starttime == 0 || delfwd) {
			free(rptr);
			return (NULL);
		} else
			return (rptr);

	}

	return (NULL);
}

/*
 * move all reminders in active queue whose runtime < cutoff
 * to old queue
 */
static void
_UpdateReminderQ(_DtCmsRemQueue *remq, int qindex)
{
	_DtCmsRemInfo	*rptr, *nptr;
	cms_entry	*entry;
	time_t		lead, tick;
	RepeatEventState *restate = NULL;

	for (; (rptr = remq->active[qindex]) != NULL &&
	    rptr->runtime < remq->cutoff; ) {

		remq->active[qindex] = rptr->next;

		rptr->next = NULL;

		if (rptr->lnode == NULL)
			_InsertReminder(&remq->oldhead[qindex], rptr);
		else {
			lead = rptr->starttime - rptr->runtime;
			entry = rptr->data.e;
			tick = ClosestTick(entry->key.time, entry->key.time,
				rptr->lnode->re, &restate);

			free(restate);

			if (tick == rptr->starttime) {
				/* add this to old queue */
				_InsertReminder(&remq->oldhead[qindex], rptr);

				/* make copy of rptr */
				nptr = (_DtCmsRemInfo *)calloc(1,
					sizeof(_DtCmsRemInfo));
				*nptr = *rptr;
				rptr = nptr;
			}

			if ((rptr->lasttick - lead < remq->cutoff) ||
			    (tick = _GetNextActiveTick(entry,
			    remq->cutoff + lead, rptr->lasttick,
			    rptr->lnode->re)) <= 0)
			{
				free(rptr);
			} else {
				rptr->starttime = tick;
				rptr->runtime = tick - lead; 
				_InsertReminder(&remq->active[qindex], rptr);
			}
		} 
	}
}

static long
_GetNextActiveTick(
	cms_entry	*entry,
	time_t		target,
	time_t		lasttick,
	RepeatEvent	*re)
{
	RepeatEventState	*restate = NULL;
	time_t			tick;

	for (tick = ClosestTick(target, entry->key.time, re, &restate);
	    tick <= lasttick;
	    tick = NextTick(tick, entry->key.time, re, restate))
	{
		if (tick <= 0 || !_DtCmsInExceptionList(entry, tick))
			break;
	}

	free(restate);
	return (tick);
}

/*
 * The reminders found will be linked with the list
 * contained in rf_f
 */
static CSA_return_code
_GetNextRemindersFromList(
	_DtCmsRemInfo		*rlist,
	time_t			giventime,
	cms_reminder_ref	**rf_r)
{
	cms_reminder_ref	*rptr, *head = NULL, *tail;
	time_t			tick;
	int			lead;

	/* get from active queue */
	for (; rlist != NULL; rlist = rlist->next) {

		if (giventime < rlist->runtime)
			break;
		else if (rlist->lnode) {

			/* check the next active tick */
			lead = rlist->starttime - rlist->runtime;
		    	tick = _GetNextActiveTick(rlist->data.e,
				giventime + lead + 1, rlist->lasttick,
				rlist->lnode->re);

			if (tick > 0 &&
			    (head == NULL || (tick-lead <= head->runtime)))
			{
				if ((rptr = _GetReminderRefFromInfo(rlist, tick,
				    tick-lead)) == NULL) {
					if (head)
						_DtCmsFreeReminderRef(head);
					return (CSA_E_INSUFFICIENT_MEMORY);
				}

				if (head == NULL)
					head = rptr;
				else if (head->runtime = rptr->runtime) {
					rptr->next = head;
					head = rptr;
				} else {
					_DtCmsFreeReminderRef(head);
					head = rptr;
				}
			}
		}
	}

	if (rlist) {
		if (head) {
			if (head->runtime > rlist->runtime) {
				_DtCmsFreeReminderRef(head);
				head = NULL;
			} else if (head->runtime < rlist->runtime)
				goto _done;
		}

		/* now do lookup in the remaining list */

		while (rlist != NULL) {
			if (rptr = _GetReminderRefFromInfo(rlist, 0, 0)) {
				rptr->next = head;
				head = rptr;
			} else {
				if (head) _DtCmsFreeReminderRef(head);
				return (CSA_E_INSUFFICIENT_MEMORY);
			}

			if (rlist->next &&
			    rlist->next->runtime == rlist->runtime)
				rlist = rlist->next;
			else
				break;
		}
	}

_done:
	/* find tail */
	if (head) {
		for (tail = head; tail->next != NULL; tail = tail->next) ;
		tail->next = *rf_r;
		*rf_r = head;
	}

	return (CSA_SUCCESS);
}

static void
_DtCmsAddReminder4EntryToQ(
	_DtCmsRemQueue	*remq,
	cms_entry	*entry,
	int		aindex,
	List_node	*lnode)
{
	_DtCmsRemInfo	*rptr, *rptr2;
	int		i;

	if ((rptr = _BuildReminder4Entry(entry, aindex, lnode, remq->cutoff,
	    &rptr2)) == NULL)
		return;

	for (i = 0; i < remq->num_queues; i++) {
		if (remq->aindex[i] == aindex) {
			if (rptr->runtime >= remq->cutoff)
				_InsertReminder(&remq->active[i], rptr);
			else
				_InsertReminder(&remq->oldhead[i], rptr);

			if (rptr2)
				_InsertReminder(&remq->active[i], rptr2);
			break;
		}
	}

	if (i == remq->num_queues) {
		/* expand the queue */
	}
}

