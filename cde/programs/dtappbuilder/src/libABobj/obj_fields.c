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
 * $XConsortium: obj_fields.c /main/4 1996/10/29 15:20:22 mustafa $
 * 
 * @(#)obj_fields.c	3.72 15 Feb 1994	cde_app_builder/src/libABobj
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
 * fields.c - set and get fields of the gobj structure
 */

#include <stdio.h>
#include <string.h>
#include "objP.h"
#include "obj_notifyP.h"
#include "obj_names_listP.h"
#include <ab_private/util.h>

/*
 * This function verifies the integrity of the object, and is called at the
 * beginning of each field access, when debugging
 */
static int          verify_for_write(ABObj obj);
static int          verify_for_read(ABObj obj);

/*
 * If a bad field (member) access is attempted, use member_error() to report
 * the error. E.g.: member_error(obj, "bg_color"). It's automatically removed
 * from non-debugging builds.
 */
static int          member_error(ABObj obj, STRING member_name);

/*
 * Define our local debugging routines
 */
#ifdef DEBUG
static int 
member_error_impl(
		  ABObj obj, STRING member_name, STRING file, int line);
#define member_error(obj, member_name) \
	    (member_error_impl(obj, member_name, __FILE__, __LINE__))
#else
#define member_error(obj, member_name)	/* ignore this! */
#endif

#ifdef DEBUG
static int          verify_for_read_impl(ABObj obj, STRING file, int line);
static int          verify_for_write_impl(ABObj obj, STRING file, int line);
#define verify_for_read(obj) (verify_for_read_impl(obj, __FILE__, __LINE__))
#define verify_for_write(obj) (verify_for_write_impl(obj, __FILE__, __LINE__))
#else
#define verify_for_read(obj)		/* ignore this! */
#define verify_for_write(obj)		/* ignore this! */
#endif



int
obj_set_file(ABObj obj, STRING file)
{
    int                 iRet = 0;
    ISTRING            *old_name = NULL;

    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_FILE:
	iRet = obj_set_name(obj, file);		/* use name field */
	break;

    case AB_TYPE_MODULE:
	old_name = &(obj->info.module.file);
	break;

    case AB_TYPE_PROJECT:
	old_name = &(obj->info.project.file);
	break;

    default:
	member_error(obj, "file");
	iRet = -1;
	break;
    }
    if (old_name != NULL)
    {
	istr_destroy(*old_name);
	*old_name = istr_create(file);
    }
    return iRet;
}

/*
 * Note - obj_get_file is more complex and is in utils.c
 */

int
obj_set_has_border(ABObj obj, BOOL has_border)
{
    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_CONTAINER:
	obj->info.container.has_border = has_border;
	return 0;

    case AB_TYPE_TEXT_FIELD:
    case AB_TYPE_TEXT_PANE:
	obj->info.text.has_border = TRUE;
	return 0;

    default:
	member_error(obj, "has_border");
    }
    return -1;
}

int
obj_set_hscrollbar_policy(ABObj obj, AB_SCROLLBAR_POLICY hscrollbar)
{
    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_DRAWING_AREA:
	obj->info.drawing_area.hscrollbar = hscrollbar;
	break;
    case AB_TYPE_TEXT_PANE:
	obj->info.text.hscrollbar = hscrollbar;
	break;
    }

    return 0;
}


BOOL
obj_has_border(ABObj obj)
{
    verify_for_read(obj);
    switch (obj->type)
    {
    case AB_TYPE_CONTAINER:
	return obj->info.container.has_border;

    case AB_TYPE_TEXT_FIELD:
	return obj->info.text.has_border;
    }
    return FALSE;
}

int
obj_get_increment(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_spin_box(obj))
        return(obj->info.spin_box.increment);
    else if (obj_is_scale(obj))
	return(obj->info.scale.increment);
 
    return -1; 
} 
 
int
obj_set_increment(ABObj obj, int incr)
{
    verify_for_write(obj);
    if (obj_is_spin_box(obj))
    {
        obj->info.spin_box.increment = incr;
        return 0;
    }
    else if (obj_is_scale(obj))
    {
	obj->info.scale.increment = incr;
	return 0;
    }
    member_error(obj, "increment");
    return -1;
}

int
obj_set_is_default(ABObj obj, BOOL is_default)
{
    verify_for_write(obj);
    if (obj->type == AB_TYPE_PROJECT)
    {
	obj->info.project.is_default = is_default;
	return 0;
    }
    member_error(obj, "is_default");
    return -1;
}


BOOL
obj_is_default(ABObj obj)
{
    verify_for_read(obj);
    if (obj->type == AB_TYPE_PROJECT)
    {
	return obj->info.project.is_default;
    }
    return FALSE;
}


int
obj_set_is_defined(ABObj obj, BOOL is_defined)
{
    verify_for_write(obj);
    if (is_defined)
    {
	obj_set_impl_flags(obj, ObjFlagIsDefined);
    }
    else
    {
	obj_clear_impl_flags(obj, ObjFlagIsDefined);
    }
    return 0;
}

BOOL
obj_is_defined(ABObj obj)
{
    verify_for_read(obj);
    return obj_has_impl_flags(obj, ObjFlagIsDefined);
}

int
obj_set_is_help_item(
		     ABObj obj,
		     BOOL is_help_item
)
{
    verify_for_write(obj);
    if (obj_is_item(obj))
    {
	/* Only Settable for Cascade Items */
	if (obj->info.item.type == AB_ITEM_FOR_MENUBAR)
	    obj->info.item.is_help_item = is_help_item;
	return 0;
    }
    member_error(obj, "is_help_item");
    return -1;
}

BOOL
obj_is_help_item(
		 ABObj obj
)
{
    verify_for_read(obj);
    if (obj_is_item(obj))
    {
	return obj->info.item.is_help_item;
    }
    return FALSE;
}

int
obj_set_accelerator(ABObj obj, STRING accel)
{
    verify_for_write(obj);
    if (!obj_is_item(obj))
    {
	member_error(obj, "accelerator");
	return -1;
    }
    istr_destroy(obj->info.item.accelerator);
    obj->info.item.accelerator = istr_create(accel);
    return 0;
}

int
obj_set_bg_color(ABObj obj, STRING bg_color)
{
    verify_for_write(obj);
    istr_destroy(obj->bg_color);
    obj->bg_color = istr_create(bg_color);
    return 0;
}

int
obj_set_fg_color(ABObj obj, STRING fg_color)
{
    verify_for_write(obj);
    istr_destroy(obj->fg_color);
    obj->fg_color = istr_create(fg_color);
    return 0;
}

int
obj_set_filter_pattern(ABObj obj, STRING filter_pattern)
{
    verify_for_write(obj);
    if (!obj_is_file_chooser(obj))
    {
	member_error(obj, "filter_pattern");
	return -1;
    }
    istr_destroy(obj->info.file_chooser.filter_pattern);
    obj->info.file_chooser.filter_pattern = istr_create(filter_pattern);
    return 0;
}

STRING
obj_get_filter_pattern( ABObj obj)
{
    STRING              pattern = NULL;

    verify_for_read(obj);
    pattern = istr_string(obj->info.file_chooser.filter_pattern);
    return (pattern);
}

int
obj_set_icon(ABObj obj, STRING icon)
{
    verify_for_write(obj);
    if (obj_is_window(obj) && !obj_is_file_chooser(obj))
    {
	istr_destroy(obj->info.window.icon);
	obj->info.window.icon = istr_create(icon);
	return 0;
    }
    member_error(obj, "icon");
    return -1;
}

STRING
obj_get_icon(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_window(obj) && !obj_is_file_chooser(obj))
	return istr_string(obj->info.window.icon);

    return NULL;
}

int
obj_set_icon_label(ABObj obj, STRING icon_label)
{
    verify_for_write(obj);
    if (obj_is_window(obj) && !obj_is_file_chooser(obj))
    {
	istr_destroy(obj->info.window.icon_label);
	obj->info.window.icon_label = istr_create(icon_label);
	return 0;
    }
    member_error(obj, "icon_label");
    return -1;
}

STRING
obj_get_icon_label(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_window(obj) && !obj_is_file_chooser(obj))
	return istr_string(obj->info.window.icon_label);

    return NULL;
}
int
obj_set_icon_mask(ABObj obj, STRING icon_mask)
{
    verify_for_write(obj);
    if (obj_is_window(obj) && !obj_is_file_chooser(obj))
    {
        istr_destroy(obj->info.window.icon_mask);
        obj->info.window.icon_mask = istr_create(icon_mask);
        return 0;
    }   
    member_error(obj, "icon_mask");
    return -1;
}
 
STRING
obj_get_icon_mask(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_window(obj) && !obj_is_file_chooser(obj))
        return istr_string(obj->info.window.icon_mask);
 
    return NULL;
}

int
obj_set_initial_value_string(ABObj obj, STRING val)
{
    verify_for_write(obj);
    if (obj_is_text(obj))
    {
	istr_destroy(obj->info.text.initial_value_string);
	obj->info.text.initial_value_string = istr_create(val);
	return 0;
    }
    member_error(obj, "initial_value_string");
    return -1;
}

STRING
obj_get_initial_value_string(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_text(obj))
    {
	return (istr_string(obj->info.text.initial_value_string));
    }
    return NULL;
}

/*
 * REMIND: FIND AND CHANGE ALL BY-NAME REFERENCES TO THIS OBJECT!!!
 *
 * NOTE: don't verify at the top of the function - the indexes may be 
 *       out-of-sync until we update them (i.e., the object is temporarily
 * 	 invalid).
 */
int
obj_set_name_istr(ABObj obj, ISTRING new_name)
{
    ISTRING		old_name = NULL;
    StringList		names = NULL;
    ABObj		oldNameObj = NULL;

    if (istr_equal(new_name, obj->name))
    {
	/* no change! */
	return 0;
    }
    if ((names = objP_get_names_scope(obj)) != NULL)
    {
	oldNameObj = (ABObj)strlist_get_istr_data(names, new_name);
	if (oldNameObj != NULL) 
	{
	    if (oldNameObj == obj)
	    {
		strlist_remove_istr(names, new_name);
	    }
	    else
	    {
	        /* it's not unique! */
                #ifdef DEBUG
	            util_dprintf(1, "ERROR: duplicate name rejected: %s\n",
		        istr_string_safe(new_name));
                #endif /* DEBUG */
	        return ERR_DUPLICATE_KEY;
	    }
	}
    }

    old_name = istr_dup(obj->name);
    istr_destroy(obj->name);
    obj->name = istr_dup(new_name);

#ifdef DEBUG
    obj->debug_name = istr_string(obj->name);
#endif

    /*
     * Update the names list
     */
    if ((names = objP_get_names_scope(obj)) != NULL)
    {
	if (strlist_get_istr_data(names, old_name) == ((void*)obj))
	{
	    /* this was probably called during a reparent, so the */
	    /* old_name is actuall from a different names list */
	    strlist_remove_istr(names, old_name);
	}
	strlist_add_istr(names, new_name, (void *)obj);
    }

    /*
     * The indexes are up-to-date - verify everything
     */
    verify_for_write(obj);

    objP_notify_send_name_change(obj, old_name);
    istr_destroy(old_name);

    return 0;
}


int
obj_set_name(ABObj obj, STRING strName)
{
    int		return_value = 0;
    ISTRING	name = istr_create(strName);
    return_value = obj_set_name_istr(obj, name);
    istr_destroy(name);
    return return_value;
}


#ifndef obj_get_name
STRING
obj_get_name(ABObj obj)
{
    verify_for_read(obj);
    return istr_string(obj->name);
}
#endif /* !obj_get_name */

#ifndef obj_get_name_istr
ISTRING
obj_get_name_istr(ABObj obj)
{
    verify_for_read(obj);
    return obj->name;
}
#endif /* !obj_get_name_istr */


