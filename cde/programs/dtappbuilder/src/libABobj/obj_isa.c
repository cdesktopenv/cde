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
 * $XConsortium: obj_isa.c /main/4 1996/08/28 14:01:31 mustafa $
 * 
 * @(#)obj_isa.c	1.46 15 Feb 1994	cde_app_builder/src/libABobj
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */


/*
 * isa.c - isa relationships for AB_OBJ objects
 */

#include "objP.h"

/*
 * Returns whether or not an object can be manipulated directly by the user.
 * For example, with a system-defined composite object, the root node is
 * salient and visible, but the subobjects are not.
 */
BOOL
obj_is_salient(ABObj obj)
{

    /*
     * Check type first
     */
    BOOL                itIs =
    (obj_is_ui(obj)
     || (obj->type == AB_TYPE_MODULE)
     || (obj->type == AB_TYPE_PROJECT)
     || (obj->type == AB_TYPE_ACTION)
    );

    /*
     * regardless of the type, composite subobjs and menu references are not
     * salient
     */
    if (itIs &&
	((obj_is_ref(obj)) ||
	 (!obj_is_root(obj)))
	)
    {
	itIs = FALSE;
    }

    return itIs;
}


/*
 * Return whether object is virtual - that is, it doesn't exist directly in
 * the compiled code (e.g. layered panes, an interface node)
 * 
 * Note: "virtual" objects often have non-virtual children that must be written!
 */
BOOL
obj_is_virtual(ABObj obj)
{
    if (obj == NULL)
        return FALSE;

    switch (obj->type)
    {
	case AB_TYPE_MODULE:
	case AB_TYPE_PROJECT:
	case AB_TYPE_MENU:
	case AB_TYPE_UNKNOWN:
	case AB_TYPE_MESSAGE:
	    return TRUE;
	case AB_TYPE_ITEM:
	    if (obj->info.item.type == AB_ITEM_FOR_MENU)
		return TRUE;
    }
    return FALSE;
}


/*
 * Return whether the object is the composite "root"obj  of a subhierarchy of
 * ABObjs which make up one UI object
 */
BOOL
obj_is_root(ABObj obj)
{
    ABObj               croot = obj_get_root(obj);;
    return (croot == obj);
}


/*
 * Return whether the object is a composite subobj of a subhierarchy of
 * ABObjs which make up one UI object
 */
BOOL
obj_is_sub(ABObj obj)
{
    ABObj               vroot;

    vroot = obj->part_of;

    if (vroot == NULL || vroot == obj)
	return FALSE;

    return TRUE;

}

/*
 * TRUE if the object is an item of some sort (list, choice, menu)
 */
BOOL
obj_is_item(ABObj obj)
{

    return (obj->type == AB_TYPE_ITEM);

}

/*
 * Returns whether an object is a numeric field.
 */
BOOL
obj_is_numeric_field(ABObj obj)
{
    return ((obj->type == AB_TYPE_TEXT_FIELD)
	    && (obj->info.text.type == AB_TEXT_NUMERIC));
}

/*
 * Return whether an object is a top-level object.
 */
BOOL
obj_is_top_level(ABObj obj)
{
    return obj_is_window(obj);
}

/*
 * Return whether a type is a button
 */
BOOL
obj_is_button(ABObj obj)
{
    if (obj->type == AB_TYPE_BUTTON)
	return TRUE;
    return FALSE;
}


/*
 * An option menu is a type of choice. It is *not* considered a menu (it just
 * sorta looks like one)
 */
BOOL
obj_is_option_menu(ABObj obj)
{
    return (obj_is_choice(obj)
	    && (obj_get_choice_type(obj) == AB_CHOICE_OPTION_MENU));
}


/*
 * Return whether a type is a pushbutton
 */
BOOL
obj_is_push_button(ABObj obj)
{
    if (obj->type == AB_TYPE_BUTTON &&
	obj->info.button.type == AB_BUT_PUSH)
	return TRUE;
    return FALSE;
}

/*
 * Return whether a type is a menubutton
 */
BOOL
obj_is_menu_button(ABObj obj)
{
    if (obj->type == AB_TYPE_BUTTON &&
	obj->info.button.type == AB_BUT_MENU)
	return TRUE;
    return FALSE;
}

/*
 * Return whether a type is a label
 */
BOOL
obj_is_label(ABObj obj)
{
    return (obj->type == AB_TYPE_LABEL);
}

/*
 * Does the object affect more than one module?
 */
BOOL
obj_is_cross_module(ABObj obj)
{
    return !obj_is_intra_module(obj);
}


/*
 * Does the object affect only one module?
 */
BOOL
obj_is_intra_module(ABObj obj)
{
/*
 *
 */
    BOOL	isIt = TRUE;

    switch (obj->type)
    {
    /*
     * dialog windows can have window parents in other modules.
     */
    case AB_TYPE_DIALOG:
    {
        ABObj winParent = obj_get_win_parent(obj);
        if (   (winParent != NULL) 
	    && (obj_get_module(winParent) != obj_get_module(obj)) )
        {
	    isIt = FALSE;
        }
    }
    break;

    /*
     * Actions can be cross-module connections
     */
    case AB_TYPE_ACTION:
    {
	ABObj from = obj_get_from(obj);
	ABObj to = obj_get_to(obj);
	if ((from != NULL) && (to != NULL))
	{
	    if (obj_get_module(from) != obj_get_module(to))
	    {
		isIt = FALSE;
	    }
	}
    }
    }

    return isIt;
}


/*
 * Return whether a type is a drawing_area
 */
BOOL
obj_is_drawing_area(ABObj obj)
{
    return (obj->type == AB_TYPE_DRAWING_AREA);
}

/*
 * Return whether a type is a term pane
 */
BOOL
obj_is_term_pane(
		 ABObj obj
)
{
    return (obj->type == AB_TYPE_TERM_PANE);
}

/*
 * Return whether a type is a text pane
 */
BOOL
obj_is_text_pane(ABObj obj)
{
    return (obj->type == AB_TYPE_TEXT_PANE);
}


/*
 * Return whether a type is a canvas
 */
BOOL
obj_is_container(ABObj obj)
{
    return (obj->type == AB_TYPE_CONTAINER);
}

/*
 * Return whether a type is a control-area
 */
BOOL
obj_is_control_panel(ABObj obj)
{

    return (obj->type == AB_TYPE_CONTAINER &&
	    (obj->info.container.type == AB_CONT_ABSOLUTE ||
	     obj->info.container.type == AB_CONT_RELATIVE ||
	     obj->info.container.type == AB_CONT_BUTTON_PANEL ||
	     obj->info.container.type == AB_CONT_TOOL_BAR ||
	     obj->info.container.type == AB_CONT_FOOTER ||
	     obj->info.container.type == AB_CONT_ROW_COLUMN));

}

/*
 * Return whether a type is a scrolling list
 */
BOOL
obj_is_list(ABObj obj)
{
    return (obj->type == AB_TYPE_LIST);
}

/*
 * Return whether a type is a paned window
 */
BOOL
obj_is_paned_win(ABObj obj)
{
    return ((obj->type == AB_TYPE_CONTAINER)
	    && (obj->info.container.type == AB_CONT_PANED));

}

/*
 * Return whether a type is a menu
 */
BOOL
obj_is_menu(ABObj obj)
{
    return (obj->type == AB_TYPE_MENU);
}

/*
 * All functions dealing with references must allow NULL params
 */
BOOL
obj_is_ref(ABObj obj)
{
    return ((obj == NULL)? FALSE:(obj->ref_to != NULL));
}

BOOL
obj_is_menubar(ABObj obj)
{
    return ((obj->type == AB_TYPE_CONTAINER)
	    && (obj->info.container.type == AB_CONT_MENU_BAR));
}


/*
 * Return whether a type is a separator
 */
BOOL
obj_is_separator(ABObj obj)
{
    return (obj->type == AB_TYPE_SEPARATOR);
}

BOOL
obj_is_scale(ABObj obj)
{
    return (obj->type == AB_TYPE_SCALE);
}

/*
 * Return whether a type is a combo box
 */
BOOL
obj_is_combo_box(ABObj obj)
{
    return (obj->type == AB_TYPE_COMBO_BOX);
}
/*
 * Return whether a type is a spin box
 */
BOOL
obj_is_spin_box(ABObj obj)
{
    return (obj->type == AB_TYPE_SPIN_BOX);
}

BOOL
obj_is_text(ABObj obj)
{
    return (obj->type == AB_TYPE_TEXT_FIELD ||
	    obj->type == AB_TYPE_TEXT_PANE);
}

BOOL
obj_is_text_field(ABObj obj)
{
    return (obj->type == AB_TYPE_TEXT_FIELD);
}


/*
 * Return whether an object is a pane.
 */
BOOL
obj_is_pane(ABObj obj)
{
    switch (obj->type)
    {
	/* case AB_CANVAS_PANE:  REMIND: do we need this? */
    case AB_TYPE_CONTAINER:
	{
	    AB_CONTAINER_TYPE   ctype = obj_get_container_type(obj);

	    switch (ctype)
	    {
	    case AB_CONT_ABSOLUTE:
	    case AB_CONT_RELATIVE:
	    case AB_CONT_ROW_COLUMN:
	    case AB_CONT_TOOL_BAR:
	    case AB_CONT_FOOTER:
	    case AB_CONT_BUTTON_PANEL:
		return TRUE;
	    default:
		return FALSE;
	    }
	}
    case AB_TYPE_TEXT_PANE:
    case AB_TYPE_TERM_PANE:
    case AB_TYPE_DRAWING_AREA:
	return TRUE;
    }
    return FALSE;
}

BOOL
obj_is_popup(ABObj obj)
{
    BOOL                is_popup = FALSE;

    switch (obj->type)
    {
    case AB_TYPE_DIALOG:
	is_popup = TRUE;
	break;
    case AB_TYPE_MENU:
	is_popup = obj_is_pane(obj->parent) || 
		   obj_is_list(obj->parent);
	break;
    }
    return is_popup;
}

/*
 * Return whether a type is a menu
 */
BOOL
obj_is_choice(ABObj obj)
{
    return (obj->type == AB_TYPE_CHOICE);
}

/*
 * Return whether a type is a control.
 */
BOOL
obj_is_control(ABObj obj)
{
    switch (obj->type)
    {
    case AB_TYPE_BUTTON:
    case AB_TYPE_LABEL:
    case AB_TYPE_LIST:
    case AB_TYPE_CHOICE:
    case AB_TYPE_COMBO_BOX:
    case AB_TYPE_SCALE:
    case AB_TYPE_SEPARATOR:
    case AB_TYPE_SPIN_BOX:
    case AB_TYPE_TEXT_FIELD:
	return TRUE;
    }
    return FALSE;
}


BOOL
obj_is_layers(ABObj obj)
{
    return (obj->type == AB_TYPE_LAYERS);
}

BOOL
obj_is_base_win(ABObj obj)
{
    return (obj->type == AB_TYPE_BASE_WINDOW);
}

BOOL
obj_is_popup_win(ABObj obj)
{
    return (obj->type == AB_TYPE_DIALOG);
}

BOOL
obj_is_file_chooser(ABObj obj)
{
    return (obj->type == AB_TYPE_FILE_CHOOSER);
}

/*
 * Return whether an object is a message box
 */
BOOL
obj_is_message(ABObj obj)
{
    return (obj->type == AB_TYPE_MESSAGE);
}


/*
 * Return whether an object is a window.
 */
BOOL
obj_is_window(ABObj obj) 
{
    switch (obj->type)
    {
	case AB_TYPE_BASE_WINDOW:
	case AB_TYPE_DIALOG:
	case AB_TYPE_FILE_CHOOSER:
	    return TRUE;
    }
    return FALSE;
}


/*
 * Return whether an object is a group
 */
BOOL
obj_is_group(ABObj obj)
{
    return ((obj->type == AB_TYPE_CONTAINER)
	    && (obj->info.container.type == AB_CONT_GROUP));
}

/*
 * Return whether an object is a group member
 */
BOOL
obj_is_group_member(ABObj obj)
{
    /* Need to get the root of the parent, because the
     * group could have a border frame, in which case
     * obj_get_parent() returns the border frame container,
     * NOT the group.  
     */
    return (obj_is_group(obj_get_root(obj_get_parent(obj))));
}

#ifndef obj_is_action
BOOL
obj_is_action(ABObj obj)
{
    return (obj->type == AB_TYPE_ACTION);
}

#endif


/*
 * A UI object is one that actually has a visual representation in the user's
 * application.
 * 
 * HINT: THIS DOES *NOT* INCLUDE MODULES, PROJECT, OR ACTIONS
 */
BOOL
obj_is_ui(ABObj obj)
{
    BOOL	isUI = FALSE;

    switch (obj->type)
    {
	case AB_TYPE_BASE_WINDOW:
	case AB_TYPE_BUTTON:
	case AB_TYPE_CHOICE:
	case AB_TYPE_COMBO_BOX:
	case AB_TYPE_FILE_CHOOSER:
	case AB_TYPE_MESSAGE:
	case AB_TYPE_CONTAINER:
	case AB_TYPE_DIALOG:
	case AB_TYPE_DRAWING_AREA:
	case AB_TYPE_ITEM:
	case AB_TYPE_LABEL:
	case AB_TYPE_LAYERS:
	case AB_TYPE_LIST:
	case AB_TYPE_MENU:
	case AB_TYPE_SEPARATOR:
	case AB_TYPE_SPIN_BOX:
	case AB_TYPE_SCALE:
	case AB_TYPE_TERM_PANE:
	case AB_TYPE_TEXT_FIELD:
	case AB_TYPE_TEXT_PANE:
	    isUI = TRUE;
	    break;
    }

    return isUI;
}


BOOL
obj_is_xmconfiged(ABObj obj)
{
    return
	(obj_is_sub(obj) || (obj_has_flag(obj, XmConfiguredFlag)));
}

BOOL
obj_is_descendant_of(ABObj obj, ABObj ancestor)
{
    if (!obj || !ancestor)
	return (FALSE);

    while (obj = obj_get_parent(obj))
    {
	if (obj == ancestor)
	    return (TRUE);
    }

    return (FALSE);
}

BOOL
obj_is_sibling(ABObj obj, ABObj sibling)
{
    if (!obj || !sibling || (obj == sibling))
	return (FALSE);

    return (obj_get_parent(obj) == obj_get_parent(sibling));
}

BOOL
obj_is_defined_module(
    ABObj      obj
)
{
    BOOL        IsDefinedMod = FALSE;

    if (obj_is_module(obj) && obj_is_defined(obj))
    {
        IsDefinedMod = TRUE;
    }

    return(IsDefinedMod);
}
