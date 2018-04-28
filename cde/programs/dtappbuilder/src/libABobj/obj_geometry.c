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
 *	$XConsortium: obj_geometry.c /main/3 1995/11/06 18:34:59 rswiston $
 *
 *	@(#)obj_geometry.c	1.10 08 Mar 1994	cde_app_builder/src/libABobj
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
 *  obj_geometry.c - geometry management
 */

#include "objP.h"
#include "obj_notifyP.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Get values							**
**                                                                      **
**************************************************************************/

int
obj_get_geometry(ABObj obj, int *x, int *y, int *width, int *height)
{
    *x= obj->x;
    *y= obj->y;
    *width= obj->width;
    *height= obj->height;

    return OK;
}


int
obj_get_position(ABObj obj, int *x, int *y)
{
    *x= obj->x;
    *y= obj->y;
    return OK;
}


int
obj_get_size(ABObj obj, int *width, int *height)
{
    *width= obj->width;
    *height= obj->height;
    return OK;
}


/*************************************************************************
**                                                                      **
**       Set values							**
**                                                                      **
**************************************************************************/


int
obj_set_geometry(ABObj obj, int newX, int newY, int newWidth, int newHeight)
{
    int		iReturn= 0;
    int		iRC= 0;		/* return code */
    int		oldX= obj->x;
    int		oldY= obj->y;
    int		oldWidth= obj->width;
    int		oldHeight= obj->height;
    BOOL 	changed= (   (oldX != newX) 
			  || (oldY != newY) 
			  || (oldWidth != newWidth) 
			  || (oldHeight != newHeight) );

    if (!changed)
    {
	goto epilogue;
    }

    iRC= objP_notify_send_allow_geometry_change(obj, 
		newX, newY, newWidth, newHeight);
    if (iRC < 0)
    {
	iReturn= iRC;
	goto epilogue;
    }
    oldX = obj->x;
    oldY = obj->y;
    oldWidth = obj->width;
    oldHeight = obj->height;
    obj->x= newX;
    obj->y= newY;
    obj->width= newWidth;
    obj->height= newHeight;

    iReturn= objP_notify_send_geometry_change(
		obj, oldX, oldY, oldWidth, oldHeight);

epilogue:
    return iReturn;
}


int
obj_move(ABObj obj, int newX, int newY)
{
    return obj_set_geometry(obj, newX, newY, obj->width, obj->height);
}


int
obj_resize(ABObj obj, int newWidth, int newHeight)
{
    return obj_set_geometry(obj, obj->x, obj->y, newWidth, newHeight);
}


/*************************************************************************
**                                                                      **
**       Tests								**
**                                                                      **
**************************************************************************/

int
obj_test_set_geometry(ABObj obj, 
	int newX, int newY, int newHeight, int newWidth)
{
    int		iReturn= 0;
    objP_notify_push_mode();
        objP_notify_clear_mode(OBJEV_MODE_DISALLOW_ALL_EVS);

    	iReturn= objP_notify_send_allow_geometry_change(obj,
			newX, newY, newHeight, newWidth);

    objP_notify_pop_mode();

    return iReturn;
}


int
obj_test_move(ABObj obj, int newX, int newY)
{
    return obj_test_set_geometry(obj, newX, newY, obj->width, obj->height);
}


int
obj_test_resize(ABObj obj, int newWidth, int newHeight)
{
    return obj_test_set_geometry(obj, obj->x, obj->y, newWidth, newHeight);
}