int
obj_set_num_columns(ABObj obj, int num_columns)
{
    int old_num_cols;
    int iRet = -1;

    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_CONTAINER:
	old_num_cols = obj->info.container.num_columns;
	obj->info.container.num_columns = num_columns;
	iRet = 0;
	break;
    case AB_TYPE_CHOICE:
	old_num_cols = obj->info.choice.num_columns;
	obj->info.choice.num_columns = num_columns;
	iRet = 0;
	break;
    case AB_TYPE_TERM_PANE:
        old_num_cols = obj->info.term.num_columns; 
	obj->info.term.num_columns = num_columns;
	iRet = 0;
    }
    if (obj_is_text(obj))
    {
        old_num_cols = obj->info.text.num_columns; 
	obj->info.text.num_columns = num_columns;
	iRet = 0;
    }
    if (iRet == -1)
    	member_error(obj, "num_columns");

    else if (old_num_cols != num_columns)
    	iRet= objP_notify_send_rc_geometry_change(obj);

    return iRet;
}


int
obj_get_num_columns(ABObj obj)
{
    verify_for_read(obj);
    switch (obj->type)
    {
    case AB_TYPE_CONTAINER:
	return obj->info.container.num_columns;

    case AB_TYPE_CHOICE:
	return obj->info.choice.num_columns;

    case AB_TYPE_TERM_PANE:
	return obj->info.term.num_columns;
    }
    if (obj_is_text(obj))
    {
	return obj->info.text.num_columns;
    }
    return 0;
}


int
obj_set_num_rows(ABObj obj, int num_rows)
{
    int	old_num_rows;
    int iRet = -1;

    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_CONTAINER:
	old_num_rows = obj->info.container.num_rows;
	obj->info.container.num_rows = num_rows;
	iRet = 0;
 	break;
    case AB_TYPE_LIST:
	old_num_rows = obj->info.list.num_rows;
	obj->info.list.num_rows = num_rows;
	iRet = 0;
	break;
    case AB_TYPE_TERM_PANE:
	old_num_rows = obj->info.term.num_rows;
	obj->info.term.num_rows = num_rows;
	iRet = 0;
	break;
    }
    if (obj_is_text(obj))
    {
	old_num_rows = obj->info.text.num_rows;
	obj->info.text.num_rows = num_rows;
	iRet = 0;
    }
    if (iRet == -1)
    	member_error(obj, "num_rows");

    else if (old_num_rows != num_rows)
        iRet= objP_notify_send_rc_geometry_change(obj);
	
    return iRet;
}


int
obj_get_num_rows(ABObj obj)
{
    verify_for_read(obj);
    switch (obj->type)
    {
    case AB_TYPE_CONTAINER:
	return obj->info.container.num_rows;
    case AB_TYPE_LIST:
	return obj->info.list.num_rows;
    case AB_TYPE_TERM_PANE:
	return obj->info.term.num_rows;
    }
    if (obj_is_text(obj))
    {
	return obj->info.text.num_rows;
    }
    return 0;
}


int
obj_set_orientation(ABObj obj, AB_ORIENTATION orientation)
{
    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_SEPARATOR:
	obj->info.separator.orientation = orientation;
	return 0;
    case AB_TYPE_SCALE:
	obj->info.scale.orientation = orientation;
	return 0;

    case AB_TYPE_CHOICE:
	obj->info.choice.orientation = orientation;
	return 0;
    }
    member_error(obj, "orientation");
    return -1;
}


AB_ORIENTATION
obj_get_orientation(ABObj obj)
{
    verify_for_read(obj);
    switch (obj->type)
    {
    case AB_TYPE_SEPARATOR:
	return obj->info.separator.orientation;

    case AB_TYPE_SCALE:
	return obj->info.scale.orientation;

    case AB_TYPE_CHOICE:
	return obj->info.choice.orientation;
    }
    return AB_ORIENT_HORIZONTAL;
}

int
obj_set_label(ABObj obj, STRING label)
{
    verify_for_write(obj);
    istr_destroy(obj->label);
    obj->label = istr_create(label);
    return 0;
}

#ifndef obj_get_label
STRING
obj_get_label(ABObj obj)
{
    verify_for_read(obj);
    return istr_string(obj->label);
}
#endif				/* !obj_get_label */

int
obj_set_label_type(ABObj obj, AB_LABEL_TYPE label_type)
{
    verify_for_write(obj);
    obj->label_type = label_type;
    return 0;
}

#ifndef obj_get_label_type
AB_LABEL_TYPE
obj_get_label_type(ABObj obj)
{
    verify_for_read(obj);
    return obj->label_type;
}
#endif /* !obj_get_label_typ */

int
obj_set_label_alignment(ABObj obj, AB_ALIGNMENT type)
{
    verify_for_write(obj);

    switch (obj->type)
    {
    case AB_TYPE_BUTTON:
	obj->info.button.label_alignment = type;
	return 0;

    case AB_TYPE_LABEL:
	obj->info.label.label_alignment = type;
	return 0;
    }
    member_error(obj, "label_alignment");
    return -1;

}

AB_ALIGNMENT
obj_get_label_alignment(ABObj obj)
{
    verify_for_read(obj);
    switch (obj->type)
    {
    case AB_TYPE_BUTTON:
	return (obj->info.button.label_alignment);

    case AB_TYPE_LABEL:
	return (obj->info.label.label_alignment);
    }
    return (AB_ALIGNMENT) - 1;

}

int
obj_set_label_position(ABObj obj, AB_COMPASS_POINT pos)
{
    verify_for_write(obj);

    switch (obj->type)
    {
    case AB_TYPE_TEXT_FIELD:
	obj->info.text.label_position = pos;
	return 0;
    case AB_TYPE_CHOICE:
	obj->info.choice.label_position = pos;
	return 0;
    case AB_TYPE_COMBO_BOX:
	obj->info.combo_box.label_position = pos;
	return 0;
    case AB_TYPE_LIST:
	obj->info.list.label_position = pos;
	return 0;
    case AB_TYPE_SCALE:
	obj->info.scale.label_position = pos;
	return 0;
    case AB_TYPE_SPIN_BOX:
	obj->info.spin_box.label_position = pos;
	return 0;
    }
    member_error(obj, "label_position");
    return -1;
}

AB_COMPASS_POINT
obj_get_label_position(ABObj obj)
{
    verify_for_read(obj);

    switch (obj->type)
    {
    case AB_TYPE_TEXT_PANE:
    case AB_TYPE_TEXT_FIELD:
	return (obj->info.text.label_position);

    case AB_TYPE_CHOICE:
	return (obj->info.choice.label_position);

    case AB_TYPE_COMBO_BOX:
	return (obj->info.combo_box.label_position);

    case AB_TYPE_LIST:
	return (obj->info.list.label_position);

    case AB_TYPE_SCALE:
	return (obj->info.scale.label_position);

    case AB_TYPE_SPIN_BOX:
	return (obj->info.spin_box.label_position);
    }
    return (AB_COMPASS_POINT) - 1;

}

STRING
obj_get_bg_color(ABObj obj)
{
    verify_for_read(obj);
    return istr_string(obj->bg_color);
}

STRING
obj_get_fg_color(ABObj obj)
{
    verify_for_read(obj);
    return istr_string(obj->fg_color);
}

int
obj_set_menu_name(ABObj obj, STRING menu_name)
{
    verify_for_write(obj);

    istr_destroy(obj->menu_name);
    obj->menu_name = istr_create(menu_name);
    return 0;
}

STRING
obj_get_menu_name(ABObj obj)
{
    verify_for_read(obj);

    return istr_string(obj->menu_name);

}

int
obj_set_menu_title(ABObj obj, STRING menu_title)
{
    verify_for_write(obj);

    switch(obj->type)
    {
	case AB_TYPE_CONTAINER:
    	    istr_destroy(obj->info.container.menu_title);
	    obj->info.container.menu_title = istr_create(menu_title);
	    break;
	case AB_TYPE_DRAWING_AREA:
    	    istr_destroy(obj->info.drawing_area.menu_title);
	    obj->info.drawing_area.menu_title = istr_create(menu_title);
	    break;
	case AB_TYPE_LIST:
    	    istr_destroy(obj->info.list.menu_title);
	    obj->info.list.menu_title = istr_create(menu_title);
	    break;
	case AB_TYPE_TEXT_FIELD:
	case AB_TYPE_TEXT_PANE:
    	    istr_destroy(obj->info.text.menu_title);
	    obj->info.text.menu_title = istr_create(menu_title);
	    break;
	case AB_TYPE_TERM_PANE:
    	    istr_destroy(obj->info.term.menu_title);
	    obj->info.term.menu_title = istr_create(menu_title);
	    break;
	default:
	    return -1;
    }
    return 0;
}

STRING
obj_get_menu_title(ABObj obj)
{
    verify_for_read(obj);

    switch(obj->type)
    {
        case AB_TYPE_CONTAINER:
            return(istr_string(obj->info.container.menu_title));
        case AB_TYPE_DRAWING_AREA:
            return(istr_string(obj->info.drawing_area.menu_title));
        case AB_TYPE_LIST:
            return(istr_string(obj->info.list.menu_title));
        case AB_TYPE_TEXT_FIELD:
        case AB_TYPE_TEXT_PANE:
            return(istr_string(obj->info.text.menu_title));
        case AB_TYPE_TERM_PANE:
            return(istr_string(obj->info.term.menu_title));
        default:
            return NULL;
    }
}

STRING
obj_get_mnemonic(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_item(obj) && (obj->info.item.mnemonic))
    {
        return (istr_string(obj->info.item.mnemonic));
    }
    return ((STRING) NULL);
}

int
obj_set_mnemonic(ABObj obj, STRING mnem)
{
    verify_for_write(obj);
    if (!obj_is_item(obj))
    {
        member_error(obj, "mnemonic");
        return -1;
    }   
    istr_destroy(obj->info.item.mnemonic);
    obj->info.item.mnemonic = istr_create(mnem);
    return 0;
}

/*
 * Make this object actually refer to another object
 */
int
obj_cvt_to_ref(
		 ABObj obj,
		 ABObj ref_obj
)
{
    if (obj == NULL)
    {
	return (ref_obj == NULL? 0:-1);		/* NULL can only ref NULL */
    }
    verify_for_write(obj);

    /*
     * The types must match!
     */
    if (ref_obj != NULL)
    {
	obj_set_type(obj, obj_get_type(ref_obj));
    }
    obj->ref_to= ref_obj;
    return 0;
}

ABObj
obj_get_actual_obj(
		 ABObj obj
)
{
    if (obj == NULL)
    {
	return NULL;
    }
    verify_for_read(obj);

    return (obj->ref_to == NULL? obj:obj->ref_to);
}

int
obj_set_min_value(ABObj obj, int minval)
{
    int                 iRet = 0;
    verify_for_write(obj);

    switch (obj->type)
    {
    case AB_TYPE_SCALE:
	obj->info.scale.min_value = minval;
	break;
    case AB_TYPE_SPIN_BOX:
	obj->info.spin_box.min_value = minval;
	break;

    default:
	iRet = -1;
	member_error(obj, "min_value");
	break;
    }
    return iRet;
}


int
obj_get_min_value(ABObj obj)
{
    int                 iValue = 0;
    verify_for_read(obj);

    switch (obj->type)
    {
    case AB_TYPE_SCALE:
	iValue = obj->info.scale.min_value;
	break;

    case AB_TYPE_SPIN_BOX:
	iValue = obj->info.spin_box.min_value;
	break;

    default:
	iValue = -1;
	break;
    }
    return iValue;
}

int
obj_set_max_value(ABObj obj, int maxval)
{
    int                 iRet = 0;
    verify_for_write(obj);

    switch (obj->type)
    {
    case AB_TYPE_SCALE:
        obj->info.scale.max_value = maxval;
        break;
    case AB_TYPE_SPIN_BOX:
        obj->info.spin_box.max_value = maxval;
        break;

    default:
        iRet = -1;
        member_error(obj, "max_value");
        break;
    }
    return iRet;
}

