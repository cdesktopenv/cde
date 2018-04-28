/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/*
 *	$XConsortium: travP.h /main/3 1995/11/06 18:42:14 rswiston $
 *
 *	@(#)travP.h	1.9 27 Apr 1994	
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
 * TraversalP.h - private functions to be used by Traversal.c ONLY!
 */
#ifndef _ABOBJ_TRAVERSALP_H_
#define _ABOBJ_TRAVERSALP_H_

#include <ab_private/trav.h>

#define TRAV_TYPE_MASK 		((unsigned)0xff)
#define TRAV_MODIFIER_MASK	((unsigned)(~(TRAV_TYPE_MASK)))

#define travP_get_qualifier(trav) \
		((unsigned)((trav)->travType & TRAV_TYPE_MASK))

#define travP_is_safe(trav) ((((trav)->travType) & AB_TRAV_MOD_SAFE) != 0)

#define travP_is_parents_first(trav) \
		((((trav)->travType) & AB_TRAV_MOD_PARENTS_FIRST) != 0)

#define travP_clean(trav) \
	((trav)->travType= 0, \
	(trav)->rootObj= NULL, \
	(trav)->curObj= NULL, \
	(trav)->objArray= NULL, \
	(trav)->objArraySize= 0, \
	(trav)->objArrayIndex= -1, \
	(trav)->done= FALSE, \
	(trav)->testFunc= NULL \
	)

/*
 * "Normal" traversals
 */
int	travP_open(
			ABTraversal 	trav, 
			ABObj 		root, 
			unsigned 	travType,
			ABObjTestFunc	test_func
	);
int	travP_close(ABTraversal trav);
ABObj	travP_next(ABTraversal trav);
int	travP_reset(ABTraversal trav);
ABObj	travP_goto(ABTraversal trav, int nodeNum);

/*
 * Safe traversals
 */
int	travP_open_safe(
			ABTraversal	trav, 
			ABObj		root, 
			unsigned	travType,
			ABObjTestFunc	testFunc
	);
int	travP_close_safe(ABTraversal trav);
ABObj	travP_next_safe(ABTraversal trav);
int	travP_reset_safe(ABTraversal trav);
ABObj	travP_goto_safe(ABTraversal trav, int nodeNum);

/*
 * Utility functions
 */
ABObj	travP_obj_parent(ABObj obj, ABTraversal trav);
ABObj	travP_obj_first_child(ABObj obj, ABTraversal trav);
ABObj	travP_obj_next_sibling(ABObj obj, ABTraversal trav);
ABObj	travP_obj_prev_sibling(ABObj obj, ABTraversal trav);

#endif /* _ABOBJ_TRAVERSALP_H_ */

