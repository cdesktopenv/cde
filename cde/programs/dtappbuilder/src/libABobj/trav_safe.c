
/*
 *	$XConsortium: trav_safe.c /main/3 1995/11/06 18:42:43 rswiston $
 *
 *	@(#)trav_safe.c	1.12 27 Apr 1994	
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
 *  trav_safe.c - contains functions for "safe" traversals (see
 *  Traversal.h for more info).
 *
 *  Safe traversals allow the tree to be modified during the traversals
 *  To accomplish this, when the traversal is opened, all the objects
 *  selected by the traversal are stored into a dynamic array and returned
 *  from the array.
 *
 *  This implementation is done such that the safe traversals are layered
 *  on top of the normal traversals.  There is no code in the normal
 *  traversal code that knows anything about safe traversals - they are
 *  implemented completely in this file.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ab_private/trav.h>
#include "travP.h"

int 
travP_open_safe(
			ABTraversal trav, 
			ABObj root, 
			unsigned travType,
			ABObjTestFunc	testFunc
)
{
    int		iRet= 0;
    unsigned	unsafeTravType= travType & ~AB_TRAV_MOD_SAFE;
    ABObj	*objArray= NULL;
    int		objArraySize= 0;
    int		objIndex= 0;
    ABObj	obj= NULL;

    /* if we use safe, we will end up back here! */
    objArraySize= trav_count_cond(root, unsafeTravType, testFunc);
    if (objArraySize > 0)
    {
        objArray= (ABObj *)util_malloc(objArraySize * sizeof(ABObj*));
        if (objArray == NULL)
        {
	    return -1;
        }
    }

    /* travP_open will set the data correctly */
    for (objIndex= 0, travP_open(trav, root, travType, testFunc); 
	(obj= travP_next(trav)) != NULL; ++objIndex)
    {
	if (objIndex >= objArraySize)
	{
	    iRet= -1;
	    break;
	}
	objArray[objIndex]= obj;
    }
    /* don't call travP_close(trav) - keep the traversal open */
    if (iRet < 0)
    {
	return iRet;
    }

    trav->objArraySize= objArraySize;
    trav->objArray= objArray;
    iRet= travP_reset_safe(trav);

    return iRet; 
}


int
travP_close_safe(ABTraversal trav)
{
    travP_close(trav);
    util_free(trav->objArray);
    trav->objArrayIndex= 0;
    return 0;
}


ABObj
travP_next_safe(ABTraversal trav)
{
    if (trav->done)
    {
	return NULL;
    }

    ++(trav->objArrayIndex);
    if ((trav->objArrayIndex) >= (trav->objArraySize))
    {
	trav->curObj= NULL;
	trav->done= TRUE;
    }
    else
    {
	trav->curObj= trav->objArray[trav->objArrayIndex];
    }

    return trav->curObj;
}


int
travP_reset_safe(ABTraversal trav)
{
    trav->done= FALSE;
    trav->curObj= NULL;
    trav->objArrayIndex= -1;
    return 0;
}


ABObj
travP_goto_safe(ABTraversal trav, int nodeNum)
{
    if ((nodeNum < -1) || (nodeNum >= (trav->objArraySize)))
    {
	return NULL;
    }
    else if (nodeNum == -1)
    {
	travP_reset_safe(trav);
    }
    else
    {
	trav->objArrayIndex= nodeNum;
	trav->curObj= trav->objArray[trav->objArrayIndex];
    }

    return trav->curObj;
}


