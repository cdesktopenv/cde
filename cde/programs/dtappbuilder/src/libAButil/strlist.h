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
 *	$XConsortium: strlist.h /main/4 1995/11/06 18:52:03 rswiston $
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

#ifndef _ABUTIL_STRLIST_H_
#define _ABUTIL_STRLIST_H_
/*
 * strlist.h - string list manipulation
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <ab_private/AB.h>	/* everybody must include this first! */
#include <ab_private/istr.h>

typedef enum
{
    STRLIST_SORT_UNDEF = 0,
    STRLIST_SORT_ALPHANUMERIC,
    STRLIST_SORT_BEST,
    STRLIST_SORT_CLIENT_DEF,
    STRLIST_SORT_ORDER_NUM_VALUES	/* must be last */
} STRLIST_SORT_ORDER;

/*
 * string list structure.
 */
typedef struct
{
    int                 num_strings;
    int			strings_size;
    ISTRING            *strings;
    void		**user_datas;
    BOOL		unique;
    STRLIST_SORT_ORDER	sort_order;
    BOOL		indexes_dirty;
} StringListRec, *StringList;


/*
 * StringList
 */
StringList	strlist_create(void);
int		strlist_destroy(StringList);
int		strlist_set_sort_order(StringList, STRLIST_SORT_ORDER);
STRLIST_SORT_ORDER	strlist_get_sort_order(StringList);
int		strlist_construct(StringList);
int		strlist_destruct(StringList);
int		strlist_set_is_unique(StringList, BOOL unique);
BOOL		strlist_is_unique(StringList);
int		strlist_make_empty(StringList);
int		strlist_is_empty(StringList);
int		strlist_add_index(StringList, 
				int index, STRING s, void *userData);
int		strlist_set_num_strs(StringList, int numStrings);
int		strlist_get_num_strs(StringList);
int		strlist_set_max_size_soft(StringList, int max_num_strs);
int		strlist_get_max_size(StringList);
BOOL		strlist_max_size_is_soft(StringList);
int		strlist_remove_index(StringList, int index);
int		strlist_shrink_mem_to_fit(StringList);
int		strlist_dump(StringList list);

/*
 * STRING interfaces
 */
int		strlist_add_str(StringList, STRING s, void *userData);
long		strlist_remove_str(StringList, STRING s);
BOOL		strlist_str_exists(StringList list, STRING s);
long		strlist_get_str_index(StringList list, STRING s);
STRING		strlist_get_str(StringList, 
				int whichString, void **userDataOut);
long		strlist_set_str_data(StringList, STRING string, void *data);
void		*strlist_get_str_data(StringList, STRING string);
StringList	strlist_dup(StringList);


/*
 * ISTRING interfaces
 *
 * The STRINGs and ISTRING are kept in the same list, and behave identically
 */
int		strlist_add_istr(StringList, ISTRING s, void *userData);
long		strlist_add_index_istr(StringList, 
				int index, ISTRING s, void *userData);
BOOL		strlist_istr_exists(StringList list, ISTRING s);
long		strlist_get_istr_index(StringList list, ISTRING s);
ISTRING		strlist_get_istr(StringList, 
				int whichString, void **userDataOut);
long		strlist_set_istr_data(StringList, ISTRING istring, void *data);
void		*strlist_get_istr_data(StringList, ISTRING istring);
long		strlist_remove_istr(StringList, ISTRING s);

#endif /* _ABUTIL_STRLIST_H_ */