int
obj_get_max_value(ABObj obj)
{
    int                 iValue = 0;
    verify_for_read(obj);

    switch (obj->type)
    {
    case AB_TYPE_SCALE:
	iValue = obj->info.scale.max_value;
	break;

    case AB_TYPE_SPIN_BOX:
	iValue = obj->info.spin_box.max_value;
	break;

    default:
	iValue = -1;
	break;
    }
    return iValue;
}

int
obj_set_res_file_arg_classes(ABObj obj, AB_ARG_CLASS_FLAGS argClasses)
{
    if (obj->type == AB_TYPE_PROJECT)
    {
	obj->info.project.res_file_arg_classes = argClasses;
	return 0;
    }
    return ERR_NOT_FOUND;
}

AB_ARG_CLASS_FLAGS
obj_get_res_file_arg_classes(ABObj obj)
{
    if (obj->type == AB_TYPE_PROJECT)
    {
	return obj->info.project.res_file_arg_classes;
    }
    return AB_ARG_CLASS_FLAGS_NONE;
}


int
obj_set_resizable(ABObj obj, BOOL resizable)
{
    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_BASE_WINDOW:
    case AB_TYPE_DIALOG:
	obj->info.window.resizable = resizable;
	return 0;
    }
    member_error(obj, "resizeable");
    return -1;
}


BOOL
obj_get_resizable(ABObj obj)
{
    verify_for_read(obj);
    switch (obj->type)
    {
    case AB_TYPE_BASE_WINDOW:
    case AB_TYPE_DIALOG:
	return obj->info.window.resizable;
    }
    return TRUE;
}

AB_SELECT_TYPE
obj_get_selection_mode(ABObj obj)
{
    verify_for_read(obj);

    if (obj->type == AB_TYPE_LIST)
	return obj->info.list.selection_mode;

    return AB_SELECT_UNDEF;

}
int
obj_set_selection_mode(ABObj obj, AB_SELECT_TYPE sel)
{
    int iRet = 0;
    verify_for_write(obj);

    if (obj->type == AB_TYPE_LIST)
	obj->info.list.selection_mode = sel;

    return iRet;
}


int
obj_set_selection_required(ABObj obj, BOOL selreq)
{
    int                 iRet = 0;
    verify_for_write(obj);

    switch (obj->type)
    {
    case AB_TYPE_LIST:
	obj->info.list.selection_required = selreq;
	break;

    case AB_TYPE_CHOICE:
	obj->info.choice.selection_required = selreq;
	break;

    default:
	iRet = -1;
	member_error(obj, "selection_required");
	break;
    }
    return iRet;
}

BOOL
obj_get_selection_required(ABObj obj)
{
    BOOL                bRet = FALSE;
    verify_for_read(obj);

    switch (obj->type)
    {
    case AB_TYPE_CHOICE:
	bRet = obj->info.choice.selection_required;
	break;
    case AB_TYPE_LIST:
	bRet = obj->info.list.selection_required;
	break;
    }
    return bRet;
}


int
obj_set_type(ABObj obj, AB_OBJECT_TYPE new_type)
{
    verify_for_write(obj);

    if (obj_get_type(obj) != new_type)
    {
	/* only clear out the data if the type has changed */
        obj_destruct_type_specific_info(obj);
        obj->type = new_type;
        obj_construct_type_specific_info(obj);
    }
    return 0;
}

int
obj_set_subtype(ABObj obj, int subtype)
{
    verify_for_write(obj);

    if (obj_is_choice(obj))
	obj->info.choice.type = (AB_CHOICE_TYPE) subtype;

    else if (obj_is_message(obj)) 
	obj->info.message.type = (AB_MESSAGE_TYPE)subtype;

    else if (obj_is_container(obj))
	obj->info.container.type = (AB_CONTAINER_TYPE) subtype;

    else if (obj_is_item(obj))
	obj->info.item.type = (AB_ITEM_TYPE) subtype;

    else if (obj_is_button(obj))
	obj->info.button.type = (AB_BUTTON_TYPE) subtype;

    else if (obj_is_menu(obj))
	obj->info.menu.type = (AB_MENU_TYPE) subtype;

    /*
     * else if (obj_is_dialog(obj)) obj->info.dialog.type =
     * (AB_DIALOG_TYPE)subtype;
     */

    return 0;

}



#ifndef obj_get_type
AB_OBJECT_TYPE
obj_get_type(ABObj obj)
{
    verify_for_read(obj);
    return obj->type;
}

#endif				/* !obj_get_type */

int
obj_get_subtype(ABObj obj)
{
    verify_for_read(obj);

    if (obj_is_choice(obj))
	return ((int) obj->info.choice.type);

    else if (obj_is_message(obj)) 
	return((int)obj->info.message.type);

    else if (obj_is_button(obj))
	return ((int) obj->info.button.type);

    else if (obj_is_container(obj))
	return ((int) obj->info.container.type);

    else if (obj_is_item(obj))
	return ((int) obj->info.item.type);

    else if (obj_is_menu(obj))
	return ((int) obj->info.menu.type);

    return (AB_NO_SUBTYPE);

}

STRING
obj_get_process_string(
		       ABObj obj
)
{
    verify_for_read(obj);
    if (obj->type == AB_TYPE_TERM_PANE)
	return (istr_string(obj->info.term.process_string));

    return NULL;

}

int
obj_set_process_string(
		       ABObj obj,
		       STRING subproc
)
{
    verify_for_write(obj);
    if (obj->type == AB_TYPE_TERM_PANE)
    {
	istr_destroy(obj->info.term.process_string);
	obj->info.term.process_string = istr_create(subproc);
    }
    return 0;
}

int
obj_set_was_written(ABObj obj, BOOL was_written)
{
    verify_for_write(obj);
    if (was_written)
	obj_set_impl_flags(obj, ObjFlagWasWritten);
    else
	obj_clear_impl_flags(obj, ObjFlagWasWritten);
    return 0;
}

BOOL
obj_was_written(ABObj obj)
{
    verify_for_read(obj);
    return obj_has_impl_flags(obj, ObjFlagWasWritten);
}

int
obj_set_win_parent(
    ABObj obj,
    ABObj wparent
)
{
    AB_OBJECT_TYPE	type = AB_TYPE_UNDEF;

    if (!obj_is_window(obj))
	return -1;

    verify_for_write(obj);
    type = obj_get_type(obj);
    switch (type)
    {
	case AB_TYPE_BASE_WINDOW:
	case AB_TYPE_DIALOG:
	    obj->info.window.win_parent = wparent;
	    break;
	case AB_TYPE_FILE_CHOOSER:
	    obj->info.file_chooser.win_parent = wparent;
	    break;
	default:
	    break;
    }

    return 0;
}

ABObj
obj_get_win_parent(
    ABObj obj
)
{
    ABObj               wparent = NULL;
    AB_OBJECT_TYPE      type = AB_TYPE_UNDEF; 
 
    if (!obj_is_window(obj))
	return NULL;

    verify_for_read(obj);
    type = obj_get_type(obj);
    switch (type) 
    { 
	case AB_TYPE_BASE_WINDOW:
	case AB_TYPE_DIALOG:
	    wparent = obj->info.window.win_parent;
            break;
        case AB_TYPE_FILE_CHOOSER: 
            wparent = obj->info.file_chooser.win_parent;
            break;
        default: 
            break; 
    }    

    return (wparent);
}

int
obj_set_write_me(ABObj obj, BOOL write_me)
{
    verify_for_write(obj);
    if (write_me)
    {
	obj_set_impl_flags(obj, ObjFlagWriteMe);
    }
    else
    {
	obj_clear_impl_flags(obj, ObjFlagWriteMe);
    }
    return 0;
}

BOOL
obj_get_write_me(ABObj obj)
{
    verify_for_read(obj);
    return obj_has_impl_flags(obj, ObjFlagWriteMe);
}

int
obj_set_value_x(ABObj obj, int valx)
{
    int                 iRet = 0;
    verify_for_write(obj);

    switch (obj->type)
    {
    case AB_TYPE_CHOICE:
	obj->info.choice.value_x = valx;
	break;

    default:
	iRet = -1;
	member_error(obj, "value_x");
	break;
    }
    return iRet;
}


int
obj_get_value_x(ABObj obj)
{
    int                 iRet = 0;
    verify_for_read(obj);

    switch (obj->type)
    {
    case AB_TYPE_CHOICE:
	iRet = obj->info.choice.value_x;
	break;
    default:
	iRet = -1;
	break;
    }
    return iRet;
}


int
obj_set_value_y(ABObj obj, int valy)
{
    int                 iRet = 0;
    verify_for_write(obj);

    switch (obj->type)
    {
    case AB_TYPE_CHOICE:
	obj->info.choice.value_y = valy;
	break;

    default:
	iRet = -1;
	member_error(obj, "value_y");
	break;
    }
    return iRet;
}


int
obj_get_value_y(ABObj obj)
{
    int                 iRet = 0;
    verify_for_read(obj);

    switch (obj->type)
    {
    case AB_TYPE_CHOICE:
	iRet = obj->info.choice.value_y;
	break;
    default:
	iRet = -1;
	break;
    }
    return iRet;
}

int
obj_set_vscrollbar_policy(ABObj obj, AB_SCROLLBAR_POLICY vscrollbar)
{
    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_DRAWING_AREA:
	obj->info.drawing_area.vscrollbar = vscrollbar;
	break;
    case AB_TYPE_TEXT_PANE:
	obj->info.text.vscrollbar = vscrollbar;
	break;
    case AB_TYPE_TERM_PANE:
	obj->info.term.vscrollbar = vscrollbar;
	break;
    }

    return 0;
}

int
obj_set_tearoff(ABObj obj, BOOL tearoff)
{
    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_MENU:
	obj->info.menu.tear_off = tearoff;
	return 0;
    }
    member_error(obj, "tear_off");
    return -1;
}

int
obj_set_vendor(ABObj obj, STRING vendor)
{
    verify_for_write(obj);
    if (!obj_is_project(obj))
    {
        member_error(obj, "vendor");
        return -1;
    }
    istr_destroy(obj->info.project.vendor);
    obj->info.project.vendor = istr_create(vendor);
    return 0;
}

STRING
obj_get_vendor(ABObj obj)
{
    verify_for_read(obj);
    if (!obj_is_project(obj))
    {
        member_error(obj, "vendor");
        return NULL;
    }
    return (istr_string(obj->info.project.vendor));
}

int
obj_set_version(ABObj obj, STRING version)
{
    verify_for_write(obj);
    if (!obj_is_project(obj))
    {
        member_error(obj, "version");
        return -1;
    }
    istr_destroy(obj->info.project.version);
    obj->info.project.version = istr_create(version);
    return 0;
}

STRING 
obj_get_version(ABObj obj) 
{ 
    verify_for_read(obj); 
    if (!obj_is_project(obj)) 
    { 
        member_error(obj, "version");
        return NULL;
    }
    return (istr_string(obj->info.project.version));
} 


int
obj_set_root_window(ABObj obj, ABObj root_window)
{
    verify_for_write(obj);
    if (!obj_is_project(obj))
    {
	member_error(obj, "root_window");
	return -1;
    }
    obj->info.project.root_window = root_window;
    return 0;
}


ABObj
obj_get_root_window(ABObj obj)
{
    verify_for_read(obj);
    if (!obj_is_project(obj))
    {
	member_error(obj, "root_window");
	return NULL;
    }
    return (obj->info.project.root_window);
}

int
obj_set_height_is_resizable(ABObj obj, BOOL resizable)
{
    verify_for_write(obj);
    if (resizable)
	obj_set_impl_flags(obj, ObjFlagHeightIsResizable);
    else
	obj_clear_impl_flags(obj, ObjFlagHeightIsResizable);
    return 0;
}

