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
/*
 * File:         Symbolic.c $XConsortium: Symbolic.c /main/5 1996/09/27 19:00:23 drk $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifdef __osf__
#define SBSTDINC_H_NO_REDEFINE
#endif

#include <Dt/UserMsg.h>

#include <bms/sbport.h>		/* NOTE: sbport.h must be the first include. */

#include <assert.h>

#include <bms/bms.h>
#include <bms/XeUserMsg.h>
#include <bms/MemoryMgr.h>
#include <bms/Symbolic.h>

#include <codelibs/stringx.h>	/* strhash */
#include "DtSvcLock.h"

/******************************************************************************/
/* HASHING */

/* This is the default symbol table to use */
/* --------------------------------------- */
#define XE_END_OF_HASH_TABLE (XeSymtabList) -1

static XeSymTable D_sym_table = NULL;

typedef struct _unknown_entry_data {
    XeString name;
} *unknown_entry_data;


/******************************************************************************/
/*  Symbol (hash) Table							      */

/*------------------------------------------------------------------------+*/
static unsigned int
keyhash(XeSymTable t, void *key)
/*------------------------------------------------------------------------+*/
{
    unsigned int hash;
    
    if (t->hash_fn)
    {
	hash = t->hash_fn( key, t->hashsize );
	if (hash >= t->hashsize)
	    _DtSimpleError(XeProgName, XeInternalError, NULL, 
			  (XeString) "Symbolic.c: Hash value from user hash function out of range",
			  NULL);
	    /* We don't come back from the error routine */
    }
    else
    {
	hash = strhash( (const char *) key );
	hash =  hash & (t->hashsize - 1);
    }
    
    return hash;
}



/*------------------------------------------------------------------------+*/
static unsigned int
trap_bad_hash_fn(void * UNUSED_PARM(ptr), unsigned int UNUSED_PARM(size))
/*------------------------------------------------------------------------+*/
{
    _DtSimpleError(XeProgName, XeInternalError, NULL, 
		  (XeString) "Symbolic.c: Hash table at must be power of 2",
		  NULL);
    /* We don't come back from the error routine */
    return 1;
}

/*------------------------------------------------------------------------+*/
XeSymTable
Xe_new_symtab(unsigned int  hashsize)
/*------------------------------------------------------------------------+*/

/* Note, hashsize must be power of 2 if using default hash function */

{
    int i;

    XeSymTable t = (XeSymTable) XeMalloc( sizeof (struct _XeSymTable) );
    t->hashsize = hashsize;

    t->list     = (XeSymtabList *)XeMalloc( sizeof( XeSymtabList ) * hashsize );
   
    for (i = 0; i < hashsize; i++)
	t->list[i] = (XeSymtabList)NULL;

    t->curr_list = XE_END_OF_HASH_TABLE;
    t->curr_hash = 0;

    Xe_set_sym_fns(t,
		   (XeSymFn_cmp)NULL,
		   (XeSymFn_init)NULL,
		   (XeSymFn_clean)NULL,
		   (XeSymFn_hash)NULL);
    
    /* If not a power of two, user better have a hash function   */
    /* that handles that.  Install hash function trap so that if */
    /* he does not install one, we catch it.                     */
    /* --------------------------------------------------------- */
    if (hashsize & (hashsize - 1))
	t->hash_fn = trap_bad_hash_fn;
    
    return t;
}

/*------------------------------------------------------------------------+*/
XeSymTable
Xe_default_symtab(void)
/*------------------------------------------------------------------------+*/
{
#define D_HASHSIZE 256

    _DtSvcProcessLock();
    if (D_sym_table) {
	_DtSvcProcessUnlock();
	return D_sym_table;
    }

    D_sym_table = Xe_new_symtab(D_HASHSIZE);
    _DtSvcProcessUnlock();
    return(D_sym_table);
}
/*------------------------------------------------------------------------+*/
static XeSymtabList
NukeOneItem(XeSymTable t, XeSymtabList l)
/*------------------------------------------------------------------------+*/
{
    XeSymtabList next;
    
    /* For standard XeSymbols: 			*/
    /* 1) Free the name         		*/
    /* 2) Call free function if configured      */
    /* 3) Free the XeSymbol entry               */
    /* ---------------------------------------- */
    if (l->data_is_XeSymbol)
    {
	XeFree( ((XeSymbol)l->data)->name );
	if (t->clean_fn)
	    t->clean_fn( ((XeSymbol)l->data)->value );
	XeFree( l->data );
    }
    /* For "anysym" symbols:			*/
    /* 1) Call free function if configured      */
    /* 2) If we malloced the data, free it      */
    /* ---------------------------------------- */
    else 
    {
	if (t->clean_fn)
	    t->clean_fn( l->data );
	
	if (l->data_is_malloc_mem)
	    XeFree(l->data);
    }
    
    next = l->rest;
    XeFree(l);
    return next;
}

