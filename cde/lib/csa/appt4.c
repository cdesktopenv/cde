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
/* $XConsortium: appt4.c /main/1 1996/04/21 19:21:34 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

/*
 * this file contains allocate and free routines for v4 data structures
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appt4.h"

#define	FAUX_STRING	"Appointment"

/*
 * forward declaration of static functions used within the file
 */
static void free_tag4(Tag_4 *t);
static Attribute_4 *copy_attr4(Attribute_4 *a4);
static Tag_4 *copy_tag4(Tag_4 *t4);

/*****************************************************************************
 * extern functions
 *****************************************************************************/

extern Appt_4 *
_DtCm_make_appt4(boolean_t alloc)
{
	Appt_4 *a;
	
	if ((a = (Appt_4 *)calloc(1, sizeof(Appt_4))) == NULL)
		return NULL;

	if ((a->tag = (Tag_4 *)calloc(1, sizeof(Tag_4))) == NULL) {
		free(a);
		return NULL;
	}
	a->tag->tag = appointment_4;
	a->tag->next = NULL;
	a->tag->showtime = B_TRUE;

	a->period.period = single_4;
	a->appt_status = active_4;
	a->privacy = public_4;

	if (alloc) {
		a->what = calloc(1, sizeof(1));
		a->author = calloc(1, sizeof(1));
		a->client_data = calloc(1, sizeof(1));
	}

	a->next = NULL;
	return(a);
}

extern Appt_4 *
_DtCm_copy_one_appt4(Appt_4 *a4)
{
	Appt_4 *ptr, *copy;

	if (a4 == NULL)
		return NULL;

	ptr = a4->next;
	a4->next = NULL;

	copy = _DtCm_copy_appt4(a4);

	a4->next = ptr;
	return (copy);
}

extern Appt_4 *
_DtCm_copy_appt4(Appt_4 *a4)
{
	Appt_4		*a, *head, *prev;
	boolean_t	cleanup = B_FALSE;

	prev = head = NULL;
	while (a4 != NULL) {
		if ((a = (Appt_4 *)calloc(1, sizeof(Appt_4))) == NULL) {
			cleanup = B_TRUE;
			break;
		}

		a->appt_id.tick = a4->appt_id.tick;
		a->appt_id.key = a4->appt_id.key;

		a->duration = a4->duration;
		a->ntimes = a4->ntimes;

        	if (a4->what) {
			if ((a->what = strdup(a4->what)) == NULL) {
				_DtCm_free_appt4(a);
				cleanup = B_TRUE;
				break;
			}
		} else if ((a->what = (char *)calloc(1, 1)) == NULL) {
			_DtCm_free_appt4(a);
			cleanup = B_TRUE;
			break;
		}

		a->period.period = a4->period.period;
		a->period.nth = a4->period.nth;
		a->period.enddate = a4->period.enddate;

        	if (a4->author) {
			if ((a->author = strdup(a4->author)) == NULL) {
				_DtCm_free_appt4(a);
				cleanup = B_TRUE;
				break;
			}
		} else if ((a->author = (char *)calloc(1, 1)) == NULL) {
			_DtCm_free_appt4(a);
			cleanup = B_TRUE;
			break;
		}

        	if (a4->client_data) {
			if ((a->client_data = strdup(a4->client_data)) == NULL) {
				_DtCm_free_appt4(a);
				cleanup = B_TRUE;
				break;
			}
		} else if ((a->client_data = (char *)calloc(1, 1)) == NULL) {
			_DtCm_free_appt4(a);
			cleanup = B_TRUE;
			break;
		}

		if (a4->exception &&
		    (a->exception = _DtCm_copy_excpt4(a4->exception)) == NULL) {
			_DtCm_free_appt4(a);
			cleanup = B_TRUE;
			break;
		}

		if (a4->attr && (a->attr = copy_attr4(a4->attr)) == NULL) {
			_DtCm_free_appt4(a);
			cleanup = B_TRUE;
			break;
		}

		if (a4->tag) {
			if ((a->tag = copy_tag4(a4->tag)) == NULL) {
				free(a);
				cleanup = B_TRUE;
				break;
			}
		} else if ((a->tag = (Tag_4 *)calloc(1, sizeof(Tag_4))) == NULL) {
			free(a);
			cleanup = B_TRUE;
			break;
		} else {
			a->tag->tag = appointment_4;
			a->tag->showtime = B_TRUE;
			a->tag->next = NULL;
		}

		a->appt_status = a4->appt_status;
		a->privacy = a4->privacy;
          	a->next = NULL;

		if (head == NULL)
			head = a;
		else
			prev->next = a;
		prev = a;

		a4 = a4->next;
        }

	if (cleanup == B_TRUE) {
		_DtCm_free_appt4(head);
		head = NULL;
	}

        return (head);
}

