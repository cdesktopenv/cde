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
 *      $XConsortium: pal_group.c /main/6 1996/08/30 14:01:29 mustafa $
 *
 *	@(#)pal_group.c	1.58 08 May 1995
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
 * pal_group.c - Implements Group object functionality
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <ab_private/ab.h>
#include <ab_private/trav.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/brws.h>
#include <ab_private/abobj.h>
#include <ab_private/objxm.h>
#include <ab_private/abobj_set.h>
#include <ab_private/proj.h>
#include <ab_private/ui_util.h>
#include <ab_private/x_util.h>
#include <ab_private/abobj_edit.h>
#include "dtbuilder.h"
#include "abobjP.h"
#include "group_ui.h"


/*
 * Bitmaps
 */
#include "bitmaps/ggp_as_is.xbm"
#include "bitmaps/ggp_col.xbm"
#include "bitmaps/ggp_row.xbm"
#include "bitmaps/ggp_rowcol.xbm"
#include "bitmaps/align_top.xbm"
#include "bitmaps/align_hcenter.xbm"
#include "bitmaps/align_bottom.xbm"
#include "bitmaps/align_left.xbm"
#include "bitmaps/align_labels.xbm"
#include "bitmaps/align_vcenter.xbm"
#include "bitmaps/align_right.xbm"

/*
 * Defines
 */

#define HOFFSET 10
#define VOFFSET 8
#define NUMROWCOL 2

typedef struct  PROP_GROUP_SETTINGS
{
    Widget			prop_sheet;
    PropFieldSettingRec		name;
    PropOptionsSettingRec	frame;
    PropRadioSettingRec		layout_type;
    PropRadioSettingRec		grid_rowcol;
    PropFieldSettingRec		grid_rowcol_count;
    PropOptionsSettingRec	valign;
    PropFieldSettingRec		vspacing;
    PropOptionsSettingRec	halign;
    PropFieldSettingRec		hspacing;
    PropGeometrySettingRec	pos;
    PropCheckboxSettingRec	init_state;
    PropColorSettingRec         bg_color;
    ABObj			current_obj;
} PropGroupSettingsRec, *PropGroupSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      group_initialize(
                    ABObj   obj
                );
static Widget   group_prop_init(
                    Widget  parent,
		    AB_PROP_TYPE type
                );

static int	group_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int      group_prop_load(
                    ABObj   	 obj,
		    AB_PROP_TYPE type,
		    unsigned long loadkey
                );
static int	group_prop_clear(
		    AB_PROP_TYPE type
		);
static int	group_prop_apply(
		    AB_PROP_TYPE type
		);

static BOOL	group_prop_pending(
		    AB_PROP_TYPE type
		);

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);
static void	setup_grouptype_settings(
		    AB_PROP_TYPE type,
		    AB_GROUP_TYPE gtype
		);
static void	set_new_group_size(
		    ABObj obj
		);

static void	group_objects(
		    ABObj	newgroup,
		    ABObj	*obj_list,
		    int		count
		);

static void	ungroup_objects(
		    ABObj	*obj_list,
		    int		count
		);

static void	group_align_tops(
		    ABObj   obj
		);

static void	group_align_hcenters(
		    ABObj   obj,
		    BOOL    init
		);

static void	group_align_horiz_same_size(
		    ABObj   obj
		);

static void	group_align_bottoms(
		    ABObj   obj
		);

static void	group_align_left(
		    ABObj   obj
		);

static void	group_align_labels(
		    ABObj   obj
		);

static void	group_align_vcenters(
		    ABObj   obj,
		    BOOL    init
		);

static void	group_align_right(
		    ABObj   obj
		);

static void 	create_member_list(
    		   ABObj  cobj,
    		   ABObj  **member_list,
    		   int    *member_count
 	 	);

static void	get_cell_size(
		    ABObj	group,
		    int		*cell_width,
		    int		*cell_height
		);

static void	get_row_col(
		    ABObj	group,
		    int		*rows,
		    int		*cols
		);

static void	group_align_rows(
		    ABObj	obj,
		    BOOL	init
		);

static void	group_align_cols(
		    ABObj	obj,
		    BOOL	init
		);

static void	get_widest_label_obj(
		    ABObj	*list,
		    int		count,
		    ABObj	*widest_label,
		    int		*label_width
		);

static void	get_widest_value_obj(
		    ABObj	*list,
		    int		count,
		    ABObj	*widest_value,
		    int		*value_width
		);

/*
 * Callbacks
 */
static void	grouptypeCB(
		    Widget   widget,
		    XtPointer clientdata,
                    XmToggleButtonCallbackStruct *state
		);

static void	undo_group(
		    ABUndoRec	undo_rec
		);

static void	undo_ungroup(
		    ABUndoRec	undo_rec
		);

static ABObj	get_child(
		    ABObj	group,
		    int		x_pos,
		    int		y_pos
		);

static void	group_expose_handler(
		    Widget      widget,
		    XtPointer   client_data,
		    XEvent      *event,
		    Boolean     *cont_dispatch
		);

DtbGroupPropDialogInfoRec	rev_group_prop_dialog;

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
    /* is_a_test        */  obj_is_group,
    /* prop_initialize  */  group_prop_init,
    /* prop_active	*/  group_prop_activate,
    /* prop_clear	*/  group_prop_clear,
    /* prop_load        */  group_prop_load,
    /* prop_apply	*/  group_prop_apply,
    /* prop_pending	*/  group_prop_pending
};

PalItemInfo *ab_group_palitem = &group_palitem_rec;
PropGroupSettingsRec prop_group_settings_rec[AB_PROP_TYPE_NUM_VALUES];

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * Create a group object out of the selected group of objects.
 */
void
abobj_group_selected_objects(void)
{
    ABObj	  project = proj_get_project();
    ABObj 	  obj;
    ABObj         newgroup;
    ABObj	  obj_parent;
    ABSelectedRec sel;
    XRectangle    current_rect;
    int		  i;

    /* Creation may take awhile, so set busy cursor */
    ab_set_busy_cursor(True);

    abobj_get_selected(project, False, False, &sel);
    abobj_get_rect_for_objects(sel.list, sel.count, &current_rect);
    obj_parent = obj_get_parent(sel.list[0]);

    newgroup = obj_create(AB_TYPE_CONTAINER, obj_parent);
    obj_set_subtype(newgroup, AB_CONT_GROUP);

    if (group_initialize(newgroup) == ERROR)
	fprintf(stderr, "create_obj_action: couldn't initialize object\n");
    else if (abobj_show_tree(newgroup, False) == -1)
	fprintf(stderr,"create_obj_action: couldn't show object\n");
    else
    {

	/* Deselect any objects that happen to be selected */
	abobj_deselect_all(project);
        aob_deselect_all_objects(project);

	group_objects(newgroup, sel.list, sel.count);

	/*
	 * Can only create one group at a time, therefore
	 * only worry about undoing one group.
	 */
	abobj_set_undo(&newgroup, 1, undo_group, AB_UNDO_GROUP);

    }

    /* Restore to original cursor */
    ab_set_busy_cursor(False);

}

static void
group_objects(
    ABObj	newgroup,
    ABObj	*list,
    int		count
)
{
    ABObj 	  obj;
    ABSelectedRec sel;
    XRectangle    current_rect;
    int		  i;

    abobj_get_rect_for_objects(list, count, &current_rect);

    /*
    * Reparent each selected object and set x,y to group
    * coordinates.
    */
    for (i = 0; i < count; i++)
    {
	AB_TRAVERSAL	trav;
	ABObj		cur_obj = NULL;

        obj = list[i];
        obj_reparent(obj, newgroup);
        abobj_set_xy(obj, obj_get_x(obj) - current_rect.x,
        		  obj_get_y(obj) - current_rect.y);
	/*
	 * Clear instantiated flag in child
	 */
	obj_clear_flag(obj, InstantiatedFlag);

	/*
	 * Clear instantiated flag in child subtree as
	 * well
	 */
        for (trav_open(&trav, obj, AB_TRAV_ALL);
                (cur_obj = trav_next(&trav)) != NULL; )
	{
	    obj_clear_flag(cur_obj, InstantiatedFlag);
	}
	trav_close(&trav);

    }

    /*
     * For some reason, width and height need to be set on the
     * group.
     */
/*
    obj_set_width(newgroup, current_rect.width);
    obj_set_height(newgroup, current_rect.height);
*/

    abobj_tree_instantiate_changes(newgroup);

    /* Make the new obj selected */
    abobj_select(newgroup);
}



/*
 * Ungroup a group. Get list of selected objects, look for groups
 * in the list. If found, traverse throught the groups children and
 * re-parent/re-position the child. Finally, keep a list of new parents
 * to be updated (so that no parent is updated more than once) and update
 * them after all groups have been ungrouped.
 */
