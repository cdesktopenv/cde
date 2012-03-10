/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: dbck.h /main/3 1995/10/20 16:25:24 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * dbck.h
 *
 * standard/global defines common to all inspect and repair tools
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef _DBCK_H
#define _DBCK_H
#include "options.h"

extern char *progname;		// from argv[0]
extern FILE *tstream;		// for calling print() methods in dbx

extern int do_directories(const _Tt_string_list_ptr &dirs,
			  int (*fn)(_Tt_string));

#if !defined(_TT_NODEBUG)
#define DBCK_DEBUG(n) (opts->debug_level()>=(n))
#else
#define DBCK_DEBUG(n) 0
#endif

#endif /* _DBCK_H */
