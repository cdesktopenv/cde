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
 *	$XConsortium: trav_public.c /main/3 1995/11/06 18:42:28 rswiston $
 *
 *	@(#)trav_public.c	1.7 27 Apr 1994	
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
 *  trav_public.c - contains all public traversal functions
 */

#include <stdio.h>
#include <ab_private/trav.h>
#include "travP.h"

int 
trav_open_cond(
			ABTraversal	trav, 
			ABObj		root, 
			unsigned	travType, 
			ABObjTestFunc	testFunc
)
{
    int	iRet= 0;

    if ((travType & AB_TRAV_MOD_SAFE) != 0)
    {
	iRet= travP_open_safe(trav, root, travType, testFunc);
    }
    else
    {
	iRet= travP_open(trav, root, travType, testFunc);
    }

    return iRet;
}


int
trav_open(
			ABTraversal trav, 
			ABObj root, 
			unsigned travType
)
{
    return trav_open_cond(trav, root, travType, NULL);
}


int
trav_close(ABTraversal trav)
{
    int		iRet= 0;

    if (travP_is_safe(trav))
    {
	iRet= travP_close_safe(trav);
    }
    else
    {
	iRet= travP_close(trav);
    }
    return iRet;
}





ABObj
trav_next(ABTraversal trav)
{
    ABObj	obj= NULL;
    if (travP_is_safe(trav))
    {
	obj= travP_next_safe(trav);
    }
    else
    {
	obj= travP_next(trav);
    }
    return obj;
}


int
trav_reset(ABTraversal trav)
{
    int		iRet= 0;
    if (travP_is_safe(trav))
    {
	iRet= travP_reset_safe(trav);
    }
    else
    {
	iRet= travP_reset(trav);
    }

    return iRet;
}


ABObj
trav_goto(ABTraversal trav, int nodeNum)
{
    ABObj	obj= NULL;
    if (travP_is_safe(trav))
    {
	obj= travP_goto_safe(trav, nodeNum);
    }
    else
    {
	obj= travP_goto(trav, nodeNum);
    }
    return obj;
}


/*
 * Performs the traversal, calling clientfunc on each node.  A negative
 * return from clientfunc aborts the traversal.
 *
 * returns the number of nodes visited.
 */
int		
trav_perform(ABObj root, int trav_type, ABTraversalCB clientfunc)
{
	AB_TRAVERSAL	trav;
	ABObj		obj;
	int		retval= 0;

	for (trav_open(&trav, root, trav_type);
		(obj= trav_next(&trav)) != NULL; )
	{
		if ((retval= clientfunc(obj)) < 0)
		{
			break;
		}
	}
	trav_close(&trav);
	return retval;
}


/*
 * Returns the # of nodes that will be returned by the traversal
 */
int
trav_count_cond(ABObj root, int trav_type, ABObjTestFunc testFunc)
{
    AB_TRAVERSAL	trav;
    ABObj		obj= NULL;
    int			objCount= 0;

    for (trav_open_cond(&trav, root, trav_type, testFunc);
	(obj= trav_next(&trav)) != NULL; )
    {
	++objCount;
    }
    trav_close(&trav);

    return objCount;
}


int
trav_count(ABObj root, int trav_type)
{
    return trav_count_cond(root, trav_type, NULL);
}


