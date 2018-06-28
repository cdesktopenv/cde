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
 *	$XConsortium: abobj_edit.c /main/3 1995/11/06 17:15:24 rswiston $
 *
 *	@(#)abobj_edit.c	1.6 15 Feb 1994	
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
 * File: abobj_edit.c - Editing functions for the app builder i.e.
 * cut, copy, paste, undo
 *
 * This file also contains the functions to manipulate the clipboard.
 */

#include <stdio.h>
#include <sys/param.h>
#include <Xm/Xm.h>
#include <ab_private/trav.h>
#include <ab_private/ui_util.h>
#include <ab_private/brws.h>
#include <ab_private/pal.h>
#include <ab_private/proj.h>
#include <ab_private/objxm.h>
#include <ab_private/obj_notify.h>
#include <ab_private/abobj_edit.h>
#include <ab_private/ab.h>
#include "abobjP.h"
#include "palette_ui.h"

/*
 * Offset to use when pasting an object and there is already 
 * an object at the same (x, y) position.
 */
#define STACKING_X_OFFSET		10
#define STACKING_Y_OFFSET		10

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static void	init_clipboard(
		);

static void	verify_clipboard_space(
		    int		count
		);

static void	resolve_clipboard_connections(
		    ABObj		*obj,
		    int			count,
		    ABClipboardRec	clipboard
		);

static void	resolve_clipboard_attachments(
		    ABObj		*obj,
		    int			count,
		    ABClipboardRec	clipboard
		);

static void	resolve_attach_one_obj(
		    ABObj	obj,
		    ABObj	origObj,
		    ABObj	*new_list,
		    int		new_count
		);

static void	resolve_attach_one_attachment(
		    AB_COMPASS_POINT	dir,
		    ABObj		obj,
		    ABObj		origObj,
		    ABObj		*new_list,
		    int			new_count
		);

static void	resolve_attach_parent(
		    ABObj		obj,
		    ABObj		new_parent
		);

static void	resolve_attach_parent_dir(
		    AB_COMPASS_POINT	dir,
		    ABObj		obj,
		    ABObj		new_parent
		);

static void	resolve_none_attachments(
		    ABObj	obj
		);

static void	resolve_undo_rec_connections(
		    ABObj	*obj,
		    int		count,
		    ABUndoRec	undo_rec
		);

static void	resolve_undo_rec_attachments(
		    ABObj	*obj,
		    int		count,
		    ABUndoRec	undo_rec
		);

static void	init_undo_rec(
		    ABUndoRec		*undo_rec_ptr
		);

static void	init_undo(
		);

static void	verify_undo_rec_space(
		    ABUndoRec	*undo_rec_ptr,
		    int		count
		);

static void	verify_undo_space(
		    int		count
		);

static void	remove_clipboard_descendants(
		    ABSelectedRec	*sel
		);

static BOOL	any_object_at_my_position(
		    ABObj	obj,
		    ABObj	parent
		);

static void	stack_objects(
		    ABObj	newObj,
		    ABObj	parent
		);

static int	set_undo_rec (
		    ABUndoRec		*undo_rec_ptr,
		    ABObj		*obj,
		    int			count,
		    ABUndoFunc		undo_func,
		    AB_UNDO_TYPE	undo_type
		);

static void	clear_undo_rec(
		    ABUndoRec		*undo_rec_ptr
		);

static void	undo_cut(
		    ABUndoRec	undo_rec
		);

static void	undo_paste(
		    ABUndoRec	undo_rec
		);

static int	edit_destroyOCB(
		    ObjEvDestroyInfo info
		);

static ABObj	get_ctrl_pane_or_group(
		    ABObj	paste_target
		);

static DTB_MODAL_ANSWER show_modal_msg_relative_initiator(
		    ABObj	         parent_obj,
		    AB_PASTE_INITIATOR_TYPE initiator,
		    DtbMessageData	 mbr,
		    XmString		 override_msg,
		    DtbObjectHelpData	 override_help,
		    Widget         	 *modal_dlg_pane_out_ptr
		);

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/
static ABClipboardRec	ABClipboard = NULL;
static ABUndoRec	ABUndo = NULL;
static ABUndoRec	ABUndo_backup = NULL;

static void		(*Undo_func)();

BOOL			in_undo = FALSE;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * Called from main()...
 */
void
abobj_edit_init(void)
{
    obj_add_destroy_callback(edit_destroyOCB, "ATTCH_ED");
}

static int
edit_destroyOCB(
    ObjEvDestroyInfo info
)
{
    ABObj	obj, 
		parent_obj, 
		project;
    int		i;

    obj = info->obj;

    /*
     * Return if obj is NULL or obj is not salient
     */
    if (!obj || !obj_is_salient(obj))
	return (0);

    project = obj_get_project(obj);

    /*
     * Return if project is NULL
     */
    if (!project)
	return (0);

    /*
     * If the entire project is curently being destroyed,
     * skip this
     */
    if (obj_has_flag(project, BeingDestroyedFlag))
    {
	/*
	 * Do one time only during project destroy
	 *	-> clipboard and undo record cleanup
	 * so, pick the case when the project ABObj is passed in
	 */
	if (obj_is_project(obj))
	{
            /*
             * Clear clipboard
             */
            abobj_clipboard_clear();

	    /*
	     * Clear undo record
	     */
            clear_undo_rec(&ABUndo);
	}

        return (0);
    }

    /*
     * Check if destroyed object is in connection list stored in
     * clipboard.
     */
    if (ABClipboard && ABClipboard->action_count > 0)
    {
        for (i=0; i < ABClipboard->count; ++i)
        {
            ABClipbInfo	clip_info;

            clip_info = &(ABClipboard->info_list[i]);

            if (clip_info->action_list)
            {
                AB_TRAVERSAL	trav;
                ABObj		action_obj;

                for (trav_open(&trav, clip_info->action_list, 
                        AB_TRAV_ACTIONS | AB_TRAV_MOD_SAFE);
                    (action_obj = trav_next(&trav)) != NULL; )
                {
                    ABObj	to_obj;

                    to_obj = action_obj->info.action.to;

		    /*
		     * If the object being destroyed is the 'to' object OR
		     * it is a descendant of the 'to' obj, delete the
		     * connection object
		     */
		    if ((obj == to_obj) || (obj_is_descendant_of(to_obj, obj)))
		    {
		        obj_destroy(action_obj);
		        ABClipboard->action_count--;
		    }
                }
                trav_close(&trav);
            }
        }
    }

    if (ABUndo && ABUndo->action_count > 0)
    {
	for (i=0; i < ABUndo->count; ++i)
	{
	    ABUndoInfo	undo_info;

	    undo_info = &(ABUndo->info_list[i]);

	    if ((undo_info->type != AB_UNDO_CUT) && 
		(undo_info->type != AB_UNDO_DELETE))
		continue;

            if (undo_info->info.cut.from_action_list)
            {
                AB_TRAVERSAL	trav;
                ABObj		action_obj;

                for (trav_open(&trav, undo_info->info.cut.from_action_list,
                        AB_TRAV_ACTIONS | AB_TRAV_MOD_SAFE);
                    (action_obj = trav_next(&trav)) != NULL; )
                {
                    ABObj	to_obj;

                    to_obj = action_obj->info.action.to;

		    /*
		     * If the object being destroyed is the 'to' object OR
		     * it is a descendant of the 'to' obj, delete the
		     * connection object
		     */
		    if ((obj == to_obj) || (obj_is_descendant_of(to_obj, obj)))
		    {
		        obj_destroy(action_obj);
		        ABClipboard->action_count--;
		    }
                }
                trav_close(&trav);
            }

            if (undo_info->info.cut.to_action_list)
            {
                AB_TRAVERSAL	trav;
                ABObj		action_obj;

                for (trav_open(&trav, undo_info->info.cut.to_action_list,
                        AB_TRAV_ACTIONS | AB_TRAV_MOD_SAFE);
                    (action_obj = trav_next(&trav)) != NULL; )
                {
                    ABObj	from_obj;

                    from_obj = action_obj->info.action.from;

		    /*
		     * If the object being destroyed is the 'from' object OR
		     * it is a descendant of the 'from' obj, delete the
		     * connection object
		     */
		    if ((obj == from_obj) || (obj_is_descendant_of(from_obj, obj)))
		    {
		        obj_destroy(action_obj);
		        ABClipboard->action_count--;
		    }
                }
                trav_close(&trav);
            }
	}
    }

    return (0);
}

/*
 * Initializes the clipboard
 */
static void
init_clipboard(void)
{
    if (!ABClipboard)
    {
	ABClipboard = (ABClipboardRec)malloc(sizeof(AB_CLIPBOARD_REC));
	ABClipboard->list = NULL;
	ABClipboard->info_list = NULL;
	ABClipboard->count = 0;
	ABClipboard->action_count = 0;
	ABClipboard->size = 0;
    }
}

/*
 * verify_clipboard_space()
 * Makes sure the clipboard has enough nodes for 'count' more 
 * objects.
 */
static void
verify_clipboard_space(
    int		count
)
{
    int		num_nodes;

    init_clipboard();

    /*
     * Return immediately if have enuff space
     */
    if (ABClipboard->size - ABClipboard->count >= count)
	return;

    /*
     * Calculate min # of blocks/nodes for new count
     */
    num_nodes = ABOBJ_CLIPBOARD_BLOCK_SIZE * 
		((count/ABOBJ_CLIPBOARD_BLOCK_SIZE) + 1);

    /*
     * Realloc space for new count
     */
    ABClipboard->list = (ABObj *)realloc((void *)ABClipboard->list, 
					(num_nodes * sizeof(ABObj)));
    ABClipboard->info_list = (ABClipbInfo)realloc(ABClipboard->info_list,
					(num_nodes * sizeof(AB_CLIPB_INFO)));


    /*
     * If realloc went OK
     */
    if (ABClipboard->list)
    {
	int	remainder = num_nodes - ABClipboard->count;

	/*
	 * Set 'new' blocks to zero. Leave existing
	 * blocks untouched.
	 */
        memset((void *)&ABClipboard->list[ABClipboard->count], 
			0, 
			(remainder * sizeof(ABObj)));

        memset((void *)&ABClipboard->info_list[ABClipboard->count], 
			0, 
			(remainder * sizeof(AB_CLIPB_INFO)));

	/*
	 * Set new size to new node count
	 */
        ABClipboard->size = num_nodes;
    }
}

/*
 * resolve_none_attachments()
 * If 'obj' has opposite attachments of type AB_ATTACH_NONE, then
 * AB_ATTACH_POINT attachments are created on one of the sides.
 */
static void
resolve_none_attachments(
    ABObj	obj
)
{
    AB_ATTACH_TYPE	north_type,
			south_type,
			east_type,
			west_type;
    Position		x = 0, 
			y = 0;

    if (!obj)
	return;

    /*
     * If no attachments exist, create them
     */
    if (obj->attachments == NULL)
        obj_init_attachments(obj);

    /*
     * If this object has a widget associated with it, get
     * (x, y) position
     */
    if (objxm_get_widget(obj))
    {
        XtVaGetValues(objxm_get_widget(obj),
            XmNx,		&x,
            XmNy,		&y,
            NULL);
    }

    /*
     * Get all attachment types
     */
    north_type = obj_get_attach_type(obj, AB_CP_NORTH);
    south_type = obj_get_attach_type(obj, AB_CP_SOUTH);
    east_type = obj_get_attach_type(obj, AB_CP_EAST);
    west_type = obj_get_attach_type(obj, AB_CP_WEST);

    /*
     * If north/south attachments == NONE, create north point attachment
     */
    if ((north_type == AB_ATTACH_NONE) && (south_type == AB_ATTACH_NONE))
    {
	if (objxm_get_widget(obj))
            obj->y = (int)y;
	else
	{
	    if (obj->y < 0)
            obj->y = 0;
	}
        obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    }

    /*
     * If east/west attachments == NONE, create west point attachment
     */
    if ((east_type == AB_ATTACH_NONE) && (west_type == AB_ATTACH_NONE))
    {
	if (objxm_get_widget(obj))
            obj->x = (int)x;
	else
	{
	    if (obj->x < 0)
            obj->x = 0;
	}
        obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);
    }
}


