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
 *	$XConsortium: trav.h /main/3 1995/11/06 18:41:46 rswiston $
 *
 * @(#)trav.h	3.28 27 Apr 1994	cde_app_builder/src/libABobj
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
 * Traversal.h - Handles object tree traversal.
 *
 * The AB_TRAVERSAL data type allows the client to easily visit objects
 * in a tree of ABObjs.  A great deal of flexibility is provided, with the
 * ability to select the types of objects to visit, the order they will be
 * visited in, and the ability to modify the tree while the traversal is
 * being performed.
 *
 * To traverse a tree, call trav_open with the root object of the tree to be
 * searched, along with a traversal specifier, which is a set of one or more
 * AB_TRAV_ constants.  The specifier defines exactly what types of objects
 * you want returned, and in what order you want them returned.  Note that 
 * trav_open does not return an object - trav_next returns the first and all
 * successive objects.  Calling trav_next until it returns NULL will get
 * all of the objects selected by the traversal.  WHEN THE TRAVERSAL IS NO
 * LONGER NEEDED, YOU MUST USE trav_close() TO INVALIDATE IT.
 *
 * A traversal specifier is a bitwise-ored value, consisting of a traversal
 * type and zero or more modifiers.  Without a modifier, the default order
 * of visitation is undefined, although each selected object will be returned
 * exactly once. Note that once a traversal is opened, the traversal specifier 
 * cannot be changed.
 *
 * Traversal type
 * -------------- 
 * The traveral type defines the object selection (what types/classes of 
 * objects are visited).  Only one traversal type can be specified, and 
 * if no traversal type is specified, AB_TRAV_ALL is assumed. 
 * Commonly used types are AB_TRAV_UI, AB_TRAV_SALIENT and AB_TRAV_SALIENT_UI
 *
 * Traversal modifier
 * ------------------
 * Modifiers may be used in any combination to modify the behavior of a 
 * traversal.
 *
 *  AB_TRAV_MOD_PARENTS_FIRST	returns the parents (ancestors) of an object 
 *			     	before returning the object itself.  This does 
 *				not change the objects returned, only their 
 *				ordering.
 *
 *  AB_TRAV_MOD_SAFE		allows the tree to be modified while the
 *				traversal is being performed.  Note
 *				that if an object is destroyed, an invalid 
 *				reference to it will be returned by the 
 *				traversal. However, since each object is 
 *				returned only once, destroying an object 
 *				after it is returned should avoid most
 *				problems.
 *			
 * Note that all of the calls to the trav_ functions require a first
 * parameter of type (AB_TRAVERSAL *).  This must be a pointer to a
 * previously-allocated structure (e.g., the address of a local variable
 * of type AB_TRAVERSAL).
 *
 *
 * E.g.:
 *
 * int 
 * show_objects(ABObj root)
 * {
 *     AB_TRAVERSAL trav;
 *     for (trav_open(&trav, root, AB_TRAV_SALIENT_UI | AB_TRAV_MOD_SAFE);
 *          (obj= trav_next(&trav)) != NULL; )
 *     {
 *         -- operate on obj
 *     }
 *     trav_close(&trav);  -- gotta close!
 *     return 0;
 * }
 *
 *
 * Other examples of opening a traversal:
 *
 *	trav_open(&trav, rootObj, AB_TRAV_WINDOWS | AB_TRAV_MOD_SAFE);
 *	trav_open(&trav, rootObj, AB_TRAV_UI | AB_TRAV_MOD_SAFE);
 *	trav_open(&trav, rootObj, AB_TRAV_MOD_PARENT_FIRST);
 *
 */
#ifndef _ABOBJ_TRAVERSAL_H_
#define _ABOBJ_TRAVERSAL_H_

#include <ab_private/obj.h>

/*
 * Traversal types (see comment at top of this file)
 */
#define AB_TRAV_UNDEF			(0U)   /* invalid traversal type */
#define AB_TRAV_ACTIONS			(1U)   /* actions only */
#define AB_TRAV_ACTIONS_FOR_OBJ		(2U)  /* actions for root object only */
#define AB_TRAV_ALL			(3U)
#define AB_TRAV_CHILDREN		(4U)   /* ONLY IMMEDIATE CHILDREN! */
#define AB_TRAV_FILES			(5U)
#define AB_TRAV_GROUPS			(6U)
#define AB_TRAV_ITEMS			(7U)   /* all items in tree */
#define AB_TRAV_ITEMS_FOR_OBJ		(8U)   /* items for object only */
#define AB_TRAV_MENUS			(9U)   /* all types of menus */
#define AB_TRAV_MODULES			(10U)
#define AB_TRAV_PARENTS			(11U)
#define AB_TRAV_SALIENT			(12U)   /*user-manipulatable objects*/
#define AB_TRAV_SALIENT_UI		(13U)   /* salient && ui */
#define AB_TRAV_SALIENT_CHILDREN	(14U)  /*user-manipulatable children*/
#define AB_TRAV_SALIENT_UI_CHILDREN	(15U)
#define AB_TRAV_SIBLINGS		(16U)
#define AB_TRAV_UI			(17U)   /* all ui objs (no actions) */
#define AB_TRAV_WINDOWS			(18U)   /* all windows */
#define AB_TRAV_COMP_SUBOBJS		(19U)	/* subobjs of this comp. obj */

/* 
 * Traversal modifiers (see comment at top of this file)
 */
#define AB_TRAV_MOD_PARENTS_FIRST	(0x0100U)
#define AB_TRAV_MOD_SAFE		(0x0200U)


/*
 * Traversal data structure
 */
typedef struct
{
	unsigned	travType;
	ABObj		rootObj;
	ABObj		curObj;
	ABObj		*objArray;
	int		objArraySize;
	int		objArrayIndex;
	BOOL		done;
	ABObjTestFunc	testFunc;
} AB_TRAVERSAL;
typedef AB_TRAVERSAL *ABTraversal;

/*
 * callback for trav_perform.  return of negative value aborts traversal.
 */
typedef int (*AB_TRAVERSAL_CB)(ABObj obj);
typedef AB_TRAVERSAL_CB ABTraversalCB;

int		trav_open(ABTraversal trav, ABObj root, unsigned trav_type);
int		trav_open_cond(
			ABTraversal	trav, 
			ABObj		root, 
			unsigned	trav_type,
			ABObjTestFunc	cond_test_func
		);
ABObj		trav_obj(ABTraversal trav);
ABObj		trav_next(ABTraversal trav);
ABObj		trav_goto(ABTraversal trav, int which);
int		trav_close(ABTraversal trav);
BOOL		trav_is_open(ABTraversal trav);
int		trav_reset(ABTraversal trav);

/* Returns the # of objects that will be returned by the traversal.
 */
int		trav_count(ABObj root, int trav_type);
int		trav_count_cond(ABObj root, int trav_type, ABObjTestFunc);

/*
 * Performs the traversal, calling clientfunc on each object.  A negative
 * return from clientfunc aborts the traversal.
 */
int		trav_perform(ABObj root, int trav_type,
				AB_TRAVERSAL_CB clientfunc);

/*************************************************************************
**									**
**		Inline implementations					**
**									**
**************************************************************************/

#define trav_is_open(trav)	((trav)->rootObj != NULL)
#define trav_obj(trav)		((trav)->curObj)

#endif /* _ABOBJ_TRAVERSAL_H_ */

