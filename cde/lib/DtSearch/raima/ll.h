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
/* $XConsortium: ll.h /main/2 1996/05/09 04:11:29 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: LLIST_INIT
 *		LL_ELEM_INIT
 *		ll_access
 *		ll_append
 *		ll_deaccess
 *		ll_first
 *		ll_free
 *		ll_next
 *		ll_prepend
 *
 *   ORIGINS: 157
 *
 */
/* The functions in this file are defined in alloc.c */

typedef struct {struct ll_elem *ptr; LOCK_DESC} LL_P;

typedef struct ll_elem {
   LL_P next;
   CHAR_P data;
} ll_elem;

#define LL_ELEM_INIT()	{ POINTER_INIT(), POINTER_INIT() }

typedef struct {
   LL_P head;
   LL_P tail;
   LL_P *curr;
} llist;

#define LLIST_INIT()    { POINTER_INIT(), POINTER_INIT(), NULL }

BOOLEAN ll_access(P1(llist *));
int ll_append(P1(llist *) Pi(CHAR_P *));
#define ll_deaccess(ll)	    /**/
CHAR_P *ll_first(P1(llist *));
void ll_free(P1(llist *));
CHAR_P *ll_next(P1(llist *));
int ll_prepend(P1(llist *) Pi(CHAR_P *));
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin ll.h */
