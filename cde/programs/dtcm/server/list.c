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
/* $XConsortium: list.c /main/4 1995/11/09 12:45:31 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include "tree.h"
#include "list.h"

extern int debug;

typedef struct {
	_DtCmsGetKeyProc get;
	_DtCmsEnumerateProc enumerate;
	_DtCmsCompareProc compare;
} Private;


extern	List_node *
hc_lookup_node (Hc_list	*hc_list, caddr_t key)
{
	Private		*private;
	List_node	*p_node;
	_DtCmsComparisonResult	result;

	p_node = hc_list->root;
	private = (Private *) hc_list->private;
	while (p_node != NULL)
	{
		result = private->compare (key, p_node->data);
		if (result == _DtCmsIsGreater)
		{
			p_node = hc_lookup_next (p_node);
		}
		else if (result == _DtCmsIsEqual)
		{
			return (p_node);
		}
		else
		{
			break;
		}
	}
	return (NULL);
}

extern	caddr_t
hc_lookup (Hc_list *hc_list, caddr_t key)
{
	List_node	*p_node;

	p_node = hc_lookup_node (hc_list, key);
	if (p_node != NULL)
		return ((caddr_t) p_node->data);
	return (NULL);
}

extern	int
hc_size (Hc_list *hc_list)
{
	int		n = 0;
	List_node	*p_node;

	p_node = hc_list->root;
	while (p_node != NULL)
	{
		n++;
		p_node = hc_lookup_next (p_node);
	}
	return (n);
}


extern Hc_list *
hc_create(_DtCmsGetKeyProc get, _DtCmsCompareProc compare)
{
	Private	*p;
	List_node	*root = NULL;
	Hc_list	*list;

	p = (Private *) calloc (1, sizeof (*p));

	list = (Hc_list *) calloc (1, sizeof (*list));
	list->root = NULL;
	list->private = (caddr_t) p;

	p->get = get;
	p->enumerate = NULL;
	p->compare = compare;

	return (list);
}

extern void
hc_destroy(Hc_list *hc_list, Destroy_proc destroy_func)
{
	Private	*p;
	List_node	*p_node, *p_next;

	if (hc_list == NULL)
		return;
	if ((p = (Private *) hc_list->private) != NULL)
		free (p);

	p_node = hc_list->root;
	while (p_node != NULL)
	{
		p_next = hc_lookup_next(p_node);
		if (p_node->data != NULL)
		{
			if (destroy_func)
				destroy_func(p_node->data);
		}
		free (p_node);
		p_node = p_next;
	}
}

extern Rb_Status
hc_insert_node (Hc_list *hc_list, List_node *p_node, caddr_t key)
{
	Private	*private;
	List_node	*p_curr;

	if (hc_list == NULL)
		return (rb_notable);
	private = (Private *) hc_list->private;

	p_curr = hc_list->root;
	while (p_curr != NULL)
	{
		if (private->compare (key, p_curr->data) == _DtCmsIsGreater)
		{
			if (p_curr->rlink != NULL)
				p_curr = p_curr->rlink;
			else
			{
				/* Insert at end of the list */
				p_curr->rlink = p_node;
				p_node->llink = p_curr;
				return (rb_ok);
			}
		}
		else
		{
			/* Insert at head of the list */
			if ((p_node->llink = p_curr->llink) == NULL)
				break;

			/* Insert before the current node */
			p_curr->llink = p_node->llink->rlink = p_node;
			p_node->rlink = p_curr;
			return (rb_ok);
		}
	}

	/* Insert at head of the list */
	p_node->rlink = hc_list->root;
	if (p_node->rlink != NULL)
		p_node->rlink->llink = p_node;
	hc_list->root = p_node;
	return (rb_ok);
}

extern Rb_Status
hc_insert(
	Hc_list		*hc_list,
	caddr_t 	data,
	caddr_t 	key,
	RepeatEvent	*re,
	List_node	**node_r)
{
	List_node	*p_node;
	Rb_Status	stat;

	if (hc_list == NULL)
		return (rb_notable);
	p_node = (List_node *) calloc (1, sizeof(*p_node));
	p_node->data = data;
	p_node->re = re;
	stat = hc_insert_node (hc_list, p_node, key);

	if (stat == rb_ok && node_r)
		*node_r = p_node;

	return (stat);
}

extern List_node *
hc_delete_node (Hc_list *hc_list, List_node	*p_node)
{
	if (p_node->llink == NULL)
		hc_list->root = p_node->rlink;
	else
		p_node->llink->rlink = p_node->rlink;
	if (p_node->rlink != NULL)
		p_node->rlink->llink = p_node->llink;
	return (p_node);
}

extern List_node *
hc_delete (Hc_list *hc_list, caddr_t key)
{
	List_node	*p_node;
	Private *private;

	p_node = hc_list->root;
	private = (Private *) hc_list->private;
	while (p_node != NULL)
	{
		if (private->compare (key, p_node->data) == _DtCmsIsEqual)
		{
			(void) hc_delete_node (hc_list, p_node);
			return (p_node);
		}
		p_node = hc_lookup_next(p_node);
	}
	return (NULL);
}

extern caddr_t
hc_lookup_smallest (Hc_list *hc_list)
{
	if ((hc_list == NULL) || (hc_list->root == NULL))
		return (NULL);
	return ((caddr_t) hc_list->root->data);
}

extern caddr_t
hc_lookup_next_larger (Hc_list *hc_list, caddr_t key)
{
	List_node	*p_node;
	Private *private;

	p_node = hc_list->root;
	private = (Private *) hc_list->private;
	while (p_node != NULL)
	{
		if (private->compare (key, p_node->data) == _DtCmsIsLess)
			return ((caddr_t) p_node->data);
		p_node = hc_lookup_next(p_node);
	}
	return (NULL);
}

extern caddr_t
hc_lookup_largest (Hc_list *hc_list)
{
	List_node	*p_node;

	if ((hc_list == NULL) || (hc_list->root == NULL))
		return (NULL);

	p_node = hc_list->root;
	while (p_node->rlink != NULL)
		p_node = hc_lookup_next (p_node);
	return ((caddr_t) p_node->data);
}

extern caddr_t
hc_lookup_next_smaller (Hc_list *hc_list, caddr_t key)
{
	List_node	*p_node;
	Private *private;

	p_node = hc_list->root;
	private = (Private *) hc_list->private;
	while (p_node != NULL)
	{
		if (private->compare (key, p_node->data) != _DtCmsIsGreater)
		{
			if (p_node->llink == NULL)
				return (NULL);
			else
				return ((caddr_t) p_node->llink->data);
		}
		p_node = hc_lookup_next(p_node);
	}
	return (NULL);
}

extern	Rb_Status
hc_check_list (Hc_list *hc_list)
{
	if ((hc_list == NULL) || (hc_list->root == NULL))
		return (rb_notable);
	return (rb_ok);
}


extern void
hc_enumerate_down(Hc_list *hc_list, _DtCmsEnumerateProc doit)
{
	List_node	*p_node;

	p_node = hc_list->root;
	while (p_node != NULL)
	{
		if (doit ((caddr_t)p_node, p_node->data))
			return;
		p_node = p_node->llink;
	}
}

extern Rb_Status
hc_enumerate_up(Hc_list *hc_list, _DtCmsEnumerateProc doit)
{
	List_node	*p_node;

	p_node = hc_list->root;
	while (p_node != NULL)
	{
		if (doit ((caddr_t)p_node, p_node->data))
			return (rb_failed);
		p_node = hc_lookup_next (p_node);
	}

	return (rb_ok);
}
