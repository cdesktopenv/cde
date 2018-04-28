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
/*
 *      $XConsortium: util_ds.c /main/4 1995/11/06 18:53:21 rswiston $
 *
 * @(#)util_ds.c	1.3 23 Feb 1994      cde_app_builder/src/libAButil
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


#include <X11/Intrinsic.h>
#include <ab_private/util_ds.h>

/***************************************************
 * Linked List and Active Set ADT implementations
 ***************************************************/

struct _LListStruct {
    void		*value;
    struct _LListStruct	*next;
};

static void		llist_delete_next(
			    LList	llist
			);

extern LList
util_llist_create(
    void
)
{
    LList tmp = (LList) XtMalloc(sizeof(struct _LListStruct));

    tmp->next = NULL;
    return(tmp);
}

extern void
util_llist_destroy(
    LList	llist
)
{
    while (llist != NULL)
    {
	LList	tmp = llist;

	llist = llist->next;
	XtFree((char *)tmp);
    }
}

extern void
util_llist_insert_after(
    LList	llist,
    void	*cl_data
)
{
    LList	tmp = util_llist_create();

    tmp->value  = cl_data;
    tmp->next   = llist->next;
    llist->next = tmp;
}

extern LList
util_llist_find(
    LList	llist,
    void	*cl_data
)
{
    ASet	prev;
    ASet	trav;

    for (prev = llist, trav = llist->next; trav != NULL;
			    prev = trav, trav = trav->next)
	if (trav->value == cl_data)
	    return(prev);
    return(NULL);
}

extern void
util_llist_delete(
    LList	llist,
    void	*cl_data
)
{
    LList	tmp = util_llist_find(llist, cl_data);

    if (tmp != NULL)
	llist_delete_next(tmp);
}

static void
llist_delete_next(
    LList	llist
)
{
    LList	tmp = llist->next;

    if (tmp != NULL)
    {
	llist->next = tmp->next;
	XtFree((char *)tmp);
    }
}

extern void
util_llist_iterate(
    LList	llist,
    LListIterFn	fn
)
{
    for (llist = llist->next; llist != NULL; llist = llist->next)
	(*fn)(llist->value);
}

extern void
util_aset_add(
    ASet	aset,
    void	*cl_data
)
{
    if (util_llist_find(aset, cl_data) == NULL)
	util_llist_insert_after(aset, cl_data);
}
