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
 *      $XConsortium: abobj_layout.c /main/3 1995/11/06 17:16:15 rswiston $
 *
 * @(#)abobj_layout.c	1.17 15 Feb 1994      cde_app_builder/src/libABobj
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


/*
 ***********************************************************************
 * ab_layout.c - Implements Layout semantics for objects
 *        
 *
 ***********************************************************************
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <ab_private/objxm.h>
#include <ab_private/trav.h>
#include "abobjP.h"

#define	LL_BUF_SIZE	20

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static void clear_layout(
    		ABObj obj
	    );

/*************************************************************************
**		    		    		    		          **
**       Private Data		    		    		       **
**		    		    		    		          **
**************************************************************************/
static ABObj	layout_list[LL_BUF_SIZE];
static int	layout_count = 0;


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
int
abobj_layout_changed(
    ABObj	obj
)
{

    if (obj->attachments)
	layout_list[layout_count++] = obj;
    return 0;
}


/*
 * Given a new x,y,width,height, calculate appropriate layout info
 */
int
abobj_calculate_new_layout(
    ABObj	obj,
    int		new_x,
    int		new_y,
    Dimension	new_width,
    Dimension	new_height
)
{
    ABObj	parent;
    ABAttachListPtr attachments;
    Position    x, y;
    Dimension   width, height;
    Dimension   p_width, p_height;
    long	gridpos;

    if ((attachments = obj->attachments) == NULL)
        return -1;

    if (obj->ui_handle == NULL)
    {
	if (util_get_verbosity() > 0)
	    fprintf(stderr,"abobj_calculate_new_layout: %s: NULL widget\n", obj_get_name(obj));
	return -1;
    }

    XtVaGetValues((Widget)obj->ui_handle,
			XmNx,		&x,
			XmNy,		&y,
                        XmNwidth,       &width,
                        XmNheight,      &height,
			NULL);

    parent = obj_get_parent(obj);
    XtVaGetValues((Widget)parent->ui_handle,
                        XmNwidth,       &p_width,
                        XmNheight,      &p_height,
                        NULL);

    switch(attachments->west.type)
    {
	case AB_ATTACH_POINT:
	   attachments->west.offset = new_x;
	   break;
	case AB_ATTACH_GRIDLINE:
	   gridpos = (100 * new_x)/(int)p_width;
	   attachments->west.value = (void*)gridpos;
	   break;
	case AB_ATTACH_CENTER_GRIDLINE:
           gridpos = (100 * (new_x + (int)new_width/2))/(int)p_width;
	   attachments->west.value = (void*)gridpos;
	   break;
	case AB_ATTACH_OBJ:
	case AB_ATTACH_ALIGN_OBJ_EDGE:
	   attachments->west.offset += (new_x - x);
	   if (new_x == 0 && attachments->west.offset < 0)
		attachments->west.offset = 0;
	   break;
    }

    switch(attachments->east.type) 
    { 
        case AB_ATTACH_POINT: 
           attachments->east.offset = p_width - (new_x + new_width);
           break;
        case AB_ATTACH_GRIDLINE:
           gridpos = (100 * (new_x + (int)new_width))/(int)p_width;
           attachments->east.value = (void*)gridpos;
           break;
	case AB_ATTACH_CENTER_GRIDLINE:
           gridpos = (100 * (new_x + (int)new_width/2))/(int)p_width;
           attachments->east.value = (void*)gridpos;
           break;
	case AB_ATTACH_OBJ:
	   attachments->east.offset = p_width - (new_x + new_width + 1);
	   break;
	case AB_ATTACH_ALIGN_OBJ_EDGE:
	   attachments->east.offset += (x + width - new_x - new_width);
	   break;
    }

    switch(attachments->north.type)
    {
        case AB_ATTACH_POINT:
           attachments->north.offset = new_y;
           break;
        case AB_ATTACH_GRIDLINE:
           gridpos = (100 * new_y)/(int)p_height;
           attachments->north.value = (void*)gridpos;
           break;
	case AB_ATTACH_CENTER_GRIDLINE:
           gridpos = (100 * (new_y + (int)new_height/2))/(int)p_height;
           attachments->north.value = (void*)gridpos;
           break;
        case AB_ATTACH_OBJ: 
        case AB_ATTACH_ALIGN_OBJ_EDGE: 
           attachments->north.offset += (new_y - y); 
           if (new_y == 0 && attachments->north.offset < 0) 
        	attachments->north.offset = 0; 
           break; 
    }

    switch(attachments->south.type)
    {
        case AB_ATTACH_POINT:
           attachments->south.offset = p_height - (new_y + new_height);
           break;
        case AB_ATTACH_GRIDLINE:
           gridpos = (100 * (new_y + (int)new_height))/(int)p_height;
           attachments->south.value = (void*)gridpos;
           break;
	case AB_ATTACH_CENTER_GRIDLINE:
           gridpos = (100 * (new_y + (int)new_height/2))/(int)p_height;
           attachments->south.value = (void*)gridpos;
           break;
        case AB_ATTACH_OBJ:
           attachments->south.offset = p_height - (new_y + new_height + 1);
	   break;
        case AB_ATTACH_ALIGN_OBJ_EDGE: 
           attachments->south.offset += (y + height - new_y - new_height); 
           break; 
    }

    objxm_obj_set_attachment_args(obj, OBJXM_CONFIG_BUILD);

    return 0;

}

/* 
 * Sort the objects children by either X or Y.
 */
void
abobj_sort_children(
		    ABObj obj, 
		    int sort
)
{
    ABObj	child1, child2;
    ABObj	pobj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);	
    int 	num_children = obj_get_num_children(pobj);
    int 	i, j;
    int 	x1, y1, x2, y2;

    for (i = 0; i < num_children; i++)
    {
	for (j = i; j < num_children; j++) 
	{
	    child1 = obj_get_child(pobj, i);
	    child2 = obj_get_child(pobj, j);

	    x1 = obj_get_x(child1);
	    y1 = obj_get_y(child1);
	    x2 = obj_get_x(child2);
	    y2 = obj_get_y(child2);

	    if (sort == XSORT) 
	    {
		if (x2 < x1 || (x2 == x1 && y2 < y1)) 
		    obj_swap_siblings(child1, child2);
	    } else 
	    {
		if (y2 < y1 || (y2 == y1 && x2 < x1)) 
		    obj_swap_siblings(child1, child2);
	    }
	}
    }
}