void
abobj_ungroup_selected_objects(void)
{
    ABObj	  project = proj_get_project();
    ABSelectedRec sel;

    /* Ungrouping may take awhile, so set busy cursor */
    ab_set_busy_cursor(True);

    /* Get list of selected objects */
    abobj_get_selected(project, False, False, &sel);

    /*
     * Remember all objects that were ungrouped so can undo this later
     */
    abobj_set_undo(sel.list, sel.count, undo_ungroup, AB_UNDO_UNGROUP);

    ungroup_objects(sel.list, sel.count);

    /* Restore to original cursor */
    ab_set_busy_cursor(False);
}

/*
 * Ungroups passed objects. Look for groups in the list. If found,
 * traverse throught the groups children and re-parent/re-position the
 * child. Finally, keep a list of new parents to be updated so that
 * no parent is updated more than once and update them after all groups
 * have been ungrouped.
 */
static void
ungroup_objects(
    ABObj	*obj_list,
    int		count
)
{
    ABObj	  group;
    ABObj	  group_parent;
    ABObj	  parent_list = NULL;
    XRectangle    group_rect,
			rect;
    AB_TRAVERSAL  trav;
    ABObj	  member;
    int		  i, j;
    ABObj	  *list = NULL;
    int		  list_items;
    Boolean	  parent_exists = True;

    if (!obj_list || (count <= 0))
	return;

    for (i = 0; i < count; i++)
    {
	group = obj_list[i];
	if (obj_is_group(group))
	{
	    ABObj	oobj = NULL,
			*member_list = NULL;
	    int		num_children,
			cur_member = 0;

	    abobj_deselect(group);
	    group_parent = obj_get_parent(group);

	    /*
	     * Remember group's position
	     */
	    group_rect.x = group_rect.y = 0;
	    x_get_widget_rect(objxm_get_widget(group), &group_rect);

	    oobj = objxm_comp_get_subobj(group, AB_CFG_OBJECT_OBJ);
	    num_children = obj_get_num_children(oobj);

	    member_list = (ABObj *)util_malloc(num_children * sizeof(ABObj*));

	    /*
	     * Group members are all direct children of the AB_CFG_OBJECT_OBJ
	     * object
	     */
	    for (trav_open(&trav, oobj, AB_TRAV_CHILDREN |
			    AB_TRAV_MOD_SAFE);
		(member = trav_next(&trav)) != NULL; )
	    {
		Widget	member_w;

		member_w = objxm_get_widget(member);

		/*
		 * Get member's current position
		 */
	        rect.x = rect.y = 0;
		x_get_widget_rect(member_w, &rect);

		obj_reparent(member, group_parent);

		/*
		 * Set attachments with 0 x and y. abobj_set_xy will
		 * set the correct values.
		*/
	        obj_init_attachments(member);
		obj_set_attachment(member, AB_CP_NORTH, AB_ATTACH_POINT,
					NULL, 0);
		obj_set_attachment(member, AB_CP_WEST,  AB_ATTACH_POINT,
					NULL, 0);

		/*
		 * Set new position to be:
		 *	group's (x, y) + current (x, y)
		 */
		abobj_set_xy(member, group_rect.x + rect.x,
				     group_rect.y + rect.y);

		/*
		 * Keep track of objects to re-instantiate later
		 * We don't do it here because we don't want to screw up
		 * the x_get_widget_rect() calls in the current group.
		 */
		member_list[cur_member++] = member;

	    }
	    trav_close(&trav);

	    /*
	     * Un/re-instantiate former group members
	     */
	    for (cur_member = 0; cur_member < num_children; ++cur_member)
	    {
		ABObj		cur_obj = NULL;

		/*
	 	 * Clear instantiated flag in former group member
	 	 */
                obj_clear_flag(member_list[cur_member], InstantiatedFlag);

		/*
	 	 * Clear instantiated flag in former group member's subtree
	 	 * as well
	 	 */
                for (trav_open(&trav, member_list[cur_member], AB_TRAV_ALL);
                    (cur_obj = trav_next(&trav)) != NULL; )
                {
                    obj_clear_flag(cur_obj, InstantiatedFlag);
                }

		/*
		 * The manage_lat parameter used to be FALSE.
		 * It caused list objects to get the wrong size when
		 * ungroup was done. Changing to TRUE somehow fixes
		 * this.
		 */
	        abobj_show_tree(member_list[cur_member], TRUE);
	    }

	    util_free(member_list);

	    obj_destroy(group);

	   /*
	    * Keep a list of parents (control panels) that need
	    * to be updated after all groups are ungrouped.
	    */
	    if (list == NULL)
	    {
	    	list = (ABObj*)XtMalloc(sizeof(ABObj));
		list[0] = group_parent;
		list_items = 1;
	    }
	    else
	    for (j = 0; j < list_items; j++)
	    {
		if (list[j] == group_parent)
		    {
			parent_exists = True;
			break;
		    }
	    }
	    if (parent_exists == False)
	    {
		XtRealloc((char*)list, sizeof(ABObj));
		list[list_items++] = group_parent;
	    }
	    parent_exists = False;
	}
    }
}


static int
group_initialize(
    ABObj    obj
)
{
    ABObj	   iobj, mobj;
    AB_GROUP_TYPE group_type;
    int		   i;
    ABObj	   parent = obj_get_parent(obj);
    ABSelectedRec  sel;
    XRectangle     current_rect;


    abobj_get_selected(proj_get_project(), False, False, &sel);
    abobj_get_rect_for_objects(sel.list, sel.count, &current_rect);

    obj_set_unique_name(obj, "group");

/*
    abobj_set_initial_state(obj, AB_STATE_ACTIVE);
*/
    obj_set_is_initially_active(obj, True);

    obj_set_x(obj, current_rect.x);
    obj_set_y(obj, current_rect.y);
    /*
    obj_set_width(obj, current_rect.width);
    obj_set_height(obj, current_rect.height);
    */

    obj_set_group_type(obj, AB_GROUP_IGNORE);

    /* Set up Default Layout for group*/
    obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);

    return OK;
}



