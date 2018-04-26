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
 *      $XConsortium: util_ds.h /main/4 1995/11/06 18:53:31 rswiston $
 *
 * @(#)util_ds.h	1.1 19 Feb 1994      cde_app_builder/src/libAButil
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

#ifndef _UTIL_DS_H_
#define _UTIL_DS_H_
/*
 * util_ds.h - LList and ASet ADTs
 */
#include <X11/Intrinsic.h>

typedef struct	_LListStruct	*LList;
typedef void	(*LListIterFn)(void *cl_data);

#define ASet		LList
#define ASetIterFn	LListIterFn


/*******************************
 * LList and ASet ADT operations
 *******************************/

extern LList	util_llist_create(
		    void
		);
extern void	util_llist_destroy(
		    LList	llist
		);
extern void	util_llist_insert_after(
		    LList	llist,
		    void	*cl_data
		);
extern LList	util_llist_find(
		    LList	llist,
		    void	*cl_data
		);
extern void	util_llist_delete(
		    LList	llist,
		    void	*cl_data
		);
extern void	util_llist_iterate(
		    LList	llist,
		    LListIterFn	fn
		);

#define util_aset_create	util_llist_create
#define util_aset_destroy	util_llist_destroy
#define util_aset_iterate	util_llist_iterate
#define util_aset_remove	util_llist_delete

extern void	util_aset_add(
		    ASet	aset,
		    void	*cl_data
		);

#endif /* _UTIL_DS_H_ */
