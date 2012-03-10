/*
 * File:         MemoryMgr.h $XConsortium: MemoryMgr.h /main/3 1995/10/26 15:45:08 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

/* Allocation routines */

#ifndef  _MemoryMgr_h
#define  _MemoryMgr_h

void *XeMalloc   
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (size_t size);
#else
   ();
#endif

void XeFree   
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (void * ptr);
#else
   ();
#endif

#define Xe_make_struct(type)     ((struct type *)XeMalloc(sizeof(struct type)))
#define Xe_make_ntype(n, type)   ((type *)       XeMalloc((n)*sizeof(type)))
#define Xe_make_buffer(size)     ((XeString)     XeMalloc(size))
#define Xe_make_str(size)        ((XeString)     XeMalloc((size)+1))
   
#define XeCopyStringM(string_xxx) (string_xxx				\
 ? (XeString) strcpy(Xe_make_str(strlen(string_xxx)), string_xxx)	\
 : (XeString) NULL)


/* Dellocation */       

#define Xe_release_ntype(ptr, n, type)     XeFree(ptr)
#define Xe_release_str(ptr)                XeFree(ptr)

#endif  /* _MemoryMgr_h */
/* PLACE NOTHING AFTER THIS endif */

