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
 *      $XConsortium: abobj_align.c /main/3 1995/11/06 17:14:58 rswiston $
 *
 * @(#)abobj_align.c	1.21 24 May 1994      cde_app_builder/src/libABobj
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
 * ab_align.c - User-Interface alignment functions. 
 *	
 *
 ***********************************************************************
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Composite.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
#include <Xm/ScrolledW.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <ab_private/trav.h>
#include <ab_private/proj.h>
#include <Dt/xpm.h>		/* will this be in include/Dt? */
#include "abobjP.h"


static void	align_left_edges(ABObj *sel_list, int sel_count);
static void	align_vcenters(ABObj *sel_list, int sel_count);
static void	align_right_edges(ABObj *sel_list, int sel_count);
static void	align_labels(ABObj *sel_list, int sel_count);


static void	align_top_edges(ABObj *sel_list, int sel_count);
static void	align_hcenters(ABObj *sel_list, int sel_count);
static void	align_bottom_edges(ABObj *sel_list, int sel_count);

static void	distribute_horizontal(ABObj *sel_list, int sel_count, int hspacing);
static void	distribute_vertical(ABObj *sel_list, int sel_count, int vspacing);
static void	center_horizontal(ABObj *sel_list, int sel_count);
static void	center_vertical(ABObj *sel_list, int sel_count);

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
void
abobj_align(
    unsigned long align_mask
)
{
    ABSelectedRec sel;
    extern int  AB_grid_size;
    int	x = MAXINT;

    if (!align_mask)
	return;

    abobj_get_selected(proj_get_project(), FALSE, FALSE, &sel);
    /*
    * Vertical alignment
     */
    if (align_mask & LEFT_EDGES)
	align_left_edges(sel.list, sel.count);
    if (align_mask & VCENTERS)
	align_vcenters(sel.list, sel.count);
    if (align_mask & RIGHT_EDGES)
	align_right_edges(sel.list, sel.count);
    if (align_mask & LABELS)
	align_labels(sel.list, sel.count);

    /*
     * Horizonatal alignment
     */
    if (align_mask & TOP_EDGES)
	    align_top_edges(sel.list, sel.count);
    if (align_mask & HCENTERS)
	    align_hcenters(sel.list, sel.count);
    if (align_mask & BOTTOM_EDGES)
	    align_bottom_edges(sel.list, sel.count);

    /*
     * Spacing
     */
    if (align_mask & HSPACING)
	    distribute_horizontal(sel.list, sel.count, AB_grid_size);
    if (align_mask & VSPACING)
	    distribute_vertical(sel.list, sel.count, AB_grid_size);

    /*
     * Centering within parent
     */
    if (align_mask & HCENTERED)
	    center_horizontal(sel.list, sel.count);
    if (align_mask & VCENTERED)
	    center_vertical(sel.list, sel.count);
}


/*
 * align_left_edges
 */
static void
align_left_edges(
    ABObj    *sel_list,
    int      sel_count
    )
{
    ABObj    obj;
    int	     i;
    int      x = MAXINT;

	for (i = 0; i < sel_count; i++) 
	{
	    obj = sel_list[i];
	    if (obj_get_x(obj) < x)
		x = obj_get_x(obj);
	}

	for (i = 0; i < sel_count; i++) 
	{
	    obj = sel_list[i];
	    /*
	     undo_record_move(obj);
	    */
           abobj_set_xy(obj, x, obj_get_y(obj)); 
           abobj_instantiate_changes(obj); 
	}
}


/*
 * align_vcenters
 */
static void
align_vcenters(
    ABObj    *sel_list,
    int      sel_count
)
{
    int	     i;
    ABObj    obj;
    int	     x = 0;

    for (i = 0; i < sel_count; i++) 
    {
	obj = sel_list[i];
	x += obj_get_x(obj) + abobj_get_actual_width(obj) / 2;
    }

	x /= sel_count;

	for (i = 0; i < sel_count; i++) 
	{
	    obj = sel_list[i];
	    /*
	    undo_record_move(obj);
	    */
	    abobj_set_xy(obj, x - abobj_get_actual_width(obj)/2, obj_get_y(obj));
            abobj_instantiate_changes(obj); 
	}
}

/*
 * align_right_edges
 */
static void
align_right_edges(
    ABObj    *sel_list,
    int       sel_count
    )
{
	int	i;
	ABObj	obj;
	int	x = -1;
	int	xtmp;

	for (i = 0; i < sel_count; i++) 
	{
	    obj = sel_list[i];
	    xtmp = obj_get_x(obj) + abobj_get_actual_width(obj);
	    if (xtmp > x)
		x = xtmp;
	}

	for (i = 0; i < sel_count; i++) 
	{
	    obj = sel_list[i];
	    /*
	    undo_record_move(obj);
	    */
	    abobj_set_xy(obj, x - abobj_get_actual_width(obj), obj_get_y(obj));
            abobj_instantiate_changes(obj); 
	}
}

/*
 * align_labels
 *
 *	Align all selected items to the topmost object's value x.
 */
static void
align_labels(
    ABObj    *sel_list,
    int      sel_count
)
{
    int	    i;
    ABObj   obj;
    int	    new_label_x = 0;
    int	    y = MAXINT;

    for (i = 0; i < sel_count; i++) 
    {
	obj = sel_list[i];
    	if (obj_get_y(obj) < y) 
	{
    		new_label_x = obj_get_x(obj) +
				abobj_get_label_width(obj);
    		y = obj_get_y(obj);
    	}
    }

    for (i = 0; i < sel_count; i++) 
    {
	obj = sel_list[i];
    	abobj_set_xy(obj, new_label_x - abobj_get_label_width(obj), 
			  obj_get_y(obj));
        abobj_instantiate_changes(obj); 
    }
}

/*
 * align_top_edges
 */
static void
align_top_edges(
    ABObj    *sel_list,
    int       sel_count
)
{
	int	i;
	ABObj	obj;
	int	y = MAXINT;

	for (i = 0; i < sel_count; i++) 
	{
	    obj = sel_list[i];
	    if (obj_get_y(obj) < y)
		y = obj_get_y(obj);
	}

	for (i = 0; i < sel_count; i++) 
	{
	    obj = sel_list[i];
		/*
		undo_record_move(obj);
		*/
    	    abobj_set_xy(obj, obj_get_x(obj), y);
            abobj_instantiate_changes(obj); 
	}
}


/*
 * align_hcenters
 */
static void
align_hcenters(
    ABObj    *sel_list,
    int      sel_count
)
{
    int	     i;
    ABObj    obj;
    int	     y = 0;

    for (i = 0; i < sel_count; i++) 
    {
        obj = sel_list[i];
    	y += obj_get_y(obj) + abobj_get_actual_height(obj) / 2;
    }

    y /= sel_count;

    for (i = 0; i < sel_count; i++) 
    {
        obj = sel_list[i];
	/*
    	undo_record_move(obj);
	*/
    	abobj_set_xy(obj, obj_get_x(obj), y - abobj_get_actual_height(obj)/2);
        abobj_instantiate_changes(obj); 
    }
}

/*
 * align_bottom_edges
 */
static void
align_bottom_edges(
    ABObj    *sel_list,
    int       sel_count
)
{
	int	i;
	ABObj	obj;
	int	y = -1;
	int	ytmp;

	for (i = 0; i < sel_count; i++) 
	{
	    obj = sel_list[i];
	    ytmp = obj_get_y(obj) + abobj_get_actual_height(obj);
		if (ytmp > y)
			y = ytmp;
	}

	for (i = 0; i < sel_count; i++) {
	    obj = sel_list[i];
		/*
		undo_record_move(obj);
		*/
  	    abobj_set_xy(obj, obj_get_x(obj),  y - abobj_get_actual_height(obj));
            abobj_instantiate_changes(obj); 
	}
}


/*
 * distribute_horizontal
 */
static void
distribute_horizontal(
    ABObj    *sel_list,
    int       sel_count,
    int	      hspacing
)
{
    int	    i;
    ABObj	   obj;
    ABObj	   prev;

    /*
     * Sort list of objects based on X values
     */
    abobj_sort_sel_list(sel_list, sel_count, XSORT);

    /*
     * Walk through the list and distribute the objects.
     */
    for (i = 1; i < sel_count; i++) 
    {
	obj = sel_list[i];
	prev = sel_list[i-1];
	/*
	undo_record_move(obj);
	*/
	abobj_set_xy(obj, obj_get_x(prev) + abobj_get_actual_width(prev) + 
		   hspacing, obj_get_y(obj));
	abobj_instantiate_changes(obj); 

    }
}

/*
 * distribute_vertical
 */
static void
distribute_vertical(
    ABObj    *sel_list,
    int       sel_count,
    int	      vspacing
)
{
    int	i;
    ABObj	obj;
    ABObj	prev;

    /*
     * Sort list of objects based on Y values
     */
    abobj_sort_sel_list(sel_list, sel_count, YSORT);

    /*
     * Walk through the list and distribute the objects.
     */
    for (i = 1; i < sel_count; i++) 
    {
	    obj = sel_list[i];
	    prev = sel_list[i-1];
	    /*
	    undo_record_move(obj);
	    */
	    abobj_set_xy(obj, obj_get_x(obj), obj_get_y(prev) + 
		      abobj_get_actual_height(prev) + vspacing);
	    abobj_instantiate_changes(obj); 
    }
}

/*
 * center_horizontal
 *
 * Center selected objects horizontally within their parent
 */
static void
center_horizontal(
    ABObj    *sel_list,
    int       sel_count
)
{
    ABObj	owner_obj = obj_get_parent(sel_list[0]);
    int		ownerwidth = abobj_get_actual_width(owner_obj);
    XRectangle	current_rect;
    XRectangle	new_rect;

    abobj_get_rect_for_objects(sel_list, sel_count, &current_rect);

    new_rect.x = ownerwidth / 2 - current_rect.width / 2;
    new_rect.y = current_rect.y;

    abobj_move_selected(sel_list, sel_count, &current_rect, &new_rect);
}

/*
 * center_vertical
 *
 * Center selected objects vertically within their parent
 */
static void
center_vertical(
    ABObj    *sel_list,
    int       sel_count
)
{
    ABObj	owner_obj = obj_get_parent(sel_list[0]);
    int		owneheight = abobj_get_actual_height(owner_obj);
    XRectangle	current_rect;
    XRectangle	new_rect;

    abobj_get_rect_for_objects(sel_list, sel_count, &current_rect);

    new_rect.x = current_rect.x;
    new_rect.y = owneheight / 2 - current_rect.height / 2;
    abobj_move_selected(sel_list, sel_count, &current_rect, &new_rect);
}