/*------------------------------------------------------------------------+*/
XeSymTable
Xe_set_sym_fns(XeSymTable 	t,
	       XeSymFn_cmp	cmp_fn, 
	       XeSymFn_init	init_fn,
	       XeSymFn_clean	clean_fn, 
	       XeSymFn_hash	hash_fn)
/*------------------------------------------------------------------------+*/

{
    if (!t) t = Xe_default_symtab();

    t->cmp_fn    = cmp_fn;
    t->init_fn   = init_fn;
    t->clean_fn  = clean_fn;
    t->hash_fn   = hash_fn;
    return(t);
}

/*------------------------------------------------------------------------+*/
static XeSymbol 
make_sym(XeString name)
/*------------------------------------------------------------------------+*/
{
    XeSymbol sym = Xe_make_struct(_XeSymbol);
    
    sym->name  = strdup( name );
    sym->value = (void*)NULL;
    return sym;
}

/*------------------------------------------------------------------------+*/
static void *
intern_something(XeSymTable   t, 
		 void *       data, 
		 unsigned int size, 
		 Boolean      is_XeSymbol,
                 Boolean      lookup_only,
		 int	       *bucket)
/*------------------------------------------------------------------------+*/

{
   unsigned int hash;
   XeSymtabList l;
   XeSymtabList l0;
   Boolean	match;
   void *	hash_key;

   /* If no cmp function assume first item of "data" is a string pointer */
   /* ------------------------------------------------------------------ */
   if (is_XeSymbol)
       hash_key = data;
   else
       hash_key = (t->hash_fn) ? data : ((unknown_entry_data) data)->name;
   
   hash = keyhash( t, hash_key );
   l = t->list[hash];

   if (bucket)
       *bucket = hash;
   
   for (l0 = NULL; l; l0 = l, l = l->rest)
   {
       void *	cmp_key;
       void *	cmp_key2;

       if (is_XeSymbol)
	   cmp_key = data;
       else
	   cmp_key = (t->cmp_fn) ? data : ((unknown_entry_data) data)->name;

       if (l->data_is_XeSymbol)
	   cmp_key2 = ((XeSymbol) l->data)->name;
       else
	   cmp_key2 = (t->cmp_fn) ? l->data : ((unknown_entry_data) l->data)->name;

       /* Use the "compare" function to see if we have a match on our key */
       /* --------------------------------------------------------------- */
       if (t->cmp_fn)
	   match = (t->cmp_fn( cmp_key, cmp_key2 ) == 0);
       else
	   match = (strcmp((const char *) cmp_key, (const char *)cmp_key2 ) == 0);

       if (match)
	   return l->data;
   }

   /* If just doing a lookup, don't add a new symbol */
   /* ---------------------------------------------- */
   if (lookup_only) return (void *) NULL;
   
   /* There was no match.  We need to create an entry in the hash table. */
   /* ------------------------------------------------------------------ */
   l = (XeSymtabList) XeMalloc( sizeof(struct _XeSymtabList) );
   l->rest = (XeSymtabList)NULL;
   l->data_is_XeSymbol = is_XeSymbol;
   l->data_is_malloc_mem = FALSE;
   

   if (l0)
       l0->rest = l;
   else
       t->list[hash] = l;

   /* If we have a standard symbol, make the XeSymbol entry.   */
   /* -------------------------------------------------------- */
   if (is_XeSymbol)
   {
       XeSymbol sym = make_sym((XeString)data);
       l->data = (void*) sym;
       if (t->init_fn)
	 sym->value = t->init_fn( l->data, size /* will be 0 */ );
   }
   else
   {
       /*     1) If "size" != 0, 					     */
       /*        - malloc "size" bytes, 				     */
       /*        - copy "data" into malloced space,			     */
       /*        - Save pointer to malloc space as user's data pointer	     */
       /*        Else							     */
       /*        - Save "data" as pointer to user's data 		     */
       /*     2) If a "init_fn" is configured,				     */
       /*        - call init_fn( user's data pointer, "size" )	       	     */
       /*        - set user's data pointer to return value of init_fn        */
       /*          ONLY if "size" was zero.                                  */
       /*								     */
       /*	  If size is non zero AND there is a user's malloc function, */
       /*        beware that the return value from the malloc function is not*/
       /*        save anywhere by these routines.  If size was zero, the     */
       /*        return value of the user's function is kept.                */
       /* ------------------------------------------------------------------ */
       if (size)
       {
	   l->data = XeMalloc( size );
	   memcpy(l->data, data, size);
	   l->data_is_malloc_mem = TRUE;
       }
       else
	   l->data = data;

       if (t->init_fn)
       {
	   void * new_data = t->init_fn( l->data, size );

	   if (!size)
	       l->data = new_data;
       }
       
   }

   
   /* appended to the end of the hash chain (if any).                 */
   /* --------------------------------------------------------------- */
   
   t->curr_list = l;
   t->curr_hash = hash;

#ifdef DEBUG
   printf("Added data %p in list[%d] @ %p\n", l->data, hash, l);
#endif

   return l->data;
}

