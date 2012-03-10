
/*
 *	$XConsortium: MwmUtil.h /main/4 1995/11/06 18:49:29 rswiston $
 *
 * @(#)MwmUtil.h	1.4 01 Feb 1994	cde_app_builder/src/libAButil
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

#ifndef _AB_MWMUTIL_H_
#define _AB_MWMUTIL_H_
/*
 * MwmUtil.h - include X library header files, while adding necessary
 *          defines for app builder programs.
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <Xm/MwmUtil.h>

/*
 * Make sure we don't redefine types defined in standard X include files
 */
#define _AB_BOOL_DEFINED_
#define _AB_BYTE_DEFINED_

#endif /* _AB_MWMUTIL_H_ */