/*
 * resolve_clipboard_connections()
 * This function resolves the connections issues to do with cut/copy
 * of objects.
 *
 * Currently, it makes sure the target ('to') object of the connections
 * on the clipboard are correct. If the target object was also
 * copied onto the clipboard, then the the connection's target field
 * is updated to be the clipboard object.
 *
 * It is important that this function is called during abobj_clipboard_add()
 * since at this point, the copied objects are not deleted yet. This is
 * because the 'live' pointers ( clipboard->list[j] ) on the clipboard are 
 * manipulated in this function.
 */
static void
resolve_clipboard_connections(
    ABObj		*obj,
    int			count,
    ABClipboardRec	clipboard
)
{
    AB_TRAVERSAL	trav;
    ABClipbInfo	info;
    ABObj	newObj,
		origObj,
		action_obj;
    int		i,
		j;

    if (!obj || (count <= 0) || !clipboard)
	return;

    for (i=0; i<clipboard->count; ++i)
    {
	/*
	 * Get ONE clipboard record
	 */
        info = &clipboard->info_list[i];

        if (!info->action_list)
	    continue;

        newObj = info->dup_obj;
        origObj = clipboard->list[i];

	/*
	 * For all the connections that were copied to the clipboard
	 * for this object ('newObj')...
	 */
        for (trav_open(&trav, info->action_list, AB_TRAV_ACTIONS);
            (action_obj = trav_next(&trav)) != NULL; )
        {
            ABClipbInfo	to_info;
	    BOOL	to_obj_copied = FALSE;
	    ABObj	orig_to_obj,
			new_to_obj;
	    int		save_index;

	    /*
	     * Check if the target ('to') object was copied into the
	     * clipboard
	     */

	    /*
	     * Skip connections if the action type is not 'predefined' or
	     * if the source/target are the same. These connections should 
	     * have the target field set up properly when they were first 
	     * dup'd in abobj_clipboard_add()
	     */
	    if ((obj_get_func_type(action_obj) != AB_FUNC_BUILTIN) ||
		(action_obj->info.action.to == action_obj->info.action.from))
		continue;

	    orig_to_obj = action_obj->info.action.to;

	    /*
	     * Scan clipboard and check if 'orig_to_obj' is there.
	     * (or if it is a descendant of anything there)
	     */
	    for (j = 0; j < clipboard->count; ++j)
	    {
		if ((orig_to_obj == clipboard->list[j]) || 
		    (obj_is_descendant_of(orig_to_obj, clipboard->list[j])))
		{
		    to_obj_copied = TRUE;
		    save_index = j;
		    break;
		}
	    }

	    if (!to_obj_copied)
		continue;

	    /*
	     * Target object was copied onto clipboard
	     */

            to_info = &clipboard->info_list[save_index];

	    /*
	     * Search for the dup'd object. It may not be the copied object,
	     * itself, but a descendant of it.
	     *
	     * This depends on the fact that the dup'd tree maintains objs
	     * with same names
	     *
	     * This also depends on the fact that the 'info_list', and 'list' fields
	     * on the clipboard have identical indices.
	     */
	    new_to_obj = obj_find_by_name_and_type(to_info->dup_obj,
				obj_get_name(orig_to_obj),
				obj_get_type(orig_to_obj));

	    action_obj->info.action.to = new_to_obj;

        }
	trav_close(&trav);
    }
}


/*
 * resolve_clipboard_attachments()
 * This function resolves the attachments issues to do with cut/copy
 * of objects.
 *
 * Currently, it makes sure the attached object of all the attachments
 * on the clipboard are correct. If the attached object was also
 * copied onto the clipboard, then the attachment is updated to 
 * point to the clipboard object. Otherwise, the attachment is changed
 * to AB_ATTACH_NONE.
 *
 * It is important that this function is called during abobj_clipboard_add()
 * since at this point, the copied objects are not deleted yet. This is
 * because the 'live' pointers ( clipboard->list[j] ) on the clipboard are 
 * manipulated in this function.
 */
static void
resolve_clipboard_attachments(
    ABObj		*obj,
    int			count,
    ABClipboardRec	clipboard
)
{
    AB_TRAVERSAL	trav;
    ABClipbInfo	info;
    ABObj	*new_list = NULL;
    ABObj	newObj,
		origObj,
		childObj,
		action_obj;
    int		i,
		j;

    if (!obj || (count <= 0) || !clipboard)
	return;

    /*
     * 'new_list' is a list of all the new (i.e. dup'd)
     * objects. It is used to resolve object type attachments
     */
    if (count > 0)
        new_list = (ABObj *)util_malloc(count * sizeof(ABObj));

    /*
     * Populate the new list of objects
     */
    for (i=0; i<clipboard->count; ++i)
    {
	/*
	 * Get ONE clipboard record
	 */
        info = &clipboard->info_list[i];

	/*
	 * Get handle to dup'd object and put in on the new list
	 */
        newObj = info->dup_obj;
	new_list[i] = newObj;
    }

    /*
     * Resolve all attachments of every object on the clipboard. Do the
     * same for the descendants of these objects.
     */
    for (i=0; i<clipboard->count; ++i)
    {
	/*
	 * Get ONE clipboard record
	 */
        info = &clipboard->info_list[i];

	/*
	 * newObj -> dup'd object
	 * origObj -> the object in the UI that was cut/copied. 
	 *            This ptr is live.
	 */
        newObj = info->dup_obj;
        origObj = clipboard->list[i];

	/*
	 * Resolve attachment for clipboard 'top level' object.
	 */
	resolve_attach_one_obj(newObj, origObj, new_list, clipboard->count);

	/*
	 * Do the same for all salient descendants 
	 */
        for (trav_open(&trav, newObj, AB_TRAV_SALIENT);
            (childObj = trav_next(&trav)) != NULL; )
        {
	    ABObj	origChildObj = NULL;

	    /*
	     * Skip the top level object - taken care of above
	     */
	    if (childObj == newObj)
		continue;

	    /*
	     * Get the handle to the child object that
	     * corresponds to the new dup'd one
	     */
	    origChildObj = obj_find_by_name_and_type(origObj, 
				obj_get_name(childObj), 
				obj_get_type(childObj));

	    /*
	     * Resolve attachment for descendant object.
	     */
	    resolve_attach_one_obj(childObj, origChildObj, new_list, clipboard->count);
	}
	trav_close(&trav);
    }

    /*
     * Free new list if it was allocated
     */
    if (new_list)
	util_free(new_list);
}

/*
 * resolve_attach_one_obj()
 * This function resolves the attachments of the object 'obj'
 *
 *	obj		- obj whose attachments need resolving
 *	origObj		- the 'original' object which 'obj' is 
 *			  a copy of. This list may be 'live' i.e.
 *			  point to actual objects in the UI. In
 *			  cut operations, these objects will be
 *			  deleted after being copied onto the
 *			  clipboard.
 *	new_list	- the entire list of objects that was 
 *			  copied in this copy/cut operation.
 *			  This is a list of copied/static objects.
 *	new_count	- number of objects in 'new_list'.
 */
static void
resolve_attach_one_obj(
    ABObj		obj,
    ABObj		origObj,
    ABObj		*new_list,
    int			new_count
)
{
    if (!obj || !origObj)
	return;
    
    resolve_attach_one_attachment(AB_CP_NORTH, obj, origObj, new_list, new_count);

    resolve_attach_one_attachment(AB_CP_SOUTH, obj, origObj, new_list, new_count);

    resolve_attach_one_attachment(AB_CP_EAST, obj, origObj, new_list, new_count);

    resolve_attach_one_attachment(AB_CP_WEST, obj, origObj, new_list, new_count);
}

/*
 * resolve_attach_one_attachment()
 * This function resolves the attachments of the object 'obj' in the
 * 'dir' direction.
 *
 * Currently, it makes sure the attached object of all the attachments
 * on the clipboard are correct. If the attached object was also
 * copied onto the clipboard, then the attachment is updated to 
 * point to the clipboard object. Otherwise, the attachment is changed
 * to AB_ATTACH_NONE. For attach-to-parent type attachments, top level
 * clipboard objects will have the attached object field set to NULL
 * since the new parent will not be known until paste time.
 *
 *	dir		- direction of attachment to be resolved
 *	obj		- obj whose attachments need resolving
 *	origObj		- the 'original' object which 'obj' is 
 *			  a copy of. This list may be 'live' i.e.
 *			  point to actual objects in the UI. In
 *			  cut operations, these objects will be
 *			  deleted after being copied onto the
 *			  clipboard. On the other hand this can point
 *			  to objects on the clipboard. During a paste
 *			  operation, objects are copied from the
 *			  clipboard. During a copy operation, objects
 *			  are copied from the UI (to the clipboard).
 *	new_list	- the entire list of objects that was 
 *			  copied in this copy/cut operation.
 *			  This is a list of copied/static objects.
 *	new_count	- number of objects in 'new_list'.
 */
static void
resolve_attach_one_attachment(
    AB_COMPASS_POINT	dir,
    ABObj		obj,
    ABObj		origObj,
    ABObj		*new_list,
    int			new_count
)
{
    ABAttachment	*one_attachment = NULL;
    AB_ATTACH_TYPE	attachType;
    ABObj		attachObj = NULL,
			parentObj = NULL,
			origAttachObj = NULL,
			origParentObj = NULL;
    int			offset = 0;

    if (!obj || !origObj || !new_list)
	return;
    
    /*
     * Get attachment in the specified direction
     */
    one_attachment = obj_get_attachment(obj, dir);

    if (!one_attachment)
	return;
    
    /*
     * Get the attachment type
     */
    attachType = obj_get_attach_type(obj, dir);

    switch (attachType)
    {
	case AB_ATTACH_POINT:
	case AB_ATTACH_GRIDLINE:
	case AB_ATTACH_CENTER_GRIDLINE:
	case AB_ATTACH_NONE:
	/* No resolution needed */
	break;

	/*
	 * These are the only 2 attachment types which contain
	 * object references
	 */
	case AB_ATTACH_ALIGN_OBJ_EDGE:
	case AB_ATTACH_OBJ:
	    origAttachObj = obj_get_attach_value(origObj, dir);
	    origParentObj = obj_get_parent(origObj);

	    offset = obj_get_attach_offset(origObj, dir);

	    attachObj = obj_get_attach_value(obj, dir);
	    parentObj = obj_get_parent(obj);

	    /*
	     * Check if original object had a parent
	     */
	    if (origParentObj)
	    {
		/*
		 * Orignal object has parent.
		 * This can mean:
		 *	obj is a copy of a live object
		 *	obj is a copy of a descendant of a clipboard object
		 */

		/*
		 * Check if the attached object is the parent
		 */
		if (origAttachObj == origParentObj)
		{
		    /*
		     * Attach to parent
		     */

		    if (parentObj)
		    {
			 /*
			  * The dup'd object does have a parent.
			  * Use this as the new attached object.
			  */
		         attachObj = parentObj;
		    }
		    else
		    {
		        /*
			 * obj does not have a parent.
			 * This means that it is a top level object on
			 * the new list/clipboard
		         * Since the actual parent object will not be known
		         * until paste time, set this to NULL for now.
		         */
		         attachObj = NULL;
		    }
		}
		else
		{
		    /*
		     * Attach to sibling
		     */

		    if (parentObj)
		    {
		        /*
			 * obj has a parent. This means that it's parent 
			 * (and all it's siblings were copied onto the new list).
			 * Search for corresponding sibling object starting at
			 * the parent object.
			 */
			attachObj = obj_find_by_name_and_type(parentObj, 
					obj_get_name(origAttachObj), 
					obj_get_type(origAttachObj));
			
			/*
			 * Make sure the found object is a sibling of obj
			 * If not, nullify attachObj.
			 */
			if (attachObj && (obj_get_parent(attachObj) != parentObj))
			{
			    attachObj = NULL;
			}

			/*
			 * If attached sibling not found, change attachment 
			 * type to AB_ATTACH_NONE
			 */
			if (attachObj == NULL)
			{
			    attachType = AB_ATTACH_NONE;
			    offset = 0;
			}
		    }
		    else
		    {
			int	j;

		        /*
			 * obj does not have a parent.
			 * This means that it is a top level object on
			 * the new list.
			 * Check if the sibling object was also copied
			 * onto the new list as a top level object.
		         */

			attachObj = NULL;

		        for (j=0; j < new_count; ++j)
		        {
			    if (istr_equal(obj_get_name_istr(origAttachObj), 
					   obj_get_name_istr(new_list[j])) &&
				(obj_get_type(origAttachObj) == 
					obj_get_type(new_list[j])))
			    {
				attachObj = new_list[j];
			    }
		        }

			if (attachObj == NULL)
			{
			    attachType = AB_ATTACH_NONE;
			    offset = 0;
			}
		    }
		}
	    }
	    else
	    {
		/*
		 * Orignal object has no parent.
		 * This can mean:
		 *	obj is a copy of a top level clipboard object
		 *
		 * If any of the top level objects had attach-to-parent
		 * type attachments, the value field would be NULL. This
		 * will be resolved later, during paste. For now, we will
		 * only worry about sibling attachments.
		 */

		if (origAttachObj != NULL)
		{
		    int	j;

		    /*
		     * Check if the sibling object was also copied
		     * onto the new list as a top level object.
		     */

		    attachObj = NULL;

		    for (j=0; j < new_count; ++j)
		    {
		        if (istr_equal(obj_get_name_istr(origAttachObj), 
		            obj_get_name_istr(new_list[j])) &&
		            (obj_get_type(origAttachObj) == 
		                obj_get_type(new_list[j])))
		        {
		            attachObj = new_list[j];
		        }
		    }

		    /*
		     * If the sibling was not found, change the attachment type 
		     * to AB_ATTACH_NONE.
		     */
		    if (attachObj == NULL)
		    {
		        attachType = AB_ATTACH_NONE;
		        offset = 0;
		    }
		}
	    }

	/*
	 * Set new values of attachment
	 */
	(void)obj_set_attachment(obj, dir, attachType, (void *)attachObj, offset);

	break;
    }
}

