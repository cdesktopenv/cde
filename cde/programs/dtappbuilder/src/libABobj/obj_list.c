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
 *	$XConsortium: obj_list.c /main/4 1996/10/02 16:09:05 drk $
 *
 * @(#)obj_list.c	1.8 03 Feb 1995	cde_app_builder/src/libAButil
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not dobjibute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 * File: objlist.c
 */

#ifdef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ab_private/obj_list.h>

/*****************************************************************
 **								**
 **		ABObjList					**
 **								**
 *****************************************************************/

static int objlistP_grow_array(ABObjList list, int sizeDiff);
static int objlistP_shrink_array(ABObjList list, int sizeDiff);
static int objlistP_build_user_data_array(
			ABObjList list, 
			int index, 
			void *data
	    );


/*
#define objlistP_add_user_data(_list,_index,_data) \
    ((((_list)->user_datas == NULL) && ((_data) != NULL))? \
        objlistP_build_user_data_array(_list, _index, _data) \
    : \
        ((long)((_list)->user_datas[(_index)] = (_data))) \
    )
*/
	
#define objlistP_add_user_data(_list,_index,_data) \
    (((_list)->user_datas == NULL)? \
	(((_data) == NULL)? \
	    0 \
	: \
            objlistP_build_user_data_array(_list, _index, _data)) \
    : \
        ((long)((_list)->user_datas[(_index)] = (_data))) \
    )
	
#define objlistP_get_user_data(_list,_index) \
	    (((_list)->user_datas) == NULL? \
		NULL:((_list)->user_datas[(_index)]))

ABObjList
objlist_create(void)
{
    ABObjList          list = (ABObjList) util_malloc(sizeof(ABObjListRec));
    if (list == NULL)
    {
	return NULL;
    }
    objlist_construct(list);
    return list;
}


int
objlist_construct(ABObjList list)
{
    list->num_objs = 0;
    list->objs_size = 0;
    list->objs = NULL;
    list->user_datas = NULL;
    list->unique = TRUE;
    list->sort_order = OBJLIST_SORT_CLIENT_DEF;
    list->indexes_dirty = TRUE;
    return 0;
}


/*
 * Frees the obj list and all it's associated memory.
 */
int
objlist_destroy_impl(ABObjList *listInOut)
{
    ABObjList	list = (*listInOut);
    if (list != NULL)
    {
        objlist_destruct(list);
        util_free(list);
    }
    (*listInOut) = list;
    return 0;
}


int
objlist_destruct(ABObjList list)
{
    return objlist_make_empty(list);
}


OBJLIST_SORT_ORDER
objlist_get_sort_order(ABObjList list)
{
    return list->sort_order;
}


int
objlist_set_sort_order(ABObjList list, OBJLIST_SORT_ORDER new_order)
{
    int		return_value = 0;

    if (list->sort_order != new_order)
    {
	list->indexes_dirty = TRUE;
    }

    return 0;
}


int
objlist_make_empty(ABObjList list)
{
    int		return_value = 0;

    if (list->objs != NULL)
    {
	return_value = objlistP_shrink_array(list, list->objs_size);
    }

    return return_value;
}


int
objlist_is_empty(ABObjList list)
{
    return (list->num_objs < 1);
}


int
objlist_set_is_unique(ABObjList list, BOOL unique)
{
    list->unique = unique;
    return 0;
}


BOOL
objlist_get_is_unique(ABObjList list)
{
    return list->unique;
}


/*
 * Searches the list for a match.
 * 
 * returns:	true, if the obj exists. false, if it isn't in the lisT
 */
BOOL
objlist_obj_exists(ABObjList list, ABObj obj)
{
    return (objlist_get_obj_index(list, obj) >= 0);
}


void *
objlist_get_obj_data(ABObjList list, ABObj obj)
{
    int		index = objlist_get_obj_index(list, obj);
    return objlistP_get_user_data(list, index);
}


/*
 * Returns the index of the given obj, or -1 if it doesn't exist
 */
int
objlist_get_obj_index(ABObjList list, ABObj obj)
{
    int			index = -1;
    int                 i = 0;
    int                 num_objs = list->num_objs;

    for (i = 0; i < num_objs; ++i)
    {
	if (list->objs[i] == obj)
	{
	    index = i;
	    break;
	}
    }

    return index;
}


/*
 * Add an obj
 * 
 * returns:	0 if item added successfully 
 *		ERR_DUPLICATE_KEY if item is already in list
 */
int
objlist_add_obj(ABObjList list, ABObj obj, void *clientData)
{
    int			return_value = 0;
    int			rc = 0;

    /*
     * look for it
     */
    if (list->unique)
    {
	long index = objlist_get_obj_index(list, obj);
	if (index >= 0)
	{
	    objlistP_add_user_data(list, index, clientData);
	    return 0;
	}
    }

    /*
     * Allocate more space
     */
    if (list->num_objs >= list->objs_size)
    {
	/* grow the array by 50% (the "opposite" of a binary search) */
	int	new_size = (list->objs_size * 3)/2;
	int	size_inc = new_size - list->objs_size;
	size_inc = util_max(5,size_inc);
        rc = objlistP_grow_array(list, size_inc);
	if (rc < 0)
	{
	    return_value = rc;
	    goto epilogue;
	}
    }


    /*
     * save the obj (and data)
     */
    list->objs[list->num_objs] = obj;
    objlistP_add_user_data(list, list->num_objs, clientData);
    ++(list->num_objs);

epilogue:
    return return_value;
}


int
objlist_remove_obj(ABObjList list, ABObj obj)
{
    int		index = objlist_get_obj_index(list, obj);
    if (index < 0)
    {
	return 0;
    }
    return objlist_remove_index(list, index);
}


int
objlist_remove_index(ABObjList list, int doomedIndex)
{
#define num_objs (list->num_objs)
    ABObj	*objs = list->objs;
    void	**user_datas = list->user_datas;
    int		index = 0;
    int		numToMove = 0;

    if ((doomedIndex < 0) || (doomedIndex >= num_objs))
    {
	return -1;
    }

    numToMove = (num_objs - doomedIndex - 1);

    /*
     * Remove this obj and collapse the list
     */
    objs[doomedIndex] = NULL;
    if (numToMove > 0)
    {
        memmove((void *)(&(objs[doomedIndex])),
		    (void *)&(objs[doomedIndex+1]),
		    sizeof(ABObj) * numToMove);
        objs[num_objs-1] = NULL;
    }

    /*
     * Destroy user data and collapse list
     */
    if (user_datas != NULL)
    {
        user_datas[doomedIndex] = NULL;
        if (numToMove > 0)
        {
            memmove((void *)(&(user_datas[doomedIndex])), 
		    (void *)&(user_datas[doomedIndex+1]),
		    sizeof(void*) * numToMove);
            user_datas[num_objs-1] = NULL;
        }
    }

    /*
     * Actually shorten list
     */
    --num_objs;

    return 0;
#undef num_objs
}


int
objlist_get_num_objs(ABObjList list)
{
    if (list == NULL)
    {
	return 0;
    }
    return list->num_objs;
}


ABObj
objlist_get_obj(ABObjList list, int whichObj, void **clientDataOut)
{
    if ((list == NULL) || (whichObj >= list->num_objs) ||
	(whichObj < 0))
    {
	return NULL;
    }
    if (clientDataOut != NULL)
    {
	*clientDataOut = objlistP_get_user_data(list, whichObj);
    }
    return list->objs[whichObj];
}


/*
 * Only "soft" limit is supported, now
 */
BOOL
objlist_max_size_is_soft(ABObjList list)
{
    list = list;		/* avoid cc warning */
    return TRUE;
}


int
objlist_set_max_size_soft(ABObjList list, int maxSize)
{
    int		return_value = 0;
    int		sizeDiff = (maxSize - list->objs_size);

    if (sizeDiff < 0)
    {
	return_value = objlistP_shrink_array(list, -1 * sizeDiff);
    }
    else if (sizeDiff > 0)
    {
	return_value = objlistP_grow_array(list, sizeDiff);
    }

    return return_value;
}


int
objlist_get_max_size(ABObjList list)
{
    return list->objs_size;
}


/*
 * Frees any extra memory that may not be needed (multiple obj ptrs are
 * allocated at once, to reduce the number of realloc calls).
 */
int
objlist_shrink_mem_to_fit(ABObjList list)
{
    int		return_value = 0;
    int		diff = list->objs_size - list->num_objs;

    if (diff > 0)
    {
	return_value = objlistP_shrink_array(list, diff);
    }

    return return_value;
}


int
objlist_iterate(
    ABObjList       list,
    ABObjListIterFn fn
)
{
    int         i;

    if (!list)
	return -1;

    for (i=0; i<list->num_objs; i++)
    {
	(*fn)(list->objs[i]);
    }
    return 0;
}


ABObjList
objlist_dup(ABObjList list)
{
    int		i;
    ABObjList	new_list = NULL;

    new_list = objlist_create();
    new_list->unique = list->unique;
    new_list->sort_order = list->sort_order;
    new_list->indexes_dirty = list->indexes_dirty;
    for (i=0; i<list->num_objs; i++)
    {
	objlist_add_obj(
	    new_list, list->objs[i], objlistP_get_user_data(list,i));
    }
    return(new_list);
}

/*************************************************************************
**									**
**		PRIVATE FUNCTIONS					**
**									**
**************************************************************************/


objlistP_shrink_array(ABObjList list, int sizeDiff)
{
    int		return_value = 0;
    int		new_objs_size = util_max(0, list->objs_size - sizeDiff);
    ABObj	*new_objs = NULL;
    void	**new_user_datas = NULL;
    int		i = 0;
	
    for (i= new_objs_size; i < list->objs_size; ++i)
    {
	list->objs[i] = NULL;
	if (list->user_datas != NULL)
	{
	    list->user_datas[i] = NULL;
	}
    }

    if (new_objs_size > 0)
    {
        new_objs = (ABObj*) realloc(
			  list->objs, new_objs_size * sizeof(ABObj));
	if (new_user_datas != NULL)
	{
	    new_user_datas = (void **) realloc(
		  list->user_datas, new_objs_size * sizeof(void*));
	}
    }
    else
    {
	util_free(list->objs); list->objs = NULL;
	util_free(list->user_datas); list->user_datas= NULL;
    }

    if (   (new_objs_size > 0) 
	&& ((new_objs == NULL) || (new_user_datas == NULL)) )
    {
	return_value = ERR_NO_MEMORY;
	goto epilogue;
    }
    else
    {
	list->objs = new_objs;
	list->user_datas = new_user_datas;
	list->objs_size = new_objs_size;
	if (list->num_objs > list->objs_size)
	{
	    list->num_objs = util_max(0, list->objs_size);
	}
    }

epilogue:
    return return_value;
}


static int
objlistP_grow_array(ABObjList list, int sizeDiff)
{
    int		return_value = 0;
    int		old_objs_size = list->objs_size;
    int		new_objs_size = list->objs_size + sizeDiff;
    ABObj	*new_objs = NULL;
    void	**new_user_datas = NULL;
    int		i = 0;
    BOOL	user_datas_valid = (list->user_datas != NULL);

    new_objs = (ABObj*)realloc(
			list->objs, new_objs_size * sizeof(ABObj));
    if (user_datas_valid)
    {
        new_user_datas = (void **)realloc(
			list->user_datas, new_objs_size * sizeof(void *));
    }
    if (   (new_objs == NULL) 
	|| (user_datas_valid && (new_user_datas == NULL)) )
    {
	return_value = ERR_NO_MEMORY;
	goto epilogue;
    }
    else
    {
	list->objs = new_objs;
	list->user_datas = new_user_datas;
	list->objs_size = new_objs_size;
    }

    /*
     * Init the new entries to NULL;
     */
    for (i = old_objs_size; i < new_objs_size; ++i)
    {
	list->objs[i] = NULL;
	if (user_datas_valid)
	{
	    list->user_datas[i] = NULL;
	}
    }

epilogue:
    return return_value;
}


static int
objlistP_build_user_data_array(ABObjList list, int index, void *data)
{
    if (list->user_datas != NULL)
    {
	return -1;
    }
    list->user_datas = (void **)calloc(sizeof(void*), list->objs_size);
    if (list->user_datas == NULL)
    {
	return -1;
    }
    list->user_datas[index] = data;
    return 0;
}


#ifdef DEBUG
/*************************************************************************
 **									**
 **									**
 **		DEBUGGING ROUTINES					**
 **									**
 **									**
 *************************************************************************/

#include <stdio.h>	/* needed only for debug output */

/*
 * debugging output
 */
int
objlist_dump(ABObjList list)
{
    int	i = 0;
    ABObj	obj = NULL;
    BOOL	entryUsed = FALSE;
    BOOL	entryValid = FALSE;
    char	objName[1024];
    *objName = 0;

    if (list == NULL)
    {
	util_printf(0, "NULL obj list\n");
	return 0;
    }
    if (list->objs_size == 0)
    {
	util_dprintf(0, "empty obj list\n");
	return 0;
    }

    for (i= 0; i < list->objs_size; ++i)
    {
	entryUsed = (i < list->num_objs);
	entryValid = FALSE;

	if (entryUsed)
	{
	    obj = list->objs[i];
	    entryValid = (obj_verify(obj) >= 0);

	    if (entryValid)
	    {
		objName[0] = '\'';
	        obj_get_safe_name(obj, objName+1, 1023);
		strcat(objName, "'");
	    }
	    else
	    {
	        strcpy(objName, "** Invalid ABObj **");
	    }
	}
	else
	{
	    strcpy(objName, "** Unused Entry **");
	}

	util_dprintf(0, "%d[%s]: %s", 
		i, 
		(i >= list->num_objs? " ":"X"),
		objName);

	if (list->user_datas != NULL)
	{
	    util_dprintf(0, " 0x%08lx", objlistP_get_user_data(list,i));
	}
	util_dprintf(0, "\n");
    }
    util_dprintf(0, "\n");
    return 0;
}

#endif /* DEBUG */

