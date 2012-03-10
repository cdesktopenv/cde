/*
 *+SNOTICE
 *
 *	$XConsortium: Common.h /main/4 1996/04/21 19:44:30 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
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
 *+ENOTICE
 */

#ifndef _COMMON_H
#define _COMMON_H

#include <unistd.h>
#include <nl_types.h>

// XPG3 compatible. NL_CAT_LOCALE is set to non-zero in XPG4. Use NL_CAT_LOCALE
// for all catopen() calls.
#if defined(sun) && (_XOPEN_VERSION == 3) 
#undef NL_CAT_LOCALE
#define NL_CAT_LOCALE 0

// If NL_CAT_LOCALE is not defined in other platforms, set it to 0
#elif !defined(NL_CAT_LOCALE)	
#define NL_CAT_LOCALE 0
#endif

#endif   // _COMMON_H