/*
 * resolve_attach_parent()
 * For top level objects on the clipboard, if the attachment is
 * of type 'attach to parent', the attachment value field is set to
 * NULL since we don't know what the actual parent will be until
 * the paste is made. This function resolves all the NULL attachment
 * values after the paste is done.
 */
static void
resolve_attach_parent(
    ABObj		obj,
    ABObj		new_parent
)
{
    if (!obj || !new_parent)
	return;
    
    resolve_attach_parent_dir(AB_CP_NORTH, obj, new_parent);
    resolve_attach_parent_dir(AB_CP_SOUTH, obj, new_parent);
    resolve_attach_parent_dir(AB_CP_EAST, obj, new_parent);
    resolve_attach_parent_dir(AB_CP_WEST, obj, new_parent);
}

/*
 * resolve_attach_parent_dir()
 * Same as resolve_attach_parent() but for a specific dir
 */
static void
resolve_attach_parent_dir(
    AB_COMPASS_POINT	dir,
    ABObj		obj,
    ABObj		new_parent
)
{
    ABAttachment	*one_attachment;
    AB_ATTACH_TYPE	attachType;
    ABObj		attachObj = NULL;

    if (!obj || !new_parent)
	return;

    one_attachment = obj_get_attachment(obj, dir);
    attachType = obj_get_attach_type(obj, dir);
    attachObj = (void *)obj_get_attach_value(obj, dir);

    if ((attachType == AB_ATTACH_OBJ) && (attachObj == NULL))
    {
	obj_set_attach_value(obj, dir, new_parent);
    }
}

/*
 * resolve_undo_rec_connections()
 * This function resolves the connections issues to do 
 * with undoing cut/delete
 *
 * It figures out if any of the source/target of the connections
 * have to be reassigned to objects that were also cut/deleted,
 * and now live on the undo record.
 */
static void
resolve_undo_rec_connections(
    ABObj	*obj,
    int		count,
    ABUndoRec	undo_rec
)
{
    AB_TRAVERSAL	trav;
    ABUndoInfo		undo_info;
    ABObj		origObj,
			from_action_list,
			to_action_list,
			action_obj;
    int			i,
			j;

    if (!obj || (count <= 0) || !undo_rec)
	return;

    if (undo_rec->action_count <= 0)
	return;

    for (i=0; i<undo_rec->count; ++i)
    {
        undo_info = &undo_rec->info_list[i];

        from_action_list = undo_info->info.cut.from_action_list;

        if (from_action_list)
	{
            for (trav_open(&trav, from_action_list, AB_TRAV_ACTIONS);
                (action_obj = trav_next(&trav)) != NULL; )
            {
                ABUndoInfo	to_info,
				from_info;
	        BOOL	to_obj_copied = FALSE,
			from_obj_copied = FALSE;
	        ABObj	orig_to_obj,
			new_to_obj,
			orig_from_obj,
			new_from_obj;
	        int	from_save_index,
			to_save_index;

	        /*
	         * Check if the 'to' object was copied into the
	         * clipboard
	         */

	        orig_to_obj = action_obj->info.action.to;
	        orig_from_obj = action_obj->info.action.from;

		/*
		 * Skip if source == target
		 */
	        if ((obj_get_func_type(action_obj) != AB_FUNC_BUILTIN) ||
		    (orig_to_obj == orig_from_obj))
		    continue;

	        for (j = 0; j < undo_rec->count; ++j)
	        {
		    if ((orig_to_obj == undo_rec->list[j]) || 
		        (obj_is_descendant_of(orig_to_obj, undo_rec->list[j])))
		    {
		        to_obj_copied = TRUE;
		        to_save_index = j;
		    }
	        }

	        if (!to_obj_copied)
		    continue;

	        to_info = &undo_rec->info_list[to_save_index];

	        /*
	         * This depends on the fact that the dup'd tree maintains objs
	         * with same names
	         */
	        new_to_obj = obj_find_by_name_and_type(to_info->info.cut.dup_obj,
	                        obj_get_name(orig_to_obj),
	                        obj_get_type(orig_to_obj));

	        action_obj->info.action.to = new_to_obj;
            }
	    trav_close(&trav);
	}


        to_action_list = undo_info->info.cut.to_action_list;

        if (to_action_list)
	{
            for (trav_open(&trav, to_action_list, AB_TRAV_ACTIONS);
                (action_obj = trav_next(&trav)) != NULL; )
            {
                ABUndoInfo	to_info,
			from_info;
	        BOOL	to_obj_copied = FALSE,
			from_obj_copied = FALSE;
	        ABObj	orig_to_obj,
			new_to_obj,
			orig_from_obj,
			new_from_obj;
	        int	from_save_index,
			to_save_index;

	        /*
	         * Check if the 'from' object was copied into the
	         * clipboard
	         */

	        orig_to_obj = action_obj->info.action.to;
	        orig_from_obj = action_obj->info.action.from;

	        if (orig_to_obj == orig_from_obj)
		    continue;

	        for (j = 0; j < undo_rec->count; ++j)
	        {
		    if ((orig_from_obj == undo_rec->list[j]) || 
		        (obj_is_descendant_of(orig_from_obj, undo_rec->list[j])))
		    {
		        from_obj_copied = TRUE;
		        from_save_index = j;
		    }
	        }

	        if (!from_obj_copied)
		    continue;

	        from_info = &undo_rec->info_list[from_save_index];

	        /*
	         * This depends on the fact that the dup'd tree maintains objs
	         * with same names
	         */
	        new_from_obj = obj_find_by_name_and_type(from_info->info.cut.dup_obj,
	                        obj_get_name(orig_from_obj),
	                        obj_get_type(orig_from_obj));

	        action_obj->info.action.from = new_from_obj;
            }
	    trav_close(&trav);
	}
    }
}

/*
 * resolve_undo_rec_attachments()
 * This function resolves the attachments issues to do 
 * with undoing cut/delete
 *
 * Currently, it makes sure the attached object of all the attachments
 * on the undo record are correct. If the attached object was also
 * copied onto the undo record, then the attachment is updated to 
 * point to the undo record object. Otherwise, the attachment is changed
 * to AB_ATTACH_NONE.
 *
 * It is important that this function is called during 
 * abobj_set_undo()/set_undo_rec() since at this point, the copied 
 * objects are not deleted yet. This is because the 'live' pointers 
 * ( undo_rec->list[j] ) on the undo record are manipulated in this function.
 */
static void
resolve_undo_rec_attachments(
    ABObj	*obj,
    int		count,
    ABUndoRec	undo_rec
)
{
    AB_TRAVERSAL	trav;
    ABClipbInfo	info;
    ABUndoInfo		undo_info;
    ABObj	*new_list = NULL;
    ABObj	newObj,
		origObj,
		childObj,
		action_obj;
    int		i,
		j;

    if (!obj || (count <= 0) || !undo_rec)
	return;

    /*
     * 'new_list' is a list of all the new (i.e. dup'd)
     * objects. It is used to resolve object type attachments
     */
    if (count > 0)
        new_list = (ABObj *)util_malloc(count * sizeof(ABObj));

    /*
     * Populate the new list of objects
     */
    for (i=0; i<undo_rec->count; ++i)
    {
	/*
	 * Get ONE undo info record
	 */
        undo_info = &(undo_rec->info_list[i]);

	if ((undo_info->type != AB_UNDO_CUT) || 
	    (undo_info->type != AB_UNDO_DELETE))
	    util_dprintf(1, 
		"resolve_undo_rec_attachments: Warning - undo type is not cut or delete!\n");

	/*
	 * Get handle to dup'd object and put in on the new list
	 */
        newObj = undo_info->info.cut.dup_obj;
	new_list[i] = newObj;
    }

    /*
     * Resolve all attachments of every object on the undo record. Do the
     * same for the descendants of these objects.
     */
    for (i=0; i<undo_rec->count; ++i)
    {
	/*
	 * Get ONE undo info record
	 */
        undo_info = &(undo_rec->info_list[i]);

	/*
	 * newObj -> dup'd object
	 * origObj -> the object in the UI that was cut/copied. 
	 *            This ptr is live.
	 */
        newObj = undo_info->info.cut.dup_obj;
        origObj = undo_rec->list[i];

	/*
	 * Resolve attachment for clipboard 'top level' object.
	 */
	resolve_attach_one_obj(newObj, origObj, new_list, undo_rec->count);

	/*
	 * Do the same for all salient descendants 
	 */
        for (trav_open(&trav, newObj, AB_TRAV_SALIENT);
            (childObj = trav_next(&trav)) != NULL; )
        {
	    ABObj	origChildObj = NULL;

	    /*
	     * Skip the top level object - taken care of above
	     */
	    if (childObj == newObj)
		continue;

	    /*
	     * Get the handle to the child object that
	     * corresponds to the new dup'd one
	     */
	    origChildObj = obj_find_by_name_and_type(origObj, 
				obj_get_name(childObj), 
				obj_get_type(childObj));

	    /*
	     * Resolve attachment for descendant object.
	     */
	    resolve_attach_one_obj(childObj, origChildObj, new_list, undo_rec->count);
	}
	trav_close(&trav);
    }

    /*
     * Free new list if it was allocated
     */
    if (new_list)
	util_free(new_list);
}


/*
 * Initializes one undo record buffer
 */
static void
init_undo_rec(
    ABUndoRec	*undo_rec_ptr
)
{
    ABUndoRec	undo_rec;

    if (!undo_rec_ptr)
	return;

    if (!(*undo_rec_ptr))
    {
	undo_rec = (ABUndoRec)malloc(sizeof(AB_UNDO_REC));
	undo_rec->list = NULL;
	undo_rec->info_list = NULL;
	undo_rec->undo_func = NULL;
	undo_rec->count = 0;
	undo_rec->action_count = 0;
	undo_rec->size = 0;
	
	*undo_rec_ptr = undo_rec;
    }
}

/*
 * Initializes undo buffer
 */
static void
init_undo(void)
{
    init_undo_rec(&ABUndo);
}

/*
 * verify_undo_rec_space()
 * Makes sure the undo buffer has enough nodes for 'count' more 
 * objects.
 */
