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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $TOG: isdlink.c /main/4 1999/09/29 15:03:15 mgreess $ 			 				 */

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isdlink.c
 *
 * Description:
 *	Double link list functions
 */

#include "isam_impl.h"

/* _isdln_base_insert () - Insert element into list (at the front) -----------*/
void _isdln_base_insert (char *base, struct dlink *l,
                         struct dlink *e)
{
	e->dln_forward = l->dln_forward;
	l->dln_forward = (char *)e - base;
	
	e->dln_backward = (char *)l - base;
	((struct dlink *)(base + e->dln_forward))->dln_backward = (char *)e - base;
}

/* _isdln_base_append () - Append element to list (at the end) -------------*/
void _isdln_base_append (char *base, struct dlink *l,
                         struct dlink *e)
{
	e->dln_backward = l->dln_backward;
	l->dln_backward = (char *)e - base;
	
	e->dln_forward = (char *)l - base;
	((struct dlink *)(base + e->dln_backward))->dln_forward = (char *)e - base;
}

/* _isdln_base_remove () - Remove element from list -------------------------*/
void _isdln_base_remove (char *base, struct dlink *e)
{
	((struct dlink *)(base + e->dln_backward))->dln_forward = e->dln_forward;
	((struct dlink *)(base + e->dln_forward))->dln_backward = e->dln_backward;
}

/* _isdln_base_first () - Return first element of the list -------------------*/
struct dlink * _isdln_base_first(char *base, struct dlink *l)
{
	struct dlink *val = (struct dlink *)(base + l->dln_forward);

	if (val == NULL) {
		if (NULL == (struct dlink *)(base + l->dln_forward) &&
		    NULL == (struct dlink *)(base + l->dln_backward))
			_isdln_base_makeempty(base, l);
		val = l;
	}
	return(val);
}

/* _isdln_base_next () - Return next element in the list --------------------*/
struct dlink * _isdln_base_next(char *base, struct dlink *l)
{
	return (((struct dlink *)(base + l->dln_forward))); 
}

/* _isdln_base_prev () - Return previous element in the list ----------------*/
struct dlink * _isdln_base_prev(char *base, struct dlink *l)
{
	return  (((struct dlink *)(base + l->dln_backward))); 
}

/* _isdln_base_makeempty () - Make head of empty list -----------------------*/
void _isdln_base_makeempty(char *base, struct dlink *l)
{
	l->dln_forward = l->dln_backward = (char *)l - base;
}

/* _isdln_base_isempty () - Test if list is empty---------------------------*/
int _isdln_base_isempty(char *base, struct dlink *l)
{
	return (l->dln_forward == (char *)l - base &&
		l->dln_backward == (char *)l - base);
}
