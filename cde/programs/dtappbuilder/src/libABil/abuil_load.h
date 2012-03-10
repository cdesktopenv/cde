/*
 *	$XConsortium: abuil_load.h /main/3 1995/11/06 18:20:24 rswiston $
 *
 * %W% %G%	cose/unity1/cde_app_builder/src/libABil
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
#ifndef _ABIL_LOAD_UIL_H
#define _ABIL_LOAD_UIL_H

/*
 * This file declares the API of the uil load component
 */
#include <ab_private/obj.h>

extern ABObjPtr		abuil_get_uil_file(
			    STRING	uil_file_path,
			    ABObjPtr	ab_project
			);

extern void		abuil_trav_uil_tree(
			    void	*uil_root
			);

#endif	/* _ABIL_LOAD_UIL_H */