static Widget
group_prop_init(
    Widget    parent,
    AB_PROP_TYPE type
)
{
    /* Revolving Props */
    /*
    DtbGroupPropDialogInfoRec	rev_group_prop_dialog;
    */
    DtbGroupPropDialogInfo	cgen = &dtb_group_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropGroupSettingsRec    	*pgs = &(prop_group_settings_rec[type]);
    Widget			item[10];
    int				item_val[10];
    int				n;
    int				i;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbGroupPropDialogInfo_clear(&rev_group_prop_dialog);

	cgen = &(rev_group_prop_dialog);
	cgen->prop_dialog = rpd->prop_dialog;
        cgen->prop_dialog_shellform = rpd->prop_dialog_shellform;
	cgen->prop_dialog_panedwin = rpd->prop_dialog_panedwin;
	cgen->prop_dialog_form = rpd->prop_dialog_form;
	cgen->objlist_panel = rpd->objlist_panel;
	cgen->objlist_label = rpd->objlist_label2;
	cgen->objlist_scrolledwin = rpd->objlist_scrolledwin;
	cgen->objlist = rpd->objlist;
	cgen->attrs_ctrlpanel_frame = rpd->attrs_ctrlpanel_frame;
	cgen->activate_panel = rpd->activate_panel;
	cgen->apply_button = rpd->apply_button;
        cgen->ok_button = rpd->ok_button;
        cgen->cancel_button = rpd->cancel_button;
	cgen->reset_button = rpd->reset_button;
	cgen->help_button = rpd->help_button;

    }
    else /* AB_PROP_FIXED */
    	cgen = &dtb_group_prop_dialog;

    if (dtb_group_prop_dialog_initialize(cgen, parent) == 0)
    {
        pgs->prop_sheet = cgen->attrs_ctrlpanel;
        pgs->current_obj = NULL;

	if (type == AB_PROP_REVOLVING)
        	XtVaSetValues(parent,
                        XmNuserData, pgs->current_obj,
                        NULL);

        /* Dialog/Object List */
	if (type == AB_PROP_FIXED)
	{
            prop_fixed_dialog_init(ab_group_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_group_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

	/* Alternate Editor Buttons */
	prop_editors_panel_init(type, ab_group_palitem,
		    cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

	/*
	  * Prop Sheet Settings....
	 */

	/* Name */
	prop_field_init(&(pgs->name), cgen->name_field_label,
			    cgen->name_field, cgen->name_cb);

	/* Border Frame Setting */
	n = 0;
	item[n] = cgen->bframe_opmenu_items.None_item;
	item_val[n] = AB_LINE_NONE; n++;
	item[n] = cgen->bframe_opmenu_items.Shadow_Out_item;
	item_val[n] = AB_LINE_SHADOW_OUT; n++;
	item[n] = cgen->bframe_opmenu_items.Shadow_In_item;
	item_val[n] = AB_LINE_SHADOW_IN; n++;
	item[n] = cgen->bframe_opmenu_items.Etched_Out_item;
	item_val[n] = AB_LINE_ETCHED_OUT; n++;
	item[n] = cgen->bframe_opmenu_items.Etched_In_item;
	item_val[n] = AB_LINE_ETCHED_IN; n++;
	prop_options_init(&(pgs->frame), cgen->bframe_opmenu_label,
			    cgen->bframe_opmenu, cgen->bframe_opmenu_menu,
			    n, item, (XtPointer*)item_val,
			    cgen->bframe_cb);

	/* Layout Type */
        n = 0;
        item[n] = cgen->layout_rbox_items.bitmaps_ggp_as_is_xbm_item;
        item_val[n] = AB_GROUP_IGNORE; n++;
        item[n] = cgen->layout_rbox_items.bitmaps_ggp_col_xbm_item;
        item_val[n] = AB_GROUP_COLUMNS; n++;
        item[n] = cgen->layout_rbox_items.bitmaps_ggp_row_xbm_item;
        item_val[n] = AB_GROUP_ROWS; n++;
        item[n] = cgen->layout_rbox_items.bitmaps_ggp_rowcol_xbm_item;
        item_val[n] = AB_GROUP_ROWSCOLUMNS; n++;
	prop_radiobox_init(&(pgs->layout_type), cgen->layout_rbox_label,
			cgen->layout_rbox, n, item, (XtPointer*)item_val,
			cgen->layout_cb1);

	for (i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                        (XtCallbackProc)grouptypeCB, (XtPointer)type);

	/*
	ui_set_active(cgen->layout_rbox_items.bitmaps_ggp_rowcol_xbm_item, False);
	*/

	/* Grid Row/Columns */
        n = 0;
        item[n] = cgen->gridrowcol_rbox_items.Rows_item;
        item_val[n] = AB_ORIENT_HORIZONTAL; n++;
        item[n] = cgen->gridrowcol_rbox_items.Columns_item;
        item_val[n] = AB_ORIENT_VERTICAL; n++;
	prop_radiobox_init(&(pgs->grid_rowcol), NULL,
			cgen->gridrowcol_rbox, n, item, (XtPointer*)item_val,
			cgen->gridrowcol_cb);

	/* Grid Row/Column count */
	prop_field_init(&(pgs->grid_rowcol_count), NULL,
			cgen->gridrowcol_field, cgen->gridrowcol_cb);

        /* Vertical Alignment */
        n = 0;
        item[n] = cgen->valign_opmenu_items.bitmaps_align_left_xbm_item;
        item_val[n] = AB_ALIGN_LEFT; n++;
        item[n] = cgen->valign_opmenu_items.bitmaps_align_labels_xbm_item;
        item_val[n] = AB_ALIGN_LABELS; n++;
        item[n] = cgen->valign_opmenu_items.bitmaps_align_vcenter_xbm_item;
        item_val[n] = AB_ALIGN_VCENTER; n++;
        item[n] = cgen->valign_opmenu_items.bitmaps_align_right_xbm_item;
        item_val[n] = AB_ALIGN_RIGHT; n++;
	prop_options_init(&(pgs->valign), cgen->valign_opmenu_label,
			    cgen->valign_opmenu, cgen->valign_opmenu_menu,
			    n, item, (XtPointer*)item_val,
			    cgen->valign_cb);

        /* Vertical spacing value */
        prop_field_init(&(pgs->vspacing), NULL,
                        cgen->vert_spacing_field, cgen->valign_cb);

        /* Horizontal Alignment */
        n = 0;
        item[n] = cgen->halign_opmenu_items.bitmaps_align_top_xbm_item;
        item_val[n] = AB_ALIGN_TOP; n++;
        item[n] = cgen->halign_opmenu_items.bitmaps_align_hcenter_xbm_item;
        item_val[n] = AB_ALIGN_HCENTER; n++;
        item[n] = cgen->halign_opmenu_items.bitmaps_align_bottom_xbm_item;
        item_val[n] = AB_ALIGN_BOTTOM; n++;
	prop_options_init(&(pgs->halign), cgen->halign_opmenu_label,
			    cgen->halign_opmenu, cgen->halign_opmenu_menu,
			    n, item, (XtPointer*)item_val,
			    cgen->halign_cb);

        /* Horizontal spacing value */
        prop_field_init(&(pgs->hspacing), NULL,
                        cgen->horiz_spacing_field, cgen->halign_cb);

	/* Position */
	prop_geomfield_init(&(pgs->pos), cgen->pos_label,
			    cgen->x_field_label, cgen->x_field,
			    cgen->y_field_label, cgen->y_field,
			    NULL, NULL, NULL, NULL,
			    cgen->pos_cb);

	/* Initial State */
	n = 0;
	item[n] = cgen->istate_ckbox_items.Visible_item;
	item_val[n] = AB_STATE_VISIBLE; n++;
	item[n] = cgen->istate_ckbox_items.Active_item;
	item_val[n] = AB_STATE_ACTIVE; n++;
	prop_checkbox_init(&(pgs->init_state), cgen->istate_ckbox_label,
			    cgen->istate_ckbox, n, item, item_val,
			    cgen->istate_cb);

	/* Color */
	prop_colorfield_init(&(pgs->bg_color), cgen->bg_mbutton,
                cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
                cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
		cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

	prop_changebars_cleared(pgs->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
	return NULL;

}

static int
group_prop_clear(
    AB_PROP_TYPE type
)
{
    PropGroupSettingsRec	*pgs = &(prop_group_settings_rec[type]);

    if (pgs->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pgs->name), "", False);

    /* Clear Border Frame */
    prop_options_set_value(&(pgs->frame), (XtPointer)AB_LINE_NONE, False);

    /* Clear Layout Type */
    prop_radiobox_set_value(&(pgs->layout_type),
                            (XtPointer)AB_GROUP_IGNORE, False);

    /* Clear Grid Rows/Columns */
    prop_radiobox_set_value(&(pgs->grid_rowcol),
                            (XtPointer)AB_ORIENT_HORIZONTAL, False);
    prop_field_set_numeric_value(&(pgs->grid_rowcol_count), NUMROWCOL, False);

    /* Clear Vertical Alignment */
    prop_options_set_value(&(pgs->valign), (XtPointer)AB_ALIGN_LEFT, False);
    prop_field_set_numeric_value(&(pgs->vspacing), 0, False);

    /* Clear Horizontal Alignment */
    prop_options_set_value(&(pgs->halign), (XtPointer)AB_ALIGN_TOP, False);
    prop_field_set_numeric_value(&(pgs->hspacing), 0, False);

    /* Clear Object Size */

    /* Clear Object Resizing */

    /* Clear Position */
    prop_geomfield_clear(&(pgs->pos), GEOM_X);
    prop_geomfield_clear(&(pgs->pos), GEOM_Y);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pgs->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pgs->init_state), AB_STATE_ACTIVE, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pgs->bg_color), "", False);

    pgs->current_obj = NULL;

    turnoff_changebars(type);

    return OK;
}



static int
group_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_group_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
group_prop_load(
    ABObjPtr 	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropGroupSettingsRec 	*pgs = &(prop_group_settings_rec[type]);
    AB_GROUP_TYPE 		gtype;
    BOOL                        load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pgs->current_obj != NULL)
            obj = pgs->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_group(obj))
        return ERROR;
    else
        pgs->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pgs->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Border Frame */
	prop_options_set_value(&(pgs->frame), (XtPointer)obj_get_border_frame(obj), False);

	/* Load Layout Type */
	gtype = obj_get_group_type(obj);
	prop_radiobox_set_value(&(pgs->layout_type), (XtPointer)gtype, False);
	setup_grouptype_settings(type, gtype);

	/* Load Alignment, Grid-Rows/Columns */
	switch(gtype)
	{
/*
	case AB_GROUP_IGNORE:
		prop_radiobox_set_value(&(pgs->spacing_type),
				AB_ATTACH_OBJ, False);
		prop_field_set_numeric_value(&(pgs->spacing),
			0, False);
		break;
*/

	case AB_GROUP_ROWS:
		prop_options_set_value(&(pgs->halign),
				(XtPointer)obj_get_row_align(obj), False);

		prop_field_set_numeric_value(&(pgs->hspacing),
				obj_get_hoffset(obj), False);
		break;

	case AB_GROUP_COLUMNS:
		prop_options_set_value(&(pgs->valign),
				(XtPointer)obj_get_col_align(obj), False);

		prop_field_set_numeric_value(&(pgs->vspacing),
				obj_get_voffset(obj), False);
		break;
	case AB_GROUP_ROWSCOLUMNS:
		if (obj_get_num_rows(obj) ||
			(!obj_get_num_rows(obj) &&
				!obj_get_num_columns(obj)))
		{
		    prop_radiobox_set_value(&(pgs->grid_rowcol),
				(XtPointer)AB_ORIENT_HORIZONTAL, False);
		    prop_field_set_numeric_value(&(pgs->grid_rowcol_count),
				obj_get_num_rows(obj), False);
		}
		else
		{
		    prop_radiobox_set_value(&(pgs->grid_rowcol),
				(XtPointer)AB_ORIENT_VERTICAL, False);
	   	    prop_field_set_numeric_value(&(pgs->grid_rowcol_count),
				obj_get_num_columns(obj), False);
		}

		prop_options_set_value(&(pgs->halign),
				(XtPointer)obj_get_row_align(obj), False);

		prop_field_set_numeric_value(&(pgs->hspacing),
				obj_get_hoffset(obj), False);

		prop_options_set_value(&(pgs->valign),
				(XtPointer)obj_get_col_align(obj), False);

		prop_field_set_numeric_value(&(pgs->vspacing),
				obj_get_voffset(obj), False);
		break;

	}

	/* Load Object Size */

	/* Load Object Resizing */

	/* Load Initial State */
	prop_checkbox_set_value(&(pgs->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);
    	prop_checkbox_set_value(&(pgs->init_state), AB_STATE_ACTIVE,
		obj_is_initially_active(obj), False);

    	/* Load Color */
    	prop_colorfield_set_value(&(pgs->bg_color), obj_get_bg_color(obj), False);

    	turnoff_changebars(type);
    }

    /* Load Position */
    if (load_all || loadkey & LoadPosition)
        prop_load_obj_position(obj, &(pgs->pos));

    return OK;
}