BOOL
obj_get_height_is_resizable(ABObj obj)
{
    verify_for_read(obj);
    return obj_has_impl_flags(obj, ObjFlagHeightIsResizable);
}

int
obj_set_hoffset(ABObj obj, int hoffset)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.hoffset = hoffset);
    }
    return ERR_BAD_PARAM1;
}

int
obj_get_hoffset(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.hoffset);
    }
    return ERR_BAD_PARAM1;
}

int
obj_set_voffset(ABObj obj, int voffset)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.voffset = voffset);
    }
    return ERR_BAD_PARAM1;
}

int
obj_get_voffset(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.voffset);
    }
    else
    {

	/*
	 * REMIND: Not sure what to return when an ABObj is not a container
	 * type.
	 */
	return (0);
    }
}

int
obj_set_hspacing(ABObj obj, int hspacing)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.hspacing = hspacing);
    }
    return ERR_BAD_PARAM1;
}

int
obj_get_hspacing(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.hspacing);
    }
    else
    {

	/*
	 * REMIND: Not sure what to return when an ABObj is not a container
	 * type.
	 */
	return (0);
    }
}

int
obj_set_vspacing(ABObj obj, int vspacing)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.vspacing = vspacing);
    }
    return ERR_BAD_PARAM1;
}

int
obj_get_vspacing(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.vspacing);
    }
    else
    {

	/*
	 * REMIND: Not sure what to return when an ABObj is not a container
	 * type.
	 */
	return (0);
    }
}

int
obj_set_ref_point(ABObj obj, AB_COMPASS_POINT ref_point)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.ref_point = ref_point);
    }
    return ERR_BAD_PARAM1;
}

AB_COMPASS_POINT
obj_get_ref_point(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_container(obj))
    {
	return obj->info.container.ref_point;
    }
    else
    {

	/*
	 * REMIND: Not sure if I should use obj_get_label_position() or if I
	 * should do my own switch statement.
	 */

	return (obj_get_label_position(obj));
    }
}

int
obj_set_hattach_type(ABObj obj, AB_ATTACH_TYPE hattach_type)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.hattach_type = hattach_type);
    }
    return ERR_BAD_PARAM1;
}

AB_ATTACH_TYPE
obj_get_hattach_type(ABObj obj)
{
    AB_ATTACH_TYPE      attach_type = AB_ATTACH_UNDEF;
    verify_for_read(obj);

    if (obj_is_container(obj))
    {
	attach_type = (obj->info.container.hattach_type);
    }

    return attach_type;
}

int
obj_set_vattach_type(ABObj obj, AB_ATTACH_TYPE vattach_type)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.vattach_type = vattach_type);
    }
    return ERR_BAD_PARAM1;
}

AB_ATTACH_TYPE
obj_get_vattach_type(ABObj obj)
{
    AB_ATTACH_TYPE      attach_type = AB_ATTACH_UNDEF;
    verify_for_read(obj);

    if (obj_is_container(obj))
    {
	attach_type = (obj->info.container.vattach_type);
    }

    return attach_type;
}

AB_BUTTON_TYPE
obj_get_button_type(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_button(obj))
    {
	return (obj->info.button.type);
    }
    return (AB_BUT_UNDEF);
}

int
obj_set_item_type(ABObj obj, AB_ITEM_TYPE item_type)
{
    verify_for_write(obj);
    if (obj_is_item(obj))
    {
	return (obj->info.item.type = item_type);
    }
    return ERR_BAD_PARAM1;
}


AB_ITEM_TYPE
obj_get_item_type(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_item(obj))
    {
	return (obj->info.item.type);
    }
    return (AB_ITEM_FOR_UNDEF);
}

int
obj_set_group_type(ABObj obj, AB_GROUP_TYPE group_type)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.group_type = group_type);
    }
    return ERR_BAD_PARAM1;
}

AB_GROUP_TYPE
obj_get_group_type(ABObj obj)
{
    verify_for_read(obj);
    if ((obj != NULL) && obj_is_container(obj))
    {
	return (obj->info.container.group_type);
    }
    return (AB_GROUP_UNDEF);
}


int
obj_set_row_align(ABObj obj, AB_ALIGNMENT row_align)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.row_align = row_align);
    }
    return ERR_BAD_PARAM1;
}

AB_ALIGNMENT
obj_get_row_align(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.row_align);
    }
    return (AB_ALIGN_UNDEF);
}

int
obj_set_class_name(ABObj obj, STRING cname)
{
    verify_for_write(obj);
    istr_destroy(obj->class_name);
    obj->class_name = istr_create(cname);
    return 0;
}

STRING
obj_get_class_name(ABObj obj)
{
    verify_for_read(obj);
    return istr_string(obj->class_name);
}

int
obj_set_col_align(ABObj obj, AB_ALIGNMENT col_align)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.col_align = col_align);
    }
    return ERR_BAD_PARAM1;
}

AB_ALIGNMENT
obj_get_col_align(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.col_align);
    }
    return (AB_ALIGN_UNDEF);
}

int
obj_set_container_type(ABObj obj, AB_CONTAINER_TYPE cont_type)
{
    verify_for_write(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.type = cont_type);
    }
    return ERR_BAD_PARAM1;
}

AB_CONTAINER_TYPE
obj_get_container_type(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_container(obj))
    {
	return obj->info.container.type;
    }
    return AB_CONT_UNDEF;
}

int
obj_get_decimal_points(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_spin_box(obj))
	return(obj->info.spin_box.decimal_points);
    else if (obj_is_scale(obj))
	return(obj->info.scale.decimal_points);

    return -1;
}

int
obj_set_decimal_points(ABObj obj, int dec_points)
{
    verify_for_write(obj);
    if (obj_is_spin_box(obj))
    {
	obj->info.spin_box.decimal_points = dec_points;
	return 0;
    }
    else if (obj_is_scale(obj))
    {
	obj->info.scale.decimal_points = dec_points;
	return 0;
    }
    member_error(obj, "decimal_points");
    return -1;
}

int
obj_set_default_act_button(
    ABObj obj,
    ABObj defaultb 
)
{
    if (!obj_is_window(obj))
        return -1;

    verify_for_write(obj);

    obj->info.window.default_act_button= defaultb;

    return 0;
}

ABObj
obj_get_default_act_button(
    ABObj obj
)
{
    ABObj               defaultb = NULL;
 
    if (!obj_is_window(obj))
        return NULL;
 
    verify_for_read(obj);

    return(obj->info.window.default_act_button);
}

AB_DIRECTION
obj_get_direction(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_scale(obj))
        return(obj->info.scale.direction);

    return AB_DIR_UNDEF;
}

int
obj_set_direction(ABObj obj, AB_DIRECTION dir)
{
    verify_for_write(obj);
    if (obj_is_scale(obj))
    {
        obj->info.scale.direction= dir;
        return 0;
    }   
    member_error(obj, "direction");
    return -1;
}

int
obj_set_drawarea_width(
		       ABObj obj,
		       int width
)
{
    verify_for_write(obj);
    if (obj_is_drawing_area(obj))
    {
	obj->info.drawing_area.drawarea_width = width;
	return 0;
    }
    member_error(obj, "drawarea_width");
    return -1;
}

int
obj_set_drawarea_height(
			ABObj obj,
			int height
)
{
    verify_for_write(obj);
    if (obj_is_drawing_area(obj))
    {
	obj->info.drawing_area.drawarea_height = height;
	return 0;
    }
    member_error(obj, "drawarea_height");
    return -1;
}

int
obj_get_drawarea_width(
		       ABObj obj
)
{
    verify_for_read(obj);
    if (obj_is_drawing_area(obj))
	return (obj->info.drawing_area.drawarea_width);

    return -1;
}

int
obj_get_drawarea_height(
			ABObj obj
)
{
    verify_for_read(obj);
    if (obj_is_drawing_area(obj))
	return (obj->info.drawing_area.drawarea_height);

    return -1;
}

int
obj_set_textpane_width(ABObj obj,
                       int width)
{
    if (obj_is_text_pane(obj))
    {
	obj->info.text.textpane_width = width;
	return 0;
    }

    return -1;
}

int
obj_set_textpane_height(ABObj obj,
			int height)
{
    if (obj_is_text_pane(obj))
    {
	obj->info.text.textpane_height = height;
	return 0;
    }

    return -1;
}

int
obj_get_textpane_width(ABObj obj)
{
    if (obj_is_text_pane(obj))
	return (obj->info.text.textpane_width);
    else
        return -1;
}

int
obj_get_textpane_height(ABObj obj)
{
    if (obj_is_text_pane(obj))
	return (obj->info.text.textpane_height);
    else
        return -1;
}


AB_SCROLLBAR_POLICY
obj_get_hscrollbar_policy(ABObj obj)
{
    verify_for_read(obj);
    switch (obj->type)
    {
    case AB_TYPE_LIST:
	return (obj->info.list.hscrollbar);
    case AB_TYPE_DRAWING_AREA:
	return (obj->info.drawing_area.hscrollbar);
    case AB_TYPE_TEXT_PANE:
	return (obj->info.text.hscrollbar);
    }
    return AB_SCROLLBAR_UNDEF;
}

AB_SCROLLBAR_POLICY
obj_get_vscrollbar_policy(ABObj obj)
{
    verify_for_read(obj);
    switch (obj->type)
    {
    case AB_TYPE_LIST:
	return (obj->info.list.vscrollbar);
    case AB_TYPE_DRAWING_AREA:
	return (obj->info.drawing_area.vscrollbar);
    case AB_TYPE_TEXT_PANE:
	return (obj->info.text.vscrollbar);
    case AB_TYPE_TERM_PANE:
	return (obj->info.term.vscrollbar);
    }
    return AB_SCROLLBAR_UNDEF;
}

STRING
obj_get_accelerator(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_item(obj) && (obj->info.item.accelerator))
    {
	return (istr_string(obj->info.item.accelerator));
    }
    return ((STRING) NULL);
}

int
obj_get_max_length(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_text(obj))
    {
	return (obj->info.text.max_length);
    }
    return (0);
}

int
obj_set_max_length(ABObj obj, int max_len)
{
    verify_for_write(obj);
    if (obj_is_text(obj))
    {
	obj->info.text.max_length = max_len;
	return 0;
    }
    member_error(obj, "max_length");
    return -1;

}

int
obj_set_initial_value_int(ABObj obj, int ival)
{
    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_SCALE:
	obj->info.scale.initial_value = ival;
	break;
    case AB_TYPE_SPIN_BOX:
	obj->info.spin_box.initial_value = ival;
	break;
    case AB_TYPE_TEXT_FIELD:
    case AB_TYPE_TEXT_PANE:
	obj->info.text.initial_value_int = ival;
	break;
    }
    return (0);
}

int
obj_get_initial_value_int(ABObj obj)
{
    verify_for_read(obj);
    switch (obj->type)
    {
    case AB_TYPE_SCALE:
	return (obj->info.scale.initial_value);
    case AB_TYPE_SPIN_BOX:
	return (obj->info.spin_box.initial_value);
    case AB_TYPE_TEXT_FIELD:
    case AB_TYPE_TEXT_PANE:
	return (obj->info.text.initial_value_int);
    }
    return (0);
}

AB_TEXT_TYPE
obj_get_text_type(ABObj obj)
{
    verify_for_read(obj);

    if (obj_is_text(obj))
	return (obj->info.text.type);

    if (obj_is_spin_box(obj))
	return (obj->info.spin_box.type);

    return (AB_TEXT_UNDEF);
}
int
obj_set_text_type(ABObj obj, AB_TEXT_TYPE type)
{
    verify_for_write(obj);

    if (obj_is_text(obj))
        obj->info.text.type = type;

    else if (obj_is_spin_box(obj))
        obj->info.spin_box.type = type;

    else
    {
        member_error(obj, "text_type");
        return ERR;
    }
    return 0; 
}

