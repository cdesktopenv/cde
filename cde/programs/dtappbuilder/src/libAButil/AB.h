
/*
 *	$XConsortium: AB.h /main/4 1995/11/06 18:48:43 rswiston $
 *
 * @(#)AB.h	1.1 11 Feb 1994	cde_app_builder/src/libAButil
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

#ifndef _ABUTIL_AB_H_
#define _ABUTIL_AB_H_
/*
 * <ab_private/AB.h>
 *
 * This file should be included by ALL app builder source files!
 * And it should be included before any other ab include files 
 * ( <ab_private/x.h>, <ab/x.h>, "x.h" ).
 *
 * It sets DEBUG parameters, and affects the entire product
 */


/*
 * Turn on POSIX compliance.  This mostly affect system header files.
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif


/*
 * Set the DEBUG compile-time parameter to be exactly the opposite of
 * NDEBUG
 */
#ifndef NDEBUG
    #ifndef DEBUG
        #define DEBUG 1
    #endif
#else
    #undef DEBUG
#endif


#endif /* _ABUTIL_AB_H_ */