static int
group_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropGroupSettingsRec 	*pgs = &(prop_group_settings_rec[type]);
    STRING	    		value;
    AB_GROUP_TYPE   		gtype;
    int		    		num_rc;
    int		    		space_val;
    int		    		i;
    ABObj	    		obj;
    BOOL	    		changed = False;
    BOOL			reset_bg = False;

    obj = pgs->current_obj;
    gtype = (AB_GROUP_TYPE)prop_radiobox_get_value(&(pgs->layout_type));

    if (obj == NULL)
        return ERROR;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pgs->name.changebar))
    {
	value = prop_field_get_value(&(pgs->name));
        abobj_set_name(pgs->current_obj, value);
        util_free(value);
    }
    /*
     * Set rows/columns based on new layout type. If we are
     * changing from AB_GROUP_IGNORE to something else, we need
     * to sort the children so that the group comes out looking
     * close to the initial layout, regardless of the order of
     * creation.
     */
    if (prop_changed(pgs->layout_type.changebar))
    {
        changed = True;

        switch (gtype)
        {
            case AB_GROUP_IGNORE:
                abobj_set_num_rows(obj, 0);
                abobj_set_num_columns(obj, 0);

                break;

            case AB_GROUP_ROWS:
                abobj_set_num_rows(obj, 1);
                abobj_set_num_columns(obj, 0);
                if (obj_get_group_type(obj) == AB_GROUP_IGNORE)
                        abobj_sort_children(obj, XSORT);
                /*
                 * If changing types, set defaults for row stuff
                 * in case the user doesn't change them.

                if (obj_get_group_type(obj) != AB_GROUP_ROWS)
                {
                    abobj_set_row_alignment(obj, AB_ALIGN_TOP);
                    abobj_set_row_attach_type(obj, AB_ATTACH_OBJ);
                    abobj_set_row_offset(obj, HOFFSET);
                }
                 */
                break;
            case AB_GROUP_COLUMNS:
                abobj_set_num_rows(obj, 0);
                abobj_set_num_columns(obj, 1);
                if (obj_get_group_type(obj) == AB_GROUP_IGNORE)
                        abobj_sort_children(obj, YSORT);
                /*
                 * If changing types, set defaults for column stuff
                 * in case the user doesn't change them.

                if (obj_get_group_type(obj) != AB_GROUP_COLUMNS)
                {
                    abobj_set_col_alignment(obj, AB_ALIGN_LEFT);
                    abobj_set_col_attach_type(obj, AB_ATTACH_OBJ);
                    abobj_set_col_offset(obj, VOFFSET);
                }
                 */
                break;


            case AB_GROUP_ROWSCOLUMNS:

		num_rc = prop_field_get_numeric_value(&(pgs->grid_rowcol_count));

                if (prop_radiobox_get_value(&(pgs->grid_rowcol))
                                        == AB_ORIENT_HORIZONTAL)
                {
                    abobj_set_num_rows(obj, num_rc < 2 ? 2 : num_rc);
                    abobj_set_num_columns(obj, 0);
                    if (obj_get_group_type(obj) == AB_GROUP_IGNORE)
                            abobj_sort_children(obj, YSORT);
                }
                else
                {
                    abobj_set_num_columns(obj, num_rc < 2 ? 2 : num_rc);
                    abobj_set_num_rows(obj, 0);
                    if (obj_get_group_type(obj) == AB_GROUP_IGNORE)
                            abobj_sort_children(obj, XSORT);
                }
                /*
                 * If changing types, set defaults for row/column stuff
                 * in case the user doesn't change them.
                 */
                if (obj_get_group_type(obj) != AB_GROUP_ROWSCOLUMNS)
                {
		    /*
                    abobj_set_row_alignment(obj, AB_ALIGN_TOP);
                    abobj_set_row_attach_type(obj, AB_ATTACH_OBJ);
                    abobj_set_row_offset(obj, HOFFSET);

                    abobj_set_col_alignment(obj, AB_ALIGN_LEFT);
                    abobj_set_col_attach_type(obj, AB_ATTACH_OBJ);
                    abobj_set_col_offset(obj, VOFFSET);


		     * Add this later
                    abobj_set_row_alignment(obj,
			(AB_ALIGNMENT)prop_options_get_value(&(pgs->halign)));
                    abobj_set_row_attach_type(obj,
			(AB_ATTACH_TYPE)prop_options_get_value(&(pgs->hspacing_type)));
                    abobj_set_row_offset(obj,
			prop_field_get_numeric_value(&(pgs->hspacing)));

                    abobj_set_col_alignment(obj,
			(AB_ALIGNMENT)prop_options_get_value(&(pgs->valign)));
                    abobj_set_col_attach_type(obj,
			(AB_ATTACH_TYPE)prop_options_get_value(&(pgs->vspacing_type)));
                    abobj_set_col_offset(obj,
			prop_field_get_numeric_value(&(pgs->vspacing)));
		    */
                }
                break;
        }
        abobj_set_group_type(obj, gtype);
    }
    if (prop_changed(pgs->grid_rowcol_count.changebar))
    {
	/*
	 * Fix this!!
	 */
        changed = True;

        num_rc = prop_field_get_numeric_value(&(pgs->grid_rowcol_count));
        if (prop_radiobox_get_value(&(pgs->grid_rowcol))
                == AB_ORIENT_HORIZONTAL)
        {
            abobj_set_num_rows(obj, num_rc < 2 ? 2 : num_rc);
            abobj_set_num_columns(obj, 0);
            if (obj_get_group_type(obj) == AB_GROUP_IGNORE)
                abobj_sort_children(obj, YSORT);
        }
        else
        {
            abobj_set_num_columns(obj, num_rc < 2 ? 2 : num_rc);
            abobj_set_num_rows(obj, 0);
            if (obj_get_group_type(obj) == AB_GROUP_IGNORE)
                abobj_sort_children(obj, XSORT);
        }
    }
    if (prop_changed(pgs->halign.changebar))
    {
        changed = True;

        abobj_set_row_alignment(obj,
            (AB_ALIGNMENT)prop_options_get_value(&(pgs->halign)));

	space_val = prop_field_get_numeric_value(&(pgs->hspacing));

        abobj_set_row_offset(obj, space_val);
    }
    if (prop_changed(pgs->valign.changebar))
    {
        changed = True;
        abobj_set_col_alignment(obj,
            (AB_ALIGNMENT)prop_options_get_value(&(pgs->valign)));

	space_val = prop_field_get_numeric_value(&(pgs->vspacing));

        abobj_set_col_offset(obj, space_val);
    }

    if (changed)
    {
    	ABObj 	oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
	Widget	w = objxm_get_widget(oobj);
        WidgetList	children_list = NULL;
        int		num_children = 0;

        XtVaGetValues(w,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

        XtUnmanageChildren(children_list, num_children);

        abobj_layout_group(obj, FALSE);

        XtManageChildren(children_list, num_children);
    }

    /* Border Frame and Margin */
    if (prop_changed(pgs->frame.changebar))
    {
        abobj_set_border_frame(pgs->current_obj,
                (AB_LINE_TYPE)prop_options_get_value(&(pgs->frame)));
    }
    if (prop_changed(pgs->pos.changebar))
    {
        if (abobj_is_movable(pgs->current_obj))
            abobj_set_xy(pgs->current_obj,
                prop_geomfield_get_value(&(pgs->pos), GEOM_X),
                prop_geomfield_get_value(&(pgs->pos), GEOM_Y));
    }
    if (prop_changed(pgs->init_state.changebar))
    {
        abobj_set_visible(pgs->current_obj,
                prop_checkbox_get_value(&(pgs->init_state), AB_STATE_VISIBLE));
        abobj_set_active(pgs->current_obj,
                prop_checkbox_get_value(&(pgs->init_state), AB_STATE_ACTIVE));
    }
    if (prop_changed(pgs->bg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pgs->bg_color));
        abobj_set_background_color(pgs->current_obj, value);
        if (util_strempty(value))
            reset_bg = True;
        util_free(value);
        prop_colorfield_set_value(&(pgs->bg_color),
                obj_get_bg_color(pgs->current_obj), False);
    }

    abobj_tree_instantiate_changes(pgs->current_obj);

    if (reset_bg) /* Set back to No Color */
        abobj_reset_colors(pgs->current_obj, reset_bg, False);

    turnoff_changebars(type);

    return OK;
}