AB_MENU_TYPE
obj_get_menu_type(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_menu(obj))
    {
	return (obj->info.menu.type);
    }
    return (AB_MENU_UNDEF);
}

AB_PACKING
obj_get_packing(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_container(obj))
    {
	return (obj->info.container.packing);
    }
    return (AB_PACK_UNDEF);
}

AB_CHOICE_TYPE
obj_get_choice_type(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_choice(obj))
    {
	return (obj->info.choice.type);
    }
    return (AB_CHOICE_UNDEF);
}

int
obj_set_is_initially_active(ABObj obj, BOOL active)
{
    verify_for_write(obj);
    if (active)
	obj_set_impl_flags(obj, ObjFlagIsInitiallyActive);
    else
	obj_clear_impl_flags(obj, ObjFlagIsInitiallyActive);
    return 0;
}

BOOL
obj_is_initially_active(ABObj obj)
{
    verify_for_read(obj);
    return obj_has_impl_flags(obj, ObjFlagIsInitiallyActive);
}

int
obj_set_is_initially_iconic(ABObj obj, BOOL iconic)
{
    int                 return_value = 0;
    verify_for_write(obj);

    if (obj_is_window(obj) && !obj_is_file_chooser(obj))
    {
	obj->info.window.is_initially_iconic = iconic;
    }
    else
    {
	member_error(obj, "is_initially_iconic");
	return_value = ERR;
    }
    return return_value;
}

BOOL
obj_is_initially_iconic(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_window(obj) && !obj_is_file_chooser(obj))
    {
	return (obj->info.window.is_initially_iconic);
    }
    return FALSE;
}

int
obj_set_is_initially_selected(ABObj obj, BOOL selected)
{
    int                 return_value = 0;
    verify_for_write(obj);

    if (obj_is_item(obj))
    {
	obj->info.item.is_initially_selected = selected;
    }
    else
    {
	member_error(obj, "is_initially_selected");
	return_value = ERR;
    }
    return return_value;
}

BOOL
obj_is_initially_selected(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_item(obj))
    {
	return (obj->info.item.is_initially_selected);
    }
    return FALSE;
}

int
obj_set_is_initially_visible(ABObj obj, BOOL visible)
{
    verify_for_write(obj);
    if (visible)
	obj_set_impl_flags(obj, ObjFlagIsInitiallyVisible);
    else
	obj_clear_impl_flags(obj, ObjFlagIsInitiallyVisible);
    return 0;
}

BOOL
obj_is_initially_visible(ABObj obj)
{
    verify_for_read(obj);
    return obj_has_impl_flags(obj, ObjFlagIsInitiallyVisible);
}

BOOL
obj_get_tearoff(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_menu(obj))
    {
	return (obj->info.menu.tear_off);
    }
    return (0);
}

BOOL
obj_get_exclusive(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_menu(obj))
    {
	return (obj->info.menu.exclusive);
    }
    return (0);
}

int
obj_set_to(ABObj obj, ABObj to)
{
    verify_for_write(obj);
    if (!obj_is_action(obj))
    {
	member_error(obj, "to");
	return 0;
    }
    obj->info.action.to = to;
    return 0;
}

ABObj
obj_get_to(ABObj obj)
{
    verify_for_read(obj);
    if (!obj_is_action(obj))
    {
	member_error(obj, "to");
	return NULL;
    }
    return obj->info.action.to;
}

int
obj_set_from(ABObj obj, ABObj from)
{
    verify_for_write(obj);
    if (!obj_is_action(obj))
    {
	member_error(obj, "from");
	return 0;
    }
    obj->info.action.from = from;
    return 0;
}

ABObj
obj_get_from(ABObj obj)
{
    verify_for_read(obj);
    if (!obj_is_action(obj))
    {
	member_error(obj, "from");
	return NULL;
    }
    return obj->info.action.from;
}

int
obj_set_func_type(ABObj obj, AB_FUNC_TYPE new_func_type)
{
    verify_for_write(obj);
    if (!obj_is_action(obj))
    {
	member_error(obj, "func_type");
	return -1;
    }
    if (obj->info.action.func_type == new_func_type)
    {
	return 0;
    }

    /*
     * Free the old type's data
     */
    switch (obj->info.action.func_type)
    {
    case AB_FUNC_CODE_FRAG:
	istr_destroy(obj->info.action.func_value.code_frag);
	break;

    case AB_FUNC_USER_DEF:
	istr_destroy(obj->info.action.func_value.func_name);
	break;

    case AB_FUNC_ON_ITEM_HELP:
	break;

    case AB_FUNC_HELP_VOLUME:
	istr_destroy(obj->info.action.volume_id);
	istr_destroy(obj->info.action.location);
	break;

    default:
	break;
    }

    /*
     * Set default value for new type
     */
    obj->info.action.func_type = new_func_type;
    switch (obj->info.action.func_type)
    {
    case AB_FUNC_UNDEF:
	break;

    case AB_FUNC_CODE_FRAG:
	obj->info.action.func_value.code_frag = NULL;
	break;

    case AB_FUNC_USER_DEF:
	obj->info.action.func_value.func_name = NULL;
	break;

    case AB_FUNC_HELP_VOLUME:
        obj->info.action.volume_id = NULL;
        obj->info.action.location = NULL;
        break;

    default:
	break;
    }

    return 0;
}

AB_FUNC_TYPE
obj_get_func_type(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_action(obj))
    {
	return (obj->info.action.func_type);
    }
    return (AB_FUNC_UNDEF);
}

int
obj_set_func_name(ABObj obj, STRING func_name)
{
    verify_for_write(obj);
    if (!(obj_is_action(obj)
	  && (obj->info.action.func_type == AB_FUNC_USER_DEF)))
    {
	member_error(obj, "func_name");
	return -1;
    }
    istr_destroy(obj->info.action.func_value.func_name);
    obj->info.action.func_value.func_name = istr_create(func_name);
    return 0;
}

STRING
obj_get_func_name(ABObj obj)
{
    verify_for_read(obj);
    if (!(obj_is_action(obj)
	  && (obj->info.action.func_type == AB_FUNC_USER_DEF)))
    {
	return NULL;
    }
    return (istr_string(obj->info.action.func_value.func_name));
}

int
obj_set_func_name_suffix(ABObj obj, STRING suffix)
{
    verify_for_write(obj);
    if (!obj_is_action(obj))
    {
	member_error(obj, "func_name_suffix");
	return -1;
    }
    istr_destroy(obj->info.action.func_name_suffix);
    obj->info.action.func_name_suffix = istr_create(suffix);
    return 0;
}

STRING
obj_get_func_name_suffix(ABObj obj)
{
    verify_for_read(obj);
    if (!obj_is_action(obj))
    {
	member_error(obj, "func_name_suffix");
	return NULL;
    }
    return istr_string(obj->info.action.func_name_suffix);
}

int
obj_set_func_code(ABObj obj, STRING code)
{
    verify_for_write(obj);
    if (!(obj_is_action(obj)
	  && (obj->info.action.func_type == AB_FUNC_CODE_FRAG)))
    {
	member_error(obj, "func_code");
	return -1;
    }
    istr_destroy(obj->info.action.func_value.code_frag);
    obj->info.action.func_value.code_frag = istr_create(code);
    return 0;
}

STRING
obj_get_func_code(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_action(obj))
    {
	return (istr_string(obj->info.action.func_value.code_frag));
    }
    member_error(obj, "func_code");
    return ("nil");
}

AB_BUILTIN_ACTION
obj_get_func_builtin(ABObj obj)
{
    verify_for_read(obj);
    if (!(obj_is_action(obj) && (obj_get_func_type(obj) == AB_FUNC_BUILTIN)))
    {
	return AB_STDACT_UNDEF;
    }
    return (obj->info.action.func_value.builtin);
}

int
obj_set_func_builtin(ABObj obj, AB_BUILTIN_ACTION act)
{
    verify_for_write(obj);
    if (obj_is_action(obj) && (obj_get_func_type(obj) == AB_FUNC_BUILTIN))
    {
	obj->info.action.func_value.builtin = act;
	return 0;
    }
    return -1;
}

AB_WHEN
obj_get_when(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_action(obj))
    {
	return (obj->info.action.when);
    }
    return AB_WHEN_UNDEF;
}

int
obj_set_when(ABObj obj, AB_WHEN when)
{
    verify_for_write(obj);
    if (obj->type != AB_TYPE_ACTION)
    {
	member_error(obj, "when");
	return 0;
    }
    obj->info.action.when = when;
    return 0;
}

int
obj_set_width_is_resizable(ABObj obj, BOOL resizable)
{
    verify_for_write(obj);
    if (resizable)
	obj_set_impl_flags(obj, ObjFlagWidthIsResizable);
    else
	obj_clear_impl_flags(obj, ObjFlagWidthIsResizable);
    return 0;
}

BOOL
obj_get_width_is_resizable(ABObj obj)
{
    verify_for_read(obj);
    return obj_has_impl_flags(obj, ObjFlagWidthIsResizable);
}

int
obj_set_arg_type(ABObj obj, AB_ARG_TYPE arg_type)
{
    verify_for_write(obj);
    if (!obj_is_action(obj))
    {
	member_error(obj, "arg_type");
	return -1;
    }
    if (arg_type == obj->info.action.arg_type)
    {
	return 0;
    }

    /*
     * Free old data type
     */
    if (obj->info.action.arg_type == AB_ARG_STRING)
    {
	istr_destroy(obj->info.action.arg_value.sval);
    }

    /*
     * Set initial value for new data type
     */
    obj->info.action.arg_type = arg_type;
    switch (obj->info.action.arg_type)
    {
    case AB_ARG_UNDEF:
    case AB_ARG_VOID_PTR:
	break;

    case AB_ARG_FLOAT:
	obj->info.action.arg_value.fval = 0.0;
	break;

    case AB_ARG_INT:
	obj->info.action.arg_value.fval = 0;
	break;

    case AB_ARG_STRING:
	obj->info.action.arg_value.sval = NULL;
	break;
    }
    return 0;
}

AB_ARG_TYPE
obj_get_arg_type(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_action(obj))
    {
	return (obj->info.action.arg_type);
    }
    return AB_ARG_UNDEF;
}

#ifdef BOGUS
AB_ARG_VALUE
obj_get_arg_value(ABObj obj)
{
    verify_for_read(obj);
    AB_ARG_VALUE        value;
    value.ival = 0;
    if (obj_is_action(obj))
    {
	return (obj->info.action.arg_value);
    }
    return value;
}

#endif				/* BOGUS */

int
obj_set_arg_float(ABObj obj, double float_val)
{
    verify_for_write(obj);
    if (!(obj_is_action(obj) && (obj_get_arg_type(obj) == AB_ARG_FLOAT)))
    {
	member_error(obj, "arg_float");
	return -1;
    }
    obj->info.action.arg_value.fval = float_val;
    return 0;
}

double
obj_get_arg_float(ABObj obj)
{
    verify_for_read(obj);
    if (!(obj_is_action(obj) && (obj_get_arg_type(obj) == AB_ARG_FLOAT)))
    {
	member_error(obj, "arg_float");
	return 0.0;
    }
    return (obj->info.action.arg_value.fval);
}

int
obj_set_arg_int(ABObj obj, int int_value)
{
    verify_for_write(obj);
    if (!(obj_is_action(obj) && (obj->info.action.arg_type == AB_ARG_INT)))
    {
	member_error(obj, "arg_int");
	return -1;
    }
    obj->info.action.arg_value.ival = int_value;
    return 0;
}


int
obj_get_arg_int(ABObj obj)
{
    verify_for_read(obj);
    if (!(obj_is_action(obj) && (obj->info.action.arg_type == AB_ARG_INT)))
    {
	member_error(obj, "arg_int");
	return 0;
    }
    return (obj->info.action.arg_value.ival);
}