/*
 * Set North and West to objs y and x positions. Set
 * South and East to none.
 */
void
clear_layout(
    ABObj obj
)
{
    obj_set_attachment(obj, AB_CP_NORTH, 
				    AB_ATTACH_POINT,
				    NULL, 
				    obj_get_y(obj));
    obj_set_attachment(obj, AB_CP_WEST, 
				    AB_ATTACH_POINT,
				    NULL, 
				    obj_get_x(obj));
    obj_set_attachment(obj, AB_CP_SOUTH, 
				    AB_ATTACH_NONE,
				    NULL, 
				    0);
    obj_set_attachment(obj, AB_CP_EAST, 
				    AB_ATTACH_NONE,
				    NULL, 
				    0);
}

/*
 * Either works on a object or its children.
 * Clear the obj's attachments entirely if init_attachments
 * is TRUE but also configure the widget hierarchy to have two 
 * basic connections: North and West point attachments with the 
 * objs x and y offsets.
 */ 
void
abobj_clear_layout(
	ABObj	obj,
	BOOL	clear_children,
	BOOL	init_attachments
)
{
    ABObj  pos_obj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);	
    ABAttachment  attach;

    if (clear_children)
    {
        AB_TRAVERSAL  trav;
	ABObj         child;

	for (trav_open(&trav, pos_obj, AB_TRAV_SALIENT_CHILDREN |
			AB_TRAV_MOD_SAFE);
	    (child = trav_next(&trav)) != NULL; )
	{
	    /* Configure the tree to have just basic attachments */
	    attach.type = AB_ATTACH_POINT;
	    attach.value = (void *)NULL;
	    attach.offset = abobj_get_y(child);
	    abobj_set_attachment(child, AB_CP_NORTH, &attach);

	    attach.type = AB_ATTACH_POINT;
	    attach.value = (void *)NULL;
	    attach.offset = abobj_get_x(child);
	    abobj_set_attachment(child, AB_CP_WEST, &attach);

	    attach.type = AB_ATTACH_NONE;
	    attach.value = (void *)NULL;
	    attach.offset = 0;
	    abobj_set_attachment(child, AB_CP_SOUTH, &attach);

	    abobj_set_attachment(child, AB_CP_EAST, &attach);

	    abobj_instantiate_changes(child);

	    /*
	     * Clear out all attachments in obj structure
	     * if init_attachments is set
	     */
	    if (init_attachments)
		obj_init_attachments(child);
	}
	trav_close(&trav);
    }
    else
    {
	attach.type = AB_ATTACH_POINT;
	attach.value = (void *)NULL;
	attach.offset = abobj_get_y(pos_obj);
	abobj_set_attachment(pos_obj, AB_CP_NORTH, &attach);

	attach.type = AB_ATTACH_POINT;
	attach.value = (void *)NULL;
	attach.offset = abobj_get_x(pos_obj);
	abobj_set_attachment(pos_obj, AB_CP_WEST, &attach);

	attach.type = AB_ATTACH_NONE;
	attach.value = (void *)NULL;
	attach.offset = 0;
	abobj_set_attachment(pos_obj, AB_CP_SOUTH, &attach);

	abobj_set_attachment(pos_obj, AB_CP_EAST, &attach);

	abobj_instantiate_changes(pos_obj);

	/*
	 * Clear out all attachments in obj structure
	 * if init_attachments is set
	 */
	if (init_attachments)
	    obj_init_attachments(pos_obj);

    }
}
