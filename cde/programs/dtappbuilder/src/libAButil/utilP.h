
/*
 *	$XConsortium: utilP.h /main/4 1995/11/06 18:52:58 rswiston $
 *
 * @(#)utilP.h	1.7 01 Feb 1994	cde_app_builder/src/libAButil
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

#ifndef _ABUTIL_UTILP_H_
#define _ABUTIL_UTILP_H_
/*
 * utilP.h - private file for utility subsystem
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <assert.h>
#include <nl_types.h>
#include <ab_private/util.h>

/*
 * Convert mangled private identifiers to something more meaningful
 */
#define utilP_verbosity utilP_verbosityPriv3602759317

/* 
** Internationalization defines.  UTIL_MESSAGE_CATD must be defined to match
** the catalog variable declared and exported in wherever the main() routine lives
** (e.g. dtbuilder, a code generator).  UTIL_MESSAGE_SET must match what's
** defined in libAButil.msg
*/
#define UTIL_MESSAGE_CATD	Dtb_project_catd 
#define UTIL_MESSAGE_SET	202
extern  nl_catd			UTIL_MESSAGE_CATD;

#endif /* _ABUTIL_UTILP_H_ */