int
obj_set_arg_string(ABObj obj, STRING argString)
{
    verify_for_write(obj);
    if (!(obj_is_action(obj) && (obj->info.action.arg_type == AB_ARG_STRING)))
    {
	member_error(obj, "arg_string");
	return -1;
    }
    istr_destroy(obj->info.action.arg_value.sval);
    obj->info.action.arg_value.sval = istr_create(argString);
    return 0;
}

STRING
obj_get_arg_string(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_action(obj))
    {
	return (istr_string(obj->info.action.arg_value.sval));
    }
    return NULL;
}

ABAttachment       *
obj_get_attachment(
		   ABObj obj,
		   AB_COMPASS_POINT dir
)
{
    ABAttachment       *attach = NULL;
    verify_for_read(obj);

    if (obj->attachments == NULL)
    {
	goto epilogue;
    }

    switch (dir)
    {
    case AB_CP_NORTH:
	attach = (&(obj->attachments->north));
	break;
    case AB_CP_SOUTH:
	attach = (&(obj->attachments->south));
	break;
    case AB_CP_EAST:
	attach = (&(obj->attachments->east));
	break;
    case AB_CP_WEST:
	attach = (&(obj->attachments->west));
	break;
    default:
	if (util_get_verbosity() >= 3)
	    fprintf(stderr, "obj_get_attachment: invalid direction\n");
	attach = NULL;
	break;
    }

epilogue:
    return attach;
}

AB_ATTACH_TYPE
obj_get_attach_type(
		    ABObj obj,
		    AB_COMPASS_POINT dir
)
{
    verify_for_read(obj);
    if (obj->attachments == NULL)
	return AB_ATTACH_UNDEF;

    switch (dir)
    {
    case AB_CP_NORTH:
	return (obj->attachments->north.type);
    case AB_CP_SOUTH:
	return (obj->attachments->south.type);
    case AB_CP_EAST:
	return (obj->attachments->east.type);
    case AB_CP_WEST:
	return (obj->attachments->west.type);
    default:
	if (util_get_verbosity() > 0)
	    fprintf(stderr, "obj_get_attach_type: invalid direction\n");
	return AB_ATTACH_UNDEF;

    }
}

void               *
obj_get_attach_value(
		     ABObj obj,
		     AB_COMPASS_POINT dir
)
{
    verify_for_read(obj);
    if (obj->attachments == NULL)
	return NULL;

    switch (dir)
    {
    case AB_CP_NORTH:
	return (obj->attachments->north.value);
    case AB_CP_SOUTH:
	return (obj->attachments->south.value);
    case AB_CP_EAST:
	return (obj->attachments->east.value);
    case AB_CP_WEST:
	return (obj->attachments->west.value);
    default:
	if (util_get_verbosity() > 0)
	    fprintf(stderr, "obj_get_attach_value: invalid direction\n");
	return NULL;

    }
}

int
obj_get_attach_offset(
		      ABObj obj,
		      AB_COMPASS_POINT dir
)
{
    verify_for_read(obj);
    if (obj->attachments == NULL)
	return 0;

    switch (dir)
    {
    case AB_CP_NORTH:
	return (obj->attachments->north.offset);
    case AB_CP_SOUTH:
	return (obj->attachments->south.offset);
    case AB_CP_EAST:
	return (obj->attachments->east.offset);
    case AB_CP_WEST:
	return (obj->attachments->west.offset);
    default:
	if (util_get_verbosity() > 0)
	    fprintf(stderr, "obj_get_attach_offset: invalid direction\n");
	return 0;

    }
}

int
obj_set_attachment(
		   ABObj obj,
		   AB_COMPASS_POINT dir,
		   AB_ATTACH_TYPE type,
		   void *value,
		   int offset
)
{
    ABAttachment       *attachment;
    verify_for_write(obj);

    if (obj->attachments == NULL)
	obj_init_attachments(obj);

    switch (dir)
    {
    case AB_CP_NORTH:
	attachment = &(obj->attachments->north);
	break;
    case AB_CP_WEST:
	attachment = &(obj->attachments->west);
	break;
    case AB_CP_SOUTH:
	attachment = &(obj->attachments->south);
	break;
    case AB_CP_EAST:
	attachment = &(obj->attachments->east);
	break;
    }
    attachment->type = type;
    attachment->value = value;
    attachment->offset = offset;

    return 0;
}

int
obj_set_attach_type(
		    ABObj obj,
		    AB_COMPASS_POINT dir,
		    AB_ATTACH_TYPE type
)
{
    verify_for_write(obj);
    if (obj->attachments == NULL)
	obj_init_attachments(obj);

    switch (dir)
    {
    case AB_CP_NORTH:
	obj->attachments->north.type = type;
	break;
    case AB_CP_WEST:
	obj->attachments->west.type = type;
	break;
    case AB_CP_SOUTH:
	obj->attachments->south.type = type;
	break;
    case AB_CP_EAST:
	obj->attachments->east.type = type;
	break;
    }
    return 0;
}

int
obj_set_attach_value(
		     ABObj obj,
		     AB_COMPASS_POINT dir,
		     void *value
)
{
    verify_for_write(obj);
    if (obj->attachments == NULL)
	obj_init_attachments(obj);

    switch (dir)
    {
    case AB_CP_NORTH:
	obj->attachments->north.value = value;
	break;
    case AB_CP_WEST:
	obj->attachments->west.value = value;
	break;
    case AB_CP_SOUTH:
	obj->attachments->south.value = value;
	break;
    case AB_CP_EAST:
	obj->attachments->east.value = value;
	break;
    }
    return 0;
}

int
obj_set_attach_offset(
		      ABObj obj,
		      AB_COMPASS_POINT dir,
		      int offset
)
{
    verify_for_write(obj);
    if (obj->attachments == NULL)
	obj_init_attachments(obj);

    switch (dir)
    {
    case AB_CP_NORTH:
	obj->attachments->north.offset = offset;
	break;
    case AB_CP_WEST:
	obj->attachments->west.offset = offset;
	break;
    case AB_CP_SOUTH:
	obj->attachments->south.offset = offset;
	break;
    case AB_CP_EAST:
	obj->attachments->east.offset = offset;
	break;
    }
    return 0;
}

int
obj_set_button_type(ABObj obj, AB_BUTTON_TYPE type)
{
    verify_for_write(obj);
    if (!obj_is_button(obj))
	return -1;

    obj->info.button.type = type;
    return 0;
}


int
obj_set_help_act_button(
    ABObj obj,
    ABObj helpb 
)
{
    if (!obj_is_window(obj))
        return -1;

    verify_for_write(obj);
    obj->info.window.help_act_button= helpb;
    return 0;
}

ABObj
obj_get_help_act_button(
    ABObj obj
)
{
    ABObj               defaultb = NULL;
 
    if (!obj_is_window(obj))
        return NULL;

    verify_for_read(obj);
    return(obj->info.window.help_act_button);
}

/*
** Indicate whether a particular object has help data set.  This is used
** by the front end to set-up help in Test Mode and by the code generator
** to decide whether to generate help support code.
*/
BOOL		
obj_has_help_data(ABObj obj)
{
    STRING help_text = istr_string(obj->help_text);

    if( (help_text != 0) && (*help_text != 0) ) {
		return TRUE;
    }
    return FALSE;
}

int
obj_set_help_data(ABObj obj,
		  STRING help_volume,
		  STRING help_location,
		  STRING help_text
)
{
    verify_for_write(obj);
    istr_destroy(obj->help_volume);
    istr_destroy(obj->help_location);
    istr_destroy(obj->help_text);

    obj->help_volume = istr_create(help_volume);
    obj->help_location = istr_create(help_location);
    obj->help_text = istr_create(help_text);

    return 0;
}

int
obj_set_help_volume(ABObj obj,
		    STRING help_volume
)
{
    verify_for_write(obj);
    istr_destroy(obj->help_volume);
    obj->help_volume = istr_create(help_volume);

    return 0;
}

int
obj_set_help_location(ABObj obj,
		      STRING help_location
)
{
    verify_for_write(obj);
    istr_destroy(obj->help_location);
    obj->help_location = istr_create(help_location);

    return 0;
}

int
obj_set_help_text(ABObj obj,
		  STRING help_text
)
{
    verify_for_write(obj);
    istr_destroy(obj->help_text);
    obj->help_text = istr_create(help_text);

    return 0;
}

int
obj_get_help_data(ABObj obj,
		  STRING * help_volume,
		  STRING * help_location,
		  STRING * help_text
)
{
    verify_for_read(obj);
    *help_volume = istr_string(obj->help_volume);
    *help_location = istr_string(obj->help_location);
    *help_text = istr_string(obj->help_text);
    return 0;
}

STRING
obj_get_help_volume(
		    ABObj obj
)
{
    STRING              help_volume = NULL;
    verify_for_read(obj);

    help_volume = istr_string(obj->help_volume);
    return (help_volume);
}

STRING
obj_get_help_location(
		      ABObj obj
)
{
    STRING              help_location = NULL;
    verify_for_read(obj);

    help_location = istr_string(obj->help_location);
    return (help_location);
}

STRING
obj_get_help_text(
		  ABObj obj
)
{
    STRING              help_text = NULL;
    verify_for_read(obj);

    help_text = istr_string(obj->help_text);
    return (help_text);
}

int
obj_set_word_wrap(ABObj obj, BOOL word_wrap)
{
    verify_for_write(obj);
    if (obj_is_text_pane(obj))
    {
	obj->info.text.word_wrap = word_wrap;
	return 0;
    }
    member_error(obj, "word_wrap");
    return -1;
}


BOOL
obj_get_word_wrap(ABObj obj)
{
    verify_for_read(obj);
    return (obj->info.text.word_wrap);
}

int
obj_set_line_style(ABObj obj, AB_LINE_TYPE line_style)
{
    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_SEPARATOR:
	obj->info.separator.line_style = line_style;
	return 0;
    case AB_TYPE_ITEM:
	obj->info.item.line_style = line_style;
	return 0;
    }
    member_error(obj, "line_style");
    return -1;
}


AB_LINE_TYPE
obj_get_line_style(
		   ABObj obj
)
{
    verify_for_read(obj);

    switch(obj->type)
    {
	case AB_TYPE_SEPARATOR:
	    return (obj->info.separator.line_style);
	case AB_TYPE_ITEM:
	    return (obj->info.item.line_style);
	default:
	    return (AB_LINE_UNDEF);
    }
}

int
obj_set_arrow_style(ABObj obj, AB_ARROW_STYLE arrow_style)
{
    verify_for_write(obj);
    switch (obj->type)
    {
    case AB_TYPE_SPIN_BOX:
	obj->info.spin_box.arrow_style = arrow_style;
	return 0;
    }
    member_error(obj, "arrow_style");
    return -1;
}


AB_ARROW_STYLE
obj_get_arrow_style(
		    ABObj obj
)
{
    verify_for_read(obj);
    if (!obj || !obj_is_spin_box(obj))
	return (AB_ARROW_UNDEF);

    return (obj->info.spin_box.arrow_style);
}

int
obj_set_show_value(
		   ABObj obj,
		   BOOL val
)
{
    verify_for_write(obj);
    if (!obj_is_scale(obj))
    {
	member_error(obj, "scale_show_value");
	return -1;
    }

    obj->info.scale.show_value = val;
    return 0;
}

BOOL
obj_get_show_value(
		   ABObj obj
)
{
    verify_for_read(obj);
    if (!obj_is_scale(obj))
    {
	member_error(obj, "show_value");
	return FALSE;
    }

    return (obj->info.scale.show_value);
}

int
obj_set_user_data(ABObj obj, STRING user_data)
{
    verify_for_write(obj);
    istr_destroy(obj->user_data);
    obj->user_data = istr_create(user_data);
    return 0;
}

