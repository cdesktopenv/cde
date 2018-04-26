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
 * File:         scoop.h $XConsortium: scoop.h /main/3 1995/10/26 15:48:17 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _scoop_h
#define _scoop_h

#ifndef NULL
#define	NULL	0L
#endif

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif		/* TRUE */

#if !defined(__sun)
# if (defined(__STDC__) || defined(__STDCPP__) || defined(__cplusplus) || defined(c_plusplus))
#  define CAT(a,b) a##b
# else
#  define CAT(a,b) a/**/b
# endif
#else
  /* Sun C++ compiler uses the non-ansi cpp. */
# if defined(__cplusplus)
#  define CAT(a,b) a/**/b
# else
#  define CAT(a,b) a##b
# endif
#endif  


typedef long OSizeType ;

typedef struct root_class  *root_clasp, *object_clasp ;

typedef struct object_struct object;

typedef void (*SC_class_init)(object_clasp);
typedef void (*SC_object_init)(object *);
typedef object * (*SC_new_obj)(object_clasp);
typedef void (*SC_free_obj)(object *);
typedef object * (*SC_clone)(object *, object *);

#define root_class_part \
  XeString       name ;          /* class name */                        \
  SC_class_init  class_init;     /* fn to init my class struct */        \
  OSizeType      object_size ;   /* size of object of this class */      \
  short          init ;          /* boolean--class initialized state */  \
  SC_object_init object_init;    /* fn to init my object struct */       \
  SC_new_obj     new_obj;        /* fn to allocate class objects */      \
  SC_free_obj    free_obj;       /* fn to free class objects */          \
  SC_clone       clone;          /* `method' to clone an object */       \
                                 /* no root private parts */

#define root_object_part

struct object_struct
{ root_clasp class_ptr;
  root_object_part
  };

struct root_class
  { object_clasp base ;		/* pointer to base class */
    root_class_part } ;

#define memf(vref, fn, arglist)		( (vref class_ptr)->fn arglist)
#define memvf0(v,fn)			memf(v., fn, (&(v)))
#define memvf1(v,fn,a1)			memf(v., fn, (&(v),a1))
#define memvf2(v,fn,a1,a2)		memf(v., fn, (&(v),a1,a2))
#define memvf3(v,fn,a1,a2,a3)		memf(v., fn, (&(v),a1,a2,a3))
#define memvf4(v,fn,a1,a2,a3,a4)	memf(v., fn, (&(v),a1,a2,a3,a4))
#define memvf5(v,fn,a1,a2,a3,a4,a5)	memf(v., fn, (&(v),a1,a2,a3,a4,a5))
#define memvf6(v,fn,a1,a2,a3,a4,a5,a6)	memf(v., fn, (&(v),a1,a2,a3,a4,a5,a6))
#define memvf7(v,fn,a1,a2,a3,a4,a5,a6,a7) \
	memf(v., fn, (&(v),a1,a2,a3,a4,a5,a6,a7))
#define memvf8(v,fn,a1,a2,a3,a4,a5,a6,a7,a8) \
	memf(v., fn, (&(v),a1,a2,a3,a4,a5,a6,a7,a8))
#define memvf9(v,fn,a1,a2,a3,a4,a5,a6,a7,a8,a9) \
	memf(v., fn, (&(v),a1,a2,a3,a4,a5,a6,a7,a8,a9))
#define mempf0(p,fn)			memf(p->, fn, (p))
#define mempf1(p,fn,a1)			memf(p->, fn, (p,a1))
#define mempf2(p,fn,a1,a2)		memf(p->, fn, (p,a1,a2))
#define mempf3(p,fn,a1,a2,a3)		memf(p->, fn, (p,a1,a2,a3))
#define mempf4(p,fn,a1,a2,a3,a4)	memf(p->, fn, (p,a1,a2,a3,a4))
#define mempf5(p,fn,a1,a2,a3,a4,a5)	memf(p->, fn, (p,a1,a2,a3,a4,a5))
#define mempf6(p,fn,a1,a2,a3,a4,a5,a6)	memf(p->, fn, (p,a1,a2,a3,a4,a5,a6))
#define mempf7(p,fn,a1,a2,a3,a4,a5,a6,a7) \
	memf(p->, fn, (p,a1,a2,a3,a4,a5,a6,a7))
#define mempf8(p,fn,a1,a2,a3,a4,a5,a6,a7,a8) \
	memf(p->, fn, (p,a1,a2,a3,a4,a5,a6,a7,a8))
#define mempf9(p,fn,a1,a2,a3,a4,a5,a6,a7,a8,a9) \
	memf(p->, fn, (p,a1,a2,a3,a4,a5,a6,a7,a8,a9))
  
#define memd(vref, dm)			(vref dm)
#define base_memf(my_class, fn, arglist)	\
    ((*(my_class->base->fn)) arglist)

#define memfp(vref, fn, arglist)	memf (vref, CLASS.fn, arglist)
#define memdp(vref, dm)			memd (vref, CLASS.dm)

#define memvs(v, sdm)			(*((v.class_ptr)->sdm))
#define memps(p, sdm)			(*((p->class_ptr)->sdm))
#define stat_def(name, type)		type *name; type CAT(name,_static) ;
#define stat_init(def_class, class_ptr, name) \
	(class_ptr->name) = &(def_class->CAT(name,_static))

extern object_clasp root_class ;

extern void object_destroy 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (object *p);
#else
   ();
#endif

extern object *object_create 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (object_clasp c);
#else
   ();
#endif

extern void object_init 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (object_clasp c,  object *p);
#else
   ();
#endif
    
#endif /* _scoop_h */
/* PLACE NOTHING AFTER THIS endif */