static void
verify_undo_rec_space(
    ABUndoRec	*undo_rec_ptr,
    int		count
)
{
    ABUndoRec	undo_rec;
    int		num_nodes;

    init_undo_rec(undo_rec_ptr);

    undo_rec = *undo_rec_ptr;

    if (undo_rec->size - undo_rec->count >= count)
	return;

    /*
     * Calculate min # of blocks/nodes for new count
     */
    num_nodes = ABOBJ_UNDO_BLOCK_SIZE * ((count/ABOBJ_UNDO_BLOCK_SIZE) + 1);

    /*
     * Realloc space for new count
     */
    undo_rec->list = (ABObj *)realloc((void *)undo_rec->list, 
					(num_nodes * sizeof(ABObj)));
    undo_rec->info_list = (ABUndoInfo)realloc(undo_rec->info_list,
					(num_nodes * sizeof(AB_UNDO_INFO)));


    if (undo_rec->list)
    {
	int	remainder = num_nodes - undo_rec->count;

	/*
	 * Set 'new' blocks to zero. Leave existing
	 * blocks untouched.
	 */
        memset((void *)&undo_rec->list[undo_rec->count], 
			0, 
			(remainder * sizeof(ABObj)));

        memset((void *)&undo_rec->info_list[undo_rec->count], 
			0, 
			(remainder * sizeof(AB_UNDO_INFO)));

	/*
	 * Set new size to new node count
	 */
        undo_rec->size = num_nodes;
    }
}

static void
verify_undo_space(
    int		count
)
{
    verify_undo_rec_space(&ABUndo, count);
}

/*
 * remove_clipboard_descendants()
 * When copying/cutting objects onto the clipboard,
 * any object that is a descendant of another is removed.
 * For example, if a button and it's container are copied,
 * the button is removed from the copy list since it will
 * be copied anyways since it is in the container's 
 * subtree.
 */
static void
remove_clipboard_descendants(
    ABSelectedRec	*sel
)
{
    ABObj	curObj,
		curAncestor,
		*buf;
    int		i,
		j,
		numNullified = 0;

    if (!sel)
	return;

    /*
     * Make copy of select list
     */
    buf = (ABObj*)XtMalloc(sel->count * sizeof(ABObj));
    memcpy(buf, sel->list, (sel->count * sizeof(ABObj)));

    for (i = 0; i < sel->count; ++i)
    {
	/*
	 * Current object to test if it needs to be deleted 
	 * (set to NULL) from list
	 */
	curObj = sel->list[i];

	for (j = 0; j < sel->count; ++j)
	{
	    curAncestor = buf[j];

	    /*
	     * Skip if already set to NULL, or if this is the
	     * same object as curObj
	     */
	    if (!curObj || !curAncestor || (curObj == curAncestor))
		continue;
	    
	    /*
	     * Check if curAncestor is an ancestor of curObj
	     */
	    if (obj_is_descendant_of(curObj, curAncestor))
	    {
		buf[i] = NULL;

		/*
		 * Keep track of number of objects deleted from list
		 */
		++numNullified;
		break;
	    }
	}
    }

    /*
     * Update original selection list
     */
    for (i = 0, j = 0; i < sel->count; ++i)
    {
	if (buf[i])
	    sel->list[j++] = buf[i];
    }

    /*
     * Update original selection count
     */
    sel->count -= numNullified;

    free(buf);
}

/*
 * any_object_at_my_position()
 * Returns TRUE if there are any children of 'parent'
 * that lies EXACTLY at the same (x, y) position as 'obj'.
 */
static BOOL
any_object_at_my_position(
    ABObj	obj,
    ABObj	parent
)
{
    AB_TRAVERSAL	trav;
    ABObj		child;

    /*
    * Check if parent already has a child exactly at 
    * the same position as the pasted object
    */
    for (trav_open(&trav, parent, AB_TRAV_SALIENT_CHILDREN);
        (child = trav_next(&trav)) != NULL; )
    {
        if ((child->x == obj->x) && 
                (child->y == obj->y))
        {
	    return (TRUE);
        }
    }

    return (FALSE);
}

/*
 * stack_objects()
 * Changes the (x, y) position of 'newobj' if there are
 * objects in 'parent' already at the same position.
 * The search/test is done, incrementing X and Y by
 * STACKING_X_OFFSET, STACKING_Y_OFFSET.
 */
static void
stack_objects(
    ABObj	newObj,
    ABObj	parent
)
{
    BOOL		xy_changed = FALSE;

    if (!newObj || !parent)
	return;

    while (any_object_at_my_position(newObj, parent))
    {
	/*
	 * Offset the pasted object
	 */
	newObj->x += STACKING_X_OFFSET;
	newObj->y += STACKING_Y_OFFSET;
	xy_changed = TRUE;
    }

    if (xy_changed)
    {
        obj_set_attachment(newObj, AB_CP_NORTH, 
            AB_ATTACH_POINT, NULL, newObj->y);
        obj_set_attachment(newObj, AB_CP_WEST,  
            AB_ATTACH_POINT, NULL, newObj->x);
    }

}

/*
 * set_undo_rec()
 * Sets the passed undo record to contain the list of
 * objects and related info with regards to 'undo_type'.
 */
static int
set_undo_rec(
    ABUndoRec		*undo_rec_ptr,
    ABObj		*obj,
    int			count,
    ABUndoFunc		undo_func,
    AB_UNDO_TYPE	undo_type
)
{
    ABUndoRec	undo_rec;
    int		action_count = 0,
		i,
		j;
    Position	x = 0, 
		y = 0;
    Dimension	width = 0, 
		height = 0;

    if (!undo_rec_ptr || !obj || (count <= 0))
	return (0);

    clear_undo_rec(undo_rec_ptr);

    verify_undo_rec_space(undo_rec_ptr, count);

    undo_rec = *undo_rec_ptr;

    for (i=undo_rec->count, j=0; i<undo_rec->count + count; ++i, ++j)
    {
	ABUndoInfo	undo_info;
	AB_TRAVERSAL	trav;
	ABObj		project, action_obj;
	BOOL		match_to, match_from, connect_to_self;

	undo_rec->list[i] = obj[j];
	undo_info = &(undo_rec->info_list[i]);
	undo_info->type = undo_type;

	switch (undo_type)
	{
	case AB_UNDO_CUT:	/* undo for cut == undo for delete */
	case AB_UNDO_DELETE:
	    /*
	     * Dup cut object and store parent
	     */
	    undo_info->info.cut.dup_obj = abobj_dup_tree(obj[j]);
	    undo_info->info.cut.parent = obj_get_parent(obj[j]);

	    project = obj_get_project(obj[j]);

	    if (!project)
		break;

	    /*
	     * Duplicate all ACTIONs of cut/deleted object
	     * Search for all ACTIONS that have this object (or any of it's
	     * descendants) as the source/target.
	     */
	    for (trav_open(&trav, project, AB_TRAV_ACTIONS);
			(action_obj = trav_next(&trav)) != NULL; )
	    {
		/*
		 * Does this action have a source ('from') that
		 * matches the cut object or any of it's descendants ?
		 */
	        match_from = ((action_obj->info.action.from == obj[j]) || 
		      (obj_is_descendant_of(action_obj->info.action.from, 
					obj[j])));

		/*
		 * Does this action have a target ('to') that
		 * matches the cut object or any of it's descendants ?
		 * Note: The 'to' field in actions is relevant only for 
		 * predefined action types.
		 */
	        match_to = 
		    (obj_get_func_type(action_obj) == AB_FUNC_BUILTIN) &&
		    ((action_obj->info.action.to == obj[j]) || 
		     (obj_is_descendant_of(action_obj->info.action.to, obj[j])));

		/*
		 * Is the source in this action also the target ?
		 */
		connect_to_self = 
		    (obj_get_func_type(action_obj) == AB_FUNC_BUILTIN) &&
		    (action_obj->info.action.to == action_obj->info.action.from);

		/*
		 * If any match ...
		 */
	        if (match_from || match_to)
	        {
                    ABObj	new_action, new_from_obj, new_to_obj;

		    /*
		     * Duplicate action object
		     */
		    new_action = abobj_dup_tree(action_obj);

		    /*
		     * A 'from' match
		     */
		    if (match_from)
		    {
			/*
			 * Create action list to hold all the actions
			 */
		        if (!undo_info->info.cut.from_action_list)
		            undo_info->info.cut.from_action_list = 
				    obj_create(AB_TYPE_ACTION_LIST, NULL);

			/*
			 * Since the action's source ('from') is the object
			 * that is copied onto the undo record, it will
			 * need to be reassigned to the new (copied) 'from'
			 * object.
			 */

		        /*
		         * This depends on the fact that the dup'd tree maintains objs
		         * with same names
		         */
		        new_from_obj = 
		            obj_find_by_name_and_type(undo_info->info.cut.dup_obj,
				obj_get_name(action_obj->info.action.from),
				obj_get_type(action_obj->info.action.from));
	                new_action->info.action.from = new_from_obj;

			/*
			 * If connected to itself, reassign the target ('to')
			 * object as well.
			 */
			if (connect_to_self)
	                    new_action->info.action.to = new_from_obj;

		        obj_add_action(undo_info->info.cut.from_action_list, 
						new_action);

		        action_count++;
		    }

		    /*
		     * A 'to' match
		     */
		    if (!connect_to_self && match_to)
		    {
			/*
			 * Create action list to hold all the actions
			 */
		        if (!undo_info->info.cut.to_action_list)
		            undo_info->info.cut.to_action_list = 
				    obj_create(AB_TYPE_ACTION_LIST, NULL);

			/*
			 * Since the action's target ('to') is the object
			 * that is copied onto the undo record, it will
			 * need to be reassigned to the new (copied) 'to'
			 * object.
			 */

		        new_to_obj = 
		            obj_find_by_name_and_type(undo_info->info.cut.dup_obj,
				obj_get_name(action_obj->info.action.to),
				obj_get_type(action_obj->info.action.to));
	                new_action->info.action.to = new_to_obj;

		        obj_add_action(undo_info->info.cut.to_action_list, 
						new_action);
			
		        action_count++;
		    }

	        }
	    }
	    trav_close(&trav);
	    
	/*
        fprintf(stderr, 
	"abobj_set_undo(%s, AB_UNDO_CUT/DELETE): X=%d, Y=%d, W=%d, H=%d\n",
	    obj_get_name(obj[j]), 
	    obj_get_x(obj[j]), obj_get_y(obj[j]),
	    obj_get_width(obj[j]), obj_get_height(obj[j]));
	*/
	break;

	case AB_UNDO_PASTE:
	    /*
	     * No extra info need to be stored for paste.
	     */
	break;

	case AB_UNDO_MOVE:
	    /*
	     * Get (x, y) from widget.
	     * If ui handle is bogus, get from ABObj
	     */
            if (objxm_get_widget(obj[j]))
            {
	        XtVaGetValues(objxm_get_widget(obj[j]),
			XmNx,      &x,
			XmNy,      &y,
			NULL);
            }
            else
            {
	        x = (int)obj_get_x(obj[j]);
	        y = (int)obj_get_y(obj[j]);
            }

	    undo_info->info.move.x = x;
	    undo_info->info.move.y = y;

	/*
        fprintf(stderr, "abobj_set_undo(%s): X=%d, Y=%d\n",
	    obj_get_name(obj[j]), x, y);
	*/

	break;

	case AB_UNDO_RESIZE:
	    /*
	     * Get current size from widget.
	     * If ui handle is bogus, get from ABObj
	     */
            if (objxm_get_widget(obj[j]))
            {
		width = abobj_get_actual_width(obj[j]);
		height = abobj_get_actual_height(obj[j]);
            }
            else
            {
	        width = (int)obj_get_width(obj[j]);
	        height = (int)obj_get_height(obj[j]);
            }

	    undo_info->info.resize.width = width;
	    undo_info->info.resize.height = height;

	/*
        fprintf(stderr, "abobj_set_undo(%s): W=%d, H=%d\n",
	    obj_get_name(obj[j]), width, height);
	*/

	break;

	case AB_UNDO_GROUP:
	    /*
	     * No extra info need to be stored for undoing group.
	     */
	break;

	case AB_UNDO_UNGROUP:
	{
            AB_TRAVERSAL	trav;
	    int		member_count, member_index;
	    ABObj	*member_list;
	    ABObj	member;

	    /*
	     * Check if obj is a group ??
	     */

	    /*
	     * Dup group object.
	     * Also store pointers to all it's members.
	     */

	    undo_info->info.ungroup.dup_old_group = abobj_dup(obj[j]);

	    member_count = trav_count(obj[j], 
			AB_TRAV_SALIENT_CHILDREN | AB_TRAV_MOD_SAFE);
	    
	    member_list = (ABObj *)malloc(member_count * sizeof(ABObj));

	    for (trav_open(&trav, obj[j], AB_TRAV_SALIENT_CHILDREN |
	                AB_TRAV_MOD_SAFE), member_index = 0;
	                (member = trav_next(&trav)) != NULL; ++member_index)
	    {
		member_list[member_index] = member;
	    }

	    undo_info->info.ungroup.member_list = member_list;
	    undo_info->info.ungroup.member_count = member_count;
	}
	break;

	default:
		fprintf(stderr, "abobj_set_undo_rec: Bad undo type %d\n",
				undo_type);
	}

    }

    undo_rec->undo_func = undo_func;
    undo_rec->count += count;
    undo_rec->action_count += action_count;

    if ((undo_type == AB_UNDO_CUT) || (undo_type == AB_UNDO_DELETE))
    {
        resolve_undo_rec_connections(obj, count, undo_rec);
        resolve_undo_rec_attachments(obj, count, undo_rec);
    }

    return(0);
}