static BOOL
group_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_group_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropGroupSettingsRec *pgs = &(prop_group_settings_rec[type]);

    if (prop_changed(pgs->name.changebar) && !prop_name_ok(pgs->current_obj, pgs->name.field))
        return False;

    if (prop_changed(pgs->bg_color.changebar) && !prop_color_ok(pgs->bg_color.field))
        return False;

    if (prop_changed(pgs->grid_rowcol_count.changebar) &&
	    !prop_number_ok(pgs->grid_rowcol_count.field, (STRING)RowColFieldStr, 1, SHRT_MAX))
        return False;

    if (prop_changed(pgs->valign.changebar) &&
	    !prop_number_ok(pgs->vspacing.field, (STRING)VertSpacingFieldStr,
		-SHRT_MAX, SHRT_MAX))
        return False;

    if (prop_changed(pgs->halign.changebar) &&
	    !prop_number_ok(pgs->hspacing.field, (STRING)HorizSpacingFieldStr,
		-SHRT_MAX, SHRT_MAX))
        return False;


    if (prop_changed(pgs->pos.changebar) &&
            (!prop_number_ok(pgs->pos.x_field, (STRING)XFieldStr, -SHRT_MAX, SHRT_MAX) ||
            !prop_number_ok(pgs->pos.y_field, (STRING)YFieldStr, -SHRT_MAX, SHRT_MAX)))
        return False;

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropGroupSettingsRec *pgs = &(prop_group_settings_rec[type]);

    prop_set_changebar(pgs->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pgs->frame.changebar,		PROP_CB_OFF);
    prop_set_changebar(pgs->layout_type.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pgs->grid_rowcol.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pgs->valign.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pgs->halign.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pgs->pos.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pgs->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pgs->bg_color.changebar,		PROP_CB_OFF);

    prop_changebars_cleared(pgs->prop_sheet);
}


static void
grouptypeCB(
    Widget   item,
    XtPointer clientdata,
    XmToggleButtonCallbackStruct *state
)
{
    AB_PROP_TYPE	type = (AB_PROP_TYPE)clientdata;
    AB_GROUP_TYPE	value = AB_GROUP_UNDEF;

    if (state->set)
    {
        XtVaGetValues(item, XmNuserData, &value, NULL);
	setup_grouptype_settings(type, value);
    }

}


static void
setup_grouptype_settings(
    AB_PROP_TYPE type,
    AB_GROUP_TYPE gtype
)
{
    PropGroupSettingsRec *pgs = &(prop_group_settings_rec[type]);
    DtbGroupPropDialogInfo	cgen;

    if (type == AB_PROP_REVOLVING)
	cgen = &rev_group_prop_dialog;
    else
	cgen = &dtb_group_prop_dialog;

    if (!cgen || !cgen->initialized)
	return;

    switch(gtype)
    {
        case AB_GROUP_IGNORE:
            ui_set_active(cgen->valign_grp, False);
            ui_set_active(cgen->halign_grp, False);
            ui_set_active(cgen->grid_rowcol_grp, False);
            break;

        case AB_GROUP_COLUMNS:
            ui_set_active(cgen->valign_grp, True);
            ui_set_active(cgen->halign_grp, False);
            ui_set_active(cgen->grid_rowcol_grp, False);
            break;

        case AB_GROUP_ROWS:
            ui_set_active(cgen->valign_grp, False);
            ui_set_active(cgen->halign_grp, True);
            ui_set_active(cgen->grid_rowcol_grp, False);
            break;

        case AB_GROUP_ROWSCOLUMNS:
            ui_set_active(cgen->valign_grp, True);
            ui_set_active(cgen->halign_grp, True);
            ui_set_active(cgen->grid_rowcol_grp, True);
            break;
    }
}

static void
undo_group(
    ABUndoRec	undo_rec
)
{
    if (!undo_rec)
	return;

    /*
     * Remember all objects that will be ungrouped here can
     * undo this later (undo of undo)
     */
    abobj_set_undo(undo_rec->list, undo_rec->count,
		undo_ungroup, AB_UNDO_UNGROUP);

    /*
     * Ungroup objects
     */
    ungroup_objects(undo_rec->list, undo_rec->count);
}


static void
undo_ungroup(
    ABUndoRec	undo_rec
)
{
    ABObj	newgroup,
		parent,
		*new_list,
		*member_list,
		project;
    int		i,
		member_count;

    if (!undo_rec)
	return;

    new_list = (ABObj *)malloc(undo_rec->count * sizeof(ABObj));

    project = proj_get_project();

    /* Deselect any objects that happen to be selected */
    abobj_deselect_all(project);
    aob_deselect_all_objects(project);

    for (i = 0; i < undo_rec->count; ++i)
    {
	ABUndoInfo	undo_info;

	undo_info = &(undo_rec->info_list[i]);

	/*
	 * If undo record is not the right type, something is WRONG !!
	 */
	if (undo_info->type != AB_UNDO_UNGROUP)
	    continue;

	newgroup = abobj_dup_tree(undo_info->info.ungroup.dup_old_group);

	member_list = undo_info->info.ungroup.member_list;
	member_count = undo_info->info.ungroup.member_count;

	parent = obj_get_parent(member_list[0]);

	obj_append_child(parent, newgroup);

        if (abobj_show_tree(newgroup, False) == -1)
            fprintf(stderr,"create_obj_action: couldn't show object\n");
        else
        {
            group_objects(newgroup, member_list, member_count);
        }

	new_list[i] = newgroup;
    }

    abobj_set_undo(new_list, undo_rec->count, undo_group, AB_UNDO_GROUP);

    if (new_list)
	free(new_list);
}

void
set_new_group_size(
    ABObj obj
)
{
    ABObj 	  oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    int		  member_count;
    ABObj	  *member_list;
    int		  width, height;
    XRectangle    rect;

    create_member_list(oobj, &member_list, &member_count);

    /*
     * Reset groups width and height. for rows, the new height is that of
     * the tallest member, the new width is the width of the
     * rect occupied by all of the members. For colums, the new
     * width is that of the widest member, the new height is the
     * height of the rect occupied by all of the members.
     */
    abobj_get_rect_for_objects(member_list, member_count, &rect);
    abobj_get_greatest_size(member_list, member_count,
				 &width, &height,
				 (ABObj *) NULL, (ABObj *) NULL);
    if (obj_get_group_type(oobj) == AB_GROUP_ROWS)
    {
	abobj_set_pixel_width(obj, rect.width, 0);
	abobj_set_pixel_height(obj, height, 0);
    }
    else if (obj_get_group_type(oobj) == AB_GROUP_ROWS)
    {
        abobj_set_pixel_width(obj, width, 0);
        abobj_set_pixel_height(obj, rect.height, 0);
    }
    util_free(member_list);
}

void
abobj_layout_group(
    ABObj	obj,
    BOOL	init
)
{
    ABObj         pobj;
    ABObj	  child;
    AB_GROUP_TYPE type;
    int 	  i;
    int	  	  child_old_x, child_old_y;
    int	  	  group_new_x, group_new_y;
    XRectangle    rect;


    if (!obj_has_flag(obj, XmConfiguredFlag))
	return;

    pobj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

    if (obj_get_num_children(pobj) <= 0)
	return;

    type = obj_get_group_type(obj);

    /*
     * Save leftmost objects real x and y to determine group's new
     * x and y.
     */
    child = obj_get_child(pobj, 0);

    child_old_x = obj_get_x(child);
    child_old_y = obj_get_y(child);

    abobj_clear_layout(pobj, True, (type != AB_GROUP_IGNORE));

    /*
     * CLEANUP:
     * The code to do group layout here should be the code
     * used in dtb_utils.c to make sure the behaviour is the
     * same. Unfortunately, at this time, this is a big risky
     * change. So, some behavioral differences will exist.
     * This needs to be fixed in the future.
     */
    switch (type)
    {
	case AB_GROUP_IGNORE:
	    break;

	case AB_GROUP_ROWS:
            obj_set_num_rows(obj, 1);
            obj_set_num_columns(obj, 0);
	    group_align_rows(obj, init);
            group_align_left(obj);
	    break;

	case AB_GROUP_COLUMNS:
            obj_set_num_rows(obj, 0);
            obj_set_num_columns(obj, 1);
	    group_align_cols(obj, init);
            group_align_tops(obj);
	    break;

	case AB_GROUP_ROWSCOLUMNS:
	    group_align_rows(obj, init);
	    group_align_cols(obj, init);
	    break;
    }

    /*
    group_align_rows(obj);
    group_align_cols(obj);
    */


    /*
     * Re-instantiate all of the children and then the group.
     */
    for (i = 0; i < obj_get_num_children(pobj); i++)
    {
	abobj_instantiate_changes(obj_get_child(pobj, i));
    }


    /*
     * Set the childrens x,y's so obj is update with reality.
     */
    for (i = 0; i < obj_get_num_children(pobj); i++)
    {
	child = obj_get_child(pobj, i);
	x_get_widget_rect(objxm_get_widget(child), &rect);
	obj_set_x(child, rect.x);
	obj_set_y(child, rect.y);
    }

    /*
     * Place the group so that the topmost/leftmost child remains
     * in the same place.
    group_new_x = obj_get_x(pobj) +
		(child_old_x - obj_get_x(obj_get_child(pobj, 0)));

    group_new_y = obj_get_y(pobj) +
		(child_old_y - obj_get_y(obj_get_child(pobj, 0)));

    abobj_set_xy(pobj, group_new_x, group_new_y);
     */

    abobj_instantiate_changes(pobj);

    /*
     * For some reason, width and height need to be set on the
     * group.
    pobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    x_get_widget_rect(objxm_get_widget(pobj), &rect);
     */

/*
    obj_set_width(pobj, rect.width);
    obj_set_height(pobj, rect.height);
*/

}

