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
 * File:         Symbolic.h $XConsortium: Symbolic.h /main/3 1995/10/26 15:45:51 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

/*  SYMBOLS */

#ifndef _Symbolic_h_
#define _Symbolic_h_

/* -------------------------------------------- */
/* Requires:					*/
#ifdef __recursive_includes
#include <bms/bms.h>
#endif
/* -------------------------------------------- */

/****************************************************************************/
/*  Symbol (hash) Tables 						    */

/*

There are two types of symbol (hash) tables.  The common case is where the
key (index) is ascii string.  The Xe_intern() and XeRegisterSymbol()
routines assume this type of table.  In these cases an "XeSymbol" entry is
created in the symbol table (see type below). After it is created a COPY of
the string is made and stored in the "name" field.  The "value" field is
free for the user to fill in on his own.

*/

typedef struct _XeSymbol {
    XeString   name;
    void       *value;
} *XeSymbol;

/* 
This type is used for the XeRegisterFunction() routine.
*/

typedef void (*XeAnyFunction) ();


/*
The second type of table is where the user defines his own type of
data, hash functions, compare functions, etc.   
*/

/* The following types for the the user configurable functions that */
/* are set with the Xe_set_syms_fns() routine.                      */
/* ---------------------------------------------------------------- */


/* CMP_FN:  								*/
/*									*/
/*   Any user defined function should return "0" for a "match" 		*/
/*									*/
/*   Used for Xe_intern() and XeRegisterSymbol()			*/
/*									*/
/*   If NO "cmp_fn", strcmp("next_table_entry"->name, "name") 	        */
/*   If    "cmp_fn", cmp_fn("next_table_entry"->name, "name") 	        */
/*									*/
/*   Used for Xe_intern_anysym() 					*/
/*									*/
/*   If NO "cmp_fn", strcmp("next_table_entry"->name, "data"->name)     */
/*   If    "cmp_fn", cmp_fn("next_table_entry",       "data")           */
/* -------------------------------------------------------------------- */

#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   typedef int (*XeSymFn_cmp) (void *, void *);     /* 0 mean match */
#else
   typedef int (*XeSymFn_cmp) ();
#endif


/* HASH_FN:  								   */
/*									   */
/*   Note, the internal hash function in not user accesable.		   */
/*									   */
/*   Used for Xe_intern() and XeRegisterSymbol()			   */
/*									   */
/*   If NO "hash_fn", internal_hash("next_table_entry"->name, "name")      */
/*   If    "hash_fn",       hash_fn("next_table_entry"->name, "name")      */
/*									   */
/*   Used for Xe_intern_anysym() 					   */
/*									   */
/*   If NO "hash_fn", internal_hash("next_table_entry"->name, "data"->name)*/
/*   If    "hash_fn",       hash_fn("next_table_entry"      , "data")      */
/* ----------------------------------------------------------------------- */

#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   typedef unsigned int (*XeSymFn_hash)  (void *, unsigned int);
#else
   typedef unsigned int (*XeSymFn_hash)  ();
#endif

/* INIT_FN:  								*/
/*									*/
/*   Used for Xe_intern() and XeRegisterSymbol()			*/
/*									*/
/*     1) A XeSymbol entry is created,					*/
/*        - XeSymbol->name = strdup("name"),				*/
/*        - XeSymbol->value = NULL					*/
/*     2) If a "init_fn" is configured,  				*/
/*        -  XeSymbol->value = init_fn( XeSymbol, 0 )			*/
/*									*/
/*   Used for Xe_intern_anysym() 					*/
/*									*/
/*     1) If "size" != 0, 						*/
/*        - malloc "size" bytes, 					*/
/*        - copy "size" bytes from "data" into malloced space,		*/
/*        - Save pointer to malloc space as user's data pointer		*/
/*        Else								*/
/*        - Save "data" as pointer to user's data		       	*/
/*     3) If a "init_fn" is configured,					*/
/*        - call init_fn( user's data pointer, "size" )	       		*/
/*        - set user's data pointer to return value of init_fn        	*/
/*          ONLY if "size" was zero.                                    */
/*									*/
/*	  If size is non zero AND there is a user's malloc function,    */
/*        beware that the return value from the malloc function is not  */
/*        save anywhere by these routines.  If size was zero, the       */
/*        return value of the user's function is kept.                  */
/*									*/
/* -------------------------------------------------------------------- */

#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   typedef void * (*XeSymFn_init)(void *, unsigned int);
#else
   typedef void * (*XeSymFn_init)();
#endif