/*
 * clear_undo_rec()
 * Clears passed undo record.
 * The nodes for the info_list are cleared but not free'd
 */
static void
clear_undo_rec(
    ABUndoRec		*undo_rec_ptr
)
{
    ABUndoRec		undo_rec;
    int		i;
    ABObj	*list;

    if (!undo_rec_ptr)
	return;
    
    init_undo_rec(undo_rec_ptr);

    undo_rec = *undo_rec_ptr;

    if ((undo_rec->size <= 0) || (undo_rec->count <= 0))
        return;

    list = undo_rec->list;

    for (i = 0; i < undo_rec->count; ++i)
    {
	ABUndoInfo	undo_info;

	undo_info = &(undo_rec->info_list[i]);

	list[i] = NULL;

        switch (undo_info->type)
        {
	case AB_UNDO_CUT:	/* undo for cut == undo for delete */
	case AB_UNDO_DELETE:
	    /*
	     * Destroy cut object and NULLify ptrs
	     */
	    if (undo_info->info.cut.dup_obj)
	        obj_destroy(undo_info->info.cut.dup_obj);

	    if (undo_info->info.cut.from_action_list)
	        obj_destroy(undo_info->info.cut.from_action_list);

	    if (undo_info->info.cut.to_action_list)
	        obj_destroy(undo_info->info.cut.to_action_list);

	    undo_info->info.cut.dup_obj = NULL;
	    undo_info->info.cut.from_action_list = NULL;
	    undo_info->info.cut.to_action_list = NULL;
	    undo_info->info.cut.parent = NULL;
	    undo_info->info.cut.pane_sibling = NULL;
	break;

	case AB_UNDO_PASTE:
	    /*
	     * No cleanup needed for paste
	     */
	break;

	case AB_UNDO_MOVE:
	    /*
	     * Set (x, y) to (0, 0)
	     */
	    undo_info->info.move.x = 0;
	    undo_info->info.move.y = 0;

	break;

	case AB_UNDO_RESIZE:
	    /*
	     * Set current size to 0 by 0
	     */
	    undo_info->info.resize.width = 0;
	    undo_info->info.resize.height = 0;

	break;

	case AB_UNDO_GROUP:
	    /*
	     * No cleanup needed for group.
	     */
	break;

	case AB_UNDO_UNGROUP:
	{
	    int		member_count, member_index;
	    ABObj	*member_list;
	    ABObj	member;

	    /*
	     * Dup group object.
	     * Also store pointers to all it's members.
	     */

	    if (undo_info->info.ungroup.dup_old_group)
	    {
	        obj_destroy(undo_info->info.ungroup.dup_old_group);
	        undo_info->info.ungroup.dup_old_group = NULL;
	    }

	    if (undo_info->info.ungroup.member_list)
	    {
	        free(undo_info->info.ungroup.member_list);
		undo_info->info.ungroup.member_list = NULL;
	    }

	    undo_info->info.ungroup.member_count = 0;
	}
	break;

	default:
	    /* catch-all to avoid compiler warnings */
	break;
        }

        undo_info->type = AB_UNDO_NO_TYPE;
    }

    /*
     * Set count to 0.
     * Set undo func to NULL.
     */
    undo_rec->count = 0;
    undo_rec->action_count = 0;
    undo_rec->undo_func = NULL;
}


/*
 * PUBLIC:
 * Editing functions: Cut, Copy, Paste, Delete, Undo
 */

int
abobj_cut(void)
{
    ABObj	project = proj_get_project();
    ABObj	newObj = NULL;
    ABObj	*clipb_list = NULL;
    ABSelectedRec sel;
    int		i;
    int		clipb_count = 0;
    int		iRet = 0;

    if (!project)
	iRet = -1;
    else
    {
        abobj_get_selected(project, FALSE, FALSE, &sel);

	remove_clipboard_descendants(&sel);

        /* 
         * This depends on the sel.list being ordered in TOP-DOWN
         * order!!!
         */

        /*
         * Copy objects to clipboard
         */
	abobj_clipboard_set(sel.list, sel.count);

        (void)abobj_set_undo(sel.list, sel.count, undo_cut, AB_UNDO_CUT);

        for (i = (sel.count - 1); i >= 0; i--)
        {
	    /* Set the dirty bit on the module */
	    abobj_set_save_needed(obj_get_module(sel.list[i]), TRUE);

	    obj_destroy(sel.list[i]);
        }

	if (sel.count > 0)
	    XtFree((char *)sel.list);
    }
    
    return (iRet);
}

int
abobj_copy(void)
{
    ABObj	project = proj_get_project(),
    		newObj;
    ABSelectedRec sel;
    int		  i;
    int		iRet = 0;

    if (!project)
	iRet = -1;
    else
    {
        abobj_get_selected(project, FALSE, FALSE, &sel);
    
	remove_clipboard_descendants(&sel);

	abobj_clipboard_set(sel.list, sel.count);

	if (sel.count > 0)
	    XtFree((char *)sel.list);
    }

    return (iRet);
}