void
abobj_register_group_expose_handler(
    ABObj	obj
)
{
    AB_GROUP_TYPE	type;
    AB_ALIGNMENT	row_alignment,
			col_alignment;
    Widget		group_widget;


    Boolean		register_expose = False;

    if (!obj || !obj_is_group(obj))
	return;

    group_widget = objxm_get_widget(obj);

    if (!group_widget)
	return;

    type = obj_get_group_type(obj);

    switch (type)
    {
	case AB_GROUP_IGNORE:
	    break;

	case AB_GROUP_ROWS:
	    row_alignment = obj_get_row_align(obj);

	    if (row_alignment == AB_ALIGN_HCENTER)
		register_expose = True;
	    break;

	case AB_GROUP_COLUMNS:
	    col_alignment = obj_get_col_align(obj);

	    if ((col_alignment == AB_ALIGN_VCENTER) ||
		    (col_alignment == AB_ALIGN_LABELS))
		register_expose = True;
	    break;

	case AB_GROUP_ROWSCOLUMNS:
	    row_alignment = obj_get_row_align(obj);
	    col_alignment = obj_get_col_align(obj);

	    if ((row_alignment == AB_ALIGN_HCENTER) ||
		(col_alignment == AB_ALIGN_LEFT) ||
		(col_alignment == AB_ALIGN_LABELS))
		register_expose = True;
	    break;
    }

    if (register_expose)
    {

        XtAddEventHandler(group_widget,
		ExposureMask, False,
		group_expose_handler, (XtPointer)obj);
    }
}

/*
 * Group expose handler.
 *
 * Some group objects depend on it's members' sizes for their layout.
 * Unfortunately, some group members have invalid sizes prior to
 * XtRealize(), so the group layout has to be recalculated after the
 * group is realized or exposed in this case, since there is no realize
 * callback.
 */
