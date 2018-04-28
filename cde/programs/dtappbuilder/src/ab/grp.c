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
 *	$XConsortium: grp.c /main/3 1995/11/06 17:30:32 rswiston $
 *
 * @(#)grp.c	1.2 18 Jan 1994
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


#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <ab_private/ab.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h> 
#include <ab_private/abobj_set.h> 
#include <ab_private/proj.h> 
#include <ab_private/ui_util.h>
#include <ab_private/x_util.h>
#include <ab_private/conn.h> 

/*
 * Methods
 */
static int      group_initialize(
                    ABObj   obj
                );

static void     reparent_children(
		    ABObj       newparent, 
		    ABObj	*sel_list,
		    int		sel_count
                );

/*************************************************************************
**                                                                      **
**       Data 		                                                **
**                                                                      **
**************************************************************************/
PalItemInfo group_palitem_rec = {
    /* type             */  AB_TYPE_CONTAINER,
    /* name             */  "Group",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  group_initialize,
    /* is_a_test        */  obj_is_group
};
#ifdef TEMPOUT
    /* prop_initialize  */  group_prop_init,
    /* prop_clear	*/  group_prop_clear,
    /* prop_load        */  group_prop_load,
    /* prop_apply	*/  group_prop_apply
#endif /*TEMPOUT */

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
void
grp_align_left(
    ABObj obj
)
{
}

/*
 * Create a group object out of the selected group of objects.
 */
void	
grp_selected_objects(
)
{
    ABObj	  project = proj_get_project();
    ABObj         newgroup;
    ABObj	  obj_parent;
    Window	  xy_win;
    int	     	  wx, wy;
    ABSelectedRec sel;
    XRectangle    current_rect;

	
    abobj_get_selected(project, FALSE, &sel);
    abobj_get_rect_for_selected(sel.list, sel.count, &current_rect);
    obj_parent = obj_get_parent(sel.list[0]);

    /* Creation may take awhile, so set busy cursor */
    xy_win = x_xwin_at_rootxy(AB_toplevel, current_rect.x, 
		    current_rect.y, &wx, &wy);
    ui_set_busy_cursor(xy_win, TRUE);

    newgroup = obj_create(AB_TYPE_CONTAINER, obj_parent);
    obj_set_subtype(newgroup, AB_CONT_GROUP);

    if (group_initialize(newgroup) == ERROR)
	fprintf(stderr, "create_obj_action: couldn't initialize object\n");
    else if (abobj_show_tree(newgroup) == -1)
	fprintf(stderr,"create_obj_action: couldn't show object\n");
    else
    {
    	reparent_children(newgroup, sel.list, sel.count);

        obj_tree_clear_flag(newgroup, InstantiatedFlag);
        abobj_show_tree(newgroup);

	/* Deselect any objects that happen to be selected */
	abobj_deselect_all(project);

	/* Make the new obj selected */
	abobj_select(newgroup);
    }

    aob_deselect_all_objects(project);

    /* Restore to original cursor */
    ui_set_busy_cursor(xy_win, FALSE);

    grp_align_left(newgroup);
}

static int
group_initialize(
    ABObj    obj
)
{
    ABObj	  iobj, mobj;
    int		  i;
    ABObj	  parent = obj_get_parent(obj);
    ABSelectedRec sel;
    XRectangle    current_rect;


    abobj_get_selected(proj_get_project(), FALSE, &sel);
    abobj_get_rect_for_selected(sel.list, sel.count, &current_rect);

    if (obj_get_name(obj) == NULL)
    	obj_set_name(obj, "group");

    obj_ensure_unique_name(obj, obj_get_module(obj), -1); 
    obj->initial_state = AB_STATE_ACTIVE;
    obj->x = current_rect.x;
    obj->y = current_rect.y;
    obj->width = current_rect.width;
    obj->height = current_rect.height;
	
    /* Set up Default Layout for group*/
    obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);

    return OK;
}

static void
reparent_children(
    ABObj       newparent, 
    ABObj	*sel_list,
    int		sel_count
)
{
    int   i;
    ABObj obj; 
    XRectangle    current_rect;

    abobj_get_rect_for_selected(sel_list, sel_count, &current_rect);

    /*
     * Sort list of objects based on X and Y values
    abobj_sort_sel_list(sel_list, sel_count, XSORT);
    abobj_sort_sel_list(sel_list, sel_count, YSORT);
     */

    for (i = 0; i < sel_count; i++) 
    {
	
	obj = sel_list[i];
	obj_reparent(obj, newparent); 
	abobj_calculate_new_layout(obj, obj->x - current_rect.x, 
				   obj->y - current_rect.y,
				   (Dimension) obj->width,
				   (Dimension) obj->height);

    }
}

