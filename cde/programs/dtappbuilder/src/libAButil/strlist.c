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
 *	$XConsortium: strlist.c /main/5 1996/10/02 16:53:22 drk $
 *
 * @(#)strlist.c	1.8 14 Oct 1994	cde_app_builder/src/libAButil
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
 * File: strlist.c
 */

#include <string.h>
#include <ab_private/AB.h>
#include <ab_private/istr.h>
#include <ab_private/strlist.h>

/*****************************************************************
 **								**
 **		STRING LIST					**
 **								**
 *****************************************************************/

static int strlistP_grow_array(StringList list, int sizeDiff);
static int strlistP_shrink_array(StringList list, int sizeDiff);
static int strlistP_build_user_data_array(
			StringList list, 
			int index, 
			void *data
	    );


#define strlistP_add_user_data(_list,_index,_data) \
    ((((_list)->user_datas == NULL) && ((_data) != NULL))? \
        strlistP_build_user_data_array(_list, _index, _data) \
    : \
        ((long)((_list)->user_datas[(_index)] = (_data))) \
    )
	

StringList
strlist_create(void)
{
    StringList          list = (StringList) util_malloc(sizeof(StringListRec));
    if (list == NULL)
    {
	return NULL;
    }
    strlist_construct(list);
    return list;
}


int
strlist_construct(StringList list)
{
    list->num_strings = 0;
    list->strings_size = 0;
    list->strings = NULL;
    list->user_datas = NULL;
    list->unique = TRUE;
    list->sort_order = STRLIST_SORT_CLIENT_DEF;
    list->indexes_dirty = TRUE;
    return 0;
}


/*
 * Frees the string list and all it's associated memory.
 */
int
strlist_destroy(StringList list)
{
    strlist_destruct(list);
    free(list);
    return 0;
}


int
strlist_destruct(StringList list)
{
    return strlist_make_empty(list);
}


STRLIST_SORT_ORDER
strlist_get_sort_order(StringList list)
{
    return list->sort_order;
}


int
strlist_set_sort_order(StringList list, STRLIST_SORT_ORDER new_order)
{
    int		return_value = 0;

    if (list->sort_order != new_order)
    {
	list->indexes_dirty = TRUE;
    }

    return 0;
}


int
strlist_make_empty(StringList list)
{
    int		return_value = 0;

    if (list->strings != NULL)
    {
	return_value = strlistP_shrink_array(list, list->strings_size);
    }

    return return_value;
}


int
strlist_is_empty(StringList list)
{
    return (list->num_strings < 1);
}


int
strlist_set_is_unique(StringList list, BOOL unique)
{
    list->unique = unique;
    return 0;
}


BOOL
strlist_get_is_unique(StringList list)
{
    return list->unique;
}


/*
 * Searches the string list for a match.
 * 
 * returns:	true, if the string exists. false, if the string doesn't
 * exist.
 */
BOOL
strlist_istr_exists(StringList list, ISTRING istring)
{
    return (strlist_get_istr_index(list, istring) >= 0);
}


long
strlist_set_istr_data(StringList list, ISTRING istring, void *data)
{
    long		index = strlist_get_istr_index(list, istring);
    if (index < 0)
    {
	return index;
    }

    strlistP_add_user_data(list,index,data);
    return 0;
}



void *
strlist_get_istr_data(StringList list, ISTRING istring)
{
    int		index = strlist_get_istr_index(list, istring);
    return (index < 0)? NULL:list->user_datas[index];
}


/*
 * Returns the index of the given string, or -1 if it doesn't exist
 */
long
strlist_get_istr_index(StringList list, ISTRING string)
{
    long		index = -1;
    int                 i = 0;
    int                 num_strings = list->num_strings;

    for (i = 0; i < num_strings; ++i)
    {
	if (istr_equal(list->strings[i], string))
	{
	    index = i;
	    break;
	}
    }

epilogue:
    return index;
}


/*
 * Add a string to a string list.
 * 
 * Duplicates the string
 * 
 * returns:	0 if item added successfully ERR_DUPLICATE_KEY if item is
 * already in list
 */
int
strlist_add_istr(StringList list, ISTRING istring, void *clientData)
{
    int			return_value = 0;
    int			rc = 0;

    /*
     * look for it
     */
    if (list->unique)
    {
	int index = strlist_get_istr_index(list, istring);
	if (index >= 0)
	{
	    strlistP_add_user_data(list, index, clientData);
	    return 0;
	}
    }

    /*
     * Allocate more space
     */
    if (list->num_strings >= list->strings_size)
    {
	/* grow the array by 50% (the "opposite" of a binary search) */
	int	new_size = (list->strings_size * 3)/2;
	int	size_inc = new_size - list->strings_size;
	size_inc = util_max(5,size_inc);
        rc = strlistP_grow_array(list, size_inc);
	if (rc < 0)
	{
	    return_value = rc;
	    goto epilogue;
	}
    }


    /*
     * save the string (and data)
     */
    list->strings[list->num_strings] = istr_dup(istring);
    strlistP_add_user_data(list, list->num_strings, clientData);
    ++(list->num_strings);

epilogue:
    return return_value;
}


long
strlist_remove_istr(StringList list, ISTRING istring)
{
    long	index = strlist_get_istr_index(list, istring);
    if (index < 0)
    {
	return 0;
    }
    return strlist_remove_index(list, index);
}


int
strlist_remove_index(StringList list, int doomedIndex)
{
#define num_strings (list->num_strings)
    ISTRING	*strings = list->strings;
    void	**user_datas = list->user_datas;
    int		index = 0;
    int		numToMove = 0;

    if ((doomedIndex < 0) || (doomedIndex >= num_strings))
    {
	return -1;
    }

    numToMove = (num_strings - doomedIndex - 1);

    /*
     * Destroy this string and collapse the list
     */
    istr_destroy(strings[doomedIndex]);
    if (numToMove > 0)
    {
        memmove((void *)(&(strings[doomedIndex])),
		    (void *)&(strings[doomedIndex+1]),
		    sizeof(ISTRING) * numToMove);
        strings[num_strings-1] = NULL;
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
            user_datas[num_strings-1] = NULL;
        }
    }

    /*
     * Actually shorten list
     */
    --num_strings;

    return 0;
#undef num_strings
}


