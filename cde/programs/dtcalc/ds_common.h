/* $XConsortium: ds_common.h /main/3 1995/11/01 12:39:51 rswiston $ */
/*                                                                      *
 *  ds_common.h                                                         *
 *   Contains the NO_PROTO defines for functions is ds_common.h.        *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _ds_common_h
#define _ds_common_h

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Intrinsic.h>

/* For all function declarations, if ANSI then use a prototype. */

#if  defined(__STDC__)
#define P(args)  args
#else  /* ! __STDC__ */
#define P(args)  ()
#endif  /* STDC */

char *set_bool          P((int)) ;

void read_str           P((char **, char *)) ;

#endif /*_ds_common_h*/