STRING
obj_get_user_data(ABObj obj)
{
    verify_for_read(obj);
    return istr_string(obj->user_data);
}


#ifdef DEBUG

static int
verify(ABObj obj, int min_debug, STRING file, int line)
{
    int		rc = 0;

    if (debug_level() < min_debug)
    {
	return 0;
    }

    rc = objP_update_verify(obj);
    if (rc < 0)
    {
	util_dprintf(0, "Corrupted object detected (%s:%d).\n",
		     file, line);
	if (debug_level() >= 5)
	{
	    abort();
	}
	return -1;
    }
    return 0;
}

/* don't normally check obj integrity on every read (debug level 3) */
static int
verify_for_read_impl(ABObj obj, STRING file, int line)
{
    return verify(obj, 1, file, line);
}

static int
verify_for_write_impl(ABObj obj, STRING file, int line)
{
    return verify(obj, 1, file, line);
}

#endif /* DEBUG */

#ifdef DEBUG

/*
 * Report an attempt to set an unsupported attribute on an object.
 */
static int
member_error_impl(ABObj obj, STRING member_name, STRING file, int line)
{
    char                obj_name[256];

    /*
     * We "use" fields that are illegal for objects regularly, and just
     * ignore the objects that return errors.
     */
    return 0;

    /*
    if (!debugging())
    {
	return 0;
    }
    *obj_name = 0;
    obj_get_safe_name(obj, obj_name, 256);
    util_dprintf(0,
		 "ERROR(%s:%d): Bad attribute access '%s' in object '%s'.\n",
		 file, line, member_name, obj_name);
    return 0;
    */
}

#endif				/* DEBUG */

BOOL
obj_has_hscrollbar(ABObj obj)
{
    AB_OBJECT_TYPE	type = AB_TYPE_UNDEF;
    AB_SCROLLBAR_POLICY	sb_policy = AB_SCROLLBAR_UNDEF;
    BOOL		has_hscrollbar = FALSE;

    type = obj_get_type(obj);
    if (type == AB_TYPE_DRAWING_AREA || type == AB_TYPE_LIST ||
	type == AB_TYPE_TERM_PANE || type == AB_TYPE_TEXT_PANE )
    {
	sb_policy = obj_get_hscrollbar_policy(obj);
	if (sb_policy == AB_SCROLLBAR_ALWAYS || 
	    sb_policy == AB_SCROLLBAR_WHEN_NEEDED)
	{
	    has_hscrollbar = TRUE;
	}
    }

    return (has_hscrollbar);
}

BOOL 
obj_has_vscrollbar(ABObj obj) 
{ 
    AB_OBJECT_TYPE      type = AB_TYPE_UNDEF; 
    AB_SCROLLBAR_POLICY	sb_policy = AB_SCROLLBAR_UNDEF;
    BOOL                has_vscrollbar = FALSE; 
 
    type = obj_get_type(obj); 
    if (type == AB_TYPE_DRAWING_AREA || type == AB_TYPE_LIST ||
        type == AB_TYPE_TERM_PANE || type == AB_TYPE_TEXT_PANE ) 
    { 
        sb_policy = obj_get_vscrollbar_policy(obj); 
        if (sb_policy == AB_SCROLLBAR_ALWAYS || 
            sb_policy == AB_SCROLLBAR_WHEN_NEEDED)
        { 
            has_vscrollbar = TRUE; 
        } 
    }    

    return (has_vscrollbar); 
} 

int
obj_set_ok_label(ABObj obj, STRING label)
{
    verify_for_write(obj);
    if (!obj_is_file_chooser(obj))
    {
        member_error(obj, "ok_label");
        return -1;
    }
    istr_destroy(obj->info.file_chooser.ok_label);
    obj->info.file_chooser.ok_label = istr_create(label);
    return 0;
}    

STRING
obj_get_ok_label( ABObj obj)
{
    STRING              label = NULL;
 
    verify_for_read(obj);
    if (!obj_is_file_chooser(obj))
    {
        member_error(obj, "ok_label");
        return NULL;
    }
    label = istr_string(obj->info.file_chooser.ok_label);
    return (label);
}

int
obj_set_action1_label(ABObj obj, STRING label)
{
    verify_for_write(obj);
    if (!obj_is_message(obj))
    {
        member_error(obj, "action1_label");
        return -1;
    }
    istr_destroy(obj->info.message.action1_label);
    obj->info.message.action1_label = istr_create(label);
    return 0;
}

STRING
obj_get_action1_label( ABObj obj)
{
    STRING              label = NULL;

    verify_for_read(obj);
    if (!obj_is_message(obj))
    {
        member_error(obj, "action1_label");
        return NULL;
    }
    label = istr_string(obj->info.message.action1_label);
    return (label);
}

int
obj_set_action2_label(ABObj obj, STRING label)
{
    verify_for_write(obj);
    if (!obj_is_message(obj))
    {
        member_error(obj, "action2_label");
        return -1;
    }
    istr_destroy(obj->info.message.action2_label);
    obj->info.message.action2_label = istr_create(label);
    return 0;
}
 
STRING
obj_get_action2_label( ABObj obj)
{
    STRING              label = NULL;

    verify_for_read(obj);
    if (!obj_is_message(obj))
    {
        member_error(obj, "action2_label");
        return NULL;
    }
    label = istr_string(obj->info.message.action2_label);
    return (label);
}

int
obj_set_action3_label(ABObj obj, STRING label)
{
    verify_for_write(obj);
    if (!obj_is_message(obj))
    {
        member_error(obj, "action3_label");
        return -1;
    }
    istr_destroy(obj->info.message.action3_label);
    obj->info.message.action3_label = istr_create(label);
    return 0;
}
 
STRING
obj_get_action3_label( ABObj obj)
{
    STRING              label = NULL;

    verify_for_read(obj);
    if (!obj_is_message(obj))
    {
        member_error(obj, "action3_label");
        return NULL;
    }
    label = istr_string(obj->info.message.action3_label);
    return (label);
}

int 
obj_set_directory(ABObj obj, STRING dir)
{
    verify_for_write(obj);
    if (!obj_is_file_chooser(obj))
    {
        member_error(obj, "directory");
        return -1;
    }
    istr_destroy(obj->info.file_chooser.directory);
    obj->info.file_chooser.directory = istr_create(dir);
    return 0;
}

STRING 
obj_get_directory( ABObj obj)
{ 
    STRING              dir = NULL; 
 
    verify_for_read(obj);   
    if (!obj_is_file_chooser(obj))
    {
        member_error(obj, "directory");
        return NULL;
    }
    dir = istr_string(obj->info.file_chooser.directory);
    return (dir);
} 


int 
obj_set_auto_dismiss(ABObj obj, BOOL dismiss)
{
    verify_for_write(obj);
    if (!obj_is_file_chooser(obj))
    {
        member_error(obj, "auto_dismiss");
        return -1;
    }
    obj->info.file_chooser.auto_dismiss = dismiss;
    return 0;
}

BOOL
obj_get_auto_dismiss( ABObj obj)
{
    verify_for_read(obj);
    if (!obj_is_file_chooser(obj))
    {
        member_error(obj, "auto_dismiss");
        return FALSE;
    }
    return (obj->info.file_chooser.auto_dismiss);
}


int
obj_set_file_type_mask(ABObj obj, AB_FILE_TYPE_MASK ftm)
{
    verify_for_write(obj);
    if (!obj_is_file_chooser(obj))
    {
        member_error(obj, "file_type_mask");
        return -1;
    }
    obj->info.file_chooser.file_type_mask = ftm;
    return 0;
}

AB_FILE_TYPE_MASK
obj_get_file_type_mask( ABObj obj)
{
    verify_for_read(obj);
    if (!obj_is_file_chooser(obj))
    {
        member_error(obj, "file_type_mask");
        return (AB_FILE_TYPE_MASK)0;
    }

    return (obj->info.file_chooser.file_type_mask);
}

int
obj_set_msg_string(ABObj obj, STRING msg)
{
    verify_for_write(obj);
    if (!obj_is_message(obj))
    {
        member_error(obj, "message");
        return -1;
    }
    istr_destroy(obj->info.message.msg_string);
    obj->info.message.msg_string = istr_create(msg);
    return 0;
}

STRING
obj_get_msg_string( ABObj obj)
{
    STRING              msg = NULL;

    verify_for_read(obj);
    if (!obj_is_message(obj))
    {
        member_error(obj, "message");
        return NULL;
    }
    msg = istr_string(obj->info.message.msg_string);
    return (msg);
}

int
obj_set_msg_type(ABObj obj, AB_MESSAGE_TYPE msg_type)
{
    verify_for_write(obj);
    if (obj_is_message(obj))
    {
        return (obj->info.message.type = msg_type);
    }
    return ERR_BAD_PARAM1; 
}
 
AB_MESSAGE_TYPE
obj_get_msg_type( ABObj obj)
{ 
    verify_for_read(obj); 
    if (obj_is_message(obj)) 
    { 
	return (obj->info.message.type); 
    } 
    return AB_MSG_UNDEF;
} 

int
obj_set_default_btn(ABObj obj, AB_DEFAULT_BUTTON button)
{
    verify_for_write(obj);
    if (obj_is_message(obj))
    {
        return (obj->info.message.default_btn = button);
    }
    return ERR_BAD_PARAM1;
}

AB_DEFAULT_BUTTON
obj_get_default_btn( ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_message(obj))
    {
        return (obj->info.message.default_btn);
    }
    return AB_DEFAULT_BTN_UNDEF;
}

BOOL
obj_has_action1_button(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_message(obj))
	return (!util_strempty(obj_get_action1_label(obj)));
    else
	return FALSE;
}

BOOL
obj_has_action2_button(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_message(obj))
	return (!util_strempty(obj_get_action2_label(obj)));
    else
        return FALSE;
}

BOOL
obj_has_action3_button(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_message(obj))
        return (!util_strempty(obj_get_action3_label(obj)));
    else
        return FALSE;
}

BOOL
obj_has_cancel_button(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_message(obj))
        return (obj->info.message.cancel_button != 0);
    else
        return FALSE;
}

BOOL
obj_has_help_button(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_message(obj))
        return (obj->info.message.help_button != 0);
    else
        return FALSE;
}

int
obj_set_sessioning_method(ABObj obj, AB_SESSIONING_METHOD sessioning_method)
{
    verify_for_write(obj);
    if (obj_is_project(obj))
    {
        return (obj->info.project.session_mgmt.sessioning_method = sessioning_method);
    }
    return ERR_BAD_PARAM1; 
}
 
AB_SESSIONING_METHOD
obj_get_sessioning_method( ABObj obj)
{ 
    verify_for_read(obj); 
    if (obj_is_project(obj)) 
    { 
	return (obj->info.project.session_mgmt.sessioning_method); 
    } 
    return AB_SESSIONING_UNDEF;
} 
int
obj_set_cancel_button(ABObj obj, BOOL has_button)
{
    verify_for_write(obj);
    if (!obj_is_message(obj))
        return -1;
    obj->info.message.cancel_button = has_button;
    return 0;
}

int
obj_set_help_button(ABObj obj, BOOL has_button)
{
    verify_for_write(obj);
    if (!obj_is_message(obj))
        return -1;
    obj->info.message.help_button = has_button;
    return 0;
}

int
obj_set_tooltalk_level(ABObj obj, AB_TOOLTALK_LEVEL tt_level)
{
    verify_for_write(obj);
    if (obj_is_project(obj))
    {
        return (obj->info.project.tooltalk.level = tt_level);
    }   
    return ERR_BAD_PARAM1;
}

AB_TOOLTALK_LEVEL
obj_get_tooltalk_level( ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_project(obj))
    {
        return (obj->info.project.tooltalk.level);
    }
    return AB_TOOLTALK_UNDEF;
}

