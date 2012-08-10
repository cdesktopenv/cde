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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/*
 *	$XConsortium: obj_list.h /main/3 1995/11/06 18:36:04 rswiston $
 *
 * @(#)template.h	1.6 11 Feb 1994	cde_app_builder/src/libAButil
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

#ifndef _ABOBJ_OBJ_LIST_H_
#define _ABOBJ_OBJ_LIST_H_

/*
 * obj_list.h - define ABObjList data type
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <ab_private/AB.h>	/* everybody must include this first! */
#include <ab/util_types.h>

struct _AB_OBJ;
#define ABObj struct _AB_OBJ *

typedef enum
{
    OBJLIST_SORT_UNDEF = 0,
    OBJLIST_SORT_ALPHANUMERIC,
    OBJLIST_SORT_BEST,
    OBJLIST_SORT_CLIENT_DEF,
    OBJLIST_SORT_ORDER_NUM_VALUES	/* must be last */
} OBJLIST_SORT_ORDER;

typedef void (*ABObjListIterFn)(ABObj obj);

/*
 * obj list structure.
 */
typedef struct
{
    int                 num_objs;
    int			objs_size;
    ABObj		*objs;
    void		**user_datas;
    BOOL		unique;
    OBJLIST_SORT_ORDER	sort_order;
    BOOL		indexes_dirty;
} ABObjListRec, *ABObjList;

#undef ABObj
#include <ab_private/obj.h>

/*
 * ABObjList - lifecycle methods
 */
ABObjList	objlist_create(void);
int		objlist_destroy(ABObjList);
int		objlist_construct(ABObjList);
int		objlist_destruct(ABObjList);
ABObjList	objlist_dup(ABObjList);

/*
 * ABObjList - methods dealing with entire list
 */
int		objlist_set_sort_order(ABObjList, OBJLIST_SORT_ORDER);
OBJLIST_SORT_ORDER	objlist_get_sort_order(ABObjList);
int		objlist_set_is_unique(ABObjList, BOOL unique);
BOOL		objlist_is_unique(ABObjList);
int		objlist_make_empty(ABObjList);
int		objlist_is_empty(ABObjList);
int		objlist_set_num_objs(ABObjList, int numABObjs);
int		objlist_get_num_objs(ABObjList);
int		objlist_set_max_size_soft(ABObjList, int maxNumObjs);
int		objlist_get_max_size(ABObjList);
BOOL		objlist_max_size_is_soft(ABObjList);
int		objlist_shrink_mem_to_fit(ABObjList);
int             objlist_iterate(ABObjList, ABObjListIterFn);

/*
 * Methods dealing with individual objects
 */
int		objlist_remove_obj(ABObjList, ABObj obj);
int		objlist_remove_index(ABObjList, int index);
int		objlist_add_obj(ABObjList, ABObj obj, void *userData);
int		objlist_add_index(ABObjList, 
				int index, ABObj obj, void *userData);
BOOL		objlist_obj_exists(ABObjList list, ABObj obj);
int		objlist_get_obj_index(ABObjList list, ABObj obj);
ABObj		objlist_get_obj(ABObjList, int whichObj, void **userDataOut);
void		*objlist_get_obj_data(ABObjList, ABObj obj);

/*
 * Debugging methods
 */
#ifdef DEBUG
int		objlist_dump(ABObjList list);
#endif


/*************************************************************************
**************************************************************************
**									**
**		Inline definitions					**
**									**
**************************************************************************
*************************************************************************/
int	objlist_destroy_impl(ABObjList *listInOut);
#define objlist_destroy(list) (objlist_destroy_impl(&(list)))

#endif /* _ABOBJ_OBJ_LIST_H_ */
