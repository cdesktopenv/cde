/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_new.h /main/3 1995/10/23 10:41:57 rswiston $ 			 				 */
/*-*-C++-*-
 * @(#)tt_new.h	1.13 @(#)
 * tt_new.h - defines the new and delete operators for all TT classes
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#if !defined(_TT_NEW_H)
#define _TT_NEW_H

#if !defined(__STDC__)
#include <malloc.h>
#include <sys/types.h>
#endif /* __STDC__ */
#include <stdlib.h>
#if defined(ultrix)
#include <rpc/types.h>
#define bool_t int
#endif
#if defined(sun)
#  define MALLOCTYPE	char
#else
#  define MALLOCTYPE	void
#endif


//
// All classes must at least inherit from this class in order to
// remove dependencies on libC-defined "new" and "delete".
//
class _Tt_allocated {
      public:
	void		*operator new(size_t s);
	void		operator  delete(void *p);
};

#endif
