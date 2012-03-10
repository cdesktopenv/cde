/*
 *	$XConsortium: obj_utils.h /main/3 1995/11/06 18:40:03 rswiston $
 *
 * %W% %G%	cde_app_builder/src/libABobj
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

/*
 * utils.h - PRIVATE UTILS FOR LIBAB_OBJ
 */

#ifndef _AB_OBJ_UTILS_
#define _AB_OBJ_UTILS_

#include <ab/util_types.h>

#define str_safe(str)	((str) == NULL? abo_null_string : (str))

extern char *abo_empty_string;
extern char *abo_null_string;

#define nullstr(s) ((s) == NULL? "<<NULL>>":(s))

#endif /* _AB_OBJ_UTILS_ */