int
abobj_paste(
    AB_PASTE_INITIATOR_TYPE initiator
)
{
    ABObj		project = proj_get_project(),
    			newObj, 
			root,
			newroot,
			parent;
    ABSelectedRec 	sel;
    STRING		name = (STRING) NULL;
    int		  	i = 0, j = 0;
    int			iRet = 0;
    STRING		errmsg = (STRING) NULL;
    STRING		i18n_msg = (STRING) NULL;
    char		Buf[MAXPATHLEN] = "";
    BOOL		Err = False;
    BOOL		CancelPaste = False;
    DTB_MODAL_ANSWER	answer = DTB_ANSWER_NONE;
    ABObj		obj_parent = (ABObj) NULL;
    XmString		xm_buf = (XmString) NULL;

    if (!project)
	iRet = -1;
    else
    {
        abobj_get_selected(project, FALSE, FALSE, &sel);

	/*
	 * Don't paste if number of selected objects is not one,
	 * or if clipboard is empty
	 */
        if (abobj_clipboard_is_empty() || (sel.count != 1))
	    iRet = -1;
	else
	{
	    ABObj	*new_list = NULL,
			*new_action_list = NULL,
			obj;
	    int		action_index = 0;

	    /* Need to check if the selected object is a legal
	     * parent for the objects in the clipboard.  If not,
	     * then "Edit->Paste" should be made insensitive.
	     */
            root = obj_get_root(sel.list[0]);
	    for (i=0; i < ABClipboard->count && !Err; ++i)
	    {
		ABClipbInfo	info;

		info = &(ABClipboard->info_list[i]);

                obj = info->dup_obj;

		newroot = NULL;

		if (obj_is_window(obj) || obj_is_menu(obj) || obj_is_message(obj))
		{
		    if (root)
		        root = obj_get_module(root);
		    else
			root = proj_get_cur_module();
		}
		else if (obj_is_menubar(obj))
		{
		    AB_TRAVERSAL        trav;
		    ABObj               winobj = NULL;
		    ABObj		nobj = NULL;

		    winobj = root;
                    while(!(obj_is_window(winobj) && !obj_is_sub(winobj)))
			winobj = obj_get_parent(winobj);
 
                    if (!obj_is_base_win(winobj))
		    {
			Err = True;
                        i18n_msg = catgets(Dtb_project_catd, 100, 37,
				"Menubar can be pasted to a Main Window only.");

                        if (errmsg != (STRING) NULL)
			    util_free(errmsg);
   
                        errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
                        strcpy(errmsg, i18n_msg);
		    }
		    else 
		     /* Check to make sure there isn't already a Menubar for
                      * this window
                      */
                    {
			for (trav_open(&trav, winobj, AB_TRAV_UI);
                            (nobj = trav_next(&trav)) != NULL; )
			{
                            if (obj_is_menubar(nobj))
			    {
			        Err = True;
	                        i18n_msg = catgets(Dtb_project_catd, 100, 27,  
				"There is already a Menubar for the selected window.");
				if (errmsg != (STRING) NULL)
				    util_free(errmsg);  
   
				errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);  
				strcpy(errmsg, i18n_msg); 
			    }
			}
		    }
		}
		else if (obj_is_container(obj) && !obj_is_control_panel(obj) &&
			 !obj_is_group(obj))
        	{
	            if (root == NULL || (!(obj_is_container(root) &&
		       obj_is_sub(root) && obj_is_window(obj_get_root(root))
        	       && !obj_is_file_chooser(obj_get_root(root)))))
		    {
			Err = True;
                        i18n_msg = catgets(Dtb_project_catd, 100, 28,
				"Containers must be pasted to\na Main Window or Custom Dialog.");

                        if (errmsg != (STRING) NULL)
                            util_free(errmsg);

                        errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
                        strcpy(errmsg, i18n_msg);
		    }
        	}
		else if (obj_is_group(obj))
        	{
	            if (root == NULL || (!(obj_is_control_panel(obj_get_root(root))
		       || obj_is_group(obj_get_root(root)))))
		    {
			Err = True;
                        i18n_msg = catgets(Dtb_project_catd, 100, 57,
				"Groups must be pasted to\na group or control panel.");

                        if (errmsg != (STRING) NULL)
                            util_free(errmsg);

                        errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
                        strcpy(errmsg, i18n_msg);
		    }
		}
		else if (obj_is_pane(obj))
        	{
            	    if (root != NULL && (obj_is_pane(root) &&
                      !obj_is_window(obj_get_root(root))))
            	    {
                	if (!obj_is_control_panel(obj) && obj_is_control_panel(root))
                	{
			    /* Popup Modal Message and wait for answer */
			    dtb_palette_chld_or_layr_msg_initialize(
				&dtb_palette_chld_or_layr_msg);
			    answer = show_modal_msg_relative_initiator(
				root, initiator, &dtb_palette_chld_or_layr_msg,
				NULL, NULL, NULL);
			    switch(answer)
			    {
				case DTB_ANSWER_ACTION1: /* As Child */
				    break;

				case DTB_ANSWER_ACTION2: /* As Layered Pane */
				    obj_parent = abobj_handle_layered_pane(obj, root);
				    break;

                                case DTB_ANSWER_CANCEL: 
				    CancelPaste = TRUE;
				    break;
			    }
			}
			else
			{
			    /* If Pane is being pasted on a Pane which is a NORMAL
			     * child of a Control Panel, then use the Control 
			     * Panel as the actual parent instead of the pane.
			     */
			    if (obj_is_control_panel(
				    obj_get_root(
					obj_get_parent(
					    obj_get_root(root))))
				)  
			    {
			        obj_parent = obj_get_parent(obj_get_root(root));
			    }
			    else
			    {
				obj_parent = root;
			    }
 
			    /* Popup Modal Message and wait for answer */
			    dtb_palette_layered_pane_msg_initialize(
				&dtb_palette_layered_pane_msg);
			    answer = show_modal_msg_relative_initiator(
				obj_parent, initiator, &dtb_palette_layered_pane_msg,
				NULL, NULL, NULL);
			    switch(answer)
                    	    {
                        	case DTB_ANSWER_ACTION2: /* Layered Pane */
                            	    obj_parent = abobj_handle_layered_pane(obj, obj_parent);                 
                            	    break;
 
                        	case DTB_ANSWER_CANCEL: /* Cancel */
				    CancelPaste = TRUE;
                            	    break;
                    	    }
                	}
            	    }
		    /*
		     * Pasting a pane into a group
		     */
            	    else if (root != NULL && obj_is_group(root))
	    	    {
		        if (obj_is_control_panel(obj))
		        {
			    Err = True;
                            i18n_msg = catgets(Dtb_project_catd, 100, 59,
			"Control Panes must be pasted to a Main Window,\nCustom Dialog, or another pane.");

                            /* If we have an old buffer lying around, free it */
 
                            if (errmsg != (STRING) NULL)
                                util_free(errmsg);
  
                            errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
                            strcpy(errmsg, i18n_msg); 
		        }
		        else
		        {
		            dtb_palette_child_of_group_msg_initialize(
				&dtb_palette_child_of_group_msg);
			    answer = show_modal_msg_relative_initiator(
				root, initiator, &dtb_palette_child_of_group_msg,
				NULL, NULL, NULL);
                            switch(answer)
                            {
			        /* OK - Create as a child of group */
                                case DTB_ANSWER_ACTION1:
                                break;

                                case DTB_ANSWER_CANCEL:
				CancelPaste = TRUE;
			        break;
                            }
		        }
	            }
		    else if (root == NULL || (!obj_is_window(obj_get_root(root)) &&
				!(obj_is_container(root) && !obj_is_menubar(root))))
		    {
			Err = True;
                        i18n_msg = catgets(Dtb_project_catd, 100, 29,
				"Panes must be pasted to\na Main Window, Custom Dialog or Container.");

                        if (errmsg != (STRING) NULL)
                            util_free(errmsg);

                        errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
                        strcpy(errmsg, i18n_msg);
		    }
        	}  /* End if obj_is_pane() */
		else if (obj_is_control(obj))
        	{
		    newroot = get_ctrl_pane_or_group(root);

            	    if (newroot == NULL ||
                	(!obj_is_control_panel(obj_get_root(newroot)) &&
                	!obj_is_group(obj_get_root(newroot))))
		    {
			Err = True;
                        i18n_msg = catgets(Dtb_project_catd, 100, 30, 
				"Controls must be pasted to\na Control Panel or Group."); 

                        if (errmsg != (STRING) NULL) 
                            util_free(errmsg); 
 
                        errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1); 
                        strcpy(errmsg, i18n_msg); 
		    }
        	}
	    } /* End for loop */

	    if (Err)
	    {
		xm_buf = XmStringCreateLocalized(errmsg);
		dtb_palette_wrn_msg_initialize(&dtb_palette_wrn_msg);

		(void) show_modal_msg_relative_initiator(
		    root, initiator, &dtb_palette_wrn_msg,
		    xm_buf, NULL, NULL);
		XmStringFree(xm_buf);
		return -1;	
	    }

	    if (!CancelPaste)
	    {
	    parent = objxm_comp_get_subobj(newroot ? newroot : root, AB_CFG_PARENT_OBJ);

	    if (!parent)
		parent = newroot ? newroot : root;

	    /*
	     * Allocate buffer to store ptrs to new objects pasted for undo
	     */
	    new_list = (ABObj *)malloc(ABClipboard->count * sizeof(ABObj));

	    if (ABClipboard->action_count)
	        new_action_list = (ABObj *)malloc(ABClipboard->action_count * sizeof(ABObj));

            abobj_deselect_all(project);

	    /*
	     * Perform the paste operation for all objects on clipboard
	     */
	    for (i=0; i < ABClipboard->count; ++i)
	    {
		ABClipbInfo	info;

		/*
		 * Get a handle to one object on the clipboard
		 * (includes it's descendants)
		 */
		info = &(ABClipboard->info_list[i]);

		/*
		 * Duplicate clipboard object
		 */
                newObj = abobj_dup_tree(info->dup_obj);

		/*
		 * Check if this object has any connections that we
		 * should also duplicate
		 */
		if (info->action_list)
		{
		    AB_TRAVERSAL	trav;
		    ABObj		action_obj;

		    /*
		     * Traverse the action list which stores all the
		     * duplicated connections on the clipboard
		     */
                    for (trav_open(&trav, info->action_list, AB_TRAV_ACTIONS);
                        (action_obj = trav_next(&trav)) != NULL; )
                    {
		        ABObj		new_action,
					orig_from_obj,
					new_from_obj;

			/*
			 * Duplicate connection on clipboard
			 */
                        new_action = abobj_dup(action_obj);

			/*
			 * The connection that is duplicated has, as the source,
			 * an object that was on the clipboard. We need to
			 * change the source to be the duplicated object
			 * i.e. 'newObj' above. The problem, is that the
			 * source can either be 'newObj' or any of it's
			 * descendants.
			 */
		        orig_from_obj = new_action->info.action.from;

		        /*
			 * We search for the new source object by name/type.
			 *
		         * This depends on the fact that the dup'd tree 
			 * maintains objs with same names
		         */
		        new_from_obj = 
		            obj_find_by_name_and_type(newObj,
				obj_get_name(orig_from_obj),
				obj_get_type(orig_from_obj));
	                new_action->info.action.from = new_from_obj;

			/*
			 * Catch the case where the source == target
			 */
			if ((obj_get_func_type(action_obj) == AB_FUNC_BUILTIN) &&
			    (action_obj->info.action.from == 
				action_obj->info.action.to))
	                    new_action->info.action.to = new_from_obj;

			new_action_list[action_index++] = new_action;
                    }
	            trav_close(&trav);
		}

		new_list[i] = newObj;

	    }

	    /*
	     * At this point, all the objects that need to be pasted have been
	     * dupicated, but not parented to the proper parent yet.
	     * Also, all the connection objects have been created, but similarly,
	     * not added to the proper module/project.
	     *
	     * Before we do the above, we need to resolve the targets of the 
	     * duplicated connections. If the target object was also on the
	     * clipboard, then the target has to be changed to it's corresponding
	     * pasted object. This needs to be done, in fact before we parent the
	     * newly created objects because once that is done, the names of the
	     * object may change. obj_append_child() makes sure the child names 
	     * are unique.
	     *
	     * Also, the attachments of the newly dup'd objects still contain
	     * references to objects on the clipboard. This needs to be resolved
	     * as well.
	     */

	    /*
	     * Resolve the pasted connection targets
	     */
	    for (i=0; i < ABClipboard->action_count; ++i)
	    {
		ABObj		action,
				orig_to_obj;

		action = new_action_list[i];

		if (obj_get_func_type(action) != AB_FUNC_BUILTIN)
		    continue;

		orig_to_obj = action->info.action.to;

		/*
		 * Check if the pasted connection target ('to')
		 * is on the clipboard
		 */
	        for (j=0; j < ABClipboard->count; ++j)
		{
		    ABClipbInfo	info;

		    info = &(ABClipboard->info_list[j]);

		    if ((orig_to_obj == info->dup_obj) || 
			(obj_is_descendant_of(orig_to_obj, info->dup_obj)))
		    {
			ABObj	new_to_obj;

			/*
			 * The target is on the clipboard.
			 * It can either be new_list[j] or any of it's 
			 * descendants.
			 */

		        /*
			 * Search for the new target object by name/type.
			 *
		         * This depends on the fact that the dup'd tree maintains objs
		         * with same names
		         */ 
		        new_to_obj = obj_find_by_name_and_type(new_list[j],
		                                obj_get_name(orig_to_obj),
		                                obj_get_type(orig_to_obj));

			action->info.action.to = new_to_obj;
		        break;
		    }
		}
	    }

	    /*
	     * Resolve the pasted objects' (and their descendants') 
	     * attachments
	     */
	    for (i=0; i < ABClipboard->count; ++i)
	    {
		ABClipbInfo	info;
		AB_TRAVERSAL	trav;
		ABObj		origObj = NULL,
				childObj = NULL;

		info = &(ABClipboard->info_list[i]);
		origObj = info->dup_obj;

		resolve_attach_one_obj(new_list[i], origObj, new_list, ABClipboard->count);
		 
		for (trav_open(&trav, new_list[i], AB_TRAV_SALIENT);
		        (childObj = trav_next(&trav)) != NULL; )
		{
		    ABObj       origChildObj = NULL;
							  
		    /*
		     * Skip top level object - taken care of above
		     */
		    if (childObj == new_list[i])
			continue;

		    /*
		     * Get the handle to the child object that
		     * corresponds to the new dup'd one
		     */
		    origChildObj = obj_find_by_name_and_type(origObj,
		                        obj_get_name(childObj),
		                        obj_get_type(childObj));

		    resolve_attach_one_obj(childObj, origChildObj, 
				new_list, ABClipboard->count);
		}
		trav_close(&trav);
	    }

	    /*
	     * Parent the new objects to the right parent
	     */
	    for (i=0; i < ABClipboard->count; ++i)
	    {
		ABObj	proper_parent = parent;
		BOOL	manage_last = TRUE;

		/*
		 * If an object already exists at (x, y),
		 * offset the new object.
		 */
		stack_objects(new_list[i], parent);

		/*
		 * Add obj to parent, ensure unique names.
		 */

		/*
		 * If pasted object is menubar, paste to proper
		 * config child of main window
		 */
		if (obj_is_menubar(new_list[i]))
		{
		    ABObj	winobj = parent;

		    /*
		     * Get window object
		     */
                    while(!(obj_is_window(winobj) && !obj_is_sub(winobj)))
			winobj = obj_get_parent(winobj);

		    if (winobj)
		        proper_parent = 
				objxm_comp_get_subobj(winobj, AB_CFG_MENU_PARENT_OBJ);
		}

		/*
		 * Resolve attachments where the attached object is the parent
		 * object. This is for top level clipboard objects. The new
		 * parent object is only known at paste time. Until now, the value
		 * field of the attachment was NULL.
		 */
		resolve_attach_parent(new_list[i], proper_parent);

		/*
		 * Last for attchments - resolve NONE attachments
		 * If 2 opposite sides have NONE attachments, create
		 * an AB_ATTACH_POINT attachment on one side.
		 */
		resolve_none_attachments(new_list[i]);

                obj_append_child(proper_parent, new_list[i]);

		if (obj_is_layers(new_list[i]) || 
		    obj_is_group(new_list[i]) || 
		    obj_is_menu(new_list[i]))
		    manage_last = FALSE;

                abobj_show_tree(new_list[i], manage_last);

		/* 
		 * Set the dirty bit on the module 
		 */
    		abobj_set_save_needed(obj_get_module(new_list[i]), TRUE);

		abobj_select(new_list[i]);
	    }

	    /*
	     * Add connections to module/project.
	     */
	    for (i=0; i < ABClipboard->action_count; ++i)
	    {
		BOOL		is_cross_module;
		ABObj		action;

		action = new_action_list[i];

		is_cross_module = (obj_get_func_type(action) == AB_FUNC_BUILTIN) &&
					obj_is_cross_module(action);

		if (is_cross_module)
		    obj_add_action(obj_get_project(action->info.action.from), action);
		else
		    obj_add_action(obj_is_project(action->info.action.from) ?
					action->info.action.from :
					obj_get_module(action->info.action.from), action);
	    }

            (void)abobj_set_undo(new_list, ABClipboard->count,
			undo_paste, AB_UNDO_PASTE);

	    if (new_list)
		free(new_list);
	    if (new_action_list)
		free(new_action_list);
	    }

	    if (sel.count > 0)
	        XtFree((char *)sel.list);
        }
    }

    return (iRet);
}

int
abobj_delete(void)
{
    ABObj		project = proj_get_project(),
			parent = NULL;
    ABSelectedRec	sel;
    int		  i;

    if (!project)
	return (0);

    abobj_get_selected(project, FALSE, FALSE, &sel);

    /* REMIND: aim,11/23/93 - this is too simplistic ---
     * doesn't handle UNDO
     */
    /* This depends on the sel.list being ordered in TOP-DOWN
     * order!!!
     */

    abobj_set_save_needed( proj_get_project(), TRUE);

    (void)abobj_set_undo(sel.list, sel.count, 
		undo_cut, AB_UNDO_CUT);

    for (i = (sel.count - 1); i >= 0; i--)
    {
	/* Set the dirty bit on the module */
	abobj_set_save_needed(obj_get_module(sel.list[i]), TRUE);

	parent = obj_get_parent(sel.list[i]);
	if ((parent != NULL) && 
	    (obj_is_paned_win(parent) || obj_is_group(parent))
	    && (obj_get_num_children(parent) == 1) )
	{
	    /* If the last child is being deleted from a 
	     * paned window or a group, then delete the
	     * paned window or group as well.
	     */
	    obj_destroy(parent);
	}
	else
	{

	    obj_destroy(sel.list[i]);
	}
    }

    if (sel.count > 0)
	XtFree((char *)sel.list);

    return (0);
}