/*
 * Copy one appointment.
 * The what field is set to "Appointment".
 */
extern Appt_4 *
_DtCm_copy_semiprivate_appt4(Appt_4 *original)
{
	Appt_4 *temp, *copy = NULL;

	if (original != NULL) {
		temp = original->next;
		original->next = NULL;

		if ((copy = _DtCm_copy_appt4(original)) != NULL) {
			if (copy->what != NULL)
				free(copy->what);
			if ((copy->what = (char *)strdup(FAUX_STRING)) == NULL)
			{
				_DtCm_free_appt4(copy);
				copy = NULL;
			}
		}

		original->next = temp;
	}
	return(copy);
}

extern Abb_Appt_4 *
_DtCm_appt_to_abbrev4(Appt_4 *original)
{
	Abb_Appt_4 *new = NULL;

	if (original != NULL) {
		if ((new = (Abb_Appt_4 *)calloc(1, sizeof(Abb_Appt_4))) == NULL)
			return NULL;

		if ((new->tag = copy_tag4(original->tag)) == NULL) {
			free(new);
			return NULL;
		}

		new->appt_id.tick = original->appt_id.tick;
		new->appt_id.key = original->appt_id.key;

		if ((new->what = (char *)strdup(original->what)) == NULL) {
			_DtCm_free_abbrev_appt4(new);
			return NULL;
		}

		new->duration = original->duration;
		new->period.period = original->period.period;
		new->period.nth = original->period.nth;
		new->period.enddate = original->period.enddate;

		new->appt_status = original->appt_status;
		new->privacy = original->privacy;
		new->next=NULL;
	}
	return(new);
}

/*
 * The what field is set to "Appointment".
 */
extern Abb_Appt_4 *
_DtCm_appt_to_semiprivate_abbrev4(Appt_4 *original)
{
	Abb_Appt_4 *new = _DtCm_appt_to_abbrev4(original);
	if (new != NULL) {
		if (new->what != NULL)
			free(new->what);
		if ((new->what = strdup(FAUX_STRING)) == NULL) {
			_DtCm_free_abbrev_appt4(new);
			new = NULL;
		}
	}
	return(new);
}

extern void
_DtCm_free_appt4(Appt_4 *a)
{
	Appt_4 *ptr;

	while (a != NULL) {
		ptr = a->next;

		if (a->what != NULL)
			free(a->what);

		if (a->author != NULL)
			free(a->author);

		if (a->tag != NULL)
			free_tag4(a->tag);

		if (a->attr != NULL)
			_DtCm_free_attr4(a->attr);

		if (a->exception != NULL)
			_DtCm_free_excpt4(a->exception);

     		free(a);

		a = ptr;
	}
}

extern void
_DtCm_free_abbrev_appt4(Abb_Appt_4 *a)
{
	Abb_Appt_4 *ptr;

	while (a != NULL) {
		ptr = a->next;

		if (a->what != NULL) {
			free(a->what);
		}

		if (a->tag != NULL) {
			free_tag4(a->tag);
		}

		free(a);

		a = ptr;
	}
}

Attribute_4 *
_DtCm_make_attr4(void)
{
	Attribute_4 *a;
	
	if ((a = (Attribute_4 *)calloc(1, sizeof(Attribute_4))) == NULL)
		return NULL;

	if ((a->attr = (char *)calloc(1, 1)) == NULL) {
		free(a);
		return NULL;
	}

	if ((a->value = (char *)calloc(1, 1)) == NULL) {
		_DtCm_free_attr4(a);
		return NULL;
	}

	if ((a->clientdata = (char *)calloc(1, 1)) == NULL) {
		_DtCm_free_attr4(a);
		return NULL;
	}

	return(a);
}

