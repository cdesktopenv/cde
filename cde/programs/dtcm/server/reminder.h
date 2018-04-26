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
/* $XConsortium: reminder.h /main/4 1995/11/09 12:48:37 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _REMINDER_H
#define _REMINDER_H

#include "ansi_c.h"
#include "EUSCompat.h"
#include "cm.h"
#include "rtable4.h"
#include "list.h"

typedef struct _RemInfo {
	boolean_t	isentry;	/* entry vs appt */
	time_t		runtime;	/* run time of reminder */
	time_t		starttime;
	time_t		lasttick;	/* lasttick of a repeating event */
	List_node	*lnode;
	union {
		Appt_4		*a;
		cms_entry	*e;
	} data;				/* associated entry */
	union {
		Attr_4		*a;	/* v4 reminder structure */
		int		i;	/* index of attribute name */
	} rem;				/* associated reminder info */
	struct _RemInfo *next;		/* next reminder */
} _DtCmsRemInfo;

typedef struct _RemQueue {
	time_t		cutoff;
	uint		num_queues;
	int		*aindex;	/* array of attribute index */
	char		**names;	/* array of names */
	_DtCmsRemInfo	**active;	/* array of unexpired reminders */
	_DtCmsRemInfo	**oldhead;	/* head of old reminders list */
} _DtCmsRemQueue;

typedef struct reminder_q {
	time_t	remind_at;
	int     remind_ord;
	Attr_4	attr;
	Appt_4	*appt;
	struct reminder_q *next;
} Rm_que;

extern void _DtCmsAddReminderV4 P((Rm_que **qhead, Rm_que *p_reminder));

extern Rm_que *_DtCmsRemoveReminderV4 P((
		Rm_que **qhead,
		Rm_que *p_prev,
		Rm_que *p_curr));

extern Rm_que *build_reminder P((
		time_t	current_time,
		Appt_4	*p_appt,
		Attr_4	p_attr,
		time_t	start_tick,
		u_int	start_ord));

extern Reminder_4 *_DtCmsGetReminderInfoV4 P((Rm_que *original));

extern void _DtCmsPrintReminderListV4 P((Rm_que *qhead));

extern void _DtCmsObsoleteReminderV4 P((
		Rm_que		**qhead,
		Appt_4		*p_appt,
		int		ord, 
		boolean_t	delforward));

extern void _DtCmsAddReminders4Entry P((
		_DtCmsRemQueue	**qhead,
		cms_entry	*entry,
		List_node	*lnode));

extern void _DtCmsObsoleteReminder4Entry P((
		_DtCmsRemQueue	*qhead,
		cms_entry	*entry,
		List_node	*lnode,
		time_t		starttime,
		boolean_t	delfwd));

extern CSA_return_code _DtCmsLookupReminder P((
			_DtCmsRemQueue		*remq,
			time_t			tick,
			uint			num_names,
			cms_attr_name		*names,
			cms_reminder_ref	**rems));

extern void _DtCmsFreeReminderRef P((cms_reminder_ref *rems));

extern void _DtCmsUpdateReminders(_DtCmsRemQueue *remq);

#endif