int
abobj_undo(void)
{
    int		iRet = 0;

    if (!ABUndo)
	return (iRet);

    in_undo = TRUE;

    if (ABUndo->undo_func)
        ABUndo->undo_func(ABUndo);

    abobj_cancel_undo();

    in_undo = FALSE;

#ifdef AB_UNDO_UNDO
    if (ABUndo_backup && (ABUndo_backup->count > 0))
    {
	ABUndoRec	tmp;

	tmp = ABUndo;

	ABUndo = ABUndo_backup;

	ABUndo_backup = tmp;
    }
#endif

    return (iRet);
}

int
abobj_set_undo(
    ABObj		*obj,
    int			count,
    ABUndoFunc		undo_func,
    AB_UNDO_TYPE	undo_type
)
{
    int		i;
    Position	x = 0, 
		y = 0;
    Dimension	width = 0, 
		height = 0;

    if (!obj || (count <= 0))
	return (0);

/*
 * Ifdef out undo undo until fix memory trash
 */
#ifdef AB_UNDO_UNDO
    if (in_undo)
        set_undo_rec(&ABUndo_backup, obj, count, undo_func, undo_type);
    else
        set_undo_rec(&ABUndo, obj, count, undo_func, undo_type);
#else
    if (!in_undo)
        set_undo_rec(&ABUndo, obj, count, undo_func, undo_type);
#endif

    return(0);
}

int
abobj_cancel_undo(void)
{
    clear_undo_rec(&ABUndo);
    return 0;
}

BOOL
abobj_undo_active(void)
{
    BOOL		i;

    return (ABUndo && (ABUndo->undo_func != NULL));
}


/*
 * PUBLIC:
 * Functions to manipulate the ABObj clipboard
 */

/*
 * abobj_clipboard_is_empty()
 * Is clipboard empty ?
 */
BOOL
abobj_clipboard_is_empty(void)
{
    return(!ABClipboard || (ABClipboard->count == 0));
}

/*
 * abobj_in_clipboard()
 * Is object on clipboard
 */
BOOL
abobj_in_clipboard(
    ABObj	obj
)
{
    int		i;

    if (!obj || !ABClipboard || (ABClipboard->count <= 0))
	return (FALSE);
    
    for (i = 0; i < ABClipboard->count; ++i)
    {
	if (ABClipboard->list[i] == obj) 
	    return (TRUE);
    }

    return (FALSE);
}

/*
 * abobj_clipboard_clear()
 * Clears the clipboard
 */
void
abobj_clipboard_clear(void)
{
    int		i;

    init_clipboard();

    if ((ABClipboard->size <= 0) || (ABClipboard->count <= 0))
	return;
    
    for (i = 0; i < ABClipboard->count; ++i)
    {
	ABClipbInfo	info;

	ABClipboard->list[i] = NULL;

	info = &(ABClipboard->info_list[i]);
	obj_destroy(info->dup_obj);
	info->dup_obj = NULL;
	
	if (info->action_list)
	{
	    obj_destroy(info->action_list);
	    info->action_list = NULL;
	}
	    
	/*
	 * Clear any data for "other stuff here"
	 */
    }

    /*
     * Reset obj/action count to zero
     */
    ABClipboard->count = 0;
    ABClipboard->action_count = 0;
}

/*
 * abobj_clipboard_add()
 * Copies passed objects onto clipboard
 */
void
abobj_clipboard_add(
    ABObj	*obj,
    int		count
)
{
    int		action_count = 0,
		i,
		j;

    verify_clipboard_space(count);

    for (i=ABClipboard->count, j=0; i<ABClipboard->count + count; ++i, ++j)
    {
        ABObj		project, action_obj;
	ABClipbInfo	info;
	AB_TRAVERSAL	trav;

	info = &(ABClipboard->info_list[i]);

	/*
	 * Duplicate/copy ABObj tree onto clipboard
	 */
	info->dup_obj = abobj_dup_tree(obj[j]);
	ABClipboard->list[i] = obj[j];

	project = obj_get_project(obj[j]);

	/*
	 * Duplicate all connections of objects
	 * Search for all connections that have, as the source object
	 * the current object being added to the clipboard or any of it's
	 * descendants.
	 */
	for (trav_open(&trav, project, AB_TRAV_ACTIONS);
			(action_obj = trav_next(&trav)) != NULL; )
	{
	    if (!action_obj->info.action.from)
	    {
	        util_dprintf(1, 
		    "abobj_clipboard_add: action_obj->info.action.from = NIL!\n");
		continue;
	    }

	    if ((action_obj->info.action.from == obj[j]) || 
		(obj_is_descendant_of(action_obj->info.action.from, obj[j])))
	    {
                ABObj	new_action, new_from_obj, new_to_obj;

		/*
		 * Create the ACTION_LIST that will hold all the connections
		 * that we will store on the clipboard for one particular object
		 * (and it's descendants).
		 */
		if (!info->action_list)
		    info->action_list = obj_create(AB_TYPE_ACTION_LIST, NULL);

		/*
		 * Duplicate action
		 */
		new_action = abobj_dup(action_obj);

		/*
		 * OK, at this point we have duplicatd the object tree and
		 * one connection that has it as the source.
		 * The problem is the source, as seen by the connection is
		 * the object that is still part of the project. We need
		 * to make the source be the dup'd object that is in the 
		 * clipboard.
		 */

		/*
		 * Search for the dup'd object. It may not be the copied object,
		 * but a descendant of it. We search by name/type.
		 *
		 * This depends on the fact that the dup'd tree maintains objs
		 * with same names
		 */
		new_from_obj = 
		    obj_find_by_name_and_type(info->dup_obj,
				obj_get_name(action_obj->info.action.from),
				obj_get_type(action_obj->info.action.from));
	        new_action->info.action.from = new_from_obj;

		/*
		 * Catch the case where to/from are the same
		 * The target ('to') is important only if this is
		 * a 'predefined' action type
		 */
		if ((obj_get_func_type(action_obj) == AB_FUNC_BUILTIN) &&
		    (action_obj->info.action.from == action_obj->info.action.to))
	            new_action->info.action.to = new_from_obj;

		/*
		 * Append action to action lists on clipboard
		 */
		obj_add_action(info->action_list, new_action);

		action_count++;
	    }
	}
	trav_close(&trav);
    }

    ABClipboard->count += count;
    ABClipboard->action_count += action_count;

    resolve_clipboard_connections(obj, count, ABClipboard);
    resolve_clipboard_attachments(obj, count, ABClipboard);

}

/*
 * abobj_clipboard_set()
 * Sets clipboard to passed objects
 */
void
abobj_clipboard_set(
    ABObj	*obj,
    int		count
)
{
    int		i,
		j;

    /*
     * Clear clipboard first
     */
    abobj_clipboard_clear();

    abobj_clipboard_add(obj, count);
}

static ABObj
get_ctrl_pane_or_group(
    ABObj	paste_target
)
{
    ABObj	ret_obj = NULL,
		parent = paste_target;

    if (!paste_target)
	return (ret_obj);

    while (parent)
    {
	if (obj_is_control_panel(obj_get_root(parent)) || 
	    obj_is_group(obj_get_root(parent)))
	{
	    return (parent);
	}

	if (obj_is_window(parent))
	    return (NULL);

        parent = obj_get_parent(parent);
    }

    return (ret_obj);
}

/*
 * abobj_setup_undo_cut_layer()
 * This function is used for setting/updating information
 * used when undoing cuts/deletes of panes in layers.
 * When a cut/delete causes only one pane to be left
 * as a child of a lyer, the layer is deleted. This is done 
 * in obj_destroyedOCB() in abobj_layers.c. So, to undo
 * the cut/delete, a layer would have to be recreated.
 *
 * This function NULLifies the parent (layer) field in the undo
 * record since it would have been deleted. Instead, the last pane
 * is stored in the undo record. The new layer will then be 
 * created with this last pane as it's first child.
 */
void
abobj_setup_undo_cut_layer(
    ABObj	layer,
    ABObj	last_pane
)
{
    int		i;

    for (i = 0; i < ABUndo->count; ++i)
    {
	ABUndoInfo	undo_info;
	ABObj		parent;

        /*
         * If undo record is not of type CUT, skip
         */
        if (ABUndo->info_list[i].type != AB_UNDO_CUT)
            continue;
	
	undo_info = &(ABUndo->info_list[i]);
	parent = undo_info->info.cut.parent;

	/*
	 * If parent object stored in undo record matches
	 * the layer object passed in, NULLify it, and store
	 * the last_pane object ptr passed in.
	 */
	if (parent == layer)
	{
	    undo_info->info.cut.parent = NULL;
	    undo_info->info.cut.pane_sibling = last_pane;
	}
    }
}

/*
 * Undo functions for CUT and PASTE
 */
