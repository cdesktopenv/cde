
/*
 *	$XConsortium: obj_names_listP.h /main/3 1995/11/06 18:37:17 rswiston $
 *
 *	@(#)obj_names_listP.h	1.1 26 May 1994	
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
 * obj_names_listP.h - handles lists of object names
 */
#ifndef _ABOBJ_NAMES_LISTP_H_
#define _ABOBJ_NAMES_LISTP_H_

#include "objP.h"

ABObj		objP_get_names_scope_obj(ABObj obj);
StringList	objP_get_names_scope(ABObj obj);
ABObj		objP_get_names_scope_obj_for_children(ABObj obj);
StringList	objP_get_names_scope_for_children(ABObj obj);
StringList	objP_get_names_list(ABObj obj);
int		objP_remove_from_names_list(ABObj obj);
int		objP_add_to_names_list(ABObj obj);
int		objP_tree_remove_from_names_list(ABObj obj);
int		objP_tree_add_to_names_list(ABObj obj);

#endif /* _ABOBJ_NAMES_LISTP_H_ */