/*------------------------------------------------------------------------+*/
XeSymbol 
Xe_intern(XeSymTable t, ConstXeString const name)
/*------------------------------------------------------------------------+*/
{
    if (!name) return (XeSymbol)NULL;

    if (!t) t = Xe_default_symtab();

    return (XeSymbol)intern_something(t, (void *)name, 0, TRUE, FALSE, (int*)NULL);
}

/*------------------------------------------------------------------------+*/
XeSymbol 
Xe_lookup(XeSymTable t, ConstXeString const name)
/*------------------------------------------------------------------------+*/
{
    if (!name) return (XeSymbol)NULL;

    if (!t) t = Xe_default_symtab();

    return (XeSymbol)intern_something(t, (void *)name, 0, TRUE, TRUE, (int*)NULL);
}

/******************************************************************************/
/*  LISTS  								      */

/*------------------------------------------------------------------------+*/
XeList 
Xe_make_list(void * data, XeList rest)
/*------------------------------------------------------------------------+*/
{
   XeList            temp = Xe_make_struct(_XeList);

   temp->data = data;
   temp->rest = rest;
   return temp;
}


/******************************************************************************/
/* QUEUES 								      */

/*------------------------------------------------------------------------+*/
XeQueue 
Xe_init_queue(XeQueue q, void * nullval)
/*------------------------------------------------------------------------+*/
{
   q->head = 0;
   q->null = nullval;
   return q;
}

/*------------------------------------------------------------------------+*/
XeQueue 
Xe_make_queue(void * nullval)
/*------------------------------------------------------------------------+*/
{
   return Xe_init_queue(Xe_make_struct(_XeQueue), nullval);
}

/*------------------------------------------------------------------------+*/
void *
Xe_pop_queue(XeQueue q)
/*------------------------------------------------------------------------+*/
{
   XeList            head = q->head;

   if (head) {
      void *         val = head->data;

      q->head = head->rest;
      XeFree(head);
      return val;
   } else
      return q->null;
}

/*------------------------------------------------------------------------+*/
void *
Xe_delete_queue_element(XeQueue q, void * val)
/*------------------------------------------------------------------------+*/
{
   XeList            last = 0, head = q->head;

   while (head)
      if (head->data == val) {
	 if (last)
	    last->rest = head->rest;
	 else
	    q->head = head->rest;
	 if (q->tail == head)
	    q->tail = last;
	 XeFree(head);
	 return val;
      } else
	 last = head, head = head->rest;
   return q->null;
}

/*------------------------------------------------------------------------+*/
void 
Xe_push_queue(XeQueue q, void * val)
/*------------------------------------------------------------------------+*/
{
  XeList new_ptr = Xe_make_list(val, 0);
  
  if (q->head)
    q->tail->rest = new_ptr;
  else
    q->head = new_ptr;
  q->tail = new_ptr;
}

/*------------------------------------------------------------------------+*/
void 
Xe_release_queue(XeQueue q)
/*------------------------------------------------------------------------+*/
{
   if (q) {
      while (q->head)
	 Xe_pop_queue(q);
      XeFree(q);
   }
}