extern void
_DtCm_free_attr4(Attribute_4 *a)
{
	Attribute_4 *ptr;

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

extern Reminder_4 *
_DtCm_copy_reminder4(Reminder_4 *r4)
{
        Reminder_4 *rem, *head, *prev;

	prev = head = NULL;
	while (r4 != NULL) {
		rem = (Reminder_4 *)calloc(1, sizeof(Reminder_4));
		rem->appt_id = r4->appt_id;
		rem->tick = r4->tick;
		rem->attr.next = NULL;
		rem->attr.attr = strdup(r4->attr.attr);
		rem->attr.value = strdup(r4->attr.value);
		rem->attr.clientdata = strdup(r4->attr.clientdata);
		rem->next = NULL;

		if (head == NULL)
			head = rem;
		else
			prev->next = rem;
		prev = rem;

		r4 = r4->next;
	}
	return(head);
}
      
extern void
_DtCm_free_reminder4(Reminder_4 *r)
{
	Reminder_4 *ptr;

	while (r != NULL) {
		ptr = r->next;

		if (r->attr.attr != NULL)
			free (r->attr.attr);
		if (r->attr.value != NULL)
			free (r->attr.value);
		if (r->attr.clientdata != NULL)
			free (r->attr.clientdata);
		free (r);

		r = ptr;
	}
}

extern void
_DtCm_free_keyentry4(Uid_4 *k)
{
	Uid_4 *ptr;

	while (k != NULL) {
		ptr = k->next;
		free(k);
		k = ptr;
	}
}

extern Access_Entry_4 *
_DtCm_make_access_entry4(char *who, int perms)
{
        Access_Entry_4 *e;

        if (who==NULL) return((Access_Entry_4 *)NULL);
        if ((e = (Access_Entry_4 *)calloc(1, sizeof(Access_Entry_4))) == NULL)
		return NULL;

        if ((e->who = strdup(who)) == NULL) {
		free(e);
		return NULL;
	}

        e->access_type = perms;
        e->next = NULL;
        return(e);
}

extern void
_DtCm_free_access_list4(Access_Entry_4 *e)
{
	Access_Entry_4 *ptr;

	while (e != NULL) {
		ptr = e->next;

		if (e->who != NULL)
			free(e->who);
		free(e);

		e = ptr;
	}
}

extern void
_DtCm_free_excpt4(Except_4 *e)
{
	Except_4 *ptr;

	while (e != NULL) {
		ptr = e->next;
		free(e);
		e = ptr;
	}
}

extern Except_4 *
_DtCm_copy_excpt4(Except_4 *e4)
{
	Except_4	*e, *head, *prev;
	boolean_t	cleanup = B_FALSE;

	prev = head = NULL;
	while (e4 != NULL) {
		if ((e = (Except_4 *)calloc(1, sizeof(Except_4))) == NULL) {
			cleanup = B_TRUE;
			break;
		}

		e->ordinal = e4->ordinal;
		e->next = NULL;

		if (head == NULL)
			head = e;
		else
			prev->next = e;
		prev = e;

		e4 = e4->next;
	}

	if (cleanup == B_TRUE) {
		_DtCm_free_excpt4(head);
		head = NULL;
	}

	return (head);
}

/******************************************************************************
 * static functions used within the file
 ******************************************************************************/

static void
free_tag4(Tag_4 *t)
{
	Tag_4 *ptr;

	while (t != NULL) {
		ptr = t->next;
		free(t);
		t = ptr;
	}
}

/*	Copy an attribute list recursively */
static Attribute_4 *
copy_attr4(Attribute_4 *a4)
{
	Attribute_4	*a, *head, *prev;
	boolean_t	cleanup = B_FALSE;

	prev = head = NULL;
	while (a4 != NULL) {
		if ((a = (Attribute_4 *)calloc(1, sizeof(Attribute_4)))
		    == NULL) {
			cleanup = B_TRUE;
			break;
		}

		if ((a->attr = strdup(a4->attr)) == NULL) {
			free(a);
			cleanup = B_TRUE;
			break;
		}
	
		if ((a->value = strdup(a4->value)) == NULL) {
			_DtCm_free_attr4(a);
			cleanup = B_TRUE;
			break;
		}

		if ((a->clientdata = strdup(a4->clientdata)) == NULL) {
			_DtCm_free_attr4(a);
			cleanup = B_TRUE;
			break;
		}

		if (head == NULL)
			head = a;
		else
			prev->next = a;
		prev = a;

		a4 = a4->next;
	}

	if (cleanup == B_TRUE) {
		_DtCm_free_attr4(head);
		head = NULL;
	}

	return (head);
}

static Tag_4 *
copy_tag4(Tag_4 *t4)
{
	Tag_4		*t, *head, *prev;
	boolean_t	cleanup = B_FALSE;

	prev = head = NULL;
	while (t4 != NULL) {
		if ((t = (Tag_4 *)calloc(1, sizeof(Tag_4))) == NULL) {
			cleanup = B_TRUE;
			break;
		}

		t->tag = t4->tag;
		t->showtime = t4->showtime;
		t->next = NULL;

		if (head == NULL)
			head = t;
		else
			prev->next = t;
		prev = t;

		t4 = t4->next;
	}

	if (cleanup == B_TRUE) {
		free_tag4(head);
		head = NULL;
	}

	return (head);
}