static void
group_expose_handler(
    Widget      widget,
    XtPointer   client_data,
    XEvent      *event,
    Boolean     *cont_dispatch
)
{
    ABObj		obj = (ABObj)client_data;
    WidgetList		children_list;
    int			i,
			num_children = 0;
    Boolean		relayout_all = False,
			register_align_handler = False;


    if (event->type != Expose)
	return;

    if (!obj || !obj_is_group(obj))
	return;

    /*
     * Get children list
     */
    XtVaGetValues(widget,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return;

    XtRemoveEventHandler(widget,
                ExposureMask, False,
                group_expose_handler, (XtPointer)client_data);

    /*
     * Turn off 'save needed' temporarily
     */
    abobj_disable_save_needed();

    abobj_layout_group(obj, FALSE);

    abobj_enable_save_needed();
}

static void
group_align_tops(
	ABObj	obj
)
{
    ABAttachment	attach;
    ABObj	child,
		previous_child,
    		oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    int		num_children = obj_get_num_children(oobj),
		num_columns,
		num_rows,
		cell_height,
		cell_width,
		i,
		j;

    if (num_children <= 0)
	return;

    get_cell_size(obj, &cell_width, &cell_height);

    get_row_col(obj, &num_rows, &num_columns);

    for (j = 0; j < num_rows; j++)
    {
        for (i = 0; i < num_columns; i++)
        {
	    child = get_child(obj, i, j);

	    if (!child)
		continue;

	    if ((i == 0) && (j == 0))
	    {
	        attach.type = AB_ATTACH_POINT;
	        attach.value = (void *)0;
	        attach.offset = 0;

	        abobj_set_attachment(child, AB_CP_NORTH, &attach);

		continue;
	    }

	    if (i == 0)
	    {
	        previous_child = get_child(obj, 0, j-1);

	        if (previous_child)
	        {
	            int		offset = obj_get_voffset(obj);
                    AB_GROUP_TYPE type = obj_get_group_type(obj);

                    if (type == AB_GROUP_ROWSCOLUMNS)
		        offset +=
			    (cell_height - abobj_get_actual_height(previous_child));

	            attach.type = AB_ATTACH_OBJ;
	            attach.value = (void *)previous_child;
	            attach.offset = offset;

	            abobj_set_attachment(child, AB_CP_NORTH, &attach);
	        }
		continue;
	    }

	    previous_child = get_child(obj, i-1, j);

	    if (previous_child)
	    {
	        attach.type = AB_ATTACH_ALIGN_OBJ_EDGE;
	        attach.value = (void *)previous_child;
	        attach.offset = 0;

	        abobj_set_attachment(child, AB_CP_NORTH, &attach);
	    }
        }
    }
}

static void
group_align_hcenters(
	ABObj	obj,
	BOOL	init
)
{
    ABAttachment	attach;
    ABObj	child,
		previous_child,
    		oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    AB_GROUP_TYPE type = obj_get_group_type(obj);
    int		num_children = obj_get_num_children(oobj),
		num_columns,
		num_rows,
		cell_width,
		cell_height,
		group_width,
		group_height,
		offset,
		gridline,
		i,
		j;

    if (num_children <= 0)
	return;

    get_cell_size(obj, &cell_width, &cell_height);

    get_row_col(obj, &num_rows, &num_columns);

    offset = obj_get_voffset(obj);

    if (type == AB_GROUP_ROWSCOLUMNS)
    {
        group_height = (num_rows * cell_height) + ((num_rows-1) * offset);
	abobj_set_pixel_width(obj, group_height, 0);
	abobj_instantiate_changes(obj);
    }

    for (j = 0; j < num_rows; j++)
    {
        if (type == AB_GROUP_ROWSCOLUMNS)
	    gridline = (((j * (cell_height + offset)) + (cell_height/2)) * 100)/group_height;
	else
	    gridline = 50;

        for (i = 0; i < num_columns; i++)
        {
	    child = get_child(obj, i, j);

	    if (!child)
		continue;

	    if (init)
	    {
		int	init_offset = 0;
		Widget	child_widget = objxm_get_widget(child);

		if (child_widget &&
		   !XtIsSubclass(child_widget, compositeWidgetClass))
		{
		    int		height = abobj_get_actual_height(child);

		    init_offset = (cell_height - height)/2;
                    if (type == AB_GROUP_ROWSCOLUMNS)
	                init_offset += (j * (cell_height + offset));
		}

	        attach.type = AB_ATTACH_POINT;
	        attach.value = (void *)0;
	        attach.offset = init_offset;
	    }
	    else
	    {
		int		height = abobj_get_actual_height(child);

	        attach.type = AB_ATTACH_GRIDLINE;
	        attach.value = (void *)(intptr_t) gridline;
	        attach.offset = -(height/2);
	    }

	    abobj_set_attachment(child, AB_CP_NORTH, &attach);
        }
    }

    if (type == AB_GROUP_ROWSCOLUMNS)
    {
	abobj_set_pixel_width(obj, -1, 0);
	abobj_instantiate_changes(obj);
    }
}

static void
group_align_horiz_same_size(
	ABObj	obj
)
{
    ABObj sibling, previous_sibling;
    ABObj oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    int	  num_children = obj_get_num_children(oobj);
    int   i;

    sibling = obj_get_child(oobj, num_children - 1);
    /*
     * Set up childrens attachments from right to left.
     * All objects North and South attachments are to
     * the form.
     */
    for (i = num_children - 2; i >= 0; i--)
    {
	previous_sibling = obj_get_child(oobj, i);

	obj_set_attachment(sibling,
		AB_CP_NORTH,
		AB_ATTACH_OBJ,
	    	(void *) oobj,
		0);

	obj_set_attachment(sibling,
		AB_CP_WEST,
		AB_ATTACH_OBJ,
		(void *) previous_sibling,
		obj_get_hoffset(oobj));

	obj_set_attachment(sibling,
		AB_CP_SOUTH,
		AB_ATTACH_OBJ,
	    	(void *) oobj,
		0);

	objxm_obj_set_attachment_args(sibling, OBJXM_CONFIG_BUILD);
        obj_clear_flag(sibling, BeingDestroyedFlag);

	sibling = previous_sibling;
    }

    obj_set_attachment(sibling,
	    AB_CP_NORTH,
	    AB_ATTACH_OBJ,
	    (void *) oobj,
	    0);
    obj_set_attachment(sibling,
	    AB_CP_WEST,
	    AB_ATTACH_OBJ,
	    (void *) oobj,
	    0);

    obj_set_attachment(sibling,
	    AB_CP_SOUTH,
	    AB_ATTACH_OBJ,
	    (void *) oobj,
	    0);

    objxm_obj_set_attachment_args(sibling, OBJXM_CONFIG_BUILD);
    obj_clear_flag(sibling, BeingDestroyedFlag);
}

static void
group_align_bottoms(
	ABObj	obj
)
{
    ABAttachment	attach;
    ABObj	child,
		previous_child,
    		oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    AB_GROUP_TYPE type = obj_get_group_type(obj);
    int		num_children = obj_get_num_children(oobj),
		num_columns,
		num_rows,
		cell_height,
		cell_width,
		offset,
		i,
		j;

    if (num_children <= 0)
	return;

    get_cell_size(obj, &cell_width, &cell_height);

    get_row_col(obj, &num_rows, &num_columns);

    for (j = 0; j < num_rows; j++)
    {
        for (i = 0; i < num_columns; i++)
        {
	    child = get_child(obj, i, j);

	    if (!child)
		continue;

	    if ((i == 0) && (j == 0))
	    {
		offset = (cell_height - abobj_get_actual_height(child));

	        attach.type = AB_ATTACH_POINT;
	        attach.value = (void *)0;
	        attach.offset = offset;

	        abobj_set_attachment(child, AB_CP_NORTH, &attach);

		continue;
	    }

	    if (i == 0)
	    {
	        previous_child = get_child(obj, 0, j-1);

	        if (previous_child)
	        {
	            offset = obj_get_voffset(obj);

                    if (type == AB_GROUP_ROWSCOLUMNS)
		        offset +=
			    (cell_height - abobj_get_actual_height(child));

	            attach.type = AB_ATTACH_OBJ;
	            attach.value = (void *)previous_child;
	            attach.offset = offset;

	            abobj_set_attachment(child, AB_CP_NORTH, &attach);
	        }
		continue;
	    }

	    previous_child = get_child(obj, i-1, j);

	    if (child && previous_child)
	    {
	        attach.type = AB_ATTACH_ALIGN_OBJ_EDGE;
	        attach.value = (void *)previous_child;
	        attach.offset = 0;

	        abobj_set_attachment(child, AB_CP_SOUTH, &attach);
	    }
        }
    }
}

static void
group_align_left(
	ABObj	obj
)
{
    ABAttachment	attach;
    ABObj	child,
		previous_child,
    		oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    int		num_children = obj_get_num_children(oobj),
		num_columns,
		num_rows,
		cell_width,
		cell_height,
		i,
		j;

    if (num_children <= 0)
	return;

    get_cell_size(obj, &cell_width, &cell_height);

    get_row_col(obj, &num_rows, &num_columns);

    for (j = 0; j < num_rows; j++)
    {
        for (i = 0; i < num_columns; i++)
        {
	    child = get_child(obj, i, j);

	    if (!child)
		continue;

	    if ((i == 0) && (j == 0))
	    {
	        attach.type = AB_ATTACH_POINT;
	        attach.value = (void *)0;
	        attach.offset = 0;

	        abobj_set_attachment(child, AB_CP_WEST, &attach);
		continue;
	    }

	    if (j == 0)
	    {
	        int		offset = obj_get_hoffset(obj);
                AB_GROUP_TYPE type = obj_get_group_type(obj);

	        previous_child = get_child(obj, i-1, j);

		if (!previous_child)
		    continue;

                if (type == AB_GROUP_ROWSCOLUMNS)
		    offset +=
		        (cell_width - abobj_get_actual_width(previous_child));

	        attach.type = AB_ATTACH_OBJ;
	        attach.value = (void *)previous_child;
	        attach.offset = offset;

	        abobj_set_attachment(child, AB_CP_WEST, &attach);

		continue;
	    }

	    previous_child = get_child(obj, i, j-1);

	    if (previous_child)
	    {
	        attach.type = AB_ATTACH_ALIGN_OBJ_EDGE;
	        attach.value = (void *)previous_child;
	        attach.offset = 0;

	        abobj_set_attachment(child, AB_CP_WEST, &attach);
	    }
        }
    }
}

static void
group_align_labels(
	ABObj	obj
)
{
    ABAttachment	attach;
    ABObj	child,
		previous_child,
    		oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ),
		*one_col = NULL,
		*child_list,
		previous_ref_obj = NULL;
    int		num_children = 0,
		num_columns,
		num_rows,
		cell_width,
		cell_height,
		offset,
		max_label_width = 0,
		max_value_width = 0,
		i,
		j;

    if (oobj)
        num_children = obj_get_num_children(oobj);

    if (num_children <= 0)
	return;

    get_cell_size(obj, &cell_width, &cell_height);

    create_member_list(oobj, &child_list, &num_children);

    get_widest_label_obj(child_list, num_children, &child, &max_label_width);
    get_widest_value_obj(child_list, num_children, &child, &max_value_width);

    util_free(child_list);

    if (cell_width < max_label_width + max_value_width)
	cell_width = max_label_width + max_value_width;

    get_row_col(obj, &num_rows, &num_columns);

    if (num_rows > 0)
        one_col = (ABObj *)util_malloc(num_rows * sizeof(ABObj));

    for (i = 0; i < num_columns; i++)
    {
	ABObj	ref_obj;
	int	ref_width;

        for (j = 0; j < num_rows; j++)
	    one_col[j] = get_child(obj, i, j);

	get_widest_label_obj(one_col, num_rows, &ref_obj, &ref_width);

	if (!ref_obj)
	    continue;

	if (previous_ref_obj)
	{
	    offset = (i * (obj_get_hoffset(obj) + cell_width));

	    attach.offset = offset;
	}
	else
	    attach.offset = 0;

	attach.type = AB_ATTACH_POINT;
	attach.value = (void *)NULL;

	abobj_set_attachment(ref_obj, AB_CP_WEST, &attach);

        for (j = 0; j < num_rows; j++)
        {
	    child = get_child(obj, i, j);

	    if (!child || (child == ref_obj))
		continue;

	    offset = (i * (obj_get_hoffset(obj) + cell_width));
	    offset += ref_width - abobj_get_label_width(child);

	    attach.type = AB_ATTACH_POINT;
	    attach.value = (void *)NULL;
	    /*
	    attach.offset = ref_x - abobj_get_label_width(child);
	    */
	    attach.offset = offset;

	    abobj_set_attachment(child, AB_CP_WEST, &attach);
        }

	previous_ref_obj = ref_obj;
    }
}

static void
group_align_vcenters(
	ABObj	obj,
	BOOL	init
)
{
    ABAttachment	attach;
    ABObj	child,
		previous_child,
    		oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    AB_GROUP_TYPE type = obj_get_group_type(obj);
    int		num_children = obj_get_num_children(oobj),
		num_columns,
		num_rows,
		cell_width,
		cell_height,
		group_width,
		group_height,
		offset,
		gridline,
		i,
		j;

    if (num_children <= 0)
	return;

    get_cell_size(obj, &cell_width, &cell_height);

    get_row_col(obj, &num_rows, &num_columns);

    offset = obj_get_hoffset(obj);

    if (type == AB_GROUP_ROWSCOLUMNS)
    {
        group_width = (num_columns * cell_width) + ((num_columns-1) * offset);
	abobj_set_pixel_width(obj, group_width, 0);
	abobj_instantiate_changes(obj);
    }

    for (i = 0; i < num_columns; i++)
    {
        if (type == AB_GROUP_ROWSCOLUMNS)
	    gridline = (((i * (cell_width + offset)) + (cell_width/2)) * 100)/group_width;
	else
	    gridline = 50;

        for (j = 0; j < num_rows; j++)
        {
	    child = get_child(obj, i, j);

	    if (!child)
		continue;

	    if (init)
	    {
		int	init_offset = 0;
		Widget	child_widget = objxm_get_widget(child);

		if (child_widget && !XtIsSubclass(child_widget, compositeWidgetClass))
		{
		    int		width = abobj_get_actual_width(child);

	            init_offset = (cell_width - width)/2;

                    if (type == AB_GROUP_ROWSCOLUMNS)
	                init_offset += (i * (cell_width + offset));
		}

	        attach.type = AB_ATTACH_POINT;
	        attach.value = (void *)0;
	        attach.offset = init_offset;
	    }
	    else
	    {
		int		width = abobj_get_actual_width(child);

	        attach.type = AB_ATTACH_GRIDLINE;
	        attach.value = (void *)(intptr_t) gridline;
	        attach.offset = -(width/2);
	    }

	    abobj_set_attachment(child, AB_CP_WEST, &attach);
        }
    }

    if (type == AB_GROUP_ROWSCOLUMNS)
    {
	abobj_set_pixel_width(obj, -1, 0);
	abobj_instantiate_changes(obj);
    }
}