int
strlist_get_num_strs(StringList list)
{
    if (list == NULL)
    {
	return 0;
    }
    return list->num_strings;
}


ISTRING
strlist_get_istr(StringList list, int whichString, void **clientDataOut)
{
    if ((list == NULL) || (whichString >= list->num_strings) ||
	(whichString < 0))
    {
	return NULL;
    }
    if (clientDataOut != NULL)
    {
	*clientDataOut = 
	    (list->user_datas == NULL? NULL:list->user_datas[whichString]);
    }
    return list->strings[whichString];
}


/*
 * Only "soft" limit is supported, now
 */
BOOL
strlist_max_size_is_soft(StringList list)
{
    return TRUE;
}


int
strlist_set_max_size_soft(StringList list, int maxSize)
{
    int		return_value = 0;
    int		sizeDiff = (maxSize - list->strings_size);

    if (sizeDiff < 0)
    {
	return_value = strlistP_shrink_array(list, -1 * sizeDiff);
    }
    else if (sizeDiff > 0)
    {
	return_value = strlistP_grow_array(list, sizeDiff);
    }

    return return_value;
}


int
strlist_get_max_size(StringList list)
{
    return list->strings_size;
}


/*
 * Frees any extra memory that may not be needed (multiple strings are
 * allocated at once, to reduce the number of realloc calls).
 */
int
strlist_shrink_mem_to_fit(StringList list)
{
    int		return_value = 0;
    int		diff = list->strings_size - list->num_strings;

    if (diff > 0)
    {
	return_value = strlistP_shrink_array(list, diff);
    }

    return return_value;
}


/*************************************************************************
**									**
**		STRING interface 					**
**									**
**************************************************************************/

int
strlist_add_str(StringList list, STRING string, void *clientData)
{
    int		return_value = 0;
    ISTRING 	istring = istr_create(string);
    return_value = strlist_add_istr(list, istring, clientData);
    istr_destroy(istring);
    return return_value;
}


#ifdef UNIMPLEMENTED
int
strlist_add_index_str(StringList list, int index, STRING string, void *clientData)
{
    int		return_value = 0;
    ISTRING	istring = istr_create(string);
    return_value = strlist_add_index_istr(list, index, istring, clientData);
    istr_destroy(istring);
    return return_value;
}
#endif /* UNIMPLEMENTED */


BOOL
strlist_str_exists(StringList list, STRING string)
{
    BOOL	doesIt = FALSE;
    ISTRING	istring = istr_dup_existing(string);
    if (istring != NULL)
    {
	doesIt = strlist_istr_exists(list, istring);
	istr_destroy(istring);
    }
    return doesIt;
}


long
strlist_get_str_index(StringList list, STRING string)
{
    long		index = -1;
    ISTRING	istring = istr_create(string);
    if (istring != NULL)
    {
	index = strlist_get_istr_index(list, istring);
	istr_destroy(istring);
    }
    return index;
}


STRING
strlist_get_str(StringList list, int whichString, void **clientDataOut)
{
    return istr_string(strlist_get_istr(list, whichString, clientDataOut));
}


long
strlist_remove_str(StringList list, STRING string)
{
    long		return_value = 0;
    ISTRING	istring = istr_dup_existing(string);
    if (istring != NULL)
    {
	return_value = strlist_remove_istr(list, istring);
	istr_destroy(istring);
    }
    return return_value;
}


