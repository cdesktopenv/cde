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
 *	$XConsortium: abobj_edit.h /main/3 1995/11/06 17:15:39 rswiston $
 *
 *	@(#)abobj_edit.h	1.1 15 Feb 1994	
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

#ifndef _ABOBJ_EDIT_H_
#define _ABOBJ_EDIT_H_

#include <ab_private/abobj.h>

/*
 * Declarations for edit features
 */

/*
 * Size of each malloc'd block for clipboard/undo buffer
 */
#define ABOBJ_CLIPBOARD_BLOCK_SIZE	10
#define ABOBJ_UNDO_BLOCK_SIZE		10

/*
 * Edit operations
 */
typedef enum
{
    AB_EDIT_CUT = 0,
    AB_EDIT_COPY,
    AB_EDIT_PASTE,
    AB_EDIT_DELETE,
    AB_EDIT_NUM_VALUES
} AB_EDIT_TYPE;

/*
 * Supported actions that can be undone
 */
typedef enum
{
    AB_UNDO_NO_TYPE = 0,
    AB_UNDO_CUT,
    AB_UNDO_DELETE,
    AB_UNDO_PASTE,
    AB_UNDO_MOVE,
    AB_UNDO_RESIZE,
    AB_UNDO_GROUP,
    AB_UNDO_UNGROUP,
    AB_UNDO_NUM_VALUES
} AB_UNDO_TYPE;

/*
** Supported initiator locations of a paste operation
*/
typedef enum
{
    AB_PASTE_INITIATOR_NO_TYPE = 0,
    AB_PASTE_INITIATOR_OBJ_MENU,	/* Object popup menu */
    AB_PASTE_INITIATOR_BRWS_MENU,	/* Browser popup menu */
    AB_PASTE_INITIATOR_BRWS_EDIT_MENU,	/* Browser's edit menu */
    AB_PASTE_INITIATOR_PAL_EDIT_MENU,	/* Palette's edit menu */
    AB_PASTE_INITIATOR_NUM_VALUES
} AB_PASTE_INITIATOR_TYPE;


/*
 * Data structures for clipboard/undo
 * What follows are data structures that are used to store
 * ABOBj's and related data for undo and the clipboard.
 *
 * The two main data structures are:
 *	AB_CLIPBOARD_REC	and
 *	AB_UNDO_REC
 *
 * AB_UNDO_REC is a bit complicated because it consists
 * of data for cut/delete/paste/move/resize/group/ungroup
 * which are all contained in a union.
 */

/*
 * Additional info needed for clipboard
 */
typedef struct _AB_CLIPB_INFO
{
    ABObj	dup_obj;	/* duplicated object, not just ptr to */
    ABObj	action_list;	/* duplicated ACTIONS for object and it's descendants */
    char	*other_stuff;	/* placeholder for now, will be filled 
				 * in later */
}AB_CLIPB_INFO, *ABClipbInfo;

/*
 * Additional info needed for undoing cut
 */
typedef struct _AB_UNDO_CUT_INFO
{
    ABObj	dup_obj;
    ABObj	from_action_list;	/* duplicated ACTIONS for object (object == source) */
    ABObj	to_action_list;		/* duplicated ACTIONS for object (object == target) */
    ABObj	parent;			/* POINTER to parent */
    ABObj	pane_sibling;		/* POINTER to sibling - used for layers */
} AB_UNDO_CUT_INFO, *ABUndoCutInfo;

/*
 * Additional info needed for undoing move
 */
typedef struct _AB_UNDO_MOVE_INFO
{
    int		x;
    int		y;
} AB_UNDO_MOVE_INFO, *ABUndoMoveInfo;

/*
 * Additional info needed for undoing resize
 */
typedef struct _AB_UNDO_RESIZE_INFO
{
    int		width;
    int		height;
} AB_UNDO_RESIZE_INFO, *ABUndoResizeInfo;

/*
 * Additional info needed for undoing ungroup
 */
typedef struct _AB_UNDO_UNGROUP_INFO
{
    ABObj	dup_old_group;		/* DUPLICATE of ungrouped group */
    ABObj	*member_list;		/* List of POINTERS to members */
    int		member_count;
} AB_UNDO_UNGROUP_INFO, *ABUndoUngroupInfo;

/*
 * Union of additional info for undo actions
 */
typedef union _AB_UNDO_EXTRA_INFO
{
    AB_UNDO_CUT_INFO		cut;
    /* undo for cut == undo for delete */
    /* don't need special info for undoing paste */
    AB_UNDO_MOVE_INFO		move;
    AB_UNDO_RESIZE_INFO		resize;
    AB_UNDO_UNGROUP_INFO	ungroup;
    /* don't need special info for undoing group */
}AB_UNDO_EXTRA_INFO, *ABUndoExtraInfo;

/*
 * Info for undoing an action on ONE object
 */
typedef struct _AB_UNDO_INFO
{
    AB_UNDO_TYPE	type;
    AB_UNDO_EXTRA_INFO	info;
}AB_UNDO_INFO, *ABUndoInfo;

/*
 * Data structure for clipboard
 */
typedef struct _AB_CLIPBOARD_REC{
    ABObj		*list;	/* List of POINTERS to copied/cut objects */
    AB_CLIPB_INFO	*info_list;
    int			count;
    int			action_count;
    int			size;
} AB_CLIPBOARD_REC, *ABClipboardRec;

/*
 * Type for ABUndoFunc
 */
struct _AB_UNDO_REC;		/* forward ref for ABUndoFunc */
typedef void AB_UNDO_FUNC(struct _AB_UNDO_REC *undo_rec);
typedef AB_UNDO_FUNC *ABUndoFunc;

/*
 * Data structure for undo buffer
 */
typedef struct _AB_UNDO_REC{
    ABObj		*list;
    AB_UNDO_INFO	*info_list;
    ABUndoFunc		undo_func;
    int			count;
    int			action_count;
    int			size;
} AB_UNDO_REC, *ABUndoRec;


/*
 * Editing functions
 */
extern void	abobj_edit_init(
		);

extern int	abobj_cut(
		);

extern int	abobj_copy(
		);

extern int	abobj_paste(
                AB_PASTE_INITIATOR_TYPE initiator
		);

extern int	abobj_delete(void);

extern int	abobj_undo(void);

extern int	abobj_cancel_undo(void);

extern BOOL	abobj_undo_active(
		);

/*
 * CLIPBOARD manipulating functions
 */
extern BOOL	abobj_clipboard_is_empty(
		);

extern BOOL	abobj_in_clipboard(
		    ABObj	obj
		);

extern void	abobj_clipboard_clear(
		);

extern void	abobj_clipboard_set(
		    ABObj	*obj,
		    int		count
		);

extern void	abobj_clipboard_add(
		    ABObj	*obj,
		    int		count
		);

/*
 * Function to set undo buffer
 */
extern int	abobj_set_undo(
		    ABObj		*obj,
		    int			count,
		    ABUndoFunc		undo_func,
		    AB_UNDO_TYPE	undo_type
		);

extern void	abobj_setup_undo_cut_layer(
		    ABObj       layer,
		    ABObj       last_pane
		);

/*
 * Xt Callbacks for editing functions:
 *	undo
 *	cut
 *	copy
 *	paste
 *	delete
 */
extern void	abobj_undo_cb(
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

extern void	abobj_cut_cb(
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

extern void	abobj_copy_cb(
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

extern void	abobj_paste_cb(
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

extern void	abobj_delete_cb(
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

#endif /* _ABOBJ_EDIT_H_ */