/* CLEAN_FN:  								*/
/*									*/
/*   If created by Xe_intern() or XeRegisterSymbol()			*/
/*									*/
/*     1) free(XeSymbol->name)						*/
/*     2) If a "clean_fn" is configured,   				*/
/*        - clean_fn(XeSymbol->value),					*/
/*        - the XeSymbol entry is deleted.				*/
/*									*/
/*   If created by Xe_intern_anysym() 					*/
/*									*/
/*     1) If a "clean_fn" is configured,					*/
/*        - init_fn( user's data pointer, "size" )			*/
/*     2) If "size" != 0 during at creation time,			*/
/*        - free( user's data pointer )					*/
/*									*/
/*     Take note of the what happens during creation with "size" is     */
/*     non-zero and there is a user's malloc function.  The result of   */
/*     the user's malloc function will be passed to this clean function */
/*     only if "size" was zero at creation time.  Otherwise, the memory */
/*     malloc'ed by this routine due to the non-zero size parameter is  */
/*     what is passed to the user's clean function.                     */
/* -------------------------------------------------------------------- */

#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   typedef void (*XeSymFn_clean)  (void *);
#else
   typedef void (*XeSymFn_clean)  ();
#endif
   
/* This is what an internal symbol table entry looks like: */
/* ------------------------------------------------------- */
typedef struct _XeSymtabList {
    struct _XeSymtabList *rest;
    void       		 *data;
    Boolean    		 data_is_XeSymbol;
    Boolean    		 data_is_malloc_mem;
} *XeSymtabList;


/* This is the master record for a symbol table */
/* -------------------------------------------- */

typedef struct _XeSymTable
{
    unsigned int   hashsize;    /* # of hash buckets.                   */
    XeSymtabList   *list;	/* Hash buckets.                        */

    XeSymtabList   curr_list;   /* Used to dump/traver the table.       */
    unsigned int   curr_hash;
    
    XeSymFn_cmp    cmp_fn;      /* Read above description with each     */
    XeSymFn_hash   hash_fn;     /* function typedef for more info.      */
    XeSymFn_init   init_fn;	
    XeSymFn_clean  clean_fn;	
} *XeSymTable;


/* Create a new hashtable. "hashsize must be power of 2 if using default */
/* hash function (no checking is done to ensure this).                   */
/* --------------------------------------------------------------------- */
XeSymTable Xe_new_symtab
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (unsigned int hashsize);
#else
   ();
#endif


/* Create the default symbol table.  If already created, just */
/* return the symtable pointer. 			      */
/* ---------------------------------------------------------- */
XeSymTable Xe_default_symtab
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (void);
#else
   ();
#endif


/* Configure user defined function for use with symtab routines.   */
/* NOTE: Be sure you understand the interaction of these functions */
/*       Read the decriptions with each of the typdef's above.  A  */
/*       null function pointer will overwrite any previously       */
/*       installed value.					   */
/* --------------------------------------------------------------- */
XeSymTable Xe_set_sym_fns
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeSymTable     t, 
    XeSymFn_cmp    cmp_fn, 
    XeSymFn_init   init_fn,
    XeSymFn_clean  clean_fn, 
    XeSymFn_hash   hash_fn);
#else
   ();
#endif


/* Find or create a symbol for "name".  Its "value" is set to NULL.  */
/* None of the user defined init/cmp, etc functions are honored by   */
/* this routine                                                      */
/* WARNING: Read the notes for the function typedef's above.         */
/* ----------------------------------------------------------------- */
XeSymbol Xe_intern 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeSymTable t, ConstXeString const name);
#else
   ();
#endif


/* Find the symbol for "name".  NULL is returned if not found.       */
/* None of the user defined init/cmp, etc functions are honored by   */
/* this routine                                                      */
/* WARNING: Read the notes for the function typedef's above.         */
/* ----------------------------------------------------------------- */
XeSymbol Xe_lookup
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeSymTable t, ConstXeString const name);
#else
   ();
#endif
 

/****************************************************************************/
/*  LISTS 								    */

typedef struct _XeList {   
   struct _XeList *rest;
   void 	  *data; 
} *XeList;

/* make a list from data and rest */
/* ------------------------------ */
XeList Xe_make_list 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (void *data, XeList rest);
#else
   ();
#endif


/****************************************************************************/
/* QUEUES 								    */

typedef struct _XeQueue {
  XeList head;
  XeList tail;
  void * null;      
} *XeQueue;

/* for static or auto struct queues */
/* -------------------------------- */
XeQueue Xe_init_queue 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeQueue q, void * nullval);
#else
   ();
#endif
	
XeQueue Xe_make_queue 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (void * nullval);
#else
   ();
#endif

void Xe_release_queue 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeQueue q);
#else
   ();
#endif

/* nullval returned if queue empty */
/* ------------------------------- */
void * Xe_pop_queue 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeQueue q);
#else
   ();
#endif

/* nullval returned if not found   */
/* ------------------------------- */
void * Xe_delete_queue_element 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeQueue q, void * val);
#else
   ();
#endif

void Xe_push_queue 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeQueue q, void * val);
#else
   ();
#endif

/* user declares his own "type var" */
/* -------------------------------- */
#define Xe_for_queue(type,var,q)	\
	XeList q_next, q_list;		\
	for(q_list = q->head;		\
	    q_list && (var = (type) q_list->data, q_next = q_list->rest , 1);\
	    q_list = q_next)


/*  PUT NOTHING AFTER THIS endif */
#endif /* _Symbolic_h_ */