long
strlist_set_str_data(StringList list, STRING string, void *data)
{
    long		return_value = 0;
    ISTRING	istring = istr_dup_existing(string);
    if (istring != NULL)
    {
	return_value = strlist_set_istr_data(list, istring, data);
	istr_destroy(istring);
    }
    return return_value;
}


void *
strlist_get_str_data(StringList list, STRING string)
{
    void	*clientData = NULL;
    ISTRING	istring = istr_dup_existing(string);
    if (istring != NULL)
    {
	clientData = strlist_get_istr_data(list, istring);
	istr_destroy(istring);
    }
    return clientData;
}

StringList
strlist_dup(StringList list)
{
    int		i;
    StringList	new_list = NULL;

    new_list = strlist_create();
    new_list->unique = list->unique;
    new_list->sort_order = list->sort_order;
    new_list->indexes_dirty = list->indexes_dirty;
    for (i=0; i<list->num_strings; i++)
    {
	strlist_add_istr(new_list, list->strings[i], list->user_datas[i]);
    }
    return(new_list);
}

/*************************************************************************
**									**
**		PRIVATE FUNCTIONS					**
**									**
**************************************************************************/


int
strlistP_shrink_array(StringList list, int sizeDiff)
{
    int		return_value = 0;
    int		new_strings_size = util_max(0, list->strings_size - sizeDiff);
    ISTRING	*new_strings = NULL;
    void	**new_user_datas = NULL;
    int		i = 0;
	
    for (i= new_strings_size; i < list->strings_size; ++i)
    {
	if (list->strings[i] != NULL)
	{
	    istr_destroy(list->strings[i]);
	}
	list->user_datas[i] = NULL;
    }

    if (new_strings_size > 0)
    {
        new_strings = (ISTRING *) realloc(
			  list->strings, new_strings_size * sizeof(ISTRING));
	new_user_datas = (void **) realloc(
			  list->user_datas, new_strings_size * sizeof(void*));
    }
    else
    {
	util_free(list->strings); list->strings = NULL;
	util_free(list->user_datas); list->user_datas= NULL;
    }

    if (   (new_strings_size > 0) 
	&& ((new_strings == NULL) || (new_user_datas == NULL)) )
    {
	return_value = -1;
	goto epilogue;
    }
    else
    {
	list->strings = new_strings;
	list->user_datas = new_user_datas;
	list->strings_size = new_strings_size;
	if (list->num_strings > list->strings_size)
	{
	    list->num_strings = util_max(0, list->strings_size);
	}
    }

epilogue:
    return return_value;
}


static int
strlistP_grow_array(StringList list, int sizeDiff)
{
    int		return_value = 0;
    int		old_strings_size = list->strings_size;
    int		new_strings_size = list->strings_size + sizeDiff;
    ISTRING	*new_strings = NULL;
    void	**new_user_datas = NULL;
    int		i = 0;

    new_strings = (ISTRING *)realloc(
			list->strings, new_strings_size * sizeof(ISTRING));
    new_user_datas = (void **)realloc(
			list->user_datas, new_strings_size * sizeof(void *));
    if ((new_strings == NULL) || (new_user_datas == NULL))
    {
	return_value = -1;
	goto epilogue;
    }
    else
    {
	list->strings = new_strings;
	list->user_datas = new_user_datas;
	list->strings_size = new_strings_size;
    }

    /*
     * Init the new entries to NULL;
     */
    for (i = old_strings_size; i < new_strings_size; ++i)
    {
	list->strings[i] = NULL;
	list->user_datas[i] = NULL;
    }

epilogue:
    return return_value;
}


static int
strlistP_build_user_data_array(StringList list, int index, void *data)
{
    if (list->user_datas != NULL)
    {
	return -1;
    }
    list->user_datas = (void **)calloc(sizeof(void*), list->strings_size);
    if (list->user_datas == NULL)
    {
	return -1;
    }
    list->user_datas[index] = data;
    return 0;
}


/*
 * debugging output
 */
int
strlist_dump(StringList list)
{
    int	i = 0;
    ISTRING	istring = NULL;
    STRING	string = NULL;

    if (list == NULL)
    {
	util_printf(0, "NULL string list\n");
	return 0;
    }
    if (list->strings_size == 0)
    {
	util_dprintf(0, "empty string list\n");
	return 0;
    }

    for (i= 0; i < list->strings_size; ++i)
    {
	istring= list->strings[i];
	if (istr_verify(istring) < 0)
	{
	    string = "** Invalid ISTRING **";
	}
	else
	{
	    string = util_strsafe(istr_string(istring));
	}

	util_dprintf(0, "%d[%s]: '%s'", 
		i, 
		(i >= list->num_strings? " ":"X"),
		string);

	if (list->user_datas != NULL)
	{
	    util_dprintf(0, " 0x%08lx", list->user_datas[i]);
	}
	util_dprintf(0, "\n");
    }
    util_dprintf(0, "\n");
    return 0;
}