static void
undo_cut(
    ABUndoRec	undo_rec
)
{

    ABUndoInfo	undo_info;
    ABObj	*new_list = NULL,
		*new_from_action_list = NULL,
		*new_to_action_list = NULL;
    int		i,
		j,
		from_action_count = 0,
		to_action_count = 0;
    ABObj	dup_obj,
		parent,
		newObj;

    if (!undo_rec)
	return;

    abobj_deselect_all(proj_get_project());

    /*
     * List to remember what new objects were created
     */
    new_list = (ABObj *)malloc(undo_rec->count * sizeof(ABObj));

    if (undo_rec->action_count > 0)
    {
	/*
	 * Malloc space for from/to lists
	 * We malloc more space than we need here instead of
	 * traversing the lists yet again to figure out how much we need
	 * exactly for each list.
	 */
        new_from_action_list = (ABObj *)malloc(undo_rec->action_count * sizeof(ABObj));
        new_to_action_list = (ABObj *)malloc(undo_rec->action_count * sizeof(ABObj));
    }

    /*
     * Loop thru CUT undo records and re-create the saved object and it's
     * actions
     */
    for (i = 0; i < undo_rec->count; ++i)
    {
	/*
	 * If undo record is not the right type, something is WRONG !!
	 */
	if (undo_rec->info_list[i].type != AB_UNDO_CUT)
	    continue;

	undo_info = &(undo_rec->info_list[i]);
        dup_obj = undo_info->info.cut.dup_obj;
        parent = undo_info->info.cut.parent;

        /* 
         * Duplicate object in undo record
         */
        newObj = abobj_dup_tree(dup_obj);

	/*
	 * Add connections from 'from' list
	 * These are actions that have the re-created object (or any of it's
	 * descendants) as the source ('from').
	 */
	if (undo_info->info.cut.from_action_list)
	{
	    AB_TRAVERSAL    trav;
	    ABObj           action_obj;

	    for (trav_open(&trav, undo_info->info.cut.from_action_list, 
			AB_TRAV_ACTIONS);
	        (action_obj = trav_next(&trav)) != NULL; )
	    {
	        ABObj	orig_from_obj,
			new_from_obj,
			new_action;
		
		new_action = abobj_dup_tree(action_obj);

		/*
		 * Since the from object is re-created, we need to 
		 * reassign the 'from' field of the action to the new 'from'
		 * object.
		 */

		orig_from_obj = new_action->info.action.from;

		/*
		 * The new 'from' object is searched for by name/type
		 * The search is rooted at the object that was re-created.
		 */
		new_from_obj = 
			obj_find_by_name_and_type(newObj, 
				obj_get_name(orig_from_obj), 
				obj_get_type(orig_from_obj)); 
		new_action->info.action.from = new_from_obj;

		/*
		 * If source == target, reassign the 'to' field as well
		 */
		if (action_obj->info.action.from == action_obj->info.action.to)
		    new_action->info.action.to = new_from_obj;
		
		/*
		 * Keep all 'from' connections in a list
		 */
		new_from_action_list[from_action_count++] = new_action;
	    }    
	    trav_close(&trav);

	}

	/*
	 * Add connections from 'to' list
	 * These are actions that have the re-created object (or any of it's
	 * descendants) as the target ('to').
	 */
	if (undo_info->info.cut.to_action_list)
	{
	    AB_TRAVERSAL    trav;
	    ABObj           action_obj;

	    for (trav_open(&trav, undo_info->info.cut.to_action_list, AB_TRAV_ACTIONS);
	        (action_obj = trav_next(&trav)) != NULL; )
	    {
	        ABObj	orig_to_obj,
			new_to_obj,
			new_action;
		
		new_action = abobj_dup_tree(action_obj);

		/*
		 * Since the 'to' object is re-created, we need to 
		 * reassign the 'to' field of the action to the new 'to'
		 * object.
		 */

		orig_to_obj = new_action->info.action.to;

		/*
		 * The new 'to' object is searched for by name/type
		 * The search is rooted at the object that was re-created.
		 */
		new_to_obj = 
			obj_find_by_name_and_type(newObj, 
				obj_get_name(orig_to_obj), 
				obj_get_type(orig_to_obj)); 
		new_action->info.action.to = new_to_obj;

		/*
		 * Keep all 'to' connections in a list
		 */
		new_to_action_list[to_action_count++] = new_action;
	    }    
	    trav_close(&trav);
	}

	/*
	 * Keep newly created object in a list
	 */
	new_list[i] = newObj;
    }

    /*
     * At this point:
     * All objects have been created but not parented to the proper object
     * All connection objects have been created, but their to/from fields
     * need to be resolved.
     */


    /*
     * Resolve 'from' action list
     * Actions on this list may have as a target ('to') an object that
     * was dup'd onto the undo record (and re-created again in the first loop
     * above) . If this is the case, the action's target must be set to the
     * recreated object.
     */
    for (i=0; i < from_action_count; ++i)
    {
        ABObj           action,
			orig_to_obj;

        action = new_from_action_list[i];

	if (obj_get_func_type(action) != AB_FUNC_BUILTIN)
	    continue;

	orig_to_obj = action->info.action.to;

        for (j = 0; j < undo_rec->count; ++j)
	{
	    ABObj	dup_obj;

	    undo_info = &(undo_rec->info_list[j]);

            dup_obj = undo_info->info.cut.dup_obj;

            if ((orig_to_obj == dup_obj) ||
                (obj_is_descendant_of(orig_to_obj, dup_obj)))
            {
                ABObj   new_to_obj;

                /*
		 * Search by name/type
                 * This depends on the fact that the dup'd tree maintains objs
                 * with same names
                 */
                new_to_obj = obj_find_by_name_and_type(new_list[j],
                                    obj_get_name(orig_to_obj),
                                    obj_get_type(orig_to_obj));
                action->info.action.to = new_to_obj;
                break;
            }
	}
    }

    /*
     * Resolve 'to' action list
     * Actions on this list may have as a source ('from') an object that
     * was dup'd onto the undo record (and re-created again in the first loop
     * above) . If this is the case, the action's source must be set to the
     * recreated object.
     */
    for (i=0; i < to_action_count; ++i)
    {
        ABObj           action,
			orig_from_obj;

        action = new_to_action_list[i];
	orig_from_obj = action->info.action.from;

        for (j = 0; j < undo_rec->count; ++j)
	{
	    ABObj	dup_obj;

	    undo_info = &(undo_rec->info_list[j]);

            dup_obj = undo_info->info.cut.dup_obj;

            if ((orig_from_obj == dup_obj) ||
                (obj_is_descendant_of(orig_from_obj, dup_obj)))
            {
                ABObj   new_from_obj;

                /*
		 * Search by name/type
                 * This depends on the fact that the dup'd tree maintains objs
                 * with same names
                 */
                new_from_obj = obj_find_by_name_and_type(new_list[j],
                                    obj_get_name(orig_from_obj),
                                    obj_get_type(orig_from_obj));
                action->info.action.from = new_from_obj;
                break;
            }
	}
    }

    /*
     * Resolve newly created objects' (and their descendants)
     * attachments.
     */
    for (i = 0; i < undo_rec->count; ++i)
    {
        AB_TRAVERSAL	trav;
        ABObj		origObj = NULL,
                        childObj = NULL;

	undo_info = &(undo_rec->info_list[i]);

	if (!undo_info)
	    continue;

        origObj = undo_info->info.cut.dup_obj;

        resolve_attach_one_obj(new_list[i], origObj, new_list, undo_rec->count);
		 
        for (trav_open(&trav, new_list[i], AB_TRAV_SALIENT);
                (childObj = trav_next(&trav)) != NULL; )
        {
            ABObj       origChildObj = NULL;
							  
            /*
             * Skip top level object - taken care of above
             */
            if (childObj == new_list[i])
                continue;

            /*
             * Get the handle to the child object that
             * corresponds to the new dup'd one
             */
            origChildObj = obj_find_by_name_and_type(origObj,
                                obj_get_name(childObj),
                                obj_get_type(childObj));

            resolve_attach_one_obj(childObj, origChildObj, 
                            new_list, undo_rec->count);
        }
        trav_close(&trav);
    }

    /*
     * Add newly created objects to parent hierarchy
     * Create the object's widgets and manage/map them
     */
    for (i = 0; i < undo_rec->count; ++i)
    {
	BOOL	manage_last = TRUE;
	ABObj	last_pane,
		proper_parent;

	/*
	 * parent	-> parent of cut object
	 * last_pane	-> last pane of layer. This case is used
	 *		   if/when parent was a layer and a cut/delete
	 *		   caused it to have only one pane left. The
	 *		   logic in abobj_layers.c would delete the
	 *		   layer, invalidating the parent field in
	 *		   the undo record. To make undo still work,
	 *		   the last pane in the layer is stored instead
	 *		   (and the parent field NULLified).
	 */
        parent = undo_rec->info_list[i].info.cut.parent;
	last_pane = undo_rec->info_list[i].info.cut.pane_sibling;

	if (parent)
	{
	    proper_parent = parent;

	    /*
	     * If pasted object is menubar, paste to proper
	     * config child of main window
	     */
	    if (obj_is_menubar(new_list[i]))
	    {
	        ABObj	winobj = parent;

	        /*
	         * Get window object
	         */
	        while(!(obj_is_window(winobj) && !obj_is_sub(winobj)))
	            winobj = obj_get_parent(winobj);

	        if (winobj)
	            proper_parent = 
	                objxm_comp_get_subobj(winobj, AB_CFG_MENU_PARENT_OBJ);
	    }
	}
	else
	{
	    /*
	     * parent == NULL
	     * This is the layer case described above.
	     * Use 'last_pane' to create a layer and append the new 
	     * dup'd object to it.
	     */
	    ABObj	layer;

	    layer = abobj_handle_layered_pane(new_list[i], last_pane);
	    proper_parent = layer;
	}

	/*
	 * Resolve attachments where the attached object is the parent
	 * object. This is for top level clipboard objects. The new
	 * parent object is only known at paste time. Until now, the value
	 * field of the attachment was NULL.
	 */
	resolve_attach_parent(new_list[i], proper_parent);

	/*
	 * Last for attchments - resolve NONE attachments
	 * If 2 opposite sides have NONE attachments, create
	 * an AB_ATTACH_POINT attachment on one side.
	 */
	resolve_none_attachments(new_list[i]);

        obj_append_child(proper_parent, new_list[i]);

        if (obj_is_layers(new_list[i]) || 
            obj_is_group(new_list[i]) || 
            obj_is_menu(new_list[i]))
            manage_last = FALSE;

        abobj_show_tree(new_list[i], manage_last);

	abobj_select(new_list[i]);

	abobj_set_save_needed(obj_get_module(new_list[i]), TRUE);
    }


    /*
     * Go thru to/from action lists and add the actions to
     * the proper module or project
     */
    for (i=0; i < from_action_count; ++i)
    {
        BOOL            is_cross_module;
        ABObj           action;

        action = new_from_action_list[i];

	is_cross_module = (obj_get_func_type(action) == AB_FUNC_BUILTIN) && 
				obj_is_cross_module(action);

        if (is_cross_module)
	{
            obj_add_action(obj_get_project(action->info.action.from), action);
	    abobj_set_save_needed(obj_get_project(action->info.action.from), TRUE);
	}
        else
	{
            obj_add_action(obj_is_project(action->info.action.from) ?
                                action->info.action.from :
                            obj_get_module(action->info.action.from), action);
	}
    }

    for (i=0; i < to_action_count; ++i)
    {
        BOOL            is_cross_module;
        ABObj           action;

        action = new_to_action_list[i];
	is_cross_module = (obj_get_func_type(action) == AB_FUNC_BUILTIN) && 
				obj_is_cross_module(action);

        if (is_cross_module)
	{
            obj_add_action(obj_get_project(action->info.action.from), action);
	    abobj_set_save_needed(obj_get_project(action->info.action.from), TRUE);
	}
        else
	{
            obj_add_action(obj_is_project(action->info.action.from) ?
                                action->info.action.from :
                            obj_get_module(action->info.action.from), action);
	}
    }

    /*
     * Set undo to undo this 'paste'
     */
    (void)abobj_set_undo(new_list, undo_rec->count, 
				undo_paste, AB_UNDO_PASTE);
    
    if (new_list)
	free(new_list);

    if (new_from_action_list)
	free(new_from_action_list);

    if (new_to_action_list)
	free(new_to_action_list);
}

static void
undo_paste(
    ABUndoRec	undo_rec
)
{
    int		x,
		y,
		width, 
    		height,
		i = 0;
    ABObj       obj,
		dup_obj,
		parent_obj,
		newObj;

    if (!undo_rec)
	return;

    /*
     * Set undo to undo this 'cut'
     */
    (void)abobj_set_undo(undo_rec->list, undo_rec->count, 
			undo_cut, AB_UNDO_CUT);

    for (i = 0; i < undo_rec->count; ++i)
    {
	/*
	 * If undo record is not the right type, something is WRONG !!
	 */
	if (undo_rec->info_list[i].type != AB_UNDO_PASTE)
	    continue;

        obj = undo_rec->list[i];

	abobj_set_save_needed(obj_get_module(obj), TRUE);

        /* 
         * Remove the object from to the hierarchy
         */
        obj_destroy(obj);
    }
}

/*
 * Xt Callbacks for editing functions:
 *	undo
 *	cut
 *	copy
 *	paste
 *	delete
 */
void
abobj_undo_cb(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    (void)abobj_undo();
}

void
abobj_cut_cb(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    (void)abobj_cut();
}

void
abobj_copy_cb(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    (void)abobj_copy();
}

void
abobj_paste_cb(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    (void)abobj_paste((AB_PASTE_INITIATOR_TYPE) client_data);
}

void
abobj_delete_cb(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    (void)abobj_delete();
}

/*
** If the window the parent object is on is iconified, show the modal dialog
** relative to where the paste was initiated, otherwise parent the dialog off
** of the parent_obj.
*/
static DTB_MODAL_ANSWER
show_modal_msg_relative_initiator(
    ABObj	         parent_obj,
    AB_PASTE_INITIATOR_TYPE initiator,
    DtbMessageData	 mbr,
    XmString		 override_msg,
    DtbObjectHelpData    override_help,
    Widget         	 *modal_dlg_pane_out_ptr
)
{
    ABObj  winobj = NULL;
    Widget parent = NULL;
    
    if (!parent_obj || !mbr)
	return DTB_ANSWER_CANCEL;
    
    winobj = obj_get_window(parent_obj);
			
    if ((winobj != NULL) && obj_has_flag(winobj, IconifiedFlag))
    {
	/*
	** The window is iconified, determine who to parent the dialog off
	** of.
	*/
	switch(initiator)
	{
	case AB_PASTE_INITIATOR_OBJ_MENU:
	    /*
	    ** paste initiated from obj menu! The window shouldn't be iconified.
	    ** popup over the palette to be safe.
	    */
	    parent = dtb_get_toplevel_widget();
	    break;
	case AB_PASTE_INITIATOR_BRWS_MENU:
	case AB_PASTE_INITIATOR_BRWS_EDIT_MENU:
	    parent = brws_get_browser_shell_for_obj(parent_obj);
	    if (!parent)
		parent = dtb_get_toplevel_widget();
	    break;
	case AB_PASTE_INITIATOR_PAL_EDIT_MENU:
	    parent = dtb_get_toplevel_widget();
	    break;
	default:
	    return DTB_ANSWER_CANCEL;
	}
    }
    else
    {
	/*
	** The window parent_obj is on is not iconified, parent the dialog
	** off it.
	*/
	parent = objxm_get_widget(parent_obj);
    }
    return(dtb_show_modal_message(
	       parent, mbr, override_msg,
	       override_help, modal_dlg_pane_out_ptr)
          );
}

