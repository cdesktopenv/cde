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
/* $XConsortium: alloc.c /main/2 1996/05/09 03:55:49 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: CurrLock
 *		CurrUnlock
 *		I_CALLOC
 *		I_FREE
 *		I_MALLOC
 *		Memlock
 *		Memunlock
 *		NewInit
 *		calloc
 *		d_alloc
 *		d_calloc
 *		d_free
 *		ll_access
 *		ll_append
 *		ll_first
 *		ll_free
 *		ll_next
 *		ll_prepend
 *		malloc
 *		memlock
 *		memunlock
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   alloc.c -- db_VISTA dynamic memory allocation module.

   (C) Copyright 1986,1987 by Raima Corporation.
-----------------------------------------------------------------------*/
/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  420 15-Aug-88 RTK Lock level wasn't incremented when stack had overflowed
  420 27-Sep-88 RSC there was unreachable code in ll_free (repl 'return' w/
		    'break' inside loop)
  550 10-Jan-88 RSC ll_free was referencing a segment that was free'd
  420 13-Feb-89 WLW Removed unnecessary FAR's, modified d_lockpop
  550 14-Feb-89 RTK Better fix to ll_free problem (windows compatible)

*/
#include <stdio.h>
#include <stdlib.h>
#include "vista.h"
#include "dbtype.h"

#define memlock(p)   (p)
#define memunlock(p) 1

#define Memlock(cp)		/**/
#define Memunlock(cp)		/**/
#define I_CALLOC(cp, i, s)	calloc(i, s)
#define I_MALLOC(cp, b)		malloc(b)
#define I_FREE(cp)		free((cp)->ptr)
#define CurrUnlock(ll)		/**/
#define CurrLock(ll)		/**/


/*-----------------------------------------------------------------------
   Uninstrumented dynamic memory allocation functions
-----------------------------------------------------------------------*/

/* Allocate and clear i*s bytes of memory
*/
char FAR * d_calloc(i, s)
unsigned i, s;
{
   return (I_CALLOC(cp, i, s));
}


/* Allocate b bytes of memory
*/
char FAR * d_alloc(b)
unsigned b;
{
   return (I_MALLOC(cp, b));
}

/* Free memory
*/
void d_free(p)
CHAR_P FAR *p;
{
   if ((p != NULL) && (p->ptr != NULL)) {
      I_FREE(p);
      p->ptr = NULL;
   }
}


static int NewInit(P1(LL_P FAR *));

/* Resets pointer to current element and checks for an empty list
*/
BOOLEAN ll_access(ll)
llist FAR *ll;
{
   ll->curr = NULL;
   return( ll->head.ptr != NULL && ll->tail.ptr != NULL );
}

/* Appends item to list
*/
int ll_append(ll, data)
llist FAR *ll;
CHAR_P FAR *data;
{
   LL_P item;
   LL_P hold;

   if ( NewInit(&item) != S_OKAY ) {
      return( db_status );
   }
   if ( ll->head.ptr == NULL ) {
      /* Empty list */
      ll->head = ll->tail = item;
      ll->curr = (LL_P FAR *)&ll->head;
      CurrLock(ll);
   }
   else {
      CurrUnlock(ll);
      ll->curr = (LL_P FAR *)&ll->tail;
      CurrLock(ll);
      hold = item;
      MEM_LOCK(&item);
      ll->curr->ptr->next = item;
      CurrUnlock(ll);
      ll->curr = (LL_P FAR *)&ll->tail;
      *ll->curr = hold;
      CurrLock(ll);
   }
   ll->curr->ptr->data = *data;
   return( db_status );
}

/* Finds the first element of a list and returns its data 
*/
CHAR_P FAR *ll_first(ll)
llist FAR *ll;
{
   if ( ll->head.ptr == NULL ) {
      return( NULL );
   }
   CurrUnlock(ll);
   ll->curr = (LL_P FAR *)&ll->head;
   CurrLock(ll);
   return( (CHAR_P FAR *)&ll->curr->ptr->data );
}

/* Frees a list
*/
void ll_free(ll)
llist FAR *ll;
{
   LL_P FAR *curr, next, free;

   curr = (LL_P FAR *)&ll->head;
   while ( TRUE ) {
      MEM_LOCK(curr);
      if ( curr->ptr == NULL )
	 break;
      bytecpy(&next, &curr->ptr->next, sizeof(LL_P));
      if ( curr != NULL )
	 d_free((CHAR_P FAR *)curr);
      bytecpy(&free, &next, sizeof(LL_P));
      curr = &free;
   }
   byteset(&ll->head, '\0', sizeof(LL_P));
   byteset(&ll->tail, '\0', sizeof(LL_P));
}

/* Finds the next element and returns its data
*/
CHAR_P FAR *ll_next(ll)
llist FAR *ll;
{
   LL_P FAR *next;

   if ( ll->curr == NULL ) {
      return( ll_first(ll) );
   }
   if ( ll->curr->ptr->next.ptr == NULL ) {
      return( NULL );
   }
   next = &ll->curr->ptr->next;
   CurrUnlock(ll);
   ll->curr = next;
   CurrLock(ll);
   return( (CHAR_P FAR *)&ll->curr->ptr->data );
}

/* Prepends (stacks) item 
*/
int ll_prepend(ll, data)
llist FAR *ll;
CHAR_P FAR *data;
{
   LL_P item;

   if ( NewInit(&item) != S_OKAY ) {
      return( db_status );
   }
   if ( ll->head.ptr == NULL ) {
      /* Empty list */
      ll->head = ll->tail = item;
      ll->curr = (LL_P FAR *)&ll->head;
      CurrLock(ll);
   }
   else {
      CurrUnlock(ll);
      MEM_LOCK(&item);
      item.ptr->next = ll->head;
      MEM_UNLOCK(&item);
      ll->curr = (LL_P FAR *)&ll->head;
      *ll->curr = item;
      CurrLock(ll);
   }
   ll->curr->ptr->data = *data;
   return( db_status );
}

/* Allocates and initializes a new list element
*/
static int NewInit(new)
LL_P FAR *new;
{
   new->ptr = (ll_elem FAR *)ALLOC(new, sizeof(ll_elem), "new");
   if ( new->ptr == NULL )
      return( dberr(S_NOMEMORY) );
   byteset(new->ptr, '\0', sizeof(ll_elem));
   MEM_UNLOCK(new);
   return( db_status );
}

/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin alloc.c */