int
obj_set_pane_min( ABObj obj, int val)
{
    verify_for_write(obj);
    if (!obj_is_pane(obj) && !obj_is_layers(obj))
        return -1;

    if (obj_is_text_pane(obj))
	obj->info.text.pane_min = val;
    if (obj_is_term_pane(obj))
	obj->info.term.pane_min = val;
    if (obj_is_drawing_area(obj))
	obj->info.drawing_area.pane_min = val;
    if (obj_is_container(obj))
	obj->info.container.pane_min = val;
    if (obj_is_layers(obj))
	obj->info.layer.pane_min = val;
    return 0;
}

int 
obj_get_pane_min( ABObj obj)
{
    verify_for_read(obj); 
    if (!obj_is_pane(obj) && !obj_is_layers(obj))
        return -1; 

    if (obj_is_text_pane(obj)) 
        return (obj->info.text.pane_min); 
    if (obj_is_term_pane(obj)) 
        return (obj->info.term.pane_min);
    if (obj_is_drawing_area(obj))   
        return (obj->info.drawing_area.pane_min); 
    if (obj_is_container(obj)) 
        return (obj->info.container.pane_min);
    if (obj_is_layers(obj))
	return (obj->info.layer.pane_min);
}

int
obj_set_pane_max( ABObj obj, int val)
{
    verify_for_write(obj);
    if (!obj_is_pane(obj) && !obj_is_layers(obj))
        return -1; 

    if (obj_is_text_pane(obj)) 
        obj->info.text.pane_max = val; 
    if (obj_is_term_pane(obj)) 
        obj->info.term.pane_max = val;
    if (obj_is_drawing_area(obj))   
        obj->info.drawing_area.pane_max = val; 
    if (obj_is_container(obj)) 
	obj->info.container.pane_max = val; 
    if (obj_is_layers(obj))
        obj->info.layer.pane_max = val;
    return 0; 
}

int 
obj_get_pane_max( ABObj obj)
{
    verify_for_read(obj);  
    if (!obj_is_pane(obj) && !obj_is_layers(obj))  
        return -1;  

    if (obj_is_text_pane(obj))  
        return (obj->info.text.pane_max); 
    if (obj_is_term_pane(obj))  
        return (obj->info.term.pane_max); 
    if (obj_is_drawing_area(obj))      
        return (obj->info.drawing_area.pane_max); 
    if (obj_is_container(obj)) 
        return (obj->info.container.pane_max);
    if (obj_is_layers(obj))
	return (obj->info.layer.pane_max);
}

int
obj_set_i18n_enabled(ABObj obj, BOOL i18n_enabled)
{
    verify_for_write(obj);
    if (obj_is_project(obj))
    {
        obj->info.project.i18n.enabled = i18n_enabled;
	return 0;
    }
    return ERR_BAD_PARAM1; 
}
 
BOOL
obj_get_i18n_enabled( ABObj obj)
{ 
    verify_for_read(obj); 
    if (obj_is_project(obj)) 
    { 
	return(obj->info.project.i18n.enabled); 
    } 
    return FALSE;
} 

int
obj_get_num_win_children( ABObj obj)
{
    ABObj       	refObj = NULL;
    AB_OBJ_REF_TYPE	refType = AB_REF_UNDEF;
    ABObjList		refs = NULL;
    void        	*voidRefType = NULL;
    int			numRefs = 0, i = 0;
    int			numWinChildren = 0;

    verify_for_read(obj);
    if (!obj_is_base_win(obj))
	return ERR;

    if ((refs = obj_get_refs_to(obj)) != NULL)
    {
	numRefs = objlist_get_num_objs(refs);
	for (i = 0; i < numRefs; i++)
	{
	    refObj = objlist_get_obj(refs, i, &voidRefType);
	    refType = (AB_OBJ_REF_TYPE)voidRefType;

	    if (refType == AB_REF_WIN_PARENT)
	    {
		numWinChildren++;
	    }
	}
	objlist_destroy(refs);
    }
 
    return (numWinChildren);
}

int
obj_set_read_only(ABObj obj, BOOL is_read_only)
{
    verify_for_write(obj);
    if (is_read_only)
    {
        obj_set_impl_flags(obj, ObjFlagIsReadOnly);
    }
    else
    {   
        obj_clear_impl_flags(obj, ObjFlagIsReadOnly);
    }
    return 0;
}
 
BOOL
obj_get_read_only(ABObj obj)
{
    verify_for_read(obj);
    return obj_has_impl_flags(obj, ObjFlagIsReadOnly);
}

STRING
obj_get_func_help_location(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_action(obj))
    {
        return (istr_string(obj->info.action.location));
    }
    member_error(obj, "func_help_location");
    return NULL;
}

int
obj_set_func_help_location(ABObj obj, STRING location)
{
    verify_for_write(obj);
    if (!(obj_is_action(obj) && 
	(obj->info.action.func_type == AB_FUNC_HELP_VOLUME)))
    {
        member_error(obj, "func_help_location");
        return -1;
    }
    istr_destroy(obj->info.action.location);
    obj->info.action.location = istr_create(location);
    return 0;
}

STRING
obj_get_func_help_volume(ABObj obj)
{
    verify_for_read(obj);
    if (obj_is_action(obj))
    {
        return (istr_string(obj->info.action.volume_id));
    }
    member_error(obj, "func_help_volume");
    return NULL;
}

int
obj_set_func_help_volume(ABObj obj, STRING volume)
{
    verify_for_write(obj); 
    if (!(obj_is_action(obj) &&  
        (obj->info.action.func_type == AB_FUNC_HELP_VOLUME)))
    {  
        member_error(obj, "func_help_volume"); 
        return -1; 
    } 
    istr_destroy(obj->info.action.volume_id);
    obj->info.action.volume_id = istr_create(volume); 
    return 0; 
}



/**************************************************************************
 **************************************************************************
 ***									***
 ***									***
 ***			Drag and Drop					***
 ***									***
 *** dnd flags:								***
 ***	bit  0     -> drag enabled					***
 ***	bit  1     -> drag to root allowed				***
 ***    bits 2-4   -> drag ops						***
 ***	bits 5-8   -> drag types					***
 ***	bit  9     -> drop enabled					***
 ***    bits 10-12 -> drop ops						***
 ***    bits 13-16 -> drop types					***
 ***									***
 **************************************************************************
 **************************************************************************/

#define ObjDndFlagDragEnabled		(((unsigned)0x0001))
#define ObjDndFlagDragToRootOK		(((unsigned)0x0001)<<1)
#define ObjDndFlagDragOpsFBIT		(2)		/* first bit */
#define ObjDndFlagDragOpsNBITS		(3)		/* num bits */
#define ObjDndFlagDragTypesFBIT		(5)		/* first bit */
#define ObjDndFlagDragTypesNBITS	(4)		/* num bits */
#define ObjDndFlagDropEnabled		(((unsigned)0x0001)<<9)
#define ObjDndFlagDropOpsFBIT		(10)		/* first bit */
#define ObjDndFlagDropOpsNBITS		(3)		/* num bits */
#define ObjDndFlagDropTypesFBIT		(13)		/* first bit */
#define ObjDndFlagDropTypesNBITS	(4)		/* num bits */
#define ObjDndFlagDropOnChildOK		(((unsigned)0x0001)<<17)


int
obj_set_drag_cursor(ABObj obj, STRING filename)
{
    verify_for_write(obj);
    istr_destroy(obj->drag_cursor);
    obj->drag_cursor = istr_create(filename);
    return 0;
}

STRING
obj_get_drag_cursor(ABObj obj)
{
    verify_for_read(obj);
    return istr_string(obj->drag_cursor);
}

int
obj_set_drag_cursor_mask(ABObj obj, STRING filename)
{
    verify_for_write(obj);
    istr_destroy(obj->drag_cursor_mask);
    obj->drag_cursor_mask = istr_create(filename);
    return 0;
}

STRING
obj_get_drag_cursor_mask(ABObj obj)
{
    verify_for_read(obj);
    return istr_string(obj->drag_cursor_mask);
}

int
obj_set_drag_initially_enabled(ABObj obj, BOOL enabled)
{
    obj->impl_dnd_flags &= ~ObjDndFlagDragEnabled;
    if (enabled)
    {
        obj->impl_dnd_flags |= ObjDndFlagDragEnabled;
    }
    return 0;
}

BOOL
obj_get_drag_initially_enabled(ABObj obj)
{
    return ((obj->impl_dnd_flags & ObjDndFlagDragEnabled) != 0);
}

int
obj_set_drag_to_root_allowed(ABObj obj, BOOL allowed)
{
    obj->impl_dnd_flags &= ~ObjDndFlagDragToRootOK;
    if (allowed)
    {
        obj->impl_dnd_flags |= ObjDndFlagDragToRootOK;
    }
    return 0;
}

BOOL
obj_get_drag_to_root_allowed(ABObj obj)
{
    return ((obj->impl_dnd_flags & ObjDndFlagDragToRootOK) != 0);
}


int
obj_set_drag_ops(ABObj obj, BYTE dragOps)
{
    obj->impl_dnd_flags &= ~(0x07U<<ObjDndFlagDragOpsFBIT);
    obj->impl_dnd_flags |= ((dragOps & 0x07)<<ObjDndFlagDragOpsFBIT);
    return 0;
}


BYTE
obj_get_drag_ops(ABObj obj)
{
    return (BYTE)((obj->impl_dnd_flags >> ObjDndFlagDragOpsFBIT) & 0x07);
}


int
obj_set_drag_types(ABObj obj, BYTE dragTypes)
{
    obj->impl_dnd_flags &= ~(0x0fU<<ObjDndFlagDragTypesFBIT);
    obj->impl_dnd_flags |= ((dragTypes & 0x0f) << ObjDndFlagDragTypesFBIT);
    return 0;
}


BYTE
obj_get_drag_types(ABObj obj)
{
    return (BYTE)((obj->impl_dnd_flags >> ObjDndFlagDragTypesFBIT) & 0x0f);
}

int
obj_set_drop_initially_enabled(ABObj obj, BOOL enabled)
{
    obj->impl_dnd_flags &= ~ObjDndFlagDropEnabled;
    if (enabled)
    {
        obj->impl_dnd_flags |= ObjDndFlagDropEnabled;
    }
    return 0;
}

BOOL
obj_get_drop_initially_enabled(ABObj obj)
{
    return ((obj->impl_dnd_flags & ObjDndFlagDropEnabled) != 0);
}

int
obj_set_drop_ops(ABObj obj, BYTE dropOps)
{
    obj->impl_dnd_flags &= ~(0x07U<<ObjDndFlagDropOpsFBIT);
    obj->impl_dnd_flags |= ((dropOps & 0x07)<<ObjDndFlagDropOpsFBIT);
    return 0;
}


BYTE
obj_get_drop_ops(ABObj obj)
{
    return (BYTE)((obj->impl_dnd_flags >> ObjDndFlagDropOpsFBIT) & 0x07);
}


int
obj_set_drop_types(ABObj obj, BYTE dropTypes)
{
    obj->impl_dnd_flags &= ~(0x0fU<<ObjDndFlagDropTypesFBIT);
    obj->impl_dnd_flags |= ((dropTypes & 0x0f) << ObjDndFlagDropTypesFBIT);
    return 0;
}


BYTE
obj_get_drop_types(ABObj obj)
{
    return (BYTE)((obj->impl_dnd_flags >> ObjDndFlagDropTypesFBIT) & 0x0f);
}


int
obj_set_drop_on_children_is_allowed(ABObj obj, BOOL allowed)
{
    obj->impl_dnd_flags &= ~ObjDndFlagDropOnChildOK;
    if (allowed)
    {
        obj->impl_dnd_flags |= ObjDndFlagDropOnChildOK;
    }
    return 0;
}

BOOL
obj_drop_on_children_is_allowed(ABObj obj)
{
    return ((obj->impl_dnd_flags & ObjDndFlagDropOnChildOK) != 0);
}

