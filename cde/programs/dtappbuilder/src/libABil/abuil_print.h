/*
 *	$XConsortium: abuil_print.h /main/3 1995/11/06 18:21:09 rswiston $
 *
 * @(#)abuil_print.h	1.9 25 Aug 1994	cose/unity1/cde_app_builder/src/libABil
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
#ifndef _ABIL_PRINT_UIL_H
#define _ABIL_PRINT_UIL_H

/*
 * This file declares the API for the print uil component
 */
#include <ab_private/obj.h>

extern void		abuil_obj_print_uil(
			    FILE	*fp,
			    ABObjPtr	ab_project
			);

#endif	/* _ABIL_PRINT_UIL_H */