static void
group_align_right(
	ABObj	obj
)
{
    ABAttachment	attach;
    ABObj	child,
		previous_child,
    		oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    AB_GROUP_TYPE type = obj_get_group_type(obj);
    int		num_children = obj_get_num_children(oobj),
		num_columns,
		num_rows,
		cell_width,
		cell_height,
	        offset,
		i,
		j;

    if (num_children <= 0)
	return;

    get_cell_size(obj, &cell_width, &cell_height);

    get_row_col(obj, &num_rows, &num_columns);

    for (j = 0; j < num_rows; j++)
    {
        for (i = 0; i < num_columns; i++)
        {
	    child = get_child(obj, i, j);

	    if (!child)
		continue;

	    if ((i == 0) && (j == 0))
	    {
		offset = (cell_width - abobj_get_actual_width(child));

	        attach.type = AB_ATTACH_POINT;
	        attach.value = (void *)0;
	        attach.offset = offset;

	        abobj_set_attachment(child, AB_CP_WEST, &attach);

		continue;
	    }

	    if (j == 0)
	    {
	        previous_child = get_child(obj, i-1, j);

		if (!previous_child)
		    continue;

	        offset = obj_get_hoffset(obj);

                if (type == AB_GROUP_ROWSCOLUMNS)
		    offset +=
		        (cell_width - abobj_get_actual_width(child));

	        attach.type = AB_ATTACH_OBJ;
	        attach.value = (void *)previous_child;
	        attach.offset = offset;

	        abobj_set_attachment(child, AB_CP_WEST, &attach);

		continue;
	    }

	    previous_child = get_child(obj, i, j-1);

	    if (previous_child)
	    {
	        attach.type = AB_ATTACH_ALIGN_OBJ_EDGE;
	        attach.value = (void *)previous_child;
	        attach.offset = 0;

	        abobj_set_attachment(child, AB_CP_EAST, &attach);
	    }
        }
    }
}

static void
create_member_list(
    ABObj  cobj,
    ABObj **member_list,
    int    *member_count
)
{
    AB_TRAVERSAL  trav;
    int		  member_index;
    ABObj	  member;

    *member_count = trav_count(cobj,
		AB_TRAV_SALIENT_CHILDREN | AB_TRAV_MOD_SAFE);

    *member_list = (ABObj *)malloc(*member_count * sizeof(ABObj));

    /*
     * Create list of members.
     */
    for (trav_open(&trav, cobj, AB_TRAV_SALIENT_CHILDREN |
		AB_TRAV_MOD_SAFE), member_index = 0;
		(member = trav_next(&trav)) != NULL; ++member_index)
    {
	(*member_list)[member_index] = member;
    }

    trav_close(&trav);
}

static ABObj
get_child(
    ABObj	group,
    int		x_pos,
    int		y_pos
)
{
    AB_GROUP_TYPE	type;
    ABObj		ret_child = NULL,
    			oobj;
    int			num_children,
			num_rows,
			num_columns,
    			i = -1;

    if (!group || !obj_is_group(group) ||
       (x_pos < 0) || (y_pos < 0))
	return (NULL);

    type = obj_get_group_type(group);
    num_rows = obj_get_num_rows(group);
    num_columns = obj_get_num_columns(group);

    oobj = objxm_comp_get_subobj(group, AB_CFG_OBJECT_OBJ);
    num_children = obj_get_num_children(oobj);

    switch (type)
    {
	case AB_GROUP_IGNORE:
	break;

	case AB_GROUP_ROWS:
	    /*
	     * num_rows = 1
	     * y_pos is ignored
	     */
	    i = x_pos;
	break;

	case AB_GROUP_COLUMNS:
	    /*
	     * num_columns = 1
	     * x_pos is ignored
	     */
	    i = y_pos;
	break;

	case AB_GROUP_ROWSCOLUMNS:
	    if (!num_rows && !num_columns)
		break;

	    if (num_rows > 0)
	    {
		/*
		 * ROWFIRST
		 */
		if (y_pos < num_rows)
		    i = (x_pos * num_rows) + y_pos;
	    }
	    else
	    {
		/*
		 * COLFIRST
		 */
		if (x_pos < num_columns)
		    i = x_pos + (y_pos * num_columns);
	    }
	break;

	default:
	break;
    }

    if ((i >= 0) && (i < num_children))
    {
        ret_child = obj_get_child(oobj, i);
    }

    return (ret_child);
}

static void
get_cell_size(
    ABObj	group,
    int		*cell_width,
    int		*cell_height
)
{
    AB_GROUP_TYPE type;

    if (!group)
    {
	*cell_width = *cell_height = -1;

	return;
    }

    type = obj_get_group_type(group);

    /*
    if (type == AB_GROUP_ROWSCOLUMNS)
    */
    if (1)
    {
        ABObj	*member_list,
    		oobj = objxm_comp_get_subobj(group, AB_CFG_OBJECT_OBJ);
        int	member_count;

        create_member_list(oobj, &member_list, &member_count);
        abobj_get_greatest_size(member_list, member_count,
                cell_width, cell_height,
                (ABObj *) NULL, (ABObj *) NULL);

	util_free(member_list);
    }
    else
	*cell_width = *cell_height = -1;
}

static void
get_row_col(
    ABObj	group,
    int		*rows,
    int		*cols
)
{
    ABObj	oobj;
    int		num_rows,
		num_cols,
    		num_children;

    if (!group)
    {
	*rows = *cols = -1;

	return;
    }

    oobj = objxm_comp_get_subobj(group, AB_CFG_OBJECT_OBJ);

    num_rows = obj_get_num_rows(group);
    num_cols = obj_get_num_columns(group);
    num_children = obj_get_num_children(oobj);

    if ((num_rows <= 0) && (num_cols <= 0))
    {
	*rows = *cols = -1;

	return;
    }

    if (num_cols <= 0)
        num_cols = (num_children/num_rows) + ((num_children % num_rows) ? 1 : 0);

    if (num_rows <= 0)
        num_rows = (num_children/num_cols) + ((num_children % num_cols) ? 1 : 0);

    *rows = num_rows;
    *cols = num_cols;
}

static void
group_align_rows(
    ABObj	obj,
    BOOL	init
)
{

    if (!obj || !obj_is_group(obj))
	return;

    switch (obj_get_row_align(obj))
    {
        case AB_ALIGN_TOP:
            group_align_tops(obj);
        break;

        case AB_ALIGN_HCENTER:
            group_align_hcenters(obj, init);
        break;

        case AB_ALIGN_BOTTOM:
            group_align_bottoms(obj);
        break;

        default:
	    /*
	     * Default to AB_ALIGN_TOP
	     */
            group_align_tops(obj);
	    break;
    }

}

static void
group_align_cols(
    ABObj	obj,
    BOOL	init
)
{

    if (!obj || !obj_is_group(obj))
	return;

    switch (obj_get_col_align(obj))
    {
        case AB_ALIGN_LEFT:
            group_align_left(obj);
        break;

        case AB_ALIGN_LABELS:
            group_align_labels(obj);
        break;

        case AB_ALIGN_VCENTER:
            group_align_vcenters(obj, init);
        break;

        case AB_ALIGN_RIGHT:
            group_align_right(obj);
        break;

        default:
	    /*
	     * Default to AB_ALIGN_LEFT
	     */
            group_align_left(obj);
	    break;
    }
}

static void
get_widest_label_obj(
    ABObj	*list,
    int		count,
    ABObj	*widest_label,
    int		*label_width
)
{
    int		i,
		tmp_width,
		cur_width = 0;
    ABObj	widest = NULL;

    if (!list || !widest_label)
	return;

    for (i = 0; i < count; ++i)
    {
	if (list[i])
	{
	    tmp_width = abobj_get_label_width(list[i]);

	    if (tmp_width > cur_width)
	    {
		cur_width = tmp_width;
		widest = list[i];
	    }
	}
    }

    *widest_label = widest;

    if (label_width)
        *label_width = cur_width;
}

static void
get_widest_value_obj(
    ABObj	*list,
    int		count,
    ABObj	*widest_value,
    int		*value_width
)
{
    int		i,
		tmp_width,
		cur_width = 0;
    ABObj	widest = NULL;

    if (!list || !widest_value)
	return;

    for (i = 0; i < count; ++i)
    {
	if (list[i])
	{
	    int		label_width,
			obj_width;

	    label_width = abobj_get_label_width(list[i]);
	    obj_width = abobj_get_actual_width(list[i]);

	    tmp_width = obj_width - label_width;

	    if (tmp_width > cur_width)
	    {
		cur_width = tmp_width;
		widest = list[i];
	    }
	}
    }

    *widest_value = widest;

    if (value_width)
        *value_width = cur_width;
}
